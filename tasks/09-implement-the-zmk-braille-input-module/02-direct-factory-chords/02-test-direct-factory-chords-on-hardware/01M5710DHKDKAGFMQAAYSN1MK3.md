---
created: Sun 18 Oct 2026 - 19h 10m 47s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
subject: Test direct factory chords on real hardware
cargo:
  type: task
  status: draft
  worker: user
---

# Test direct factory chords on real hardware

## 1. Scope

1.  Flash or build the post-phase-02 central firmware for each baseline
    keyboard and test the delivered factory commands in `ueb1`.
2.  Use an ordinary macOS text field or another observable host surface; record
    the keyboard, host setup, and any host-specific limitation.

## 2. Required checks

1.  Confirm Braille Tab is dots 2, 3, 4, 5 plus Braille space and produces one
    Tab key event.
2.  Confirm literal Braille space, Return, arrows, navigation, and a sample of
    modifier and supported system/media actions use their intended host action.
3.  Confirm direct commands neither emit alphabet characters nor leave a
    modifier armed after its documented scope.
4.  Exercise QBF Paintbrushes and Toucan, including Toucan's momentary and
    toggled Braille-layer entry.

## 3. Boundary

1.  This is acceptance of direct factory actions only.
2.  It does not claim UEB context, settings UI, persistence, or complete
    character-table coverage.
