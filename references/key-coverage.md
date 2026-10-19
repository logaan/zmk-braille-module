# Shared US keyboard coverage checklist

This is the phase-03 software checkpoint for tasks 9.3.2.2, 9.4's later
Grade-1 audit, and phase 06's final coverage audit. Update this checklist
rather than creating a parallel inventory. “Verified” below means software
capture/matching/dispatch and, where stated, native ZMK HID; it does not
mean physical keyboard or macOS/VoiceOver acceptance.

## Printable US ASCII

| Character set | UEB1 now | Computer8 now | Evidence / remaining work |
| --- | --- | --- | --- |
| Lowercase `a`–`z` (26) | Verified | Verified | Alphabet tests; exhaustive Computer8 tests |
| Uppercase `A`–`Z` (26) | Deferred to phase 04 | Verified, same cell as lowercase plus dot 7 | Computer8 raw HID + representative native HID; UEB capitalization not implemented |
| Digits `0`–`9` (10) | Deferred to phase 04 | Verified, lowered a–j cells | Exhaustive Computer8 tests; UEB numeric context not implemented |
| `, ; : ! ' - .` (7) | Verified, one cell | Verified, one cell | Core punctuation + exhaustive Computer8 tests |
| `& * @ ^ # $ % ~ / _ + = < > ( ) [ ] { }` and backslash (21) | Verified, ordered two-cell signs | Verified, one cell | Specified-symbol + exhaustive Computer8 tests |
| Double quote, question mark (2) | Deferred to phase 04 | Verified, one cell | UEB word-context/quotation choice not implemented |
| Backtick, vertical bar (2) | No literary mapping claimed; Computer8 fallback | Verified, dot 4 and dots 1,2,5,6 | Exhaustive tests + representative native HID |
| Literal space (1) | Verified, Braille-space-only | Verified, Braille-space-only | Each mode's `0x100` command, not the zero-dot cell |
| **Total (95)** | **55 verified; 40 not yet covered** | **95 verified** | No claim of complete contextual UEB1 |

Every Computer8 non-space result has exactly one dot-only cell; the table
has 94 rules. The selected variant is VoiceOver's **Liblouis US eight-dot**
table, not legacy Duxbury (underscore differs). Source registration, hashes,
all 256 entries, and the non-ASCII reservation policy are recorded in
[`research/computer8.yaml`](research/computer8.yaml).

## Ordinary keyboard functions

| Function | Both modes | Evidence / limit |
| --- | --- | --- |
| Tab | Verified: space + dots 2,3,4,5 (`0x11e`) | Host and native ZMK HID; old `0x118` is not Tab |
| Return | Verified: space + 1,5; dot 8; space + 8 | All aliases use the whitespace pending-input path |
| Delete / Backspace | Verified: space + 1,4,5; dot 7; space + 7 | macOS Delete is Backspace, not forward Delete |
| Escape | Verified: space + 1,2 | Host and native HID |
| Four arrows | Verified: space + 1/4/3/6 = Left/Right/Up/Down | Raw keys, not VoiceOver API commands |
| Home, End, Page Up, Page Down | Verified | Home/End are not macOS Command+Left/Right; use sticky Command + arrow for line boundaries |
| Shift, Control, Option, Command | Verified one-shot sticky chords | Native unmodified ZMK `sk`/HID, stacking and timeouts; not toggles |
| Globe | Consumer GLOBE verified in native HID | Host-dependent; not true Fn, Fn+Delete, or Fn+arrow emulation |
| True Fn, Dictation/Siri, Do Not Disturb, Touch ID, power | No implementation claimed | Host/ZMK constraints and out-of-scope functions are not invented literary signs |

The factory command tables each contain 22 rows. Space-bearing commands
cannot collide with dot-only characters. Dot 7/Delete and dot 8/Return
are exclusively command rows, not duplicate silent character rows.

## Isolation, pending input and overrides

- [x] All 256 eight-dot patterns are accounted for: 94 characters, two
  character-reserved command positions, and 160 reserved patterns including zero.
- [x] All 159 enterable reserved patterns send nothing, notify once, recover
  on the next cell, and can be bound by an in-memory user override.
- [x] Zero dots is not a chord and produces neither output nor notice;
  dots 7,8 together are reserved, not a newline alias.
- [x] Every Computer8 character is immediate and neutral in all 12 seeded
  UEB context combinations; indicators, word-position choice and contractions
  do not affect it. These tests do not implement phase-04 transitions.
- [x] Selecting either real mode discards pending input and resets context;
  failed name selection changes neither. A UEB symbol prefix cannot leak into
  Computer8, nor can a pending multi-cell override cross either direction.
- [x] One shared in-memory override layer remains active in both modes,
  including mapped/reserved cells and multi-chord rules. Only explicit
  multi-chord overrides introduce ordinary Computer8 prefix delay.
- [x] Both real character/command pairs pass factory collision validation;
  all commands, Braille space, Tab and Return aliases remain available.
- [ ] Brief visual unassigned/pending feedback: phase 05. The current
  production notice hook is weak/no-op; host tests observe its callback.
- [ ] Settings-screen mode changes and persisted selection/overrides: phase 05.
- [ ] Physical phase-03 acceptance on QBF Paintbrushes and Toucan: user-owned
  task 9.3.3 (still draft; no board flashed by this software work).

## Reproducible software checks

```sh
uv run --no-project --with pyyaml --with kconfiglib python -m unittest discover \
  -s tests/host -p 'test_computer8*.py' -v
./scripts/test-commands
./scripts/smoke-build qbf-left qbf-computer8 toucan-left toucan-computer8
```

Computer8 host tests include split/non-split, optional Grade-2 compilation,
and the pinned ZMK keycode header when available. Native tests use real
`kp`/`sk`/HID and no Computer8 stand-in overrides. Both keyboard layouts
also pass resolved-devicetree checks with the Computer8 default. All four
smoke configurations pass; their images are under
`build/baseline/build/smoke-{qbf-left,qbf-computer8,toucan-left,toucan-computer8}/zephyr/zmk.uf2`.
These are central-half images; this work changes neither peripheral firmware
nor the sibling keyboards' production defaults/pins. No firmware was flashed.

All 41 host tests outside the known older cheatsheet/coordinator suites pass
with PyYAML and kconfiglib. Those two excluded suites reproduce their existing
failures at starting commit `53e9439`; see [`README.md`](README.md).
They are not passing acceptance evidence.
