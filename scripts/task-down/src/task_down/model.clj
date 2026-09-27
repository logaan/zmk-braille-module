(ns task-down.model
  "Pure navigation model for the three-pane TaskDown TUI: state maps in, state
  maps out. No I/O, no atoms, no side effects.")

(def ^:private panes [:parents :listing :messages]) ; left to right, focus wraps
(def ^:private noop ::noop) ; a direction with nothing to move to

(defn- has-children? [node] (seq (:task/children node)))

(defn- parent-path
  "The index path of a path's parent; [] for a depth 0 path."
  [path]
  (vec (butlast path)))

(defn- node-at-path
  "The node at an index path into a task tree, or nil when there is none."
  [node path]
  (reduce (fn [n i] (nth (:task/children n) i nil)) node path))

(defn- cursor-node
  "The node a cursor points at; :heading points at fallback."
  [vault cursor fallback]
  (if (= :heading cursor) fallback (or (node-at-path vault cursor) fallback)))

(defn- heading-row [node] {:row/kind :heading :row/node node})

(defn- message-row [message expanded?]
  {:row/kind :message :row/message message :row/expanded? expanded?})

(defn- message-rows
  "Message rows for a node, oldest first, only the newest expanded."
  [node]
  (let [messages (:task/messages node)
        newest (dec (count messages))]
    (mapv (fn [i m] (message-row m (= i newest))) (range) messages)))

(defn- task-rows
  "Pre-order rows for the children of node that satisfy keep?, whose own
  children are pruned the same way."
  [node path depth keep?]
  (reduce (fn [acc [i child]]
            (if (keep? child)
              (let [p (conj path i)
                    row {:row/kind :task :row/node child
                         :row/path p :row/depth depth}]
                (into acc (cons row (task-rows child p (inc depth) keep?))))
              acc))
          []
          (map-indexed vector (:task/children node))))

(defn- pane-rows
  "Item rows under a heading; task paths start at the ancestor's own path."
  [pane node base]
  (cond
    (= :parents pane) (task-rows node base 0 has-children?)
    (= :listing pane) (task-rows node base 0 (constantly true))
    :else (message-rows node)))

(defn- row-cursor
  "The cursor value that addresses a row."
  [row]
  (cond
    (= :task (:row/kind row)) (:row/path row)
    (= :message (:row/kind row)) (get-in row [:row/message :msg/id])
    :else :heading))

(defn- index-of
  "Index of the first value equal to target, or nil."
  [values target]
  (first (keep-indexed (fn [i v] (when (= v target) i)) values)))

(defn- offset-cursor
  "The cursor offset positions from cursor in values, or nil."
  [values cursor offset]
  (let [i (index-of values cursor)
        j (when i (+ i offset))]
    (when (and j (< -1 j (count values))) (nth values j))))

(defn- sibling-row?
  "True when two rows are task rows sharing a parent."
  [a b]
  (and (= :task (:row/kind a))
       (= :task (:row/kind b))
       (= (parent-path (:row/path a)) (parent-path (:row/path b)))))

(defn- peer-cursors
  "The cursors a sibling step moves between: every row of the messages pane,
  or the siblings of a task row. A task pane heading has no peers, and a step
  that finds no peer falls back to visual movement."
  [rows index pane]
  (if (= :messages pane)
    (mapv row-cursor rows)
    (mapv row-cursor (filter #(sibling-row? % (nth rows index)) rows))))

(defn- first-child-row
  "The first displayed child row of the task row at index, or nil."
  [rows index]
  (let [path (:row/path (nth rows index))
        child? #(and (= :task (:row/kind %))
                     (= (parent-path (:row/path %)) path))]
    (first (filter child? (drop (inc index) rows)))))

(defn- visual-cursor
  "The cursor of the row step rows from index in flat row order, heading
  included; noop past either end. Follows rows, not the raw tree, so pruned
  siblings are stepped over."
  [rows index step]
  (let [row (nth rows (+ index step) nil)]
    (if row (row-cursor row) noop)))

(defn- vertical-cursor
  "The cursor after moving step rows among a row's peers, or - when no peer
  is there - the visual row step rows away: the sibling fallback."
  [rows index pane step]
  (let [cursor (row-cursor (nth rows index))
        peer (offset-cursor (peer-cursors rows index pane) cursor step)]
    (or peer (visual-cursor rows index step))))

(defn- left-cursor
  "The cursor of a row's parent; :heading for a depth 0 task or a message."
  [rows index]
  (let [row (nth rows index)
        path (:row/path row)]
    (cond
      (= :heading (:row/kind row)) noop
      (< 1 (count path)) (parent-path path)
      :else :heading)))

(defn- right-cursor
  "The cursor of a row's first child; the first row below a heading."
  [rows index]
  (let [child (if (= :heading (:row/kind (nth rows index)))
                (nth rows 1 nil)
                (first-child-row rows index))]
    (if child (row-cursor child) noop)))

(defn ancestor
  "The node a pane is rooted at: the vault root for :parents, the node under
  the parents cursor for :listing, the node under the listing cursor for
  :messages. A heading cursor means the pane's own ancestor."
  [state pane]
  (let [vault (:state/vault state)
        cursors (:state/cursors state)]
    (cond
      (= :parents pane) vault
      (= :listing pane) (cursor-node vault (:parents cursors) vault)
      :else (cursor-node vault (:listing cursors) (ancestor state :listing)))))

(defn rows
  "The rows a pane displays; row 0 is always the heading."
  [state pane]
  (let [node (ancestor state pane)
        cursor (get-in state [:state/cursors :parents])
        base (if (and (= :listing pane) (not= :heading cursor)) cursor [])]
    (into [(heading-row node)] (pane-rows pane node base))))

(defn cursor-row-index
  "The index of the row a pane's cursor is on; 0 when it is not displayed."
  [state pane]
  (let [cursor (get-in state [:state/cursors pane])
        index (index-of (mapv row-cursor (rows state pane)) cursor)]
    (if (nil? index) 0 index)))

(defn- newest-message-cursor
  "The cursor the messages pane starts at for its current ancestor."
  [state]
  (let [messages (:task/messages (ancestor state :messages))]
    (if (empty? messages) :heading (:msg/id (last messages)))))

(defn- with-newest-message
  "Reset the messages cursor to its starting position."
  [state]
  (assoc-in state [:state/cursors :messages] (newest-message-cursor state)))

(defn- moved-cursor
  "The cursor a direction moves a pane to, or noop."
  [state pane direction]
  (let [rows (rows state pane)
        index (cursor-row-index state pane)]
    (cond
      (= :up direction) (vertical-cursor rows index pane -1)
      (= :down direction) (vertical-cursor rows index pane 1)
      (= :next direction) (visual-cursor rows index 1)
      (= :prev direction) (visual-cursor rows index -1)
      (= :left direction) (left-cursor rows index)
      (= :right direction) (right-cursor rows index)
      (= :top direction) (row-cursor (first rows))
      (= :bottom direction) (row-cursor (last rows))
      :else noop)))

(defn- with-cursor
  "Set a pane cursor, resetting the cursors of the panes it is ancestor of."
  [state pane cursor]
  (let [moved (assoc-in state [:state/cursors pane] cursor)
        listing-reset (assoc-in moved [:state/cursors :listing] :heading)]
    (cond
      (= :parents pane) (with-newest-message listing-reset)
      (= :listing pane) (with-newest-message moved)
      :else moved)))

(defn move-cursor
  "The state with a pane's cursor moved in a direction: :up :down (the
  previous and next sibling - message in the messages pane - falling back to
  the visual row when the sibling does not exist), :next :prev (the row
  visually below or above in flat row order, heading included, stepping over
  pruned siblings), :left (parent), :right (first child), :top and :bottom
  (first and last row). A direction with nothing to move to returns the state
  unchanged, as does one that lands on the cursor's own row. Moving the
  parents cursor resets the listing cursor to :heading; moving the listing
  cursor resets the messages cursor to the newest message, or :heading."
  [state pane direction]
  (let [cursor (moved-cursor state pane direction)]
    (if (or (= noop cursor) (= cursor (get-in state [:state/cursors pane])))
      state
      (with-cursor state pane cursor))))

(defn move-focus
  "The state focused on the pane to the left or right, wrapping around the
  three panes. Any other direction leaves the state unchanged."
  [state direction]
  (let [step (cond (= :left direction) -1 (= :right direction) 1 :else 0)
        index (or (index-of panes (:state/focus state)) 0)
        focus (nth panes (mod (+ index step) (count panes)))]
    (if (= focus (:state/focus state)) state (assoc state :state/focus focus))))

(defn initial-state
  "The state the app starts in: the parents pane focused, the parents and
  listing cursors on their headings, the messages cursor on the newest message
  of its ancestor, or its heading when there are none."
  [vault size]
  (with-newest-message
   {:state/vault vault
    :state/focus :parents
    :state/cursors {:parents :heading :listing :heading :messages :heading}
    :state/size size
    :state/pending-key nil}))
