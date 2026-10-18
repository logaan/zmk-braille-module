// SPDX-License-Identifier: MIT

#include <zephyr/toolchain.h>
#include <braille_chord.h>
#include <braille_settings.h>

static bool settings_active;

__weak void braille_input_reset(void) {}

__weak void braille_settings_changed(bool active, struct zmk_behavior_binding_event event) {
    (void)active;
    (void)event;
}

__weak int braille_settings_chord(uint16_t mask, struct zmk_behavior_binding_event event) {
    (void)mask;
    (void)event;
    return 0;
}

bool braille_settings_active(void) { return settings_active; }

static int set_active(bool active, struct zmk_behavior_binding_event event) {
    if (settings_active == active) {
        return 0;
    }
    /* Close the typing gate before hooks. Exit never replays settings input. */
    settings_active = active;
    braille_chord_discard();
    braille_input_reset();
    braille_settings_changed(active, event);
    return 0;
}

int braille_settings_enter(struct zmk_behavior_binding_event event) {
    return set_active(true, event);
}

int braille_settings_exit(struct zmk_behavior_binding_event event) {
    return set_active(false, event);
}

int braille_input_dispatch(uint16_t mask, struct zmk_behavior_binding_event event) {
    return settings_active ? braille_settings_chord(mask, event)
                           : braille_chord_resolved(mask, event);
}
