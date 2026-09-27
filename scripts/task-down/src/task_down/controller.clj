(ns task-down.controller
  "Impure controller: the one global state atom, the key map, and the render /
  read / apply event loop (Architecture 4.5).

  Every decision about what a key means is made by the pure model; every side
  effect goes through the Terminal protocol. That split is what lets the loop
  be driven by scripted keys in tests and by a real tty in `start!`."
  (:require [task-down.model :as model]
            [task-down.terminal :as terminal]
            [task-down.vault :as vault]
            [task-down.view :as view]))

(defonce app-state
  ^{:doc "The single mutable atom in the whole app: the TaskDown UI state map."}
  (atom nil))

(def quit-key "The key that leaves the UI." \q)

(def ^:private key-actions
  "The controller action for each key; a key absent here is ignored.
  `:await-g` is the first half of the two-key jump to the top."
  {\j :down \k :up \h :left \l :right
   \g :await-g \G :bottom
   :enter :focus-left :escape :focus-right})

(def ^:private cursor-directions
  "Actions that move the focused pane's cursor, with their model direction."
  {:down :down :up :up :left :left :right :right :top :top :bottom :bottom})

(def ^:private focus-directions
  "Actions that move focus between panes, with their model direction."
  {:focus-left :left :focus-right :right})

(defn- apply-action
  "The state after `action` in the focused pane; an unknown action is a noop."
  [state action]
  (let [pane (get state :state/focus)
        cursor (get cursor-directions action)
        focus (get focus-directions action)]
    (cond
      cursor (model/move-cursor state pane cursor)
      focus (model/move-focus state focus)
      :else state)))

(defn- key-action
  "The action for `key`, completing a pending two-key sequence when it does."
  [state key]
  (if (and (= :g (:state/pending-key state)) (= \g key))
    :top
    (get key-actions key)))

(defn- apply-key
  "The state after one normalised key. A nil key, and any key the key map does
  not know, leave the state alone; both clear a pending `g`, so `g x g` is not
  a jump to the top."
  [state key]
  (let [action (key-action state (:key key))]
    (if (= :await-g action)
      (assoc state :state/pending-key :g)
      (apply-action (assoc state :state/pending-key nil) action))))

(defn- step
  "The state one loop turn later: re-read the terminal size, then apply the
  key that was read with it."
  [state term key]
  (apply-key (assoc state :state/size (terminal/size! term)) key))

(defn- quit?
  "True when `key` leaves the UI."
  [key]
  (= quit-key (:key key)))

(defn- loop!
  "Draw the state, block for a key and apply it, until the user quits."
  [term]
  (loop []
    (terminal/draw! term (view/render @app-state))
    (let [key (terminal/read-key! term)]
      (when-not (quit? key)
        (swap! app-state step term key)
        (recur)))))

(defn show!
  "Show `vault` on the Terminal `term` and block until the user quits.

  The terminal is always restored, including when the loop throws. Impure; the
  Terminal is injected so tests can drive the loop with scripted keys."
  [term vault]
  (terminal/start! term)
  (try
    (reset! app-state (model/initial-state vault (terminal/size! term)))
    (loop! term)
    (finally
      (terminal/stop! term))))

(defn start!
  "Show the read-only three-pane TUI for the TaskDown vault in `vault-dir` and
  return when the user quits. Impure.

  The vault is read before the terminal is taken over, so a bad directory
  reports on an ordinary shell instead of a raw-mode screen."
  [vault-dir]
  (show! (terminal/new-lanterna-terminal) (vault/load-vault vault-dir)))
