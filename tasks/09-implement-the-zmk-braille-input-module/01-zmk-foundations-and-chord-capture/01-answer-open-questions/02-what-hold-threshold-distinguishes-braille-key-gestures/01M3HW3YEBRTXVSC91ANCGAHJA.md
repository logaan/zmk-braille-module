---
created: Mon 28 Sep 2026 - 02h 45m 35s
author:
  type: agent
  harness: claude-code
  model: claude-opus-5.5
subject: 'Decided: no long press; settings is a chord-bound action'
cargo:
  type: task
  status: delivered
---

# Decision: no long-press gesture

## 1. Decision

1. Nothing in the module is triggered by a long press.
2. Opening Braille settings is a ZMK behavior action, like `&bt` or `&rgb_ug` actions.
    1. Factory rule data binds it to a default chord; phase 02's command-chord research allocates it.
    2. Users can rebind it or add more chords through the override layer.
3. The braille-mode-key's hold/tap distinction uses ZMK hold-tap conventions: pressing another key while it is held resolves as hold.

## 2. Impact

1. The gesture task becomes `03-implement-the-braille-mode-key-and-settings-action`.
2. Phase 02 noncharacter allocation includes the settings-entry action.
3. Leaving settings is a settings-UI control, resolved by phase 05's UI-controls question.
