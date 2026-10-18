// SPDX-License-Identifier: MIT
#include <assert.h>
#include <errno.h>
#include <braille_chord.h>
#include <braille_settings.h>

/* Fixture pending consumer only; no production matcher or action schema here. */
static unsigned pending, host_output, reset_count, transitions, settings_chords;
static struct zmk_behavior_binding_event boundary = {.position = 15, .layer = 2, .timestamp = 42};
static bool expected_active;

void braille_input_reset(void) {
    assert(braille_settings_active() == expected_active);
    pending = 0;
    ++reset_count;
}
void braille_settings_changed(bool active, struct zmk_behavior_binding_event event) {
    assert(active == expected_active && active == braille_settings_active());
    assert(!pending && reset_count == transitions + 1);
    assert(event.position == boundary.position && event.layer == boundary.layer);
    assert(event.timestamp == boundary.timestamp);
    ++transitions;
}
int braille_settings_chord(uint16_t mask, struct zmk_behavior_binding_event event) {
    (void)event;
    assert(braille_settings_active());
    assert(mask == 2);
    ++settings_chords;
    return -EIO; /* Error must not fall through into typing. */
}
int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event event) {
    (void)event;
    assert(!braille_settings_active());
    if (mask == 1) {
        ++pending; /* Unresolved multi-cell prefix. */
    } else if (mask == 2 && pending) {
        ++host_output;
        pending = 0;
    }
    return 0;
}
static int edge(unsigned position, uint16_t bit, bool pressed) {
    struct zmk_behavior_binding_event event = {.position = position};
    return braille_chord_key_event(bit, pressed, event);
}
static void chord(uint16_t bit, int result) {
    assert(edge(0, bit, true) == 0);
    assert(edge(0, bit, false) == result);
}
static void enter(void) {
    expected_active = true;
    assert(braille_settings_enter(boundary) == 0);
}
static void leave(void) {
    expected_active = false;
    assert(braille_settings_exit(boundary) == 0);
}
int main(void) {
    assert(!braille_settings_active());
    chord(1, 0);
    assert(pending == 1);
    enter();
    assert(!pending && transitions == 1);
    enter(); /* Idempotent: no reset/notice replay. */
    assert(transitions == 1);
    chord(2, -EIO);
    assert(settings_chords == 1 && !host_output && !pending);
    leave();
    leave();
    assert(transitions == 2);
    chord(2, 0); /* Neither entry nor exit committed/replayed the prefix. */
    assert(!host_output && !pending);

    /* Capture spanning entry is discarded through idle, including late joins. */
    assert(edge(0, 1, true) == 0);
    enter();
    assert(edge(1, 2, true) == 0);
    assert(edge(0, 1, false) == 0);
    assert(edge(1, 2, false) == 0);
    assert(settings_chords == 1);
    chord(2, -EIO);
    assert(settings_chords == 2);
    /* Capture spanning exit cannot leak UI input back to typing. */
    assert(edge(0, 1, true) == 0);
    leave();
    assert(edge(1, 2, true) == 0);
    assert(edge(0, 1, false) == 0);
    assert(edge(1, 2, false) == 0);
    assert(!pending && !host_output);
    /* Entire settings session while a physical key is held: still discarded. */
    assert(edge(0, 1, true) == 0);
    enter();
    leave();
    assert(edge(0, 1, false) == 0);
    assert(!pending && !host_output);
    /* Subsequent fresh typing works, but nothing stale survived. */
    chord(1, 0);
    chord(2, 0);
    assert(host_output == 1 && !pending);
    assert(transitions == 6 && reset_count == 6);
    return 0;
}
