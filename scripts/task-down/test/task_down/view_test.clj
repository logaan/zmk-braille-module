(ns task-down.view-test
  "Tests for the pure pane view. Rows are inline literals (Architecture 6.3)
  and the model is replaced by them, so these tests describe the view alone and
  hold against any task-down.model that honours Architecture 4.2."
  (:require [clojure.set :as set]
            [clojure.string :as str]
            [clojure.test :refer [deftest is]]
            [task-down.model :as model]
            [task-down.view :as view]))

(def colours "Colours the terminal accepts (Architecture 3.6)."
  #{:black :white :red :green :yellow :blue :magenta :cyan :default})
(def styles "Styles the terminal accepts (Architecture 3.6)."
  #{:bold :underline :reverse})
(def test-state "App state; only :state/focus and :state/size reach the view."
  {:state/vault nil :state/focus :parents
   :state/cursors {:parents :heading :listing :heading :messages :heading}
   :state/size {:rows 24 :cols 244} :state/pending-key nil})
(def size "Terminal size tall enough that no pane under test needs to scroll."
  {:rows 500 :cols 244})
(def msg-a "A message literal, collapsed by default."
  {:msg/id "01MSGAAAAAAAAAAAAAAAAAAAAAA"
   :msg/path "tasks/01-bake-potatoes/01MSGAAAAAAAAAAAAAAAAAAAAAA.md"
   :msg/created "Mon 28 Sep 2026 - 00h 07m 57s" :msg/author "user"
   :msg/subject "Create a read-only three-pane TaskDown TUI"
   :msg/cargo {"type" "task" "status" "draft"}
   :msg/body "# Create a read-only three-pane TaskDown TUI\n\n1.  Do a thing."})
(def msg-b "A second, older message literal."
  (assoc msg-a :msg/id "01MSGBBBBBBBBBBBBBBBBBBB" :msg/subject "Bake the potatoes"
         :msg/created "Sun 27 Sep 2026 - 13h 23m 00s" :msg/body "Just bake them."))
(def long-name "A task name too long for a single pane line."
  (str/join " " (repeat 21 "word")))

(defn- node "Literal task node named `name` at `path`." [name path]
  {:task/name name :task/path path :task/children [] :task/messages []})

(defn- heading-row "Literal heading row for a node named `name`." [name]
  {:row/kind :heading :row/node (node name "tasks")})

(defn- task-row "Literal task row `name`, `depth` generations down."
  ([name depth] (task-row name depth [0]))
  ([name depth path]
   {:row/kind :task :row/node (node name (str "tasks/" name))
    :row/path path :row/depth depth}))

(defn- message-row "Literal message row carrying `msg`."
  ([msg] (message-row msg false))
  ([msg expanded?]
   {:row/kind :message :row/message msg :row/expanded? expanded?}))

(def sample-rows "A pane holding one of every row kind."
  [(heading-row "01 Bake potatoes") (task-row "02 Peel them" 0)
   (task-row "03 Mash them" 1) (message-row msg-a) (message-row msg-b true)])

(defn- render-literals
  "Draw ops for `state`, the model replaced by literal `rows` and literal cursor
  row `indexes`, both keyed by pane."
  [state rows indexes]
  (with-redefs [model/rows (fn [_ pane] (get rows pane))
                model/cursor-row-index (fn [_ pane] (get indexes pane 0))
                model/ancestor (fn [_ _] (node "Tasks" "tasks"))]
    (view/render state)))

(defn- ops-in-pane "Those of `ops` drawn inside `pane`." [ops pane]
  (let [lo (view/pane-offsets pane)]
    (filterv #(<= lo (:x %) (+ lo view/pane-width -1)) ops)))

(defn- render-pane
  "Draw ops of `pane` alone, from literal `rows` with its cursor on row `index`,
  focus on `focus` and a terminal of `size`."
  ([pane rows index focus] (render-pane pane rows index focus size))
  ([pane rows index focus size]
   (ops-in-pane (render-literals (assoc test-state :state/focus focus
                                        :state/size size)
                                 {pane rows} {pane index})
                pane)))

(defn- texts "Texts of `ops`, with cursor padding trimmed off the right." [ops]
  (mapv #(str/trimr (:text %)) ops))

(defn- listing-of "Heading plus `n` depth-zero task rows, Task 0 upwards." [n]
  (into [(heading-row "Tasks")] (map #(task-row (str "Task " %) 0 [%])) (range n)))

(defn- row-text "Every character a row is built out of." [row]
  (case (:row/kind row)
    (:heading :task) (str (:task/name (:row/node row)))
    :message (str/join " " (vals (:row/message row)))
    ""))

(deftest wrap-packs-words-and-keeps-their-whitespace
  (is (= ["aa bb" "cc"] (view/wrap 5 0 "aa bb cc")))
  (is (= ["1.  Do a thing."] (view/wrap 80 0 "1.  Do a thing."))
      "the raw markdown body keeps its double spaces")
  (is (= [""] (view/wrap 80 4 "")) "an expanded message's blank line stays blank"))

(deftest wrap-splits-words-longer-than-the-width
  (is (= ["abcdefghij" "klmnop"] (view/wrap 10 0 "abcdefghijklmnop")))
  (is (= ["  abcdefgh" "  ijkl"] (view/wrap 10 2 "abcdefghijkl"))))

(deftest wrap-counts-indentation-toward-the-width-and-repeats-it
  (let [lines (view/wrap 10 4 "aa bb cc")]
    (is (= ["    aa bb" "    cc"] lines))
    (is (every? #(<= (count %) 10) lines))))

(deftest every-draw-op-is-a-positioned-styled-string
  (let [ops (render-literals test-state {:parents sample-rows} {:parents 1})]
    (is (vector? ops))
    (is (every? #(= #{:x :y :text :fg :styles} (set (keys %))) ops))
    (is (every? #(contains? colours (:fg %)) ops))
    (is (every? #(set/subset? (:styles %) styles) ops))))

(deftest headings-are-uppercase-and-centred-on-row-zero
  (is (= [{:x 38 :y 0 :text "TASKS" :fg :white :styles #{:bold}}]
         (render-pane :parents [(heading-row "Tasks")] 9 :parents))
      "x is 1 + (80 - 5) / 2, centred in the left pane")
  (let [rows [(heading-row "01 Bake potatoes")]
        op   (first (render-pane :listing rows 9 :listing))]
    (is (= 114 (:x op)) "82 + (80 - 16) / 2, centred in the middle pane")
    (is (= "01 BAKE POTATOES" (:text op)))))

(deftest each-pane-renders-at-its-own-column-offset
  (let [panes   {:parents sample-rows :listing sample-rows :messages sample-rows}
        nowhere {:parents 9 :listing 9 :messages 9}
        ops     (render-literals test-state panes nowhere)
        grouped (map #(ops-in-pane ops %) view/pane-order)]
    (is (= 27 (count ops)) "three panes, nine display lines each")
    (is (= [1 82 163] (mapv (comp :x second) grouped)))
    (is (every? #(= (range 9) (mapv :y %)) grouped)
        "the heading sits on row 0 and content runs below it")))

(deftest task-colour-cycles-with-generation-depth
  (let [rows (into [(heading-row "Tasks")]
                   (map #(task-row (str "0" % " Task") %) (range 8)))
        fgs  (mapv :fg (rest (render-pane :listing rows 0 :messages)))]
    (is (= [:red :green :yellow :blue :magenta :cyan :red :green] fgs))))

(deftest task-rows-indent-two-columns-per-generation
  (let [rows [(heading-row "Tasks") (task-row "01 Bake potatoes" 0)
              (task-row "02 Peel them" 1) (task-row "03 Mash them" 2)]]
    (is (= ["01 Bake potatoes" "  02 Peel them" "    03 Mash them"]
           (texts (rest (render-pane :listing rows 0 :messages)))))))

(deftest task-names-wrap-at-the-pane-width-keeping-their-indent
  (let [name (str/join " " (repeat 16 "word"))
        rows [(heading-row "Tasks") (task-row name 2)]
        ops  (rest (render-pane :listing rows 0 :messages))]
    (is (= [(str "    " (str/join " " (repeat 15 "word"))) "    word"]
           (texts ops))
        "the four column indent eats into the width, so fifteen words fit")
    (is (every? #(<= (count (:text %)) 80) ops))))

(deftest the-cursor-row-is-reversed-when-focused-and-underlined-when-not
  (let [rows    [(heading-row "Tasks") (task-row "01 Bake potatoes" 0)]
        focused (second (render-pane :parents rows 1 :parents))
        blurred (second (render-pane :parents rows 1 :messages))]
    (is (= #{:reverse} (:styles focused)))
    (is (= #{:underline} (:styles blurred)))
    (is (= :red (:fg focused) (:fg blurred)) "the depth colour survives")
    (is (= [80 80] (mapv #(count (:text %)) [focused blurred]))
        "the highlight spans the whole pane")))

(deftest the-highlight-covers-every-wrapped-line-of-the-cursor-row
  (let [rows [(heading-row "Tasks") (task-row long-name 0)]
        ops  (rest (render-pane :listing rows 1 :listing))]
    (is (= 2 (count ops)))
    (is (every? #(= #{:reverse} (:styles %)) ops))
    (is (every? #(= 80 (count (:text %))) ops))))

(deftest panes-scroll-only-when-the-cursor-row-would-leave-them
  (let [small {:rows 6 :cols 244}
        rows  (listing-of 40)
        top   (render-pane :listing rows 3 :listing small)
        down  (render-pane :listing rows 30 :listing small)]
    (is (= ["TASKS" "Task 0" "Task 1" "Task 2" "Task 3" "Task 4"] (texts top)))
    (is (= ["TASKS" "Task 25" "Task 26" "Task 27" "Task 28" "Task 29"]
           (texts down)))
    (is (= #{:reverse} (:styles (last down))) "the cursor sits last")
    (is (= [0 0] (mapv (comp :y first) [top down])) "the heading stays pinned")))

(deftest scrolling-fits-every-wrapped-line-of-the-cursor-row
  (let [rows [(heading-row "Tasks") (task-row long-name 0)]
        ops  (rest (render-pane :listing rows 1 :listing {:rows 3 :cols 244}))]
    (is (= 2 (count ops)))
    (is (every? #(= #{:reverse} (:styles %)) ops))))

(deftest a-collapsed-message-shows-its-datetime-and-subject
  (let [rows [(heading-row "01 Bake potatoes") (message-row msg-a)]
        op   (second (render-pane :messages rows 0 :messages))]
    (is (= "Mon 28 Sep - 00h 07m 57s  Create a read-only three-pane TaskDown TUI"
           (:text op))
        "the year token is gone and two spaces join subject to datetime")
    (is (= :default (:fg op)))
    (is (= #{} (:styles op)))))

(deftest an-expanded-message-shows-subject-author-created-then-its-raw-body
  (let [rows [(heading-row "01 Bake potatoes") (message-row msg-a true)]
        ops  (rest (render-pane :messages rows 0 :messages))]
    (is (= ["Create a read-only three-pane TaskDown TUI" "user"
            "Mon 28 Sep 2026 - 00h 07m 57s" ""
            "# Create a read-only three-pane TaskDown TUI" "" "1.  Do a thing."]
           (mapv :text ops)))
    (is (= #{:bold} (:styles (first ops))))
    (is (= :white (:fg (first ops))))))

(deftest an-agent-author-renders-as-a-single-line
  (let [author-line (fn [author]
                      (let [msg  (assoc msg-a :msg/author author)
                            rows [(heading-row "T") (message-row msg true)]]
                        (nth (texts (render-pane :messages rows 0 :messages)) 2)))]
    (is (= "view-01 pi qwen/qwen3.8-max-0902"
           (author-line {:type "agent" :harness "pi" :name "view-01"
                         :model "qwen/qwen3.8-max-0902"})))
    (is (= "view-01 pi qwen/qwen3.8-max-0902"
           (author-line {"type" "agent" "harness" "pi" "name" "view-01"
                         "model" "qwen/qwen3.8-max-0902"})))))

(deftest empty-panes-render-just-a-heading
  (is (= ["TASKS"] (texts (render-pane :messages [(heading-row "Tasks")]
                                       0 :listing)))
      "Tasks itself has no messages to show")
  (let [ops (render-literals test-state {} {})]
    (is (= [1 82 163] (mapv :x ops)))
    (is (= ["TASKS" "TASKS" "TASKS"] (texts ops))
        "a pane the model leaves empty falls back to its ancestor heading")))

(deftest nothing-but-data-and-spaces-reaches-the-screen
  (let [panes   {:parents sample-rows :listing sample-rows :messages sample-rows}
        indexes {:parents 1 :listing 2 :messages 4}
        drawn   (map #(str/replace (:text %) #"\s" "")
                     (render-literals test-state panes indexes))
        shown   (str/upper-case (str/join "" drawn))
        data    (str/upper-case (str/join "" (map row-text sample-rows)))]
    (is (seq shown))
    (is (every? #(str/includes? data (str %)) shown))
    (is (nil? (re-find #"[\u2500-\u257F\u2580-\u259F]" shown))
        "no box drawing or block characters anywhere")))
