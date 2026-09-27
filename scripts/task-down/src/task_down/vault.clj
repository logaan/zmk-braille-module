(ns task-down.vault
  "Reads a TaskDown vault from disk into the immutable task-tree data model.

  Shapes (Architecture 3.1 and 3.3):

    node    {:task/name     \"01 Bake potatoes\"
             :task/path     \"tasks/01-bake-potatoes\"
             :task/children [<node> ...]     ; sorted by folder name
             :task/messages [<message> ...]} ; sorted by filename ascending

    message {:msg/id      \"01M3HK39RR353S0DQZRQBBAFFJ\"
             :msg/path    \"tasks/01-x/01M3HK39RR353S0DQZRQBBAFFJ.md\"
             :msg/created \"Mon 28 Sep 2026 - 00h 07m 57s\"
             :msg/author  \"user\", or a map such as {:type \"agent\" ...}
             :msg/subject \"...\"
             :msg/cargo   {:type \"task\" :status \"ready\" ...}
             :msg/body    \"...\"}

  `:msg/author` and `:msg/cargo` are the frontmatter values exactly as
  `clj-yaml.core/parse-string` returns them, so nested keys are keywords and
  `=` against plain map literals holds.

  `load-vault` and the private `load-*` fns are the only impure code here;
  `parse-message` and `split-frontmatter` are pure and unit-testable."
  (:require [babashka.fs :as fs]
            [clj-yaml.core :as yaml]
            [clojure.string :as str]))

(def root-name
  "Display name of the conceptual vault root node."
  "Tasks")

(defn display-name
  "Task display name for the folder called `folder-name`.

  Every hyphen becomes a space, so `01-Bake-potatoes` reads as
  `01 Bake potatoes` and the leading `NN-` number is kept."
  [folder-name]
  (str/replace folder-name "-" " "))

(defn- fence?
  "True when `line` is a `---` frontmatter fence."
  [line]
  (= "---" (str/trim (or line ""))))

(defn- closing-fence
  "Index of the frontmatter closing fence in `lines`, or nil when absent."
  [lines]
  (first (keep-indexed (fn [i l] (when (and (pos? i) (fence? l)) i)) lines)))

(defn split-frontmatter
  "Splits message `text` into `[yaml body]` on its first two `---` lines.

  Returns `[nil text]` when `text` does not open with a frontmatter fence."
  [text]
  (let [lines (str/split-lines text)
        close (closing-fence lines)]
    (if (or (nil? close) (not (fence? (first lines))))
      [nil text]
      [(str/join "\n" (->> lines (take close) (drop 1)))
       (str/join "\n" (drop (inc close) lines))])))

(defn- message-id
  "The message id of the file at `path`: its filename without `.md`."
  [path]
  (-> path fs/file-name (str/replace #"(?i)\.md$" "")))

(defn parse-message
  "Parses the whole `text` of the message file at vault-relative `path`.

  Returns the message shape documented in this namespace. A file with no
  frontmatter parses to nil metadata with all of its text as the body."
  [path text]
  (let [p (str path)
        [frontmatter body] (split-frontmatter text)
        fields (when frontmatter (yaml/parse-string frontmatter))]
    {:msg/id      (message-id p)
     :msg/path    p
     :msg/created (get fields :created)
     :msg/author  (get fields :author)
     :msg/subject (get fields :subject)
     :msg/cargo   (get fields :cargo)
     :msg/body    (str/trim (or body ""))}))

(defn- junk?
  "True for entries the vault ignores, such as `.DS_Store` or `.git`."
  [path]
  (str/starts-with? (fs/file-name path) "."))

(defn- message-file?
  "True for the `*.md` files that hold messages."
  [path]
  (and (fs/regular-file? path)
       (= "md" (str/lower-case (or (fs/extension path) "")))))

(defn- child-dirs
  "Subtask directories of `dir`, sorted by folder name."
  [dir]
  (->> (fs/list-dir dir)
       (filter fs/directory?)
       (remove junk?)
       (sort-by fs/file-name)
       vec))

(defn- message-files
  "Message files in `dir`, sorted by filename ascending."
  [dir]
  (->> (fs/list-dir dir)
       (filter message-file?)
       (remove junk?)
       (sort-by fs/file-name)
       vec))

(defn- load-message
  "Reads the message file at `path` into a message map."
  [path]
  (let [p (str path)]
    (parse-message p (slurp p))))

(defn- load-task
  "Reads the task directory `dir` into a task node, including its subtasks."
  [dir]
  {:task/name     (display-name (fs/file-name dir))
   :task/path     (str dir)
   :task/children (mapv load-task (child-dirs dir))
   :task/messages (mapv load-message (message-files dir))})

(defn load-vault
  "Reads the TaskDown vault rooted at `dir` and returns its root node.

  The root is the conceptual \"Tasks\" node: its children are the top-level
  task folders under `dir` and it never has messages. Impure."
  [dir]
  (let [root (fs/path dir)]
    {:task/name     root-name
     :task/path     (str root)
     :task/children (mapv load-task (child-dirs root))
     :task/messages []}))
