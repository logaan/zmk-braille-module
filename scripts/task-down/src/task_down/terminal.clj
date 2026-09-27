(ns task-down.terminal
  "Impure terminal layer: the Terminal protocol and the Lanterna record that
  implements it. Every tty side effect lives behind this protocol, so the
  controller can run against a fake Terminal in tests.

  Backed by the Babashka pod `org.babashka/lanterna` `0.0.1` (Architecture
  1.1), which the namespace loads on first use rather than at require time, so
  requiring this namespace for the protocol alone costs nothing.

  Pod facts this code depends on, each verified in a real tty:

  - `(get-terminal :text {})` and `(terminal-screen handle)` return opaque
    handle maps; they are passed around whole and never destructured.
  - `get-size` returns `[cols rows]`, `get-key-blocking` a Character for
    printable keys and a Keyword for special ones (`:enter`, `:escape`, `:up`).
  - `put-string` takes `{:fg :bg :styles}` with exactly the draw-op keywords.
  - No `clear` var is exposed, so `draw!` blanks the screen itself; drawing
    past the right edge or below the bottom is silently clipped, not an error."
  (:require [babashka.pods :as pods]
            [clojure.string :as str]))

(defprotocol Terminal
  "The side effects the controller needs to show the UI and read its keys."
  (start! [this] "Take over the tty. Call stop! however the session ends.")
  (stop! [this] "Restore the terminal, including after an exception.")
  (draw! [this ops] "Blank the screen, paint the draw ops and flush.")
  (read-key! [this] "Block for one key: {:key k}, or nil to ignore it.")
  (size! [this] "The terminal size as {:rows n :cols n}."))

(def ^:private pod-name "Maven coordinate of the terminal pod."
  'org.babashka/lanterna)

(def ^:private pod-version "Terminal pod version." "0.0.1")

(def ^:private screen-ns "Namespace holding the pod's screen vars."
  "pod.babashka.lanterna.screen")

(def ^:private terminal-ns "Namespace holding the pod's terminal vars."
  "pod.babashka.lanterna.terminal")

(def ^:private lanterna
  "The pod's vars, resolved once on first use. A delay, so loading stays lazy
  and the pod is only ever loaded in the process that draws."
  (delay
    (pods/load-pod pod-name pod-version)
    (let [screen #(requiring-resolve (symbol screen-ns %))
          term #(requiring-resolve (symbol terminal-ns %))]
      {:new-terminal (term "get-terminal")
       :new-screen (screen "terminal-screen")
       :start (screen "start")
       :stop (screen "stop")
       :put (screen "put-string")
       :redraw (screen "redraw")
       :size (screen "get-size")
       :key (screen "get-key-blocking")})))

(def ^:private special-keys
  "Pod keywords that mean something to the controller; the rest are ignored."
  #{:enter :escape})

(defn- op-opts
  "The put-string options a draw op carries, defaulted."
  [op]
  (merge {:fg :default :styles #{}} (select-keys op [:fg :bg :styles])))

(defn- blank-ops
  "One row of spaces per screen row: the pod exposes no clear, so a frame
  starts by wiping the previous one or stale glyphs would survive."
  [{:keys [rows cols]}]
  (let [blank (str/join (repeat cols " "))]
    (mapv (fn [y] {:x 0 :y y :text blank}) (range rows))))

(defn- paint
  "Put every draw op of `ops` into the pod screen buffer."
  [screen put ops]
  (doseq [op ops]
    (put screen (:x op) (:y op) (:text op) (op-opts op))))

(defn- normalise-key
  "A pod key as {:key k}, or nil when the controller should ignore it."
  [k]
  (cond
    (char? k) {:key k}
    (contains? special-keys k) {:key k}
    :else nil))

(defn- pod-size
  "The pod's [cols rows] as the Architecture 3.4.4 size map."
  [screen size]
  (let [[cols rows] (size screen)]
    {:rows rows :cols cols}))


; A Terminal drawing on this process's controlling tty through the Lanterna
; pod. `screen` is an opaque pod handle: pass it back whole, never destructure
; it. Create instances with new-lanterna-terminal.
(defrecord lanterna-terminal [screen]
  Terminal
  (start! [_] ((:start @lanterna) screen))
  (stop! [_] ((:stop @lanterna) screen))
  (draw! [this ops]
    (let [{:keys [put redraw]} @lanterna]
      (paint screen put (into (blank-ops (size! this)) ops))
      (redraw screen)))
  (read-key! [_] (normalise-key ((:key @lanterna) screen)))
  (size! [_] (pod-size screen (:size @lanterna))))

(defn new-lanterna-terminal
  "A Terminal on this process's controlling tty, through the Lanterna pod.

  Loads the pod and creates the screen, both impure; call it once, then start!
  it. `:text` forces a console terminal, so no window can ever open."
  []
  (let [{:keys [new-terminal new-screen]} @lanterna]
    (->lanterna-terminal (new-screen (new-terminal :text {})))))
