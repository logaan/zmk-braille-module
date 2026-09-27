# ZMK Braille Input Module — Requirements

## 1. Scope

1. The module provides chorded Braille input as an out-of-tree ZMK module for Zephyr.
    1. A keyboard can include it in `zmk-config` without changing ZMK core.
    2. It targets hybrid keyboards with conventional keymaps and an optional Braille input mode.
2. The initial feature set includes:
    1. Uncontracted Unified English Braille (UEB) grade 1 input.
    2. An eight-dot computer Braille mode.
    3. Navigation and other ZMK actions.
    4. Persistent on-device chord bindings.
3. UEB grade 2 contractions and other named Braille systems are extensions.
    1. Grade 2 is not a prerequisite for the initial feature set.
4. The completeness target is the printable characters and ordinary key functions of a US MacBook Air keyboard.
    1. Literary UEB signs cover characters for which UEB defines a mapping.
    2. Computer Braille and configurable action bindings cover keys without a literary UEB sign.
    3. The target includes letters in both cases, digits, punctuation, and space.
    4. It also includes Tab, Return, Delete, Escape, arrow keys, Caps Lock, Shift, Control, Option, Command, and F1–F12.
    5. It includes Globe/Fn where the host and ZMK can represent it.
    6. Touch ID and power are excluded.
    7. Host-specific media and system functions require corresponding ZMK and host behaviors.
        1. They are not Braille characters.
5. Intended users are ZMK hobbyists, Braille learners, and sighted users of a Perkins-style keyboard.
    1. An OLED or comparable display provides feedback for settings and delayed output.
    2. Nonvisual feedback such as sound or haptics is not required.

## 2. Keys and chords

### 2.1. Physical positions

1. Braille input uses ten physical positions:
    1. Eight dot keys: dots 1–3 run down the left column, and 4–6 run down the right.
    2. Dots 7 and 8 sit below dots 3 and 6 respectively.
    3. One Braille space key is distinct from any conventional keyboard space key.
    4. One dedicated Braille key controls input mode and the on-device settings interface.
2. A keymap author can assign any physical key positions to the eight dots and Braille space key.
3. The dedicated Braille key has two gestures:
    1. A short press toggles Braille input mode.
    2. A long press enters or exits the on-device settings interface.
    3. Neither gesture is interpreted as a typing chord.

### 2.2. Chord collection

1. A chord consists of presses on any subset of the nine dot and space keys, followed by releases.
    1. Each dot contributes one bit to a nine-bit chord mask.
    2. The space key contributes the ninth bit.
    3. Space alone and space with dots are distinct, ordinary rule inputs.
    4. The dedicated Braille key is not a chord bit.
2. Collection starts with the first press and uses the configured window.
    1. Presses within the window contribute bits to the chord.
    2. Releasing one key does not finalize a chord while other keys remain held or the window is open.
    3. Each chord resolves once, after the window and the participating key releases.
    4. The detector then returns to idle.

## 3. ZMK integration and configuration

### 3.1. Module layout

1. The module follows ZMK's out-of-tree module conventions:
    1. `zephyr/module.yml` registers the module.
    2. Root `CMakeLists.txt` and `Kconfig` configure its build and enable option.
    3. A behavior binding resides under `dts/bindings/behaviors/`.
    4. Implementation lives in `src/`, with shared declarations in `include/` as needed.
    5. The root `README.md` documents installation and keymap use.
2. The behavior driver connects key events to chord detection and rule matching.
    1. It uses ZMK behavior APIs and Zephyr timing facilities.

### 3.2. Devicetree binding

1. The behavior compatible is `zmk,behavior-braille`.
    1. It takes no bind-time parameters (`#binding-cells = <0>`).
2. Its configuration supplies:
    1. Nine physical roles: `dot-one` through `dot-eight` and `space-key`.
    2. `timeout-ms`, defaulting to 50 ms and configurable per keyboard for different typing speeds.
    3. A named `mode`, defaulting to `ueb1`.

### 3.3. Mode selection

1. Initial character modes are `ueb1` and `computer8`.
    1. `ueb2` is available when grade 2 rules are included.
    2. Only one character rule set is active at a time.
    3. Navigation rules and user overrides remain available in every mode.
2. The on-device settings interface also selects the active mode.
    1. Selection takes effect without a separate confirmation prompt.
3. Disabling Braille input or capturing a settings gesture must not produce unintended character output.

## 4. Rule and action model

### 4.1. Rules and actions

1. A rule matches one chord or a short sequence of chords.
    1. It produces an ordered, bounded sequence of ZMK behavior invocations.
    2. It may have a word-position constraint: `any`, `word-start`, `word-middle`, or `word-end`.
2. Letters, spaces, navigation commands, and custom actions share the same action representation.
    1. There is no separate character, command, or custom-action output type.
    2. Ordinary grade 1 text defaults to host keystrokes, not literal Unicode Braille glyphs.
    3. Configured rules may invoke other behaviors.
3. Each action uses a ZMK behavior binding: a behavior device and up to two parameters.
    1. Bindings are invoked with appropriate key events.
    2. Multiple actions execute in order.

### 4.2. Matching

1. The shared engine matches rules from:
    1. The active character table.
    2. The always-active navigation table.
    3. The user override layer.
2. User overrides take precedence over factory rules with the same input.
    1. Factory mappings must not conflict within a mode.
3. The matcher prefers the longest valid chord sequence, then falls back to shorter matches.
    1. It tracks word boundaries for position-dependent rules.
    2. It delays output until it can determine which such rule applies.
4. The nine-bit mask is used identically for space-only, dot-only, and space-plus-dot chords.
    1. The engine contains no per-character or per-command dispatch branches.

### 4.3. Source responsibilities

1. The source separates these responsibilities:
    1. `behavior_braille.c`: thin ZMK behavior driver.
    2. `braille_engine.c`: shared matcher and buffer.
    3. `braille_rules.h`: common rule definition.
    4. `braille_rules_grade1.c`, `braille_rules_nav.c`, and `braille_rules_computer8.c`: rule data.
    5. `braille_rules_grade2.c`: grade 2 rule data when supported.

## 5. UEB grade 1 rules

### 5.1. Alphabet

1. Grade 1 input follows the [ICEB UEB rulebook](iceb/rules-of-unified-english-braille-2024.pdf), third edition (2024).
    1. It uses the uncontracted rules.
2. The alphabet uses these six-dot patterns:

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

### 5.2. Indicators and digits

1. The numeric indicator is dots 3, 4, 5, 6.
    1. It precedes patterns `a` through `j` to produce digits `1` through `0`.
    2. It applies across a run of digits according to UEB numeric-mode rules.
    3. It is not repeated before every digit.
2. Dot 6 preceding a lowercase letter capitalizes that letter.
3. The rulebook governs capitalized words and passages, numeric-mode termination, and grade 1 indicators.
    1. Uncontracted UEB is not necessarily a one-cell-to-one-character translation.

### 5.3. Punctuation and symbols

1. Core punctuation includes:
    1. Comma: dot 2.
    2. Semicolon: dots 2, 3.
    3. Colon: dots 2, 5.
    4. Exclamation mark: dots 2, 3, 5.
    5. Apostrophe: dot 3.
    6. Hyphen: dots 3, 6.
    7. Period: dots 2, 5, 6.
    8. Question mark: dots 2, 3, 6.
2. These multi-cell signs specify additional printable keys; “then” separates successive cells:

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

### 5.4. Literary scope

1. Other supported literary signs, including quotation marks, follow the ICEB rulebook.
    1. They do not use an inferred ASCII-to-cell mapping.
2. The grave accent has a UEB diacritic sign: dots 4, 5 then 1, 6.
    1. This does not establish a standalone literary backtick key.
3. A literary vertical bar is not specified here.
    1. Both `` ` `` and `|` remain available through computer Braille or custom actions.
    2. No UEB sign is invented for either key.
4. Dots 7 and 8 remain available for nonliterary or custom chords in `ueb1` mode.

## 6. Computer Braille and key coverage

### 6.1. Eight-dot mode

1. `computer8` is a separate, named eight-dot character rule set.
    1. It does not reinterpret UEB indicators or contractions.
    2. Its consistent eight-dot mapping covers printable US ASCII with one cell per character.
    3. The selected mapping is documented as part of the mode.
2. All 256 dot patterns have an explicit mapping or reserved entry.
    1. Dots 7 and 8 are independent bits, not UEB capital or numeric indicator cells.
    2. This mode needs no word-position constraints or contraction buffer.

### 6.2. Space and navigation

1. A Braille-space-only chord emits a literal space by default.
2. Space-plus-dot factory chords invoke navigation through the same rule mechanism as character chords.
    1. Factory navigation covers left and right movement.
    2. It covers beginning and end of line.
    3. It covers previous and next UI elements.
    4. Chord assignments do not conflict.
3. Navigation actions should be usable with VoiceOver-style navigation.
    1. User overrides can adapt them to other host or screen-reader conventions.
    2. Navigation chords remain active in each character mode.

### 6.3. Noncharacter functions

1. Tab, Return, Delete, Escape, arrows, modifiers, and F1–F12 use ZMK behavior bindings.
    1. They are not literary Braille rules.
    2. Factory rule data provides chords where ZMK and the host support the target functions.
    3. These functions require no changes to the engine.
2. A Mac function-row label, such as brightness or volume, denotes a host/system action where supported.
    1. It is not a distinct Braille character.

## 7. Persistent on-device bindings

### 7.1. Interface

1. Users can create and change chord-to-action bindings on the keyboard.
    1. No host or firmware reflash is required.
    2. The interface uses an OLED or comparable display and ZMK display widgets.
2. Normal Braille typing is suspended while the interface captures input.
3. The display uses a compact text-oriented widget suitable for small screens.

### 7.2. Binding flow

1. Capture a chord or short chord sequence with the normal chord detector.
    1. Display the detected dot and space combination.
    2. If the input has a factory or user binding, offer **override**, **re-record**, and **cancel**.
2. Select an action by fuzzy searching the behaviors compiled into the firmware by name.
    1. Use ordinary grade 1 Braille input for the search.
    2. Show matching behaviors on the display.
    3. Provide a second selection or entry step for behaviors requiring parameters.
    4. On-device parameter choices cover at least common key presses, modifier combinations, and Bluetooth profile commands.
    5. Behaviors without an available parameter editor need not be offered for selection.
3. Display the proposed input and action for confirmation, then save the rule.
    1. Return to the normal display after saving.
    2. Allow removal of one override or reset of all overrides to factory defaults.
4. The underlying rule and action representation supports any compiled ZMK behavior.
    1. It supports an ordered sequence of invocations.
    2. The on-device parameter editor may expose only a subset of behaviors.

### 7.3. Persistence and precedence

1. Factory rule tables are read-only.
2. A separate user override layer stores up to 256 added or replacement rules.
    1. It is empty by default.
    2. Deleting an override restores the factory behavior.
3. User rules survive resets and power cycles through a dedicated key in ZMK/Zephyr settings storage.
    1. Persist stable behavior identifiers and parameters, not device pointers.
    2. Resolve identifiers to live behavior devices on load.
    3. Persisted entries use the factory rule shape, including chord sequences and ordered actions.

## 8. Grade 2 and additional rule sets

### 8.1. Grade 2

1. Grade 2 uses the existing chord detector, action representation, and matching engine.
    1. Its character table contains alphabetic wordsigns, strong contractions and wordsigns.
    2. It also contains group signs and short-form words.
    3. Rules may depend on word position and word-part boundaries.
    4. The ICEB rulebook governs permitted uses.
2. The engine buffers cells while a longer match or positional constraint is unresolved.
    1. Word boundaries, punctuation, and subsequent cells determine when output can be committed.
    2. The display shows pending cells until they are committed to the host.
    3. Buffering is bounded in size and latency.
    4. Uncontracted output is used where no valid contraction applies.
3. Grade 2 adds data and any necessary word-context state.
    1. It does not add a separate key-capture or action-dispatch path.

### 8.2. Other Braille systems

1. Other Braille systems, such as Japanese or Spanish Braille, can be named character rule sets.
    1. Their tables use the common rule representation and mode-selection mechanism.
    2. Adding a language does not require changes to chord detection, matching, or navigation handling.
2. No particular non-English mapping is required for the initial feature set.

## 9. Reference material

1. The [local ICEB rulebook](iceb/rules-of-unified-english-braille-2024.pdf) is authoritative for UEB.
    1. It defines signs, indicators, contractions, and positional restrictions.
    2. Its provenance and license are recorded in [the references README](README.md).
2. Secondary guides do not supersede the ICEB rules:
    1. The [Braille Authority of North America's *ABCs of UEB*](https://www.brailleauthority.org/ueb/abcs/abcs-ueb.html).
    2. The [UEB contractions summary](https://uebot.niu.edu/courses/pluginfile.php/96/block_html/content/UEB%20Contaction%20Summary.pdf).
