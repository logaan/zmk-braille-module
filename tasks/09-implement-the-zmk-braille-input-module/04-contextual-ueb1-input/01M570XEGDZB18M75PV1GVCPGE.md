---
created: Sun 18 Oct 2026 - 19h 09m 10s
author:
  type: agent
  harness: pi
  model: gpt-5.6-terra
  name: task-reorg
subject: Contextual UEB1 input
cargo:
  type: task
  status: draft
---

# Contextual UEB1 input

## 1. Purpose

1.  Complete the stateful work that cannot be expressed as unconditional table
    data.
2.  Reuse the alphabet, phase-02 factory commands, and phase-03 static tables;
    do not replace their capture, matching, or action path.

## 2. Required implementation work

1.  Apply class-based UEB context transitions and word-position constraints.
2.  Complete reset and pending-input behavior at layer, non-Braille-key,
    settings, and command boundaries.
3.  Implement numeric mode, capitalization, Grade 1 indicators, and the
    context-dependent question-mark and quotation-sign choices.
4.  Retain the generic prefix matcher for multi-cell signs and give phase 05
    its pending-cell and dropped-prefix lifecycle hooks.

## 3. Exclusions

1.  Do not regress phase-02 direct command chords or phase-03 stateless tables
    while adding context.
2.  Persistent overrides, on-device settings, display UI, and UEB Grade 2 stay
    in their later phases.
