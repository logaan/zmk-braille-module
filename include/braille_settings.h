// SPDX-License-Identifier: MIT
#ifndef BRAILLE_SETTINGS_H
#define BRAILLE_SETTINGS_H

#include <stdbool.h>
#include <stdint.h>
#include <zmk/behavior.h>

/* Serialized central behavior path, just like chord capture. Entry is idempotent;
 * release of the entry behavior does not exit. The phase-05 UI calls exit.
 * Both boundaries discard in-flight capture and call input_reset before notifying
 * the UI. Neither boundary commits pending input or emits host output.
 */
int braille_settings_enter(struct zmk_behavior_binding_event event);
int braille_settings_exit(struct zmk_behavior_binding_event event);
bool braille_settings_active(void);

/* Shared engine hook: discard unresolved multi-chord input and reset context,
 * without dispatching output. Weak no-op until that engine exists. Output/action
 * dispatch must also check braille_settings_active(), including queued actions.
 */
void braille_input_reset(void);

/* Phase-05 hooks. State is already updated and input reset when changed runs.
 * Chords captured wholly inside settings go only to settings_chord, never the
 * typing consumer. Defaults are no-op/success. UI must not emit host keystrokes.
 */
void braille_settings_changed(bool active, struct zmk_behavior_binding_event event);
int braille_settings_chord(uint16_t mask, struct zmk_behavior_binding_event event);

/* Capture-to-consumer gate, not a second rule/action representation. */
int braille_input_dispatch(uint16_t mask, struct zmk_behavior_binding_event event);

#endif /* BRAILLE_SETTINGS_H */
