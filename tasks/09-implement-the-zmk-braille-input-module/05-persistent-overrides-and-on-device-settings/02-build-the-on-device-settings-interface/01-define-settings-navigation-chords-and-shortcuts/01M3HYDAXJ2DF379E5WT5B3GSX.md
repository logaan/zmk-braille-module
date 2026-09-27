---
created: Mon 28 Sep 2026 - 03h 25m 40s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: Define settings navigation chords and shortcuts
cargo:
  type: task
  status: draft
  worker:
    type: agent
    harness: pi
    model: unknown
---

# Define settings navigation chords and shortcuts

1. Define the settings controls: next item, previous item, activate, back, cancel, and leave settings, plus the editing controls search needs, such as deleting a query character.
2. Take each navigation chord from VoiceOver's Braille keyboard navigation commands, then Android TalkBack, then Hable One; each carries a comment naming its source.
3. Give menu items mnemonic letter shortcuts, typed as Grade 1 letters, such as `r` for reset or `d` for done.
    1. Choose a letter from the item's name wherever possible, rather than labelling items `a`, `b`, `c` in order.
    2. Shortcuts are unique within a menu; resolve collisions with another letter from the item's name.
4. Navigation chords work on every settings screen; letter shortcuts apply only in menus, because search letters form the query.
5. Settings controls are fixed: user overrides do not change them.
6. Publish the control and shortcut table for the documentation task.
7. Unit-test alongside the implementation: each control on each screen, shortcut uniqueness within every menu, letters entering the query during search, and controls unaffected by conflicting user overrides.
