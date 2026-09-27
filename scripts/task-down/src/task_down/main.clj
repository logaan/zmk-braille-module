(ns task-down.main
  "Entry point for the read-only three-pane TaskDown TUI.

  Run it from the repository root with `scripts/task-down/task-down
  [vault-dir]`, or from `scripts/task-down/` with `bb -m task-down.main
  [vault-dir]`. Nothing here does work: it picks the vault directory and hands
  over to the controller."
  (:require [task-down.controller :as controller]))

(def default-vault-dir
  "The vault shown when no directory is given: the one this repository uses."
  "tasks")

(defn -main
  "Run the TUI over the TaskDown vault in the first argument, `tasks` by
  default, and return when the user quits."
  [& args]
  (controller/start! (or (first args) default-vault-dir)))
