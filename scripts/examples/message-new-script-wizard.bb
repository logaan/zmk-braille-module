#!/usr/bin/env bb
;; Run: ./scripts/examples/message-new-script-wizard.bb
;; Requires Babashka; the script-wizard pod (v0.3.0) is loaded from the pod registry.
;; Set EDITOR (or VISUAL) to edit the complete Markdown message before confirming.
;; Escape/Ctrl-C cancels without creating a message.

(require '[babashka.fs :as fs]
         '[babashka.process :as p]
         '[babashka.pods :as pods]
         '[clj-yaml.core :as yaml]
         '[clojure.string :as str])
(import '[java.time ZonedDateTime]
        '[java.time.format DateTimeFormatter]
        '[java.util Locale])

(pods/load-pod 'enigmacurry/script-wizard "0.3.0")
(require '[pod.enigmacurry.script-wizard :as sw])

(def root (-> *file* fs/absolutize fs/parent fs/parent fs/parent))
(def statuses ["draft" "ready" "started" "blocked" "delivered" "done" "cancelled"])
(def created-formatter (DateTimeFormatter/ofPattern "EEE dd MMM HH'h'mm" Locale/ENGLISH))

(defn task-folders []
  (->> (fs/glob (fs/path root "tasks") "**")
       (filter fs/directory?)
       (map #(str (fs/relativize (fs/path root "tasks") %)))
       sort vec))

(defn subject-input []
  (loop []
    (let [subject (sw/ask "Subject (max 64 characters)")]
      (if (and (not (str/blank? subject)) (<= (count subject) 64))
        subject
        (do (println "Subject must be between 1 and 64 characters.")
            (recur))))))

(defn draft-content [subject type status]
  (let [frontmatter (array-map
                     :created (.format (ZonedDateTime/now) created-formatter)
                     :author "user"
                     :subject subject
                     :cargo (cond-> (array-map :type type)
                              status (assoc :status status)))]
    (str "---\n" (yaml/generate-string frontmatter :dumper-options {:flow-style :block})
         "---\n\n# " subject "\n\n\n")))

(defn edit-draft [draft]
  ;; The pod's editor captures stdout, hiding terminal editors such as vim.
  ;; Run the user's editor with all three streams attached to the terminal.
  (let [editor (or (first (remove str/blank? [(System/getenv "EDITOR")
                                              (System/getenv "VISUAL")]))
                   "vi")
        {:keys [exit]} @(p/process ["/bin/sh" "-c" (str "exec " editor " \"$1\"")
                                     "editor" (str draft)]
                                    {:in :inherit :out :inherit :err :inherit})]
    (when-not (zero? exit)
      (throw (ex-info (str "Editor exited with status " exit) {:exit exit})))))

(defn confirm-create []
  ;; The wrapper calls System/exit on Escape, which would leave our temp file behind.
  (try (sw/confirm* "Create this message?" {"default" "no"})
       (catch Exception e
         (if (= "canceled" (ex-message e)) false (throw e)))))

(defn form [folders]
  (println "New TaskDown message (Escape/Ctrl-C to cancel)")
  (let [task (sw/choose "Task folder (type to filter)" folders)
        subject (subject-input)
        type (sw/choose "Message type" ["task" "question" "answer"])
        status (when (= type "task") (sw/choose "Status" statuses))
        draft (fs/create-temp-file {:prefix "taskdown-message-" :suffix ".md"
                                    :posix-file-permissions "rw-------"})
        keep? (atom false)]
    (try
      (spit (str draft) (draft-content subject type status))
      (println (str "Editing message in " draft " (save and quit to continue)."))
      (flush)
      (edit-draft draft)
      (println (str "\nMessage to create in tasks/" task ":"))
      (let [content (slurp (str draft))]
        (print content)
        (when-not (str/ends-with? content "\n") (println))
        (flush))
      (if (confirm-create)
        (let [{:keys [exit]} @(p/process ["./scripts/td" "message" "new"
                                          "--task" task "--file" (str draft)]
                                         {:dir (str root) :inherit true})]
          (when-not (zero? exit) (reset! keep? true))
          exit)
        (do (println "Cancelled.") 0))
      (catch Exception e
        (reset! keep? true)
        (binding [*out* *err*] (println (.getMessage e)))
        1)
      (finally
        (if @keep?
          (binding [*out* *err*] (println (str "Draft kept at " draft)))
          (fs/delete-if-exists draft))))))

(defn -main []
  (let [folders (task-folders)]
    (when (empty? folders)
      (binding [*out* *err*] (println "No task folders found under tasks/."))
      (System/exit 1))
    (System/exit (form folders))))

(-main)
