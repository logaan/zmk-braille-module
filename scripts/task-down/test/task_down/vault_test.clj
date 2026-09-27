(ns task-down.vault-test
  "Tests for task-down.vault: pure message parsing plus fixture-driven loading."
  (:require [babashka.fs :as fs]
            [clojure.string :as str]
            [clojure.test :refer [deftest is testing]]
            [task-down.vault :as vault]))

(def ^:private vault-dir
  "The fake vault that every load-vault test reads."
  (fs/path "test" "fixtures" "vault"))

(def ^:private agent-message
  "A message file with a multi-line body and an agent author."
  (str "---\n"
       "created: Sat 26 Sep 2026 - 10h 30m 00s\n"
       "author:\n"
       "  type: agent\n"
       "  harness: pi\n"
       "  model: qwen/qwen3.8-max-0902\n"
       "  name: vault-01\n"
       "subject: Results\n"
       "cargo:\n"
       "  type: task\n"
       "  status: delivered\n"
       "---\n"
       "\n"
       "# Results\n"
       "\n"
       "1.  First line.\n"
       "\n"
       "    1.  Nested line.\n"
       "\n"
       "\n"))

(defn- fixture-path
  "The path of the fixture at the path segments `parts`, as the loader reports it."
  [& parts]
  (str (apply fs/path vault-dir parts)))

(defn- subtask
  "The child of `node` whose folder is called `folder`."
  [node folder]
  (first (filter #(= folder (fs/file-name (:task/path %)))
                 (:task/children node))))

(defn- msg-ids
  "The ids of the messages of `node`, in load order."
  [node]
  (mapv :msg/id (:task/messages node)))

(deftest split-frontmatter-test
  (is (= ["a: 1" "body"] (vault/split-frontmatter "---\na: 1\n---\nbody")))
  (is (= [nil "no frontmatter"] (vault/split-frontmatter "no frontmatter")))
  (is (= [nil ""] (vault/split-frontmatter "")))
  (is (= [nil "---\na: 1"] (vault/split-frontmatter "---\na: 1"))))

(deftest parse-message-frontmatter-test
  (let [text (str "---\n"
                  "created: Mon 28 Sep 2026 - 00h 07m 57s\n"
                  "author: user\n"
                  "subject: Create a read-only three-pane TaskDown TUI\n"
                  "cargo:\n"
                  "  type: task\n"
                  "  status: ready\n"
                  "---\n")
        msg (vault/parse-message "tasks/01-x/01M3ABC.md" text)]
    (is (= "01M3ABC" (:msg/id msg)))
    (is (= "tasks/01-x/01M3ABC.md" (:msg/path msg)))
    (is (= "Mon 28 Sep 2026 - 00h 07m 57s" (:msg/created msg)))
    (is (= "Create a read-only three-pane TaskDown TUI" (:msg/subject msg)))
    (is (= {:type "task" :status "ready"} (:msg/cargo msg)))
    (testing "a user author is the plain string from the frontmatter"
      (is (= "user" (:msg/author msg))))
    (testing "a message with no body has an empty body"
      (is (= "" (:msg/body msg))))))

(deftest parse-message-agent-author-test
  (let [msg (vault/parse-message "tasks/01-x/01M3DEF.md" agent-message)]
    (is (= {:type "agent"
            :harness "pi"
            :model "qwen/qwen3.8-max-0902"
            :name "vault-01"}
           (:msg/author msg)))
    (is (= {:type "task" :status "delivered"} (:msg/cargo msg)))
    (testing "only the outer whitespace of the body is trimmed"
      (is (= "# Results\n\n1.  First line.\n\n    1.  Nested line."
             (:msg/body msg))))))

(deftest parse-message-without-frontmatter-test
  (let [msg (vault/parse-message "tasks/01-x/plain.md" "# Just markdown\n")]
    (is (= "plain" (:msg/id msg)))
    (is (= "tasks/01-x/plain.md" (:msg/path msg)))
    (is (= "# Just markdown" (:msg/body msg)))
    (is (nil? (:msg/created msg)))
    (is (nil? (:msg/author msg)))
    (is (nil? (:msg/subject msg)))
    (is (nil? (:msg/cargo msg)))))

(deftest display-name-test
  (is (= "01 Bake potatoes" (vault/display-name "01-Bake-potatoes")))
  (is (= "08 create a read only three column taskdown tui"
         (vault/display-name "08-create-a-read-only-three-column-taskdown-tui")))
  (is (= "tasks" (vault/display-name "tasks"))))

(deftest load-vault-root-test
  (let [root (vault/load-vault (str vault-dir))]
    (is (= "Tasks" (:task/name root)))
    (is (= (str vault-dir) (:task/path root)))
    (testing "the root node never has messages"
      (is (= [] (:task/messages root))))
    (testing "children are sorted by folder name"
      (is (= ["01-bake-potatoes" "02-write-the-report"]
             (mapv #(fs/file-name (:task/path %)) (:task/children root)))))))

(deftest load-vault-display-names-test
  (let [root (vault/load-vault (str vault-dir))]
    (is (= "01 bake potatoes" (:task/name (subtask root "01-bake-potatoes"))))
    (is (= "02 write the report" (:task/name (subtask root "02-write-the-report"))))
    (is (= "02 choose a variety"
           (:task/name (subtask (subtask root "01-bake-potatoes")
                                "02-choose-a-variety"))))))

(deftest load-vault-paths-test
  (let [root (vault/load-vault (str vault-dir))
        potatoes (subtask root "01-bake-potatoes")]
    (is (= (fixture-path "01-bake-potatoes") (:task/path potatoes)))
    (is (= (fixture-path "01-bake-potatoes" "02-choose-a-variety")
           (:task/path (subtask potatoes "02-choose-a-variety"))))
    (is (= [(fixture-path "01-bake-potatoes" "01M3FIXTUREAAAAAAAAAAAAAAA.md")
            (fixture-path "01-bake-potatoes" "01M3FIXTUREBBBBBBBBBBBBBBB.md")]
           (mapv :msg/path (:task/messages potatoes))))))

(deftest load-vault-nesting-test
  (let [potatoes (subtask (vault/load-vault (str vault-dir)) "01-bake-potatoes")
        variety (subtask potatoes "02-choose-a-variety")]
    (testing "a nested subtask keeps its own messages"
      (is (= ["01M3FIXTURECCCCCCCCCCCCCCC"] (msg-ids variety)))
      (is (= "Which variety?"
             (:msg/subject (first (:task/messages variety))))))
    (testing "a --- line inside the body is not the closing fence"
      (is (str/includes? (:msg/body (first (:task/messages variety)))
                         "---\n\n2.  The rule above")))
    (is (= [] (:task/children variety)))))

(deftest load-vault-ordering-test
  (let [root (vault/load-vault (str vault-dir))]
    (testing "messages are sorted by filename ascending"
      (is (= ["01M3FIXTUREAAAAAAAAAAAAAAA" "01M3FIXTUREBBBBBBBBBBBBBBB"]
             (msg-ids (subtask root "01-bake-potatoes"))))
      (is (= ["01M3FIXTUREDDDDDDDDDDDDDDD" "01M3FIXTUREEEEEEEEEEEEEEEE"]
             (msg-ids (subtask root "02-write-the-report")))))))

(deftest load-vault-ignores-junk-test
  (let [root (vault/load-vault (str vault-dir))
        potatoes (subtask root "01-bake-potatoes")]
    (testing "the root .DS_Store is not a task"
      (is (= ["01-bake-potatoes" "02-write-the-report"]
             (mapv #(fs/file-name (:task/path %)) (:task/children root)))))
    (testing "only *.md files become messages, so notes.txt is ignored"
      (is (= ["01M3FIXTUREAAAAAAAAAAAAAAA" "01M3FIXTUREBBBBBBBBBBBBBBB"]
             (msg-ids potatoes))))
    (testing "dot-directories are not subtasks, so .hidden is ignored"
      (is (= ["02-choose-a-variety"]
             (mapv #(fs/file-name (:task/path %)) (:task/children potatoes)))))
    (testing "a task folder with no *.md and no subtasks still loads"
      (let [numbers (subtask (subtask root "02-write-the-report")
                             "01-collect-the-numbers")]
        (is (= "01 collect the numbers" (:task/name numbers)))
        (is (= [] (:task/children numbers)))
        (is (= [] (:task/messages numbers)))))))

(deftest load-vault-message-fields-test
  (let [root (vault/load-vault (str vault-dir))
        [status-change results] (:task/messages (subtask root "01-bake-potatoes"))]
    (is (= "user" (:msg/author status-change)))
    (is (= "" (:msg/body status-change)))
    (is (= {:type "agent" :harness "pi" :model "qwen/qwen3.8-max-0902" :name "baker-01"}
           (:msg/author results)))
    (is (str/starts-with? (:msg/body results) "# Results"))))
