# ZMK Braille Input Module — Requirements

## Scope

The module provides chorded Braille input as an out-of-tree ZMK module for Zephyr. A keyboard can include it in a `zmk-config` without modifying ZMK core. It is intended for hybrid keyboards that normally type through a conventional keymap and can switch into Braille input mode.

The initial feature set includes uncontracted Unified English Braille (UEB) grade 1 input, an eight-dot computer Braille mode, navigation and other ZMK actions, and persistent on-device chord bindings. UEB grade 2 contractions are an extension, not a prerequisite for the initial feature set. The input and rule architecture must accommodate grade 2 and other named Braille systems without a separate chord detector.

The completeness target is the printable characters and ordinary key functions of a US MacBook Air keyboard, excluding Touch ID and power. Literary UEB signs cover the characters for which UEB defines a mapping; computer Braille and configurable action bindings cover keys that do not have a literary UEB sign. The target includes letters in both cases, digits, punctuation, space, Tab, Return, Delete, Escape, arrow keys, Caps Lock, Shift, Control, Option, Command, Globe/Fn where the host and ZMK can represent it, and F1–F12. Host-specific media and system functions depend on the corresponding ZMK and host behaviors; they are not Braille characters.

The intended users are ZMK hobbyists, Braille learners, and sighted users of a Perkins-style keyboard. An OLED or comparable display is the feedback channel for settings and any delayed output. Nonvisual feedback such as sound or haptics is not required.

## Keys and chords

The Braille input uses ten physical positions:

- Eight dot keys, numbered 1–3 down the left column, 4–6 down the right, and 7 and 8 below 3 and 6 respectively.
- One Braille space key, distinct from any conventional keyboard space key.
- One dedicated Braille key: a short press toggles Braille input mode, and a long press enters or exits the on-device settings interface. Neither gesture is interpreted as a typing chord.

A keymap author can assign any physical key positions to the eight dots and the Braille space key. A chord consists of presses on any subset of these nine keys, followed by releases. The space key contributes one bit to the same nine-bit chord mask as the dots: space alone and space with dots are ordinary, distinct rule inputs. No separate spacebar or command dispatch path is required. The dedicated Braille key is not a bit in the chord mask.

Chord collection uses a configurable window, defaulting to 50 ms, starting with the first press. Presses within that window contribute bits to the chord; releasing one key does not finalize it while others remain held or while the collection window is open. Each collected chord resolves once, after the window and its participating key releases, and the detector then returns to idle. The timeout must be configurable per keyboard so users can tune it to their typing speed.

## ZMK integration and configuration

The module follows ZMK's out-of-tree module conventions: `zephyr/module.yml`, root `CMakeLists.txt` and `Kconfig`, a behavior binding under `dts/bindings/behaviors/`, implementation in `src/`, shared declarations in `include/` as needed, and installation and keymap instructions in the root `README.md`. Kconfig provides an enable option. The behavior driver connects key events to the chord detector and the rule engine using ZMK behavior APIs and Zephyr timing facilities.

The devicetree behavior compatible is `zmk,behavior-braille`. It takes no bind-time parameters (`#binding-cells = <0>`). Its configuration supplies the nine physical roles (`dot-one` through `dot-eight` and `space-key`), a `timeout-ms` property defaulting to 50 ms, and a named `mode` property defaulting to `ueb1`. Supported character modes are `ueb1` and `computer8`, with `ueb2` available when grade 2 rules are included. The mode identifier and rule registration mechanism must permit additional named rule sets without redesigning chord detection or matching. Only one character rule set is active at a time; navigation rules and user overrides remain available in every mode.

Mode selection is also available from the on-device settings interface. A selection takes effect without a separate confirmation prompt. Chords and settings operations must not produce unintended character output when Braille input is disabled or when the on-device interface is capturing a binding.

## Rule and action model

A rule matches a chord or short sequence of chords and produces an ordered, bounded sequence of ZMK behavior invocations. Rules can also carry a word-position constraint (`any`, `word-start`, `word-middle`, or `word-end`). There is no separate character, navigation-command, or custom-action output type: a rule for a letter, a space, or a navigation operation uses the same action representation. The output for ordinary grade 1 text defaults to host keystrokes, not literal Unicode Braille glyphs; rules may invoke other behaviors when configured.

The shared engine matches rules from the active character table, the always-active navigation table, and the user override layer. User overrides take precedence over factory rules with the same input. Factory mappings must not conflict within a mode. The matcher prefers the longest valid chord sequence, then falls back to shorter matches. For position-dependent rules, it tracks word boundaries and delays output until it can determine which rule applies. The nine-bit mask is used identically for space-only, dot-only, and space-plus-dot chords.

Each action uses ZMK's behavior-binding mechanism (a behavior device and up to two parameters), invoked with the appropriate key events. Multiple actions in a rule execute in order. The rule engine does not contain per-character or per-command dispatch branches.

The source responsibilities are separable into a thin ZMK behavior driver (`behavior_braille.c`), a shared matcher and buffer (`braille_engine.c`), a common rule definition (`braille_rules.h`), and rule data such as `braille_rules_grade1.c`, `braille_rules_nav.c`, `braille_rules_computer8.c`, and, when supported, `braille_rules_grade2.c`. Other language rule sets can use the same definition and registration mechanism.

## UEB grade 1 rules

Grade 1 input follows the [ICEB *Rules of Unified English Braille*, third edition (2024)](iceb/rules-of-unified-english-braille-2024.pdf), using its uncontracted rules. The alphabet uses the following six-dot patterns:

| Letter | Dots          |
|--------|---------------|
| a      | 1             |
| b      | 1, 2          |
| c      | 1, 4          |
| d      | 1, 4, 5       |
| e      | 1, 5          |
| f      | 1, 2, 4       |
| g      | 1, 2, 4, 5    |
| h      | 1, 2, 5       |
| i      | 2, 4          |
| j      | 2, 4, 5       |
| k      | 1, 3          |
| l      | 1, 2, 3       |
| m      | 1, 3, 4       |
| n      | 1, 3, 4, 5    |
| o      | 1, 3, 5       |
| p      | 1, 2, 3, 4    |
| q      | 1, 2, 3, 4, 5 |
| r      | 1, 2, 3, 5    |
| s      | 2, 3, 4       |
| t      | 2, 3, 4, 5    |
| u      | 1, 3, 6       |
| v      | 1, 2, 3, 6    |
| w      | 2, 4, 5, 6    |
| x      | 1, 3, 4, 6    |
| y      | 1, 3, 4, 5, 6 |
| z      | 1, 3, 5, 6    |

The numeric indicator (dots 3, 4, 5, 6) followed by the patterns for `a` through `j` produces digits `1` through `0`. It applies across a run of digits according to UEB numeric-mode rules; it is not repeated before every digit. Dot 6 preceding a lowercase letter capitalizes that letter. Capitalized words and passages, numeric-mode termination, and grade 1 indicators follow the rulebook's context rules. These indicators mean that uncontracted UEB is not necessarily a one-cell-to-one-character translation.

Core punctuation includes comma (2), semicolon (2, 3), colon (2, 5), exclamation mark (2, 3, 5), apostrophe (3), hyphen (3, 6), period (2, 5, 6), and question mark (2, 3, 6). The following multi-cell signs specify additional printable keys; cells separated by “then” are entered in order:

| Key or sign              | UEB dots           |
|--------------------------|--------------------|
| `&` ampersand            | 4 then 1,2,3,4,6   |
| `*` asterisk             | 5 then 3,5         |
| `@` at sign              | 4 then 1           |
| `^` caret                | 4 then 2,6         |
| `#` number sign          | 4,5,6 then 1,4,5,6 |
| `$` dollar sign          | 4 then 2,3,4       |
| `%` percent sign         | 4,6 then 3,5,6     |
| `~` tilde                | 4 then 3,5         |
| `/` forward slash        | 4,5,6 then 3,4     |
| `_` underscore           | 4,6 then 3,6       |
| `+` plus                 | 5 then 2,3,5       |
| `=` equals               | 5 then 2,3,5,6     |
| `<` less-than            | 4 then 1,2,6       |
| `>` greater-than         | 4 then 3,4,5       |
| `(` open parenthesis     | 5 then 1,2,6       |
| `)` close parenthesis    | 5 then 3,4,5       |
| `[` open square bracket  | 4,6 then 1,2,6     |
| `]` close square bracket | 4,6 then 3,4,5     |
| `{` open curly brace     | 4,5,6 then 1,2,6   |
| `}` close curly brace    | 4,5,6 then 3,4,5   |
| `\` backslash           | 4,5,6 then 1,6     |

All other supported literary signs, including quotation marks, must follow the ICEB rulebook rather than an inferred ASCII-to-cell mapping. The grave accent has a UEB diacritic sign (dots 4, 5 then 1, 6), but this does not establish a standalone literary backtick key. A literary vertical bar is not specified here. Both `` ` `` and `|` remain available through computer Braille or custom actions without inventing UEB signs for them. Dots 7 and 8 remain available for nonliterary or custom chords in `ueb1` mode.

## Computer Braille and key coverage

`computer8` is a separate, named eight-dot character rule set, not an alternative interpretation of UEB indicators or contractions. It must specify a consistent eight-dot mapping covering printable US ASCII with one cell per character; the selected mapping must be documented as part of the mode. The 256 possible dot patterns are represented explicitly by mappings or reserved entries. Dot 7 and dot 8 are independent bits, not substitutes for UEB's capital or numeric indicator cells. No word-position constraints or contraction buffer are needed for this character mode.

A Braille-space-only chord emits a literal space by default. Space-plus-dot factory chords invoke navigation actions using the same rule mechanism as character chords. Factory navigation covers movement left and right, beginning and end of line, and previous and next UI elements, with nonconflicting chord assignments. The actions should be usable with VoiceOver-style navigation; user overrides can adapt the mapping to other host or screen-reader conventions. Navigation chords remain active in each character mode.

Noncharacter keyboard functions, including Tab, Return, Delete, Escape, arrows, modifiers, and F1–F12, are ZMK behavior bindings, not literary Braille rules. Factory rule data provides chords for the target functions where ZMK and the host support them, without changing the engine. A physical Mac function-row label (for example, brightness or volume) is a host/system action where supported, not a distinct Braille character.

## Persistent on-device bindings

Users can create and change chord-to-action bindings on the keyboard, without a host or firmware reflash. The interface uses an OLED or comparable display and ZMK's display widget facilities. A long press on the Braille key opens or closes the settings interface; normal Braille typing is suspended while it captures input.

The binding flow is:

1. Capture a chord or short chord sequence with the normal chord detector, and display the detected dot and space combination. If it already has a factory or user binding, offer **override**, **re-record**, and **cancel** before continuing.
2. Search the behaviors compiled into the firmware by name using fuzzy matching and ordinary grade 1 Braille input. Show matching behaviors on the display. For behaviors requiring parameters, provide a second selection or entry step. At minimum the on-device parameter choices cover common key presses, modifier combinations, and Bluetooth profile commands; behaviors without an available parameter editor need not be offered for selection.
3. Display the proposed input and action for confirmation, save the rule, and return to the normal display. The user can also remove an individual override or reset all overrides to factory defaults.

The underlying rule and action representation supports any compiled ZMK behavior and an ordered sequence of invocations, even if the on-device parameter editor exposes only a subset. The display uses a compact text-oriented widget suitable for small screens. Mode selection is available from the same settings interface and does not require the binding-override confirmation flow.

Factory tables are read-only. A separate user override layer, empty by default, stores up to 256 added or replacement rules. A matching override takes precedence over the factory rule; deleting the override restores the factory behavior. User rules survive resets and power cycles via a dedicated key in ZMK/Zephyr settings storage. Persist stable behavior identifiers and parameters, not device pointers; resolve identifiers to live behavior devices on load. The persisted entries use the same rule shape as factory entries, including any chord sequence and ordered actions.

## Grade 2 and additional rule sets

Grade 2 support uses the existing chord detector, action representation, and matching engine. Its character table contains alphabetic wordsigns, strong contractions and wordsigns, group signs, and short-form words. Rules may depend on word position and word-part boundaries; the ICEB rulebook governs their permitted use. Only the selected character mode is active, while navigation and user overrides remain active.

The engine buffers cells when it cannot yet distinguish a shorter match from a longer one or validate a positional constraint. Word boundaries, punctuation, and subsequent cells determine when it can commit output. Pending cells are shown on the display until committed to the host. Buffering is bounded in size and latency, with uncontracted output used where no valid contraction applies. Grade 2 adds data and any necessary word-context state, not a separate key-capture or action-dispatch path.

Other Braille systems, such as Japanese or Spanish Braille, can be added as named character rule sets. Their tables use the common rule representation and register with the same mode-selection mechanism; adding a language does not require changing the chord detector or navigation handling. No particular non-English mapping is required for the initial feature set.

## Reference material

The [local ICEB rulebook](iceb/rules-of-unified-english-braille-2024.pdf) is authoritative for UEB signs, indicators, contractions, and positional restrictions. Its provenance and license are recorded in [the references README](README.md). Secondary guides include the [Braille Authority of North America's *ABCs of UEB*](https://www.brailleauthority.org/ueb/abcs/abcs-ueb.html) and a [UEB contractions summary](https://uebot.niu.edu/courses/pluginfile.php/96/block_html/content/UEB%20Contaction%20Summary.pdf); these do not supersede the ICEB rules.
