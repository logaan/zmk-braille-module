# ZMK Braille Input Module — Design Spec

Sep 27, 2026 · @Logan

## Overview

This module adds Unified English Braille input to ZMK, ZMK being the open source keyboard firmware built on the Zephyr real time operating system. It ships as a standalone out of tree ZMK module, so anyone can pull it into their own zmk-config repository and add it to their keymap without modifying ZMK core.

Scope for version one is Unified English Braille Grade 1, meaning uncontracted braille where each cell maps to exactly one letter, number, or punctuation mark. Grade 2, which uses contractions and shorthand for common letter groups and whole words, is planned as a second phase built on top of the same input mechanism.

Target users are ZMK hobbyists, Braille learners, and sighted users who want a Perkins style chorded Braille input method on a mechanical keyboard, for example for faster typing, an accessibility teaching tool, or simply as a novel input method. This version of the device is not intended as a primary input tool for blind users, and the design assumes the OLED or similar display is a reliable, sighted feedback channel throughout, rather than needing non-visual feedback such as sound or haptics.

## Hardware model

The module assumes ten physical key positions in total: eight dot keys, covering the full eight dot Braille cell used by computer Braille and Unified English Braille, one Braille specific spacebar, kept separate from any standard QWERTY spacebar the keyboard might also have, and one dedicated Braille key, whose role is to toggle Braille input mode on and off, intended for hybrid keyboards that are normally a regular typing keyboard and only sometimes used for Braille input, unlike a keyboard built exclusively as a Braille input device, where Braille mode might simply always be active.

Input is chorded. A person presses some combination of the eight dot keys together, roughly at the same time, then releases them, and that combination is interpreted as one Braille cell. The spacebar contributes its own bit to that same chord alongside the eight dots, with no special casing, so a chord using only the spacebar bit and a chord mixing the spacebar with dot bits are just different rows in the same lookup, detailed further in the Clarification section below.

Dot numbering follows the standard Braille convention, dots 1 through 3 down the left column, top to bottom, and dots 4 through 6 down the right column, top to bottom, with dots 7 and 8 added below 3 and 6 respectively for eight dot cells. The devicetree binding should let the keymap author assign any physical key position to any dot number, so the layout is not hardcoded to a specific keyboard shape.

## Module structure

Following ZMK's documented module conventions, the repository, working name zmk-behavior-braille, should be laid out as follows.

A zephyr folder at the root containing module.yml, which declares the module to Zephyr's build system and points to the settings below.

A dts folder containing a bindings subfolder, then a behaviors subfolder inside that, holding the devicetree binding yaml file, for example zmk,behavior-braille.yaml.

An src folder holding the C source implementation, following the unified data driven design detailed in the Unified source code layout section below, behavior\_braille.c for the driver itself, braille\_engine.c for the shared table walking logic, and braille\_rules\_grade1.c, braille\_rules\_nav.c, braille\_rules\_grade2.c for the rule data, all sharing the same struct defined in the include folder.

An include folder for any public headers other modules or the keymap might need.

A CMakeLists.txt and Kconfig at the root, following the same pattern as other ZMK behavior modules, so the build system picks up the source files and exposes a configuration option such as CONFIG\_ZMK\_BEHAVIOR\_BRAILLE to enable it.

A README at the root explaining installation via the zmk module add command, and how to reference the behavior in a keymap.

This mirrors the structure shown in ZMK's own module creation and new behavior documentation, so it should drop into any existing zmk-config west manifest cleanly.

## Devicetree binding design

The behavior node uses compatible equals zmk comma behavior dash braille, following ZMK naming convention. Binding cells should be zero, since the behavior itself takes no parameter at bind time, it acts on whichever of the nine key positions are physically wired to it.

Rather than hardcoding key positions, the binding should accept the nine dot and space roles as devicetree properties, so the keymap author declares which key position corresponds to which dot. Suggested properties, dot dash one through dot dash eight, each taking a key position value, plus a space key property for the Braille spacebar.

An additional property, timeout dash ms, sets the chord detection window in milliseconds, how long the behavior waits after the first dot key press before deciding the chord is complete, with a sensible default such as fifty milliseconds, overridable per keyboard.

A further optional property, mode, takes a value such as ueb1, ueb2, or computer8, defaulting to ueb1, selecting between Unified English Braille grade one, grade two, or eight dot Computer Braille Code, detailed in its own section below. As with grade two, this does not select between separate code paths, it simply controls which single character rule set, out of the mutually exclusive grade one, grade two, or computer eight tables, is merged into the active lookup alongside the always present navigation table and the user override layer on top.

The yaml file itself follows the same shape as ZMK's existing mod dash morph binding, a short description string, the compatible string, and a properties block listing each of the above with its type and whether it is required.

## Behavior driver logic

At a high level the driver keeps a per instance bitmask, one bit per dot, plus a small state machine with three states, idle, collecting, and settled.

On the first dot key press, the driver moves from idle to collecting, sets that dot's bit, and starts the timeout timer using the timeout dash ms value from the binding. Each further dot key press while still within the window simply sets its bit. On release of any key, the driver does not immediately finalize, since Braille chords are typically pressed together and released at slightly different times, so the release itself does not end the chord, only the timeout or an explicit rule does.

When the timeout fires, or when the last key is released after the window has already elapsed, the driver looks at the accumulated chord bitmask, including the spacebar bit, and passes it to the shared engine described in the Unified source code layout section, which walks the single rule table covering every character, navigation command, and user binding alike, and resolves it to whatever that rule specifies, which is always one or more arbitrary ZMK behavior invocations, a single action or a short sequence of them, never a distinct character or command type of its own.

After finalizing, the driver resets to idle and clears the bitmask, ready for the next chord.

The spacebar is not handled separately from the dot keys. It contributes its own bit to the same chord bitmask used for dots one through eight, giving a nine bit chord value overall, and it is looked up the exact same way as any other chord, in the single unified rule table, with no separate command table and no special casing based on whether the spacebar bit happens to be set. See the Clarification section below for the full statement of this, and Open Questions for the specific navigation command mappings still to be decided.

This logic lives in behavior braille dot c, using ZMK's standard behavior driver API, meaning the pressed and released callback functions ZMK calls for any custom behavior, plus a Zephyr kernel timer for the chord timeout.

## Grade 1 lookup table

Dots are numbered by the standard Braille convention, one, two, three down the left column top to bottom, and four, five, six down the right column top to bottom. The table below lists which dots are raised for each letter.

| Letter | Dots |
| --- | --- |
| a | 1 |
| b | 1, 2 |
| c | 1, 4 |
| d | 1, 4, 5 |
| e | 1, 5 |
| f | 1, 2, 4 |
| g | 1, 2, 4, 5 |
| h | 1, 2, 5 |
| i | 2, 4 |
| j | 2, 4, 5 |
| k | 1, 3 |
| l | 1, 2, 3 |
| m | 1, 3, 4 |
| n | 1, 3, 4, 5 |
| o | 1, 3, 5 |
| p | 1, 2, 3, 4 |
| q | 1, 2, 3, 4, 5 |
| r | 1, 2, 3, 5 |
| s | 2, 3, 4 |
| t | 2, 3, 4, 5 |
| u | 1, 3, 6 |
| v | 1, 2, 3, 6 |
| w | 2, 4, 5, 6 |
| x | 1, 3, 4, 6 |
| y | 1, 3, 4, 5, 6 |
| z | 1, 3, 5, 6 |

Numbers reuse the letter a through j patterns, preceded by the numeric indicator, dots 3, 4, 5, 6. So the numeric indicator followed by the pattern for a means the digit one, the indicator followed by b means two, and so on through the indicator followed by j meaning zero. The numeric indicator is written once before a run of digits, not before each one.

Common punctuation in Grade 1 includes comma, dot 2 alone, period, dots 2, 5, 6, question mark, dots 2, 3, 6, and apostrophe, dot 3 alone. A full punctuation and symbols table, including capital indicators and other UEB signs, is documented in the Braille Authority of North America's UEB symbols list, which should be consulted directly when Claude Code implements the complete table, rather than relying on this summary alone.

Implementation note, the lookup table in code should be a simple array or switch statement keyed by the eight bit dot bitmask, mapping to either a plain ASCII character or a Unicode Braille pattern codepoint, Unicode block U plus 2800 onward, if the module also wants to support displaying the raised Braille cell itself, not just the translated letter.

## Grade 2 contractions, future phase

Grade two Unified English Braille adds roughly one hundred eighty nine contractions across four categories, detailed with sources in the Reference sources subsection below, alphabetic wordsigns, strong contractions and wordsigns, group signs, and short form words. Each has its own dot pattern and positional rules, some valid only at the start, middle, or end of a word, some blocked from spanning separate word parts.

The architectural approach for phase two is to keep the same chord detection and per cell dot to pattern lookup from grade one, but add a second stage above it, a small state machine or buffer that holds the last few decoded cells and checks them against a contraction table before committing output. Many contractions are context sensitive, for example a whole word sign only applies when the cell stands alone as a full word, so the buffer needs some notion of word boundaries, likely driven by the space key and punctuation.

This phase is explicitly out of scope for the first implementation pass. It is flagged here so the module's internal architecture, particularly the boundary between chord detection and character output, is designed to be extended rather than rewritten later.

## Eight dot Computer Braille Code, a third selectable mode

Alongside Unified English Braille grade one and grade two, the module supports a third mode, eight dot Computer Braille Code, sometimes just called eight dot Braille. This is a genuinely separate system, not a variant of grade one, and it is the reason the hardware model was built around a full eight dot cell from the start rather than the traditional six dot cell.

In eight dot Braille, dot seven and dot eight carry their own dedicated meaning rather than standing in for UEB's separate capital and number indicator cells. Dot seven marks a character as a control code or, depending on the specific table used, a capital letter, effectively shifting the encoded ASCII value. Dot eight extends the range further, covering the upper half of an eight bit character set, letting extended or accented characters be encoded. Because of this, every printable ASCII character maps to exactly one Braille cell in this mode, with no separate indicator cells needed before a run of capitals or digits the way grade one and two require. This maps directly onto the two hundred fifty six possible eight dot patterns defined by the Unicode Braille Patterns block, U plus 2800 through U plus 28FF, versus only sixty four patterns available in a six dot cell.

Architecturally, this fits the same unified rule table design used for grade one, navigation, and grade two. Eight dot Braille becomes a fourth rule set, braille dash rules dash computer8 dot c in the Unified source code layout, populated with a full one cell to one ASCII character mapping across all two hundred fifty six patterns, position constraint field left as any throughout, since this mode uses no contractions or positional rules at all.

Mode selection. Since grade one, grade two, and eight dot Braille are mutually exclusive interpretations of the same physical chords, the devicetree grade property described in the Devicetree binding design section should be widened from a one or two value to a three way choice, for example a string or enum property taking values such as ueb1, ueb2, or computer8, rather than a numeric grade. Only one of the three character rule sets, grade one, grade two, or computer eight, should be active in the lookup at a time, though the navigation command rules and the user override layer remain active regardless of which one is selected. This also means the on-device rebinding UI's mode should ideally let a user switch between these three at runtime, not just at compile time, an addition to the open questions for that feature.

A useful secondary source once implementation reaches this section is the Braille Authority of North America's page on eight dot Braille, which confirms the standard dot numbering, one, two, three, seven downward on the left column, four, five, six, eight downward on the right, differing from plain six dot numbering only in where seven and eight sit.

## MacBook Air key coverage, the v1 completeness target

Rather than the full grade two contraction set, v1's actual completeness bar is narrower and concrete, every key on a standard MacBook Air must be producible through this module. This was checked directly against the uploaded rulebook, Rules of Unified English Braille, 2024, Christine Simpson, 408 pages, using targeted searches of its glossary and symbol chapters rather than a read-through, so the section numbers below point straight at the relevant pages rather than requiring another full pass through the book.

A stock US MacBook Air keyboard breaks down into the 95 printable US-ASCII characters, 26 letters in both cases, 10 digits, 32 punctuation and symbol characters, plus space, and a set of control and modifier keys, Tab, Return, Delete, Escape, the four arrow keys, Caps Lock, Shift, Control, Option, Command, the Globe or Fn key, and the F1 through F12 function row, currently brightness down and up, Mission Control, Spotlight, Dictation, Do Not Disturb, previous track, play or pause, next track, mute, and volume down and up. The Touch ID or power button is deliberately excluded, not something a Braille chord should be able to trigger.

### Letters, digits, and core punctuation, already established

The 26 letter patterns, the numeric indicator, dots 3 4 5 6, plus letters a through j for digits, and comma, dot 2, period, dots 2 5 6, question mark, dots 2 3 6, and apostrophe, dot 3, are already confirmed in the Grade 1 lookup table above. One gap is now closed too, the rulebook states plainly, in its own words, that a capital letter is formed by the prefix dot 6 placed before the lowercase letter, which directly resolves part of the Capitalisation and numeric mode item in Open Questions below, single letter capitalisation is dot 6 alone, two cells total.

### Newly confirmed symbols, with dot patterns

Every other punctuation and symbol key on the keyboard has a defined UEB sign. Where a symbol needs a prefix cell, both cells are listed in order.

| Key | UEB dots |
| --- | --- |
| ; semicolon | 2,3 |
| : colon | 2,5 |
| ! exclamation mark | 2,3,5 |
| - hyphen | 3,6 |
| & ampersand (3.1) | 4 then 1,2,3,4,6 |
| \* asterisk (3.3) | 5 then 3,5 |
| @ at sign (3.7) | 4 then 1 |
| ^ caret (3.6) | 4 then 2,6 |
| # number sign (3.19) | 4,5,6 then 1,4,5,6 |
| $ dollar sign (3.10) | 4 then 2,3,4 |
| % percent sign (3.21) | 4,6 then 3,5,6 |
| \~ tilde (3.25) | 4 then 3,5 |
| / forward slash (7.4) | 4,5,6 then 3,4 |
| \_ underscore (7.2) | 4,6 then 3,6 |
| \` grave accent (used as a diacritic; see note below) | 4,5 then 1,6 |
| + plus (3.17) | 5 then 2,3,5 |
| = equals (3.17) | 5 then 2,3,5,6 |
| < less-than (3.17) | 4 then 1,2,6 |
| > greater-than (3.17) | 4 then 3,4,5 |
| ( open parenthesis | 5 then 1,2,6 |
| ) close parenthesis | 5 then 3,4,5 |
| \[ open square bracket | 4,6 then 1,2,6 |
| \] close square bracket | 4,6 then 3,4,5 |
| { open curly brace | 4,5,6 then 1,2,6 |
| } close curly brace | 4,5,6 then 3,4,5 |
| \\ backslash, reverse solidus | 4,5,6 then 1,6 |

Parentheses, square brackets, and curly braces share the same pair of base shapes, dots 1,2,6 for opening and dots 3,4,5 for closing, distinguished only by which prefix cell comes first, dot 5 for round, dots 4,6 for square, dots 4,5,6 for curly, a clean, regular pattern worth keeping in mind when Claude Code builds this part of the table.

One genuine gap, the vertical bar or pipe character has no defined literary UEB sign; it simply is not part of ordinary print text the code was designed to represent. The grave accent above is technically defined as a diacritic mark rather than a standalone print character, so using its pattern for a standalone backtick key is a reasonable but not literally rulebook-sanctioned choice, worth a light sanity check later. Neither gap actually matters for completeness, though, since the eight dot Computer Braille Code mode, already designed above, guarantees full ASCII coverage as a fallback regardless of what literary UEB does or doesn't define, so both keys remain producible either way.

### Control and modifier keys need no braille rules at all

Tab, Return, Delete, Escape, the four arrows, Caps Lock, Shift, Control, Option, Command, the Globe or Fn key, and F1 through F12 are not braille characters and were never going to appear in the rulebook. They're already covered by the existing architecture, any chord can invoke any ZMK behavior via the Bindable actions mechanism described above, so each of these just needs a default chord bound to the matching key press behavior with the right HID keycode, exactly like any other rule in the unified table. No new design work is needed here, only populating the default rule set once implementation starts.

Taken together, full MacBook Air key coverage is confirmed achievable with the architecture already designed, literary UEB grade 1 for the great majority of characters, the eight dot Computer Braille Code mode as a guaranteed complete fallback for any edge case, and the existing bindable-action mechanism for every control and modifier key. This is the actual v1 target; the full grade two contraction set remains a valuable, separately scoped enhancement rather than a requirement for calling v1 complete.

## Future growth direction, other national Braille systems

Beyond Unified English Braille grade one and two and eight dot Computer Braille Code, an explicit longer term goal is supporting other countries' native Braille systems, for example Japanese Braille, which is structured very differently from Latin alphabet Braille since it encodes the Japanese kana syllabary rather than individual letters, or Spanish Braille, which shares a Latin alphabet base with English but differs in some letters, punctuation, and its own set of contractions.

This is not part of the initial implementation, but it is the reason the character rule set should be treated as a swappable, named module from the start, in the same way grade one, grade two, and computer eight already are three swappable, mutually exclusive rule sets behind the same mode property. A future Japanese or Spanish Braille rule set would be simply another same shaped braille dash rule dash t array, contributed as its own file, for example braille dash rules dash ja dot c or braille dash rules dash es dot c, without changing the shared engine at all, since the engine's job is already just walking whichever rule table or tables are active.

The main design implication for now is to keep the mode property, and the internal mechanism that selects which character rule set is active, from being hardcoded to only three specific values. It should be built as an open, extensible choice from day one, so adding a new language later is a matter of contributing a new rule data file and registering its identifier, not restructuring the mode selection logic itself. No specific non-English Braille system needs to be designed yet, this section exists only to keep that door open architecturally.

## Clarification, no special cased keys, everything is a uniform chord to action mapping

An important refinement to the architecture described above, the spacebar is not a conceptually special key, and characters, navigation commands, and other bound actions are not conceptually different kinds of output. Every one of the nine physical key positions, the eight dots and the spacebar, is simply one more bit in the same chord bitmask, with no key treated differently by the engine itself. Every entry in the unified rule table is uniformly a chord, or short chord sequence, mapped to one or more ZMK actions, a single arbitrary ZMK behavior invocation or a short ordered sequence of them, whether the net effect happens to look like typing a letter, running a navigation command, or anything else, there is no distinct character or command output type, only a sequence of ZMK actions to invoke. This holds identically across all three character modes, grade one, grade two, and eight dot Computer Braille Code, none of them special case the spacebar bit or treat navigation chords as a separate mechanism from character chords. This is consistent with, and simplifies, the Unified source code layout and the Bindable actions sections above, both of which should be read with this in mind, the distinction drawn earlier between a spacebar chord being handled separately from dot chords was an oversimplification that this section supersedes.

One open question introduced by this clarification, how switching between grade one, grade two, and eight dot mode itself should work has not actually been designed yet. It was agreed this switching should happen through the on-device rebinding menu described earlier rather than a separate dedicated gesture, but the mechanism, whether mode switching is itself just another chord to action binding using the same uniform mechanism, invoking some kind of set dash mode action, or requires its own distinct handling, still needs to be designed. It was decided that mode switching does not need its own confirmation step, even though it changes the meaning of every other chord on the keyboard at once, unlike an individual rebinding, which does go through the override versus re-record versus cancel prompt described above.

### Reference sources

The authoritative source is Rules of Unified English Braille, third edition 2024, published by the International Council on English Braille, [available as a PDF here](https://iceb.org/wp-content/uploads/2025/10/Rules-of-Unified-English-Braille-2024.pdf). This is the definitive rulebook, including every contraction, its permitted position within a word, and exceptions. Claude Code should treat this as the source of truth for the actual table Claude Code builds, rather than any summary, including this document.

Two secondary references are useful for a faster overview while implementing. The Braille Authority of North America's ABCs of UEB guide, [available here](https://www.brailleauthority.org/ueb/abcs/abcs-ueb.html), explains the categories of contractions in plain language. A condensed contractions summary PDF, [available here](https://uebot.niu.edu/courses/pluginfile.php/96/block_html/content/UEB%20Contaction%20Summary.pdf), lists the roughly one hundred eighty nine contractions grouped by type, useful as a quick checklist when verifying the code's table is complete.

Grade 2 contractions fall into four broad categories worth structuring the code around. Alphabetic wordsigns, single letters that stand for whole common words when written alone, for example the letter b alone means but. Strong contractions and wordsigns, single cells representing common short words or letter groups such as and, for, of, the, and with. Group signs, cells representing common letter groups such as ch, sh, th, ing, ou, ow, wherever they appear in a word, subject to positional rules. Short form words, longer words abbreviated to just a few cells, for example the pattern for about is ab.

Many contractions carry positional restrictions, only valid at the start, middle, or end of a word, and some are blocked entirely when they would span two separate word parts, such as the two ds in the word address. These positional rules are extensive and should be pulled directly from the rulebook table by table, not reconstructed from memory or summary, since getting them wrong would produce invalid Braille.

## Unified source code layout

Rather than three separate code paths for grade one letters, navigation modifiers, and grade two contractions, the engine is a single generic table walker. Every entry, a plain Braille letter, a navigation command, or a grade two contraction, is expressed as data, a row describing what chord or chord sequence it matches and what it produces, rather than as bespoke C logic per category.

Core data structure, a single entry type, something like braille dash rule dash t, with fields for the matching chord bitmask or short sequence of chord bitmasks, an ordered list of one or more zmk underscore behavior underscore binding entries to invoke in sequence when the chord resolves, no output type tag needed since every entry's output is uniformly this same kind of action sequence, and a position constraint field, any, word dash start, word dash middle, or word dash end, used by grade two rules and left as any for grade one, navigation, and eight dot entries. One shared array of these rules is the whole dictionary.

Engine behavior, the same lookup and matching function walks this one rule table regardless of category. It first tries to match the longest possible chord sequence buffered so far, falling back to shorter matches, which naturally handles grade two group signs and short form words needing multiple cells while single cell grade one letters and navigation commands still resolve immediately. Position constraints are checked using the same word boundary tracking the spacebar chord and punctuation already provide.

Suggested file split. Braille dash rules dot h, the shared braille dash rule dash t struct definition and the function signatures for lookup, used by everything else. Braille dash rules dash grade1 dot c, the grade one alphabet, numbers, and punctuation, expressed purely as an array of braille dash rule dash t entries, position field always any. Braille dash rules dash nav dot c, the spacebar plus dot chord navigation commands, same struct, same array shape, spacebar bit set as an ordinary part of the chord like any dot, output value simply whichever ZMK action that chord maps to. Braille dash rules dash grade2 dot c, the grade two contractions, same struct, populated from the rulebook, using the position field this time since many entries need it. Braille dash engine dot c, the actual chord buffering, sequence matching, and table walking logic, written once, with no awareness of which category a rule came from. Behavior dash braille dot c, the ZMK behavior driver itself, thin, just wiring key press and release callbacks to the engine and combining all rule tables, currently active grade decides whether the grade two array is included in the active search set.

This means adding a new grade two contraction later is purely a data change, one new line in braille dash rules dash grade2 dot c, never a change to the engine logic itself. It also means the grade property on the devicetree binding becomes simple to implement, it just controls which rule arrays get merged into the active lookup set at init time.

## Output buffering for position dependent grade 2 rules

Some grade two contractions are only valid depending on where they fall in a word, only at word start, only at word end, or blocked entirely when they would span two word parts. The engine often cannot know whether a position constraint is satisfied the instant a chord resolves, since word end and mid word spanning can only be confirmed once a following word boundary, a space, punctuation, or the end of input, actually arrives. This means output cannot always be committed immediately, some cells need to be held in a small pending buffer until enough follow-on context arrives to confirm which rule, if any, legally applies, then flushed as a batch.

Critically, this buffering needs to be visible to the user, not a hidden internal delay. Since output only reaches the host computer once flushed, a user typing quickly could otherwise see nothing happen for a moment, then several characters appear at once, which is disorienting. Since this device targets sighted users, per the Overview, the OLED display already used for status and the rebinding menu is the natural feedback channel, showing the pending, not yet flushed cells directly on screen as they accumulate, then clearing that indicator once they flush to the host. No separate sound or haptic feedback mechanism is needed for this.

Architecturally, this sits inside the shared engine's chord sequence matching described in the Unified source code layout section, alongside the existing longest match buffering, but adds a second reason to hold output beyond simply waiting to see if a longer chord sequence is still being typed, waiting to see whether a position constraint will be satisfied. The maximum buffering depth, how many pending cells or how long a delay before either flushing or timing out and falling back to grade one style uncontracted output, is an open question to resolve during grade two implementation, likely requiring testing with real contraction sequences from the rulebook to size properly.

## On-device custom binding UI

Users should be able to bind their own chords to actions entirely on the keyboard itself, no host computer, no reflashing, and have those bindings survive a power cycle or reset. ZMK already provides the two subsystems this leans on, its persistent settings subsystem, built on Zephyr's settings layer and stored in the flash storage partition, and its display widget framework for OLED and similar small screens, so this feature is additive on top of both rather than a reinvention.

Persistence layer. There is no separate output type or tag for user defined bindings. Every rule, whether it ships as part of grade one, navigation, or grade two, or is created later by a user through the on-device menu, is the exact same braille dash rule dash t entry described in the Unified source code layout section, indistinguishable in shape once it exists. The grade one, navigation, and grade two tables are simply pre-populated at build time by the module's authors, using the same structure and the same eventual storage format a user's own custom rule would use, rather than being built in through separate logic. On any change, the user rule set, just the chord pattern and chosen action for each custom binding, is serialized and written to a dedicated settings key under the ZMK settings subsystem, for example a key namespaced something like braille slash user dash rules. On boot, the driver reads this key back, deserializes it, and merges those entries into the active lookup table used by the shared engine, so custom bindings behave exactly like built in ones from the engine's point of view.

Default rules are not deletable, but they are overridable, and the two are kept as separate layers rather than one merged table on disk. The factory rules, grade one, navigation, and grade two, compiled into firmware as read only data, are never modified or removed. A second layer, the user override table, lives entirely in flash settings storage, empty by default, and holds only the rules a user has changed or added, keyed by chord pattern. At lookup time, the engine checks the user override layer first for a matching chord, falling back to the factory layer only when no override exists, so a user redefining which dots produce the letter b simply adds an override entry for that chord, the factory entry underneath is untouched and inert for as long as the override exists. Resetting to defaults is then just clearing the user override layer entirely, or removing a single entry from it to un-override just one binding, both trivial and safe operations since the factory layer was never touched in the first place.

On-device menu flow. A dedicated mode, entered via the Braille key described in the Hardware model section or a reserved chord, open question, whether the same Braille key that toggles Braille input on and off also serves as the rebind mode entry gesture, for example via a long press, or whether rebind mode uses an entirely separate combination, switches the display from its normal status view into a small rebinding wizard. Step one, capture, the display prompts the user to press the chord or short chord sequence they want to bind, the driver listens using the same chord buffering logic already built for normal input, and echoes back which dots or spacebar combination it detected. If that chord already resolves to an existing binding, factory or user, the display presents three options before proceeding, override the existing binding, re-record and capture a different chord instead, or cancel out of the rebinding flow entirely. Step two, action select, rather than a flat or categorized scrollable menu, the user narrows down the full list of available behaviors using a fuzzy find search, typing a few letters of the action's name using ordinary grade one Braille input, with the display updating live to show matching behaviors as the search narrows, then confirming the highlighted match. This suits a chorded keyboard much better than paging through a long list, since a user can jump straight to something like a Bluetooth or macro action by typing a distinguishing fragment of its name rather than scrolling past everything else. Step three, confirm and save, the display shows a short summary of the new binding before committing it, the driver writes the updated rule into the table and persists it through the settings subsystem immediately, then returns to normal display mode.

Display rendering. Since ZMK's display feature is built on Zephyr's LVGL based widget system, this menu is implemented as its own widget, swapped in over the status widget while in rebind mode, using simple text lines given the small screen sizes typical on these boards, no need for graphics beyond basic text and perhaps a selection cursor character.

Open questions for this feature, exactly which chord or gesture reliably enters and exits rebind mode without colliding with normal typing, the custom binding budget is resolved at 256 user overrides, a generously round number given even that many entries, each a full behavior binding with parameters, comes to only about 4 kilobytes, comfortably small against flash storage constraints on typical nice nano class controllers, conflict handling is resolved, see the On-device menu flow paragraph above, override, re-record, or cancel, offered whenever a captured chord already has a binding.

## Bindable actions, the full ZMK behavior surface

What a chord can be bound to is not a closed, module specific list of actions. It is any ZMK behavior at all, the same universe of things a person could bind to any ordinary key position in a normal keymap, individual keystrokes, modifier and key combinations, Bluetooth profile selection and clearing, output selection, macros, layer switches, or any other behavior ZMK or another module exposes.

This works because ZMK already has exactly the right mechanism for this. Every behavior binding at runtime is represented as a struct, zmk underscore behavior underscore binding, holding a pointer to the target behavior device plus up to two integer parameters, and ZMK provides a function, zmk underscore behavior underscore invoke underscore binding, that fires a binding directly, given that struct and a key event. This is exactly how existing ZMK behaviors like mod dash morph and hold dash tap forward to other behaviors internally, so this module reuses the identical mechanism rather than building a parallel one.

This confirms what the output field on every braille dash rule dash t entry actually is, for any entry, not a special case, an ordered list of one or more zmk underscore behavior underscore binding entries, each a behavior device reference plus its one or two parameters, consistent with the Clarification section below stating there is no conceptual difference between a character output and any other bound action. The engine's job when a chord resolves to a rule is simply to call zmk underscore behavior underscore invoke underscore binding once for each stored binding in order, the same call ZMK's own composite behaviors already make, chaining several the same way a macro chains its steps when the rule holds more than one.

This has a direct effect on the on-device rebinding UI described above. Step two of that wizard, action select, cannot just page through a small fixed menu, since the space of valid ZMK behaviors and their parameters is open ended, for example a key press behavior needs a keycode parameter, a Bluetooth behavior needs a command and profile index, a macro takes no extra parameters at all. As described in the On-device menu flow section above, the behavior itself is chosen via a fuzzy find search over the behaviors compiled into the firmware and registered with Zephyr's device driver model, typed using ordinary grade one Braille input, which handles the first tier well even as that list grows large. If the matched behavior takes parameters, a second tier is still needed to choose or enter each one, likely from a curated, per behavior list of common parameter choices, since typing arbitrary numeric or enum values is a separate concern from searching behavior names. Which behaviors and which parameter shortlists to expose in that second tier is an open question to resolve during implementation, likely starting with a modest, useful subset, key presses, common modifier combinations, and Bluetooth profile commands, rather than attempting full open ended coverage on day one.

Storage follows the same shape. The user override entry persisted to flash needs to record which behavior device it targets, typically by a stable string label matching Zephyr's devicetree label for that behavior node, plus its one or two parameters, rather than a raw pointer, since pointers are not stable across a firmware rebuild or reset. On boot, the driver resolves that stored label back to a live behavior device pointer before it can be invoked.

## Open questions

This section gathers every open question raised elsewhere in this document, each also flagged inline at the point it arises. Navigation command mappings, the specific chord to command assignments for VoiceOver style navigation are not yet designed, VoiceOver is the primary reference, TalkBack secondary, the Hable One a concrete precedent, see the Spacebar double duty entry below. Rebind mode entry gesture, whether the dedicated Braille key described in the Hardware model also serves, for example via long press, to enter the on-device rebinding menu, or whether a separate combination is used, see the On-device menu flow paragraph. Mode switching mechanism, how switching between grade one, grade two, and eight dot mode is technically implemented once selected through the rebinding menu, whether as an ordinary chord to action binding or something requiring distinct handling, no confirmation step is needed once decided, see the Clarification section. Grade two output buffer sizing, how many pending cells or how long a delay before flushing or falling back to uncontracted output, see Output buffering for position dependent grade 2 rules. Rebinding menu parameter shortlist, which ZMK behaviors and which curated parameter choices to expose in the second tier of the action select step, see the Bindable actions section.

Output format, largely resolved by the unified rule design, each rule's output type tag already allows character, keycode, command, or arbitrary behavior invocation side by side in the same table, so this is configurable per rule rather than a single global choice. What remains open is only the default output type to use for plain grade one letters, HID keycodes typed as if from a regular keyboard, versus raw Unicode Braille pattern codepoints for applications expecting literal Braille glyphs.

Chord timeout value, whether fifty milliseconds is the right default, or whether it should be tuned based on testing with actual users, since finger drop timing varies a lot between experienced Braille typists and learners.

Capitalization and numeric mode, partially resolved. Single letter capitalization is confirmed as dot 6 placed before the lowercase letter, directly from the rulebook's own wording, see MacBook Air key coverage above. What still needs a full design pass is the persistent mode or lookahead state for runs, a capitalized word or passage, and a run of digits under the numeric indicator, since both need the driver to track when that mode should end, not just when it starts.

Spacebar double duty, resolved. The spacebar does double duty from grade one onward, not just in a future grade two phase. Pressed alone, it emits a literal space. Pressed as part of a chord together with one or more dot keys, it acts as a modifier that triggers Braille display style command keys, common conventions on refreshable Braille displays for computer navigation, such as space with dot four for pan or move right, space with dot one for pan or move left, space with dots one two three for home or beginning of line, space with dots four five six for end of line, and space with dot eight or similar combinations for functions like next or previous UI element. As stated in the Clarification section, the spacebar bit is just an ordinary part of the chord, looked up in the single unified rule table like any other, with no separate table for navigation versus plain characters. The exact command mappings still need to be chosen. The primary design reference for these navigation commands is Apple's VoiceOver screen reader on macOS, its rotor navigation, element focus movement, and interaction model should guide the shape of these commands first. Google's Android TalkBack is a secondary reference, useful where VoiceOver's model does not map cleanly onto a physical chorded keyboard. Third, the Hable One, an existing Braille input accessory for smartphones, is a useful concrete precedent, since it already maps Braille letter chords directly onto screen reader commands by mnemonic, for example dots one two five, the letter h, for home, and dots one two, the letter b, for back, which is worth drawing on directly for commands with an obvious letter mnemonic.

Interaction with ZMK layers, largely resolved, the devicetree binding design assigns all nine dot and spacebar positions directly to one behavior instance, so this is a single behavior watching nine key positions at once, not something bound per individual key position the way a simple key press behavior is. What remains open is only whether it makes sense to allow more than one instance active on different layers at once, for example a plain typing layer versus a Braille layer, or whether one instance covering the whole keyboard is sufficient.

## Handoff notes for Claude Code

Start by reading ZMK's own module creation and new behavior pages, since this spec follows their conventions but the actual file templates and CMake or Kconfig boilerplate should be copied from ZMK's official module template repository rather than reinvented.

Build order suggestion, first scaffold the module skeleton and confirm it loads with west and appears as a no-op behavior in a test keymap. Second, build the shared engine and chord buffering state machine described in the Unified source code layout section, tested with just a small stub rule table, to prove the timing logic works before any real data is added. Third, populate the grade one rule table and confirm full alphabet, number, and punctuation output through the real engine. Fourth, populate the navigation command rules and the spacebar chord handling. Fifth, build the user override layer, its flash persistence, and the on-device rebinding menu, starting with plain key press and modifier bindings before attempting the fuller ZMK behavior surface described later in this document. Grade two contraction data, and full coverage of arbitrary ZMK behaviors in the rebinding menu, are follow up phases once this base is solid.

The grade one dot to letter table earlier in this document is verified against standard Unified English Braille references and safe to use directly as the seed data, but the punctuation and symbol set was only partially covered here, so Claude Code should cross check the Braille Authority of North America's UEB symbols list before finalizing that table.
