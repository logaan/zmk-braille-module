#!/usr/bin/env bb
;; Run: ./scripts/examples/message-new-gum.bb (requires https://github.com/charmbracelet/gum)
;; gum supplies fuzzy filtering and selection, text input, a multiline editor and confirmation.
;; td's command is `message new` (singular); td supplies author=user and created=now.

(require '[babashka.fs :as fs]
         '[babashka.process :as p]
         '[clojure.string :as str])

(def root (-> *file* fs/absolutize fs/parent fs/parent fs/parent))
(def statuses ["draft" "ready" "started" "blocked" "delivered" "done" "cancelled"])
(def color-output? (and (System/console)
                        (nil? (System/getenv "NO_COLOR"))
                        (not= "dumb" (System/getenv "TERM"))))

(defn styled [code text]
  (if color-output?
    (str "\u001b[" code "m" text "\u001b[0m")
    text))

(defn task-folders []
  (->> (fs/glob (fs/path root "tasks") "**")
       (filter fs/directory?)
       (map #(str (fs/relativize (fs/path root "tasks") %)))
       sort vec))

(defn gum [args]
  ;; gum renders its UI on stderr and returns the selected value on stdout.
  (let [{:keys [exit out]} @(p/process (into ["gum"] args)
                                       {:in :inherit :err :inherit :out :string})]
    (if (zero? exit)
      (str/replace out #"\r?\n$" "")
      (throw (ex-info "Cancelled" {:cancelled true})))))

(defn show-completed
  ([label value] (show-completed label value false))
  ([label value multiline?]
   ;; Gum erases its widget; keep an accepted-value summary separate from the next prompt.
   (let [empty? (str/blank? value)
         marker (styled "1;32" "✓")
         label-text (str label ":")]
     (if (and multiline? (not empty?))
       (do (println (str marker "  " (styled "1;36" label-text)))
           (doseq [line (str/split value #"\r?\n" -1)]
             (println (str "   " (styled "32" "│") " " line))))
       (println (str marker "  " (styled "1;36" (format "%-13s" label-text))
                     (styled (if empty? "2" "1;32")
                             (if empty? "(empty)" value)))))
     (flush)
     value)))

(defn choose [label options]
  (show-completed label (gum (into ["filter" "--fuzzy" "--strict"
                                   "--header" (str label " (type to filter, or use arrow keys to browse)")]
                                  options))))

(defn pick-task-folder [folders]
  ;; gum filter also lets you browse with arrow keys and Enter without typing.
  (let [folder (gum (into ["filter" "--fuzzy" "--strict"
                           "--header" "Task folder (type to filter, or use arrow keys to browse)"]
                          folders))]
    (show-completed "Task folder" (str "tasks/" folder))
    folder))

(defn subject-input []
  (loop []
    (let [subject (gum ["input" "--header" "Subject (max 64 characters)"
                        "--prompt" "> " "--char-limit" "64"])]
      (if (and (not (str/blank? subject)) (<= (count subject) 64))
        (show-completed "Subject" subject)
        (do (println "Subject must be between 1 and 64 characters.")
            (flush)
            (recur))))))

(defn body-input [type]
  (loop []
    (let [header (if (= type "question")
                   "Question body (exactly one ?)"
                   "Body (optional)")
          body (gum ["write" "--header" header "--placeholder" "Markdown body"])]
      (if (or (not= type "question") (= 1 (count (re-seq #"\?" body))))
        (show-completed "Body" body true)
        (do (println "A question body must contain exactly one question mark.")
            (flush)
            (recur))))))

(defn form [folders]
  (println "New TaskDown message (Ctrl-C to cancel)")
  (flush)
  (let [task (pick-task-folder folders)
        subject (subject-input)
        type (choose "Type" ["task" "question" "answer"])
        status (when (= type "task") (choose "Status" statuses))
        body (body-input type)]
    (when (zero? (:exit @(p/process ["gum" "confirm"
                                    (str "Create " type " message in tasks/" task
                                         " (" subject ")?")]
                                    {:in :inherit :err :inherit :out :inherit})))
      (cond-> ["--task" task "--subject" subject "--type" type]
        status (into ["--status" status])
        (not (str/blank? body)) (into ["--body" body])))))

(defn -main []
  (when-not (fs/which "gum")
    (binding [*out* *err*] (println "gum is required: https://github.com/charmbracelet/gum"))
    (System/exit 1))
  (let [folders (task-folders)]
    (when (empty? folders)
      (binding [*out* *err*] (println "No task folders found under tasks/."))
      (System/exit 1))
    (let [flags (try (form folders)
                     (catch clojure.lang.ExceptionInfo e
                       (if (:cancelled (ex-data e)) nil (throw e))))]
      (if flags
        (let [{:keys [exit]} @(p/process (into ["./scripts/td" "message" "new"] flags)
                                         {:dir (str root) :inherit true})]
          (System/exit exit))
        (println "Cancelled.")))))

(-main)
