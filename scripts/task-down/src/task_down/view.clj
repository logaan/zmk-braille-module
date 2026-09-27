(ns task-down.view
  "Pure view layer: renders TaskDown app state into draw ops.

  Three 80-column panes at x offsets 1, 82 and 163, an uppercase centred
  heading pinned on row 0 and scrolled content from row 1. Nothing here
  performs I/O; rows and cursor positions come from the pure model."
  (:require [clojure.string :as str]
            [task-down.model :as model]))

(def pane-width "Columns available to every pane." 80)
(def pane-order "Panes, left to right." [:parents :listing :messages])
(def pane-offsets "First column of each pane."
  {:parents 1 :listing 82 :messages 163})
(def heading-row "Screen row of each pane's heading; it never scrolls away." 0)
(def content-row "Screen row of a pane's first scrolled content line." 1)
(def depth-colours "Task colour per generation depth; the cycle repeats."
  [:red :green :yellow :blue :magenta :cyan])
(def cursor-styles "Cursor row styles, by whether its pane holds focus."
  {:focused #{:reverse} :blurred #{:underline}})

(defn- chunks "Split `word` into pieces of at most `width` characters."
  [width word]
  (loop [w word acc []]
    (if (<= (count w) width)
      (conj acc w)
      (recur (subs w width) (conj acc (subs w 0 width))))))

(defn- split-word "Break an over-long `word` across lines, filling this one."
  [budget lines word]
  (let [room  (- budget (count (peek lines)))
        base  (if (pos? room)
                (conj (pop lines) (str (peek lines) (subs word 0 room)))
                lines)
        parts (chunks budget (subs word (max 0 room)))]
    (conj (into base (butlast parts)) (last parts))))

(defn- add-token "Append word or whitespace `token` to the in-progress `lines`."
  [budget lines token]
  (let [line (peek lines)
        fits (<= (+ (count line) (count token)) budget)]
    (cond
      fits (conj (pop lines) (str line token))
      (re-matches #"\s+" token) (conj lines "")
      (> (count token) budget) (split-word budget lines token)
      :else (conj lines token))))

(defn wrap
  "Wrap logical `line` to `width` columns, indenting every display line by
  `indent` spaces. Indentation counts toward the width, whitespace runs inside
  the text survive, over-long words split, and an empty line yields one \"\"."
  [width indent line]
  (let [tokens (re-seq #"\S+|\s+" (str line))]
    (if (empty? tokens)
      [""]
      (let [prefix (apply str (repeat indent " "))
            budget (max 1 (- width indent))]
        (mapv #(str prefix (str/trimr %))
              (reduce (fn [ls t] (add-token budget ls t)) [""] tokens))))))

(defn- without-year "Drop the year token from a TaskDown datetime string."
  [created]
  (->> (str/split (str created) #"\s+")
       (remove #(re-matches #"\d{4}" %))
       (str/join " ")))

(defn- frontmatter-field "Value of `k` in frontmatter map `m`, string keys too."
  [m k] (or (get m k) (get m (name k))))

(defn- author-text "A message author, string or contributor map, as one line."
  [author]
  (cond
    (string? author) author
    (map? author) (str/join " " (keep #(frontmatter-field author %)
                                      [:name :harness :model]))
    :else (str author)))

(defn- tinted "Content line drawn in a single foreground colour."
  [text fg] {:line/text (str text) :line/fg fg :line/styles #{}})

(defn- strong "Content line carrying the hierarchy: white and bold."
  [text] {:line/text (str text) :line/fg :white :line/styles #{:bold}})

(defn- collapsed-text "Datetime without its year, two spaces, then subject."
  [msg] (str (without-year (:msg/created msg)) "  " (:msg/subject msg)))

(defn- expanded-lines "Subject, author, created, a blank line, then the body."
  [msg]
  (let [header [(strong (:msg/subject msg))
                (tinted (author-text (:msg/author msg)) :default)
                (tinted (str (:msg/created msg)) :default)
                (tinted "" :default)]]
    (into header (map #(tinted % :default))
          (str/split-lines (str (:msg/body msg))))))

(defn- message-lines "Styled logical lines of a message row."
  [row]
  (if (:row/expanded? row)
    (expanded-lines (:row/message row))
    [(tinted (collapsed-text (:row/message row)) :default)]))

(defn- depth-colour "Foreground colour of a task `depth` generations down."
  [depth] (nth depth-colours (mod depth (count depth-colours))))

(defn- row-lines "Styled logical lines of `row`, before wrapping."
  [row]
  (case (:row/kind row)
    :heading [(strong (str/upper-case (str (:task/name (:row/node row)))))]
    :task    [(tinted (str (:task/name (:row/node row)))
                      (depth-colour (or (:row/depth row) 0)))]
    :message (message-lines row)
    []))

(defn- row-indent "Leading columns of `row`: two per generation of a task."
  [row] (if (= :task (:row/kind row)) (* 2 (or (:row/depth row) 0)) 0))

(defn- display-line "Logical line `l` showing `text`, tagged with its row."
  [index row l text] (assoc l :line/text text :line/index index :line/row row))

(defn- row-display-lines "Wrapped display lines of `row`, in drawing order."
  [index row]
  (let [indent (row-indent row)]
    (mapcat (fn [l]
              (map #(display-line index row l %)
                   (wrap pane-width indent (:line/text l))))
            (row-lines row))))

(defn- pane-lines "Display lines of `rows`, numbered from row index `base`."
  [base rows]
  (vec (mapcat row-display-lines (range base (+ base (count rows))) rows)))

(defn- cursor-span "First and last display-line index of the cursor row."
  [lines cursor-index]
  (let [indexes (map :line/index lines)
        hits    (keep-indexed #(when (= cursor-index %2) %1) indexes)]
    [(first hits) (last hits)]))

(defn- scroll-offset "First display line to show, keeping the cursor in view."
  [lines cursor-index height]
  (let [[start end] (cursor-span lines cursor-index)]
    (if (nil? start) 0 (min start (max 0 (inc (- end height)))))))

(defn- pad "Pad `text` to the pane width, so a cursor row highlights as one."
  [text]
  (let [gap (- pane-width (count text))]
    (if (pos? gap) (str text (apply str (repeat gap " "))) text)))

(defn- draw-op "Draw op for one display `line` at screen row `y` of `pane`."
  [pane y focused? cursor-index line]
  (let [cursor? (= cursor-index (:line/index line))
        text    (if cursor? (pad (:line/text line)) (:line/text line))
        centre  (if (= :heading (:row/kind (:line/row line)))
                  (quot (max 0 (- pane-width (count text))) 2)
                  0)]
    {:x      (+ (pane-offsets pane) centre)
     :y      y
     :text   text
     :fg     (:line/fg line)
     :styles (if cursor?
               (get cursor-styles (if focused? :focused :blurred))
               (:line/styles line))}))

(defn- pane-rows "Rows of `pane`, or a heading for its ancestor if it has none."
  [state pane]
  (or (seq (model/rows state pane))
      [{:row/kind :heading :row/node (model/ancestor state pane)}]))

(defn- viewport-height "Content lines a pane shows below its heading."
  [state] (max 0 (dec (get-in state [:state/size :rows]))))

(defn- heading-ops "Draw ops for the heading of `pane`, pinned and cut to fit."
  [rows pane focused? cursor]
  (map #(draw-op pane heading-row focused? cursor %)
       (take 1 (pane-lines 0 (take 1 rows)))))

(defn- content-ops "Draw ops for `pane` content, scrolled to its cursor row."
  [rows pane focused? cursor height]
  (let [lines  (pane-lines 1 (rest rows))
        offset (scroll-offset lines cursor height)]
    (map-indexed #(draw-op pane (+ content-row %1) focused? cursor %2)
                 (->> lines (drop offset) (take height)))))

(defn- pane-ops "Draw ops for `pane`: its heading, then its scrolled content."
  [state pane]
  (let [rows     (pane-rows state pane)
        cursor   (model/cursor-row-index state pane)
        focused? (= pane (:state/focus state))
        height   (viewport-height state)]
    (concat (heading-ops rows pane focused? cursor)
            (content-ops rows pane focused? cursor height))))

(defn render
  "Render `state` into a vector of draw ops, left pane first."
  [state]
  (vec (mapcat #(pane-ops state %) pane-order)))
