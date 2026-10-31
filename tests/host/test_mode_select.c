// SPDX-License-Identifier: MIT
#include <assert.h>
#include <errno.h>
#include <string.h>
#define TEST_HAS_zmk_behavior_braille_select 1
#define TEST_NODES_zmk_behavior_braille_select(fn) fn(select_device)
#include "../../src/behavior_braille_select.c"
#include <braille_engine.h>

static unsigned output_count;
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    (void)binding; (void)event; (void)pressed;
    output_count++;
    return 0;
}
static struct zmk_behavior_binding_event event = {.position = 1};
static int select_mode(unsigned mode) {
    struct zmk_behavior_binding binding = {.param1 = mode};
    int ret = select_device->binding_pressed(&binding, event);
    assert(select_device->binding_released(&binding, event) == 0);
    return ret;
}
int main(void) {
    assert(select_device->locality == BEHAVIOR_LOCALITY_CENTRAL);
    assert(braille_tables_select_default() == 0);
    assert(braille_chord_resolved(0x20, event) == 0); // pending capital sign
    assert(braille_engine_pending_count() > 0);
    assert(braille_chord_key_event(1, true, event) == 0);
    assert(select_mode(BRAILLE_MODE_COMPUTER8) == 0);
    assert(!strcmp(braille_tables_active(), "computer8"));
    assert(braille_engine_pending_count() == 0);
    assert(braille_chord_key_event(1, false, event) == 0);
    assert(output_count == 0); // old live chord discarded
    assert(braille_chord_key_event(1, true, event) == 0);
    assert(braille_chord_key_event(1, false, event) == 0);
    assert(output_count == 2); // fresh a emitted
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
    assert(select_mode(BRAILLE_MODE_NEXT) == 0);
    assert(!strcmp(braille_tables_active(), "ueb2"));
    assert(select_mode(BRAILLE_MODE_UEB2) == 0);
#else
    assert(braille_chord_key_event(1, true, event) == 0);
    assert(select_mode(BRAILLE_MODE_UEB2) == -ENOENT);
    assert(!strcmp(braille_tables_active(), "computer8"));
    assert(braille_chord_key_event(1, false, event) == 0);
    assert(output_count == 4); // failed selection preserves live capture
#endif
    assert(select_mode(BRAILLE_MODE_NEXT) == 0);
#ifdef CONFIG_ZMK_BRAILLE_COMPUTER8_SHORT
    assert(!strcmp(braille_tables_active(), "computer8_short"));
    assert(select_mode(BRAILLE_MODE_COMPUTER8_SHORT) == 0);
    assert(braille_chord_resolved(1, event) == 0);
    assert(braille_engine_pending_count() == 1);
    assert(select_mode(BRAILLE_MODE_NEXT) == 0);
#else
    assert(select_mode(BRAILLE_MODE_COMPUTER8_SHORT) == -ENOENT);
#endif
    assert(!strcmp(braille_tables_active(), "ueb1"));
    assert(select_mode(BRAILLE_MODE_UEB1) == 0);
    struct zmk_behavior_binding bad = {.param1 = 5};
    assert(select_device->binding_pressed(&bad, event) == -EINVAL);
    assert(select_device->binding_released(&bad, event) == -EINVAL);
    bad.param1 = 0; bad.param2 = 1;
    assert(select_device->binding_pressed(&bad, event) == -EINVAL);
    assert(!strcmp(braille_tables_active(), "ueb1"));
    return 0;
}
