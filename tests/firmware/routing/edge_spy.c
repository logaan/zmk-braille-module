// SPDX-License-Identifier: MIT

#include <errno.h>
#include <drivers/behavior.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <braille_chord.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

/*
 * Strong replacement for the unavailable detector, for routing tests only.
 * It records single edges, not accumulated or finalized chords. LOG_DBG uses
 * the same backend as &kp, so the snapshot also checks their relative order.
 */
int braille_chord_key_event(uint16_t bit, bool pressed, struct zmk_behavior_binding_event event) {
    LOG_DBG("braille_edge: bit 0x%03x pressed %d position %u layer %d", bit, pressed,
            event.position, event.layer);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int observe_keycode(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *event = as_zmk_keycode_state_changed(eh);
    LOG_DBG("conventional_key: page 0x%02x usage 0x%02x pressed %d", event->usage_page,
            event->keycode, event->state);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(routing_keycode_spy, observe_keycode);
ZMK_SUBSCRIPTION(routing_keycode_spy, zmk_keycode_state_changed);

static int check_metadata(void) {
    struct zmk_behavior_binding binding = {.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(bc))};
    for (uint32_t dot = 1; dot <= 8; ++dot) {
        binding.param1 = dot;
        __ASSERT(zmk_behavior_validate_binding(&binding) == 0, "Valid dot rejected");
    }
    const uint32_t invalid[] = {0, 9, UINT32_MAX};
    for (size_t i = 0; i < ARRAY_SIZE(invalid); ++i) {
        binding.param1 = invalid[i];
        __ASSERT(zmk_behavior_validate_binding(&binding) == -EINVAL, "Invalid dot accepted");
    }
    binding.param1 = 1;
    binding.param2 = 1;
    __ASSERT(zmk_behavior_validate_binding(&binding) == -EINVAL, "Unused dot parameter accepted");
    binding = (struct zmk_behavior_binding){.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(bspc))};
    __ASSERT(zmk_behavior_validate_binding(&binding) == 0, "Braille space metadata missing");
    binding.param1 = 1;
    __ASSERT(zmk_behavior_validate_binding(&binding) < 0, "Unused space parameter accepted");
    LOG_DBG("routing_metadata: PASS");
    return 0;
}

SYS_INIT(check_metadata, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
