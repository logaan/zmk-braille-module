#!/usr/bin/env bb
;; Run: ./scripts/examples/message-new-jline.bb
;; Requires fzf to select a task folder; JLine provides line editing,
;; Tab-only type/status selection, and multiline body input (Ctrl-J for newline).
;; td's command is `message new` (singular); td supplies author=user and created=now.

(require '[babashka.fs :as fs]
         '[babashka.process :as p]
         '[clojure.string :as str])
(import '[org.jline.keymap KeyMap]
        '[org.jline.reader EndOfFileException LineReader LineReaderBuilder
           Reference UserInterruptException Widget]
        '[org.jline.terminal TerminalBuilder]
        '[org.jline.utils AttributedStringBuilder AttributedStyle])

(def root (-> *file* fs/absolutize fs/parent fs/parent fs/parent))
(def types ["task" "question" "answer"])
(def statuses ["draft" "ready" "started" "blocked" "delivered" "done" "cancelled"])

(def title-style (-> AttributedStyle/DEFAULT (.foreground AttributedStyle/MAGENTA) (.bold)))
(def label-style (-> AttributedStyle/DEFAULT (.foreground AttributedStyle/CYAN) (.bold)))
(def value-style (-> AttributedStyle/DEFAULT (.foreground AttributedStyle/GREEN) (.bold)))
(def warning-style (-> AttributedStyle/DEFAULT (.foreground AttributedStyle/YELLOW) (.bold)))
(def error-style (-> AttributedStyle/DEFAULT (.foreground AttributedStyle/RED) (.bold)))

(defn color? [terminal]
  (and (nil? (System/getenv "NO_COLOR"))
       (not= "dumb" (.getType terminal))))

(defn styled [terminal style text]
  (if (color? terminal)
    (.toAnsi (.toAttributedString (doto (AttributedStringBuilder.)
                                     (.style style)
                                     (.append (str text)))) terminal)
    (str text)))

(defn task-folders []
  (->> (fs/glob (fs/path root "tasks") "**")
       (filter fs/directory?)
       (map #(str (fs/relativize (fs/path root "tasks") %)))
       sort vec))

(defn pick-task-folder [terminal folders]
  ;; fzf reads its candidates from stdin, uses the terminal for its UI, and
  ;; writes only the chosen folder to stdout.
  (let [args (cond-> ["fzf" "--height=40%" "--layout=reverse" "--border" "--no-multi"
                      "--prompt=Task folder > "
                      "--header=Type to filter; Enter to select; Esc to cancel"]
               (not (color? terminal)) (conj "--no-color"))
        {:keys [exit out]} @(p/process args {:in (str (str/join "\n" folders) "\n")
                                              :out :string :err :inherit})]
    (cond
      (zero? exit)
      (let [folder (str/replace out #"\r?\n$" "")]
        (if ((set folders) folder)
          folder
          (throw (ex-info "fzf returned an unknown task folder" {:folder folder}))))

      (#{1 130} exit) nil
      :else (throw (ex-info (str "fzf failed with exit code " exit) {:exit exit})))))

(defn choice-reader [terminal]
  (let [reader (-> (LineReaderBuilder/builder) (.terminal terminal) (.build))
        options (atom [])
        keymap (KeyMap.)]
    ;; No self-insert, editing or completion: only Tab changes the value.
    (.setNomatch keymap (Reference. LineReader/UNDEFINED_KEY))
    (.setUnicode keymap (Reference. LineReader/UNDEFINED_KEY))
    (.bind keymap (Reference. LineReader/ACCEPT_LINE) (into-array String ["\r" "\n"]))
    (.bind keymap (Reference. LineReader/INTERRUPT) (into-array String ["\u0003" "\u0004"]))
    (.put (.getWidgets reader) "next-choice"
          (reify Widget
            (apply [_]
              (let [buffer (.getBuffer reader)
                    next-index (mod (inc (.indexOf @options (str buffer))) (count @options))]
                (.clear buffer)
                (.write buffer (nth @options next-index)))
              true)))
    (.bind keymap (Reference. "next-choice") "\t")
    ;; readLine resets the active keymap to MAIN at the start of each prompt.
    (.put (.getKeyMaps reader) LineReader/MAIN keymap)
    [reader options]))

(defn ask [reader terminal prompt valid? error]
  (loop []
    (let [value (str/trim (.readLine reader (styled terminal label-style prompt)))]
      (if (valid? value)
        value
        (do (println (styled terminal error-style error)) (recur))))))

(defn choose [reader terminal choices label options]
  (reset! choices options)
  (println (str (styled terminal label-style label) "  "
                (str/join "  " (map #(styled terminal value-style %) options))
                "  (Tab to cycle; Enter to confirm)"))
  (.readLine reader (styled terminal label-style (str label ": ")) nil (first options)))

(defn body-prompt [terminal type]
  (let [reader (-> (LineReaderBuilder/builder)
                   (.terminal terminal)
                   (.variable LineReader/SECONDARY_PROMPT_PATTERN "  | ")
                   (.build))]
    ;; In JLine, Enter accepts the buffer; bind Ctrl-J to self-insert a newline.
    (.bind (get (.getKeyMaps reader) LineReader/MAIN)
           (Reference. LineReader/SELF_INSERT) "\n")
    (println (styled terminal label-style
                     (if (= type "question")
                       "Question body (one ?; Ctrl-J for newline, Enter to finish):"
                       "Body (optional; Ctrl-J for newline, Enter to finish):")))
    (if (= type "question")
      (ask reader terminal "Body: "
           #(= 1 (count (re-seq #"\?" %)))
           "A question body must contain exactly one question mark.")
      (.readLine reader (styled terminal label-style "Body: ")))))

(defn form [terminal folders]
  (println (styled terminal title-style "New TaskDown message"))
  (println "Ctrl-C or Ctrl-D to cancel.")
  (when-let [task (pick-task-folder terminal folders)]
    (let [reader (-> (LineReaderBuilder/builder) (.terminal terminal) (.build))
          [selector choices] (choice-reader terminal)
          _ (println (str (styled terminal label-style "Task folder: ")
                          (styled terminal value-style (str "tasks/" task))))
          subject (ask reader terminal "Subject: "
                       #(and (not (str/blank? %)) (<= (count %) 64))
                       "Subject must be between 1 and 64 characters.")
          type (choose selector terminal choices "Type" types)
          status (when (= type "task") (choose selector terminal choices "Status" statuses))
          body (body-prompt terminal type)]
      (println (styled terminal warning-style "\nCreate this message?"))
      (doseq [[label value] (cond-> [["Task folder" (str "tasks/" task)]
                                    ["Subject" subject]
                                    ["Type" type]]
                             status (conj ["Status" status])
                             (not (str/blank? body)) (conj ["Body" body]))]
        (println (str "  " (styled terminal label-style (str label ": "))
                      (styled terminal value-style value))))
      (when (= "y" (str/lower-case
                     (str/trim (.readLine reader (styled terminal warning-style
                                                        "Confirm [y/N]: ")))))
        (cond-> ["--task" task "--subject" subject "--type" type]
          status (into ["--status" status])
          (not (str/blank? body)) (into ["--body" body]))))))

(defn -main []
  (when-not (fs/which "fzf")
    (binding [*out* *err*] (println "fzf is required: https://github.com/junegunn/fzf"))
    (System/exit 1))
  (let [folders (task-folders)]
    (when (empty? folders)
      (binding [*out* *err*] (println "No task folders found under tasks/."))
      (System/exit 1))
    (let [flags (try
                  (with-open [terminal (-> (TerminalBuilder/builder) (.system true) (.build))]
                    (form terminal folders))
                  (catch EndOfFileException _ nil)
                  (catch UserInterruptException _ nil)
                  (catch clojure.lang.ExceptionInfo e
                    (binding [*out* *err*] (println (.getMessage e)))
                    (System/exit 1)))]
      (if flags
        (let [{:keys [exit]} @(p/process (into ["./scripts/td" "message" "new"] flags)
                                         {:dir (str root) :inherit true})]
          (System/exit exit))
        (println "Cancelled.")))))

(-main)
