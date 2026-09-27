---
author: user
cargo:
  status: draft
  type: task
created: Mon 28 Sep 2026 - 00h 07m 57s
subject: Create a read-only three-pane TaskDown TUI
---

# Create a read-only three-pane TaskDown TUI

1.  Find a Babashka library or pod appropriate for creating a sophisticated
    interactive long-running TUI.

2.  Use it to create a three-pane UI for TaskDown.

3.  Give users a read-only (for now) view of their tasks and messages.

4.  Data model

    1.  A TaskDown vault is a tree.
    2.  The root note of that tree is called "Tasks"
        1.  It's a conceptual entity, not an actual task or message.
        2.  It's children are the tasks that have no parent tasks.
    3.  Tasks may have child tasks.
    4.  Tasks have messages.

5.  Panes

    1.  Each pane is 80 columns wide.

        1.  Lines in those pane that would make it wider than 80 columns wraps.
        2.  Each pane has a column of empty space between it and the other pane
        3.  There is one empty column to the left of the left pane, and another
            to the right of the right pane.

    2.  Each pane has a heading.

        1.  The heading is centered and capitalized.

    3.  Left pane: Parents

        1.  Shows a task tree
            1.  Only tasks with child tasks.

    4.  Middle pane: Listing

        1.  Shows a task tree.
        2.  Heading is the ancestor node of the tasks.

    5.  Right pane: Messages

        1.  Shows messages.
        2.  By default all messages are collapsed except for the most recent
        3.  Collapsed messages just show their datetime (excluding year) and
            subject
        4.  The heading of the messages pane is the name of the task they belong
            to.

6.  Task tree

    1.  Each task uses its folder name as the task name
        1.  Hyphens are replaced with spaces
        2.  So `01-Bake-potatoes/` is shown as `01 Bake potatoes`
    2.  A tasks decendent tasks are shown after it.
        1.  Each generation is indented 2 spaces more than its parent.
    3.  Each generation, across all parents, is given a unique colour.
        1.  These colours are the same every time the app runs, not random.
        2.  There should be 6 of them. Beyond that the cycle can repeat.

7.  Minimal UI.

    1.  No lines or dividers or anything.
    2.  The only characters in the UI will be content that has come from the
        data model.
    3.  We will use spacing to bring structure to the UI.
    4.  Colour, bold text, and capitalisation will be used to bring heirarchy to
        the data.

8.  Focus

    1.  One pane has focus at each point in time.
    2.  A user can switch focus to the left using the `enter` key or to the
        right using the `esc` key.

9.  Cursors

    1.  There are three cursors in the UI, one per pane.
    2.  Cursors can be on headings or on any of the tasks, or messages in a
        pane.
    3.  The cursor is on the item, not on a specific line.
    4.  Starting positions
        1.  Parents pane: Heading.
        2.  Listing pane: Heading.
        3.  Messages pane: Most recent message.
    5.  The user can navigate a task tree with the keyboard:
        1.  Between sibling nodes in the tree
            1.  `j` to go down to next sibling.
            2.  `k` to go up to previous sibling.
        2.  Between parent and child nodes in a the tree
            1.  `h` to go left to parent.
            2.  `l` to go right to child.
        3.  If there is no task in the direction the user is going then their
            input is a noop.
            1.  A user can not navigate directly between cousin nodes. They must
                go up to parent level, then to sibling, then down to child.
    6.  The item under the cursor is highlighted.
        1.  The highlighting is dimmed in panes that are not currently in focus.

10. Pane ancestor

    1.  Each pane has an ancestor.
    2.  The ancestor of the parents pane is always the "Tasks" root node.
    3.  The ancestor of the listing pane is set to the item under the cursor
        parents pane.
    4.  The ancestor of the messages pane is set to the item under the listing
        pane cursor.
    5.  When the app first loads this means that Tasks will be the ancesor of
        all 3 panes. This will mean that no messages are shown because Tasks has
        no messages.

11. Keyboard shortcuts

    1.  `q` quits out of the view
    2.  `gg` jumps the cursor to the top item in the focussed pane.
    3.  `G` jumps the cursor to the bottom item in the focussed pane.

12. Implementation

    1.  This is going to be a significant amount of code.
    2.  Use a MVC style architecture.
        1.  Models should be pure code, free of side effects, only concerned
            with transforming and working with the in-memory data model.
        2.  Views should be pure code, free of side effects. They should be pure
            functions that take data and produce representations of the UI.
        3.  Controllers should be impure code. They handle user interaction,
            state manipulation, and side effects.
    3.  All data should be immutable save for a single global atom.
    4.  Implement this UI in `./scripts/task-down` as a fully fledged babushka
        codebase.
        1.  Proper dependencies and build config
        2.  A proper babashka test suite
            1.  Following the test pyramid. Most tests should be pure functional
                code.
    5.  Avoid functions larger than about a dozen lines of code.
    6.  Avoid files larger than about 200 lines of code.
    7.  Don't use multimethods.
    8.  Do use records, and polymorphism via protocols, where appropriate.
    9.  Don't use macros.
