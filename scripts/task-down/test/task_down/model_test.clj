(ns task-down.model-test
  "Tests for the pure navigation model.

  Every fixture here is a literal built to the shared data shapes: the model
  never reads the filesystem, so these tests never touch it either."
  (:require [clojure.test :refer [deftest is testing]]
            [task-down.model :as model]))

(def size {:rows 24 :cols 244})

(def boil
  {:msg/id "01BOIL" :msg/path "tasks/01-bake-potatoes/02-salt-water/01BOIL.md"
   :msg/created "Mon 28 Sep 2026 - 02h 00m 00s" :msg/author "user"
   :msg/subject "Salt the water" :msg/cargo {:type "task" :status "draft"}
   :msg/body "1.  Plenty of salt."})

(def wash-older
  {:msg/id "01WASHOLD"
   :msg/path "tasks/01-bake-potatoes/01-wash-potatoes/01WASHOLD.md"
   :msg/created "Mon 28 Sep 2026 - 01h 00m 00s" :msg/author "user"
   :msg/subject "Get the brush" :msg/cargo {:type "task" :status "draft"}
   :msg/body "1.  It is under the sink."})

(def wash-newer
  {:msg/id "01WASHNEW"
   :msg/path "tasks/01-bake-potatoes/01-wash-potatoes/01WASHNEW.md"
   :msg/created "Mon 28 Sep 2026 - 01h 30m 00s" :msg/author "user"
   :msg/subject "Scrub them" :msg/cargo {:type "task" :status "ready"}
   :msg/body "1.  All of the eyes."})

(def vault
  {:task/name "Tasks" :task/path "tasks" :task/messages []
   :task/children
   [{:task/name "01 Bake potatoes" :task/path "tasks/01-bake-potatoes"
     :task/messages []
     :task/children
     [{:task/name "01 Wash potatoes"
       :task/path "tasks/01-bake-potatoes/01-wash-potatoes"
       :task/messages [wash-older wash-newer] :task/children []}
      {:task/name "02 Salt water"
       :task/path "tasks/01-bake-potatoes/02-salt-water"
       :task/messages []
       :task/children
       [{:task/name "01 Boil"
         :task/path "tasks/01-bake-potatoes/02-salt-water/01-boil"
         :task/messages [boil] :task/children []}]}]}
    {:task/name "02 Set table" :task/path "tasks/02-set-table"
     :task/messages [] :task/children []}
    {:task/name "03 Serve" :task/path "tasks/03-serve"
     :task/messages []
     :task/children
     [{:task/name "01 Call everyone"
       :task/path "tasks/03-serve/01-call-everyone"
       :task/messages [] :task/children []}]}]})

(def start-cursors {:parents :heading :listing :heading :messages :heading})

(def state (model/initial-state vault size))

(defn- cursor-of
  "The cursor value that addresses a row."
  [row]
  (cond
    (= :task (:row/kind row)) (:row/path row)
    (= :message (:row/kind row)) (get-in row [:row/message :msg/id])
    :else :heading))

(defn- cursors [st pane] (mapv cursor-of (model/rows st pane)))

(defn- names [st pane] (mapv (comp :task/name :row/node) (model/rows st pane)))

(defn- depths [st pane] (mapv :row/depth (rest (model/rows st pane))))

(defn- walk
  "Moves a pane cursor through directions, one key at a time."
  [st pane & directions]
  (reduce #(model/move-cursor %1 pane %2) st directions))

(defn- cursor [st pane] (get-in st [:state/cursors pane]))

(defn- ancestor-name [st pane] (:task/name (model/ancestor st pane)))

(defn- focused
  "The focus after moving it in a direction."
  [st direction]
  (:state/focus (model/move-focus st direction)))

(deftest initial-state-uses-the-starting-positions
  (is (= {:state/vault vault
          :state/focus :parents
          :state/cursors start-cursors
          :state/size size
          :state/pending-key nil}
         state)))

(deftest ancestor-follows-the-cursors-up-the-panes
  (testing "with every cursor on a heading all three panes share the root"
    (is (= "Tasks" (ancestor-name state :parents)))
    (is (= "Tasks" (ancestor-name state :listing)))
    (is (= "Tasks" (ancestor-name state :messages))))
  (testing "the parents cursor is the listing ancestor"
    (let [st (walk state :parents :right)]
      (is (= [0] (cursor st :parents)))
      (is (= "01 Bake potatoes" (ancestor-name st :listing)))
      (is (= "Tasks" (ancestor-name st :parents)))
      (testing "a listing heading means the listing ancestor itself"
        (is (= :heading (cursor st :listing)))
        (is (= "01 Bake potatoes" (ancestor-name st :messages))))))
  (testing "the listing cursor is the messages ancestor"
    (let [st (walk (walk state :parents :right) :listing :right)]
      (is (= [0 0] (cursor st :listing)))
      (is (= "01 Wash potatoes" (ancestor-name st :messages)))
      (is (= "01 Bake potatoes" (ancestor-name st :listing))))))

(deftest parents-rows-keep-only-tasks-with-children
  (is (= [:heading [0] [0 1] [2]] (cursors state :parents)))
  (is (= ["Tasks" "01 Bake potatoes" "02 Salt water" "03 Serve"]
         (names state :parents)))
  (is (= [0 1 0] (depths state :parents)))
  (is (= vault (:row/node (first (model/rows state :parents)))))
  (testing "pruned paths stay vault relative, so 03 Serve is still index 2"
    (is (= "tasks/03-serve"
           (:task/path (:row/node (last (model/rows state :parents))))))))

(deftest listing-rows-show-the-whole-subtree
  (is (= [:heading [0] [0 0] [0 1] [0 1 0] [1] [2] [2 0]]
         (cursors state :listing)))
  (is (= [0 1 1 2 0 0 1] (depths state :listing)))
  (is (= ["Tasks" "01 Bake potatoes" "01 Wash potatoes" "02 Salt water"
          "01 Boil" "02 Set table" "03 Serve" "01 Call everyone"]
         (names state :listing)))
  (testing "under an ancestor, depths restart at 0 but paths stay absolute"
    (let [st (walk state :parents :right)]
      (is (= [:heading [0 0] [0 1] [0 1 0]] (cursors st :listing)))
      (is (= ["01 Bake potatoes" "01 Wash potatoes" "02 Salt water" "01 Boil"]
             (names st :listing)))
      (is (= [0 0 1] (depths st :listing))))))

(deftest messages-rows-show-the-task-messages-oldest-first
  (let [st (walk (walk state :parents :right) :listing :right)
        rows (model/rows st :messages)]
    (is (= [:heading "01WASHOLD" "01WASHNEW"] (cursors st :messages)))
    (is (= ["Get the brush" "Scrub them"]
           (mapv (comp :msg/subject :row/message) (rest rows))))
    (is (= [nil false true] (mapv :row/expanded? rows)))
    (is (= wash-older (:row/message (second rows))))
    (is (= "01 Wash potatoes" (:task/name (:row/node (first rows)))))
    (is (= "01 Wash potatoes" (ancestor-name st :messages))))
  (testing "a task with no messages of its own shows only its heading"
    (let [st (walk (walk state :parents :right) :listing :right :down)]
      (is (= "02 Salt water" (ancestor-name st :messages)))
      (is (= [:heading] (cursors st :messages))))))

(deftest cursor-row-index-locates-the-cursor
  (is (= 0 (model/cursor-row-index state :parents)))
  (is (= 1 (model/cursor-row-index (walk state :parents :right) :parents)))
  (is (= 3 (model/cursor-row-index (walk state :parents :bottom) :parents)))
  (is (= 7 (model/cursor-row-index (walk state :listing :bottom) :listing)))
  (is (= 2 (model/cursor-row-index
            (walk (walk state :parents :right) :listing :right) :messages)))
  (testing "a cursor that is not displayed falls back to the heading row"
    (is (= 0 (model/cursor-row-index
              (assoc-in state [:state/cursors :listing] [7 7]) :listing)))))

(deftest parents-pane-navigation
  (testing "right from the heading reaches the first row"
    (is (= [0] (cursor (walk state :parents :right) :parents))))
  (testing "down and up step between siblings, skipping pruned tasks"
    (is (= [2] (cursor (walk state :parents :right :down) :parents)))
    (is (= [0] (cursor (walk state :parents :right :down :up) :parents))))
  (testing "left goes to the parent, and to the heading from depth 0"
    (is (= :heading (cursor (walk state :parents :right :left) :parents)))
    (is (= [0] (cursor (walk state :parents :right :right :left) :parents))))
  (testing "right from a task reaches its first kept child"
    (is (= [0 1] (cursor (walk state :parents :right :right) :parents))))
  (testing "cousins are unreachable and pruned leaves have no children"
    (let [salt (walk state :parents :right :right)]
      (is (= [0 1] (cursor salt :parents)))
      (is (identical? salt (walk salt :parents :down)))
      (is (identical? salt (walk salt :parents :right)))))
  (testing "top and bottom jump to the first and last row"
    (is (= :heading (cursor (walk state :parents :top) :parents)))
    (is (= [2] (cursor (walk state :parents :bottom) :parents))))
  (testing "missing targets and unknown directions leave the state alone"
    (is (identical? state (walk state :parents :up)))
    (is (identical? state (walk state :parents :left)))
    (is (identical? state (walk state :parents :top)))
    (let [bottom (walk state :parents :bottom)]
      (is (identical? bottom (walk bottom :parents :down))))
    (is (identical? state (model/move-cursor state :parents :sideways)))))

(deftest listing-pane-navigation
  (testing "down skips a task's descendants to reach its next sibling"
    (is (= [1] (cursor (walk state :listing :right :down) :listing)))
    (is (= [0] (cursor (walk state :listing :right :down :up) :listing))))
  (testing "siblings are only the tasks with the same parent"
    (let [salt (walk state :listing :right :right :down)]
      (is (= [0 1] (cursor salt :listing)))
      (is (identical? salt (walk salt :listing :down)))))
  (testing "the last task of a subtree has no siblings of its own"
    (let [call (walk state :listing :bottom)]
      (is (= [2 0] (cursor call :listing)))
      (is (identical? call (walk call :listing :down)))))
  (testing "left climbs to the parent and then to the heading"
    (let [boil (walk state :listing :right :right :down :right)
          salt (walk boil :listing :left)
          bake (walk salt :listing :left)
          heading (walk bake :listing :left)]
      (is (= [0 1 0] (cursor boil :listing)))
      (is (= [0 1] (cursor salt :listing)))
      (is (= [0] (cursor bake :listing)))
      (is (= :heading (cursor heading :listing)))
      (is (identical? heading (walk heading :listing :left)))))
  (testing "right enters the first child but not a leaf"
    (is (= [0 0] (cursor (walk state :listing :right :right) :listing)))
    (let [table (walk state :listing :right :down :right)]
      (is (= [1] (cursor table :listing)))
      (is (identical? table (walk table :listing :right)))))
  (testing "top and bottom jump to the first and last row"
    (is (= :heading (cursor (walk state :listing :top) :listing)))
    (is (= [2 0] (cursor (walk state :listing :bottom) :listing))))
  (testing "a heading has no parent and no siblings"
    (is (identical? state (walk state :listing :left)))
    (is (identical? state (walk state :listing :up)))
    (is (identical? state (walk state :listing :down)))))

(deftest messages-pane-navigation
  (let [st (walk (walk state :parents :right) :listing :right)]
    (testing "the cursor starts on the newest message"
      (is (= "01WASHNEW" (cursor st :messages)))
      (is (= 2 (model/cursor-row-index st :messages))))
    (testing "up steps back through the messages to the heading, then stops"
      (let [older (walk st :messages :up)
            heading (walk older :messages :up)]
        (is (= "01WASHOLD" (cursor older :messages)))
        (is (= :heading (cursor heading :messages)))
        (is (identical? heading (walk heading :messages :up)))))
    (testing "down from the heading reaches the oldest message"
      (is (= "01WASHOLD" (cursor (walk st :messages :top :down) :messages))))
    (testing "down from the newest message goes nowhere"
      (is (identical? st (walk st :messages :down))))
    (testing "left and right move between the heading and the messages"
      (is (= :heading (cursor (walk st :messages :left) :messages)))
      (is (= "01WASHOLD" (cursor (walk st :messages :top :right) :messages)))
      (is (identical? st (walk st :messages :right)))
      (let [heading (walk st :messages :top)]
        (is (identical? heading (walk heading :messages :left)))))
    (testing "top and bottom jump to the first and last row"
      (is (= :heading (cursor (walk st :messages :top) :messages)))
      (is (= "01WASHNEW" (cursor (walk st :messages :bottom) :messages)))))
  (testing "a task with no messages has nowhere to go"
    (let [st (walk state :parents :right)]
      (is (= [:heading] (cursors st :messages)))
      (is (identical? st (walk st :messages :down)))
      (is (identical? st (walk st :messages :next)))
      (is (identical? st (walk st :messages :prev)))
      (is (identical? st (walk st :messages :right)))
      (is (identical? st (walk st :messages :bottom))))))

(deftest next-and-prev-follow-the-visual-row-order
  (testing "in the parents pane they step over pruned tasks"
    (is (= [0] (cursor (walk state :parents :next) :parents)))
    (is (= [0 1] (cursor (walk state :parents :next :next) :parents)))
    (is (= [2] (cursor (walk state :parents :next :next :next) :parents)))
    (is (= [0 1] (cursor (walk state :parents :next :next :next :prev)
                         :parents))))
  (testing "in the listing pane they enter children where down skips them"
    (is (= [0 0] (cursor (walk state :listing :next :next) :listing)))
    (is (= [1] (cursor (walk state :listing :right :down) :listing)))
    (is (= [0 1 0] (cursor (walk state :listing :next :next :next :next)
                           :listing))))
  (testing "prev climbs back to the heading and stops there"
    (is (= :heading (cursor (walk state :listing :next :prev) :listing)))
    (is (identical? state (walk state :listing :prev)))
    (is (identical? state (walk state :parents :prev))))
  (testing "next past the last row goes nowhere"
    (let [bottom (walk state :parents :bottom)]
      (is (= [2] (cursor bottom :parents)))
      (is (identical? bottom (walk bottom :parents :next))))))

(deftest next-and-prev-in-the-messages-pane
  (let [st (walk (walk state :parents :right) :listing :right)]
    (testing "prev walks back from the newest message to the heading"
      (is (= "01WASHOLD" (cursor (walk st :messages :prev) :messages)))
      (is (= :heading (cursor (walk st :messages :prev :prev) :messages)))
      (let [heading (walk st :messages :prev :prev)]
        (is (identical? heading (walk heading :messages :prev)))))
    (testing "next from the newest message goes nowhere"
      (is (identical? st (walk st :messages :next))))
    (testing "from the heading next reaches the oldest message"
      (is (= "01WASHOLD"
             (cursor (walk st :messages :prev :prev :next) :messages))))))

(deftest next-and-prev-reset-the-panes-below-their-own
  (testing "moving the parents cursor with next resets listing and messages"
    (let [deep (walk (walk (walk state :parents :right) :listing :right)
                     :messages :prev)
          reset (walk deep :parents :next)]
      (is (= "01WASHOLD" (cursor deep :messages)))
      (is (= [0 1] (cursor reset :parents)))
      (is (= :heading (cursor reset :listing)))
      (is (= :heading (cursor reset :messages)))))
  (testing "moving the listing cursor with next resets the messages cursor"
    (let [wash (walk (walk state :parents :right) :listing :right)
          older (walk wash :messages :prev)
          salt (walk older :listing :next)
          boil (walk salt :listing :next)]
      (is (= "01WASHNEW" (cursor wash :messages)))
      (is (= "01WASHOLD" (cursor older :messages)))
      (is (= [0 1] (cursor salt :listing)))
      (is (= :heading (cursor salt :messages)))
      (is (= [0 1 0] (cursor boil :listing)))
      (is (= "01BOIL" (cursor boil :messages)))))
  (testing "moving the messages cursor with prev leaves the rest alone"
    (let [st (walk (walk state :parents :right) :listing :right)
          moved (walk st :messages :prev)]
      (is (= [0] (cursor moved :parents)))
      (is (= [0 0] (cursor moved :listing)))
      (is (= "01WASHOLD" (cursor moved :messages))))))

(deftest moving-an-ancestor-cursor-resets-the-panes-below-it
  (testing "moving the parents cursor resets the listing and messages cursors"
    (let [deep (walk (walk (walk state :parents :right) :listing :right)
                     :messages :up)
          reset (walk deep :parents :down)]
      (is (= "01WASHOLD" (cursor deep :messages)))
      (is (= [2] (cursor reset :parents)))
      (is (= :heading (cursor reset :listing)))
      (is (= :heading (cursor reset :messages)))))
  (testing "moving the listing cursor resets the messages cursor"
    (let [wash (walk (walk state :parents :right) :listing :right)
          older (walk wash :messages :up)
          salt (walk older :listing :down)
          boil (walk salt :listing :right)]
      (is (= "01WASHNEW" (cursor wash :messages)))
      (is (= "01WASHOLD" (cursor older :messages)))
      (is (= :heading (cursor salt :messages)))
      (is (= "01BOIL" (cursor boil :messages)))))
  (testing "moving the messages cursor leaves everything else alone"
    (let [st (walk (walk state :parents :right) :listing :right)
          moved (walk st :messages :up)]
      (is (= [0] (cursor moved :parents)))
      (is (= [0 0] (cursor moved :listing)))
      (is (= "01WASHOLD" (cursor moved :messages)))
      (is (= vault (:state/vault moved)))
      (is (= size (:state/size moved))))))

(deftest an-empty-vault-has-nothing-to-navigate
  (let [bare {:task/name "Tasks" :task/path "tasks"
              :task/messages [] :task/children []}
        st (model/initial-state bare size)]
    (is (= [:heading] (cursors st :parents)))
    (is (= [:heading] (cursors st :listing)))
    (is (= [:heading] (cursors st :messages)))
    (is (= 0 (model/cursor-row-index st :listing)))
    (is (identical? st (walk st :parents :right)))
    (is (identical? st (walk st :listing :bottom)))
    (is (identical? st (walk st :messages :down)))))

(deftest move-focus-wraps-around-the-panes
  (is (= :listing (focused state :right)))
  (is (= :messages (focused state :left)))
  (is (= :parents (focused (assoc state :state/focus :messages) :right)))
  (is (= :messages (focused (assoc state :state/focus :parents) :left)))
  (is (= :parents (focused (assoc state :state/focus :listing) :left)))
  (testing "moving focus leaves the cursors alone"
    (is (= (:state/cursors state)
           (:state/cursors (model/move-focus state :right)))))
  (testing "any other direction leaves the focus alone"
    (is (= :parents (focused state :down)))
    (is (identical? state (model/move-focus state :sideways)))))
