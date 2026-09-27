(ns task-down.controller-test
  "Tests for the impure controller, driven through a fake Terminal.

  The loop is the interesting part: it must render the view, block for a key,
  apply the pure model to it, and quit cleanly - restoring the terminal even
  when the tty throws. The vault here is a literal (Architecture 6.3): nothing
  touches disk and the Lanterna pod is never loaded, because requiring
  task-down.terminal only defines the protocol."
  (:require [clojure.string :as str]
            [clojure.test :refer [deftest is testing]]
            [task-down.controller :as controller]
            [task-down.main :as main]
            [task-down.terminal :as terminal]
            [task-down.view :as view]))

;;; Fixtures: two branching tasks, one pruned leaf, two messages on one task.

(def size "The size the fake terminal reports until its script says otherwise."
  {:rows 24 :cols 244})

(defn- message "A message literal in the Architecture 3.3 shape."
  [id subject hour]
  {:msg/id id :msg/path (str "tasks/01-alpha/" id ".md")
   :msg/created (str "Mon 28 Sep 2026 - " hour "h 00m 00s")
   :msg/author "user" :msg/subject subject
   :msg/cargo {:type "task" :status "started"}
   :msg/body (str "The body of " subject ".")})

(defn- task "A task node literal in the Architecture 3.1 shape."
  [task-name path children messages]
  {:task/name task-name :task/path path
   :task/children children :task/messages messages})

(defn- leaf "A task node with neither subtasks nor messages."
  [task-name path]
  (task task-name path [] []))

(def deep (leaf "01 Deep" "tasks/01-alpha/01-alpha-one/01-deep"))
(def alpha-one (task "01 Alpha one" "tasks/01-alpha/01-alpha-one" [deep] []))
(def alpha (task "01 Alpha" "tasks/01-alpha" [alpha-one]
                 [(message "01OLD" "Old news" "01")
                  (message "01NEW" "New news" "02")]))
(def gamma (task "03 Gamma" "tasks/03-gamma"
                 [(leaf "01 Gamma one" "tasks/03-gamma/01-gamma-one")] []))
(def vault (task "Tasks" "tasks"
                 [alpha (leaf "02 Beta" "tasks/02-beta") gamma] []))

;;; The fake Terminal: scripted keys and sizes, recorded draws and calls.

(defn- next-key "The next scripted key; an exhausted script quits, so no test
  can hang on a missing q."
  [keys]
  (if (empty? @keys)
    {:key controller/quit-key}
    (let [key (first @keys)]
      (swap! keys rest)
      key)))

(defn- next-size "The next scripted size, holding the last one once the script
  runs out."
  [sizes]
  (let [value (first @sizes)]
    (when (< 1 (count @sizes)) (swap! sizes rest))
    value))

(defrecord fake-terminal [keys sizes drawn calls]
  terminal/Terminal
  (start! [_] (swap! calls conj :start))
  (stop! [_] (swap! calls conj :stop))
  (draw! [_ ops] (swap! drawn conj ops) (swap! calls conj :draw))
  (read-key! [_] (swap! calls conj :key) (next-key keys))
  (size! [_] (next-size sizes)))

(defn- fake "A fake Terminal replaying `keys`, then quitting."
  ([keys] (fake keys [size]))
  ([keys sizes] (->fake-terminal (atom keys) (atom sizes) (atom []) (atom []))))

(defn- driven "Run the controller over the fixture vault with the keys and
  sizes a test scripts; returns the fake terminal to inspect."
  ([keys] (driven keys [size]))
  ([keys sizes]
   (let [term (fake keys sizes)]
     (controller/show! term vault)
     term)))

; A Terminal whose tty dies after `budget` keys.
(defrecord throwing-terminal [budget calls]
  terminal/Terminal
  (start! [_] (swap! calls conj :start))
  (stop! [_] (swap! calls conj :stop))
  (draw! [_ _] (swap! calls conj :draw))
  (read-key! [_]
    (if (pos? (swap! budget dec))
      {:key \j}
      (throw (ex-info "The tty went away" {:terminal :throwing}))))
  (size! [_] size))

;;; Inspecting what the loop left behind.

(defn- frame "The ops of the `n`th frame the terminal drew." [term n]
  (nth @(:drawn term) n))

(defn- texts "The trimmed text of every op." [ops]
  (mapv #(str/trim (:text %)) ops))

(defn- pane-texts "The trimmed text drawn inside one pane." [ops pane]
  (let [x (get view/pane-offsets pane)]
    (texts (filter #(<= x (:x %) (+ x view/pane-width)) ops))))

(defn- op-with-text "The first op showing `text`, cursor padding aside."
  [ops text]
  (first (filter #(= text (str/trim (:text %))) ops)))

(defn- cursor "The cursor a pane ended on." [pane]
  (get-in @controller/app-state [:state/cursors pane]))

(defn- state-key "A top-level key of the state the loop ended on." [k]
  (get @controller/app-state k))

;;; Tests

(deftest seeds-the-global-atom-with-the-initial-state
  (driven [{:key \q}])
  (is (= :parents (state-key :state/focus)))
  (is (= {:parents :heading :listing :heading :messages :heading}
         (state-key :state/cursors)))
  (is (= size (state-key :state/size)))
  (is (nil? (state-key :state/pending-key))))

(deftest draws-one-frame-per-key-and-restores-the-terminal
  (let [term (driven [{:key \l} {:key \q}])]
    (is (= [:start :draw :key :draw :key :stop] @(:calls term)))
    (is (= 2 (count @(:drawn term))))))

(deftest hands-the-view-ops-straight-to-the-terminal
  (let [term (driven [{:key \q}])]
    (is (= (view/render @controller/app-state) (frame term 0)))))

(deftest shows-three-panes-of-headings-with-the-focused-cursor-reversed
  (let [ops (frame (driven [{:key \q}]) 0)
        headings (filter #(= "TASKS" (str/trim (:text %))) ops)]
    ; Cursor rows are padded to the pane width, so a heading under a cursor is
    ; drawn at its pane offset rather than centred.
    (is (= [1 82 163] (mapv :x headings)))
    (is (= [:white :white :white] (mapv :fg headings)))
    (is (= [#{:reverse} #{:underline} #{:underline}] (mapv :styles headings)))))

(deftest moving-the-parents-cursor-refocuses-the-panes-to-its-right
  (let [ops (frame (driven [{:key \l} {:key \q}]) 1)]
    (is (= "01 ALPHA" (first (pane-texts ops :listing))))
    (is (= "TASKS" (first (pane-texts ops :parents))))
    (is (= 38 (:x (op-with-text ops "TASKS")))) ; off its cursor, so centred
    (is (some #(str/includes? % "Mon 28 Sep - 01h 00m 00s  Old news")
              (pane-texts ops :messages)))
    (is (some #(str/includes? % "New news") (pane-texts ops :messages)))))

(deftest braces-step-between-siblings
  (testing "in the parents pane"
    (driven [{:key \l} {:key \}} {:key \q}])
    (is (= [2] (cursor :parents)))
    (driven [{:key \l} {:key \}} {:key \{} {:key \q}])
    (is (= [0] (cursor :parents))))
  (testing "in the listing pane they skip descendants where j enters them"
    (driven [{:key :escape} {:key \j} {:key \}} {:key \q}])
    (is (= [1] (cursor :listing))))
  (testing "without a sibling } falls back to the next visual row"
    (driven [{:key \j} {:key \j} {:key \}} {:key \q}])
    (is (= [2] (cursor :parents)))))

(deftest j-and-k-follow-the-visual-row-order
  (testing "j moves to whatever row is below: child, then a parent's sibling"
    (driven [{:key :escape} {:key \j} {:key \j} {:key \q}])
    (is (= [0 0] (cursor :listing)))
    (driven [{:key :escape} {:key \j} {:key \j} {:key \j} {:key \j} {:key \q}])
    (is (= [1] (cursor :listing))))
  (testing "k moves back up, and stops at the heading"
    (driven [{:key :escape} {:key \j} {:key \j} {:key \k} {:key \q}])
    (is (= [0] (cursor :listing)))
    (driven [{:key :escape} {:key \j} {:key \k} {:key \k} {:key \q}])
    (is (= :heading (cursor :listing))))
  (testing "in the parents pane j enters children where } skips them"
    (driven [{:key \j} {:key \j} {:key \q}])
    (is (= [0 0] (cursor :parents)))
    (driven [{:key \j} {:key \j} {:key \j} {:key \q}])
    (is (= [2] (cursor :parents)))))

(deftest h-and-l-step-between-a-task-and-its-parent
  (driven [{:key \l} {:key \l} {:key \q}])
  (is (= [0 0] (cursor :parents)))
  (driven [{:key \l} {:key \l} {:key \h} {:key \q}])
  (is (= [0] (cursor :parents)))
  (driven [{:key \l} {:key \h} {:key \q}])
  (is (= :heading (cursor :parents))))

(deftest keys-move-the-cursor-of-the-focused-pane-only
  (driven [{:key :escape} {:key \l} {:key \q}])
  (is (= [0] (cursor :listing)))
  (is (= :heading (cursor :parents))))

(deftest enter-and-escape-move-focus-and-wrap-around
  (driven [{:key :escape} {:key \q}])
  (is (= :listing (state-key :state/focus)))
  (driven [{:key :enter} {:key \q}])
  (is (= :messages (state-key :state/focus)))
  (driven [{:key :enter} {:key :enter} {:key \q}])
  (is (= :listing (state-key :state/focus))))

(deftest G-and-gg-jump-to-the-ends-of-the-focused-pane
  (testing "G goes to the last row"
    (driven [{:key \G} {:key \q}])
    (is (= [2] (cursor :parents))))
  (testing "gg goes to the first"
    (driven [{:key \G} {:key \g} {:key \g} {:key \q}])
    (is (= :heading (cursor :parents)))))

(deftest the-two-key-jump-needs-both-keys
  (testing "a single g waits for its partner"
    (driven [{:key \l} {:key \g} {:key \q}])
    (is (= :g (state-key :state/pending-key)))
    (is (= [0] (cursor :parents))))
  (testing "a navigation key cancels the wait"
    (driven [{:key \g} {:key \j} {:key \q}])
    (is (nil? (state-key :state/pending-key))))
  (testing "so does a key the controller does not know"
    (driven [{:key \l} {:key \g} {:key \x} {:key \g} {:key \q}])
    (is (= [0] (cursor :parents)))))

(deftest ignores-keys-it-does-not-know
  (let [term (driven [{:key \z} {:key :up} {:key :backspace} nil {:key \q}])]
    (is (= 5 (count @(:drawn term))))
    (is (= :heading (cursor :parents)))
    (is (= :parents (state-key :state/focus)))))

(deftest re-reads-the-terminal-size-every-turn
  (let [small {:rows 5 :cols 244}
        term (driven [{:key \j} {:key \q}] [size small])]
    (is (= small (state-key :state/size)))
    (is (every? #(< (:y %) (:rows small)) (frame term 1)))))

(deftest restores-the-terminal-even-when-the-loop-throws
  (let [term (->throwing-terminal (atom 2) (atom []))]
    (is (thrown? clojure.lang.ExceptionInfo (controller/show! term vault)))
    (is (= :start (first @(:calls term))))
    (is (= :stop (last @(:calls term))))))

(deftest main-hands-the-vault-directory-to-the-controller
  (let [seen (atom nil)]
    (with-redefs [controller/start! #(reset! seen %)]
      (main/-main "elsewhere")
      (is (= "elsewhere" @seen))
      (main/-main)
      (is (= main/default-vault-dir @seen))
      (is (= "tasks" @seen)))))
