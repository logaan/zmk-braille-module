// SPDX-License-Identifier: MIT
#include <errno.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <braille_chord.h>
#include <braille_settings.h>
#include <zmk/keymap.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static unsigned completed, pending, host_output, ui_chords, resets, notices, checkpoints;

void braille_input_reset(void) {
    pending = 0;
    ++resets;
}
void braille_settings_changed(bool active, struct zmk_behavior_binding_event event) {
    (void)event;
    __ASSERT(active == braille_settings_active() && !pending, "Transition ordering");
    ++notices;
}
int braille_settings_chord(uint16_t mask, struct zmk_behavior_binding_event event) {
    (void)event;
    __ASSERT(braille_settings_active() && mask == 2, "Unexpected UI chord");
    ++ui_chords;
    return 0;
}
int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event event) {
    __ASSERT(!braille_settings_active(), "Host consumer called in settings");
    const uint16_t expected[] = {3, 0x100, 1, 0x80, 2};
    __ASSERT(completed < ARRAY_SIZE(expected) && mask == expected[completed], "Wrong chord");
    ++completed;
    if (mask == 1) {
        ++pending;
    } else if (mask == 2 && pending) {
        ++host_output;
    } else if (mask == 0x80) {
        /* Fixture action only, not an alternate production rule representation. */
        struct zmk_behavior_binding action = {
            .behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(bsettings)),
        };
        __ASSERT(zmk_behavior_invoke_binding(&action, event, true) == 0, "Entry failed");
        __ASSERT(zmk_behavior_invoke_binding(&action, event, false) == 0, "Release failed");
        __ASSERT(braille_settings_active(), "Release exited settings");
    }
    return 0;
}

static int no_host_keys(const zmk_event_t *eh) {
    (void)eh;
    __ASSERT(false, "Mode resolved the base conventional key instead of target Braille");
    return ZMK_EV_EVENT_BUBBLE;
}
ZMK_LISTENER(mode_no_host_keys, no_host_keys);
ZMK_SUBSCRIPTION(mode_no_host_keys, zmk_keycode_state_changed);

static int check_pressed(struct zmk_behavior_binding *binding,
                         struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    static const struct {
        unsigned layer, completed, pending, ui_chords, resets;
        bool active;
    } expected[] = {
        {0, 0, 0, 0, 0, false}, {0, 1, 0, 0, 0, false}, {0, 2, 0, 0, 0, false},
        {1, 2, 0, 0, 0, false}, {0, 2, 0, 0, 0, false}, {0, 2, 0, 0, 0, false},
        {0, 3, 1, 0, 0, false}, {1, 4, 0, 0, 1, true},  {1, 4, 0, 1, 1, true},
        {1, 5, 0, 1, 2, false}, {1, 5, 0, 2, 4, false}, {0, 5, 0, 2, 4, false},
    };
    __ASSERT(checkpoints < ARRAY_SIZE(expected), "Extra checkpoint");
    unsigned i = checkpoints++;
    if (i == 0) {
        struct zmk_behavior_binding mode = {.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(bmode)),
                                            .param1 = 1};
        __ASSERT(zmk_behavior_validate_binding(&mode) == 0, "Mode layer metadata missing");
        /* v0.3 layer metadata uses physical key count as its upper bound.
         * Our adapter additionally enforces the actual layer count at runtime. */
        mode.param1 = UINT32_MAX;
        __ASSERT(zmk_behavior_validate_binding(&mode) < 0, "Invalid layer metadata accepted");
        mode.param1 = 2;
        __ASSERT(zmk_behavior_invoke_binding(&mode, event, true) == -EINVAL,
                 "Runtime invalid layer accepted");
        mode.param1 = 1;
        mode.param2 = 1;
        __ASSERT(zmk_behavior_validate_binding(&mode) == -EINVAL, "Unused mode parameter accepted");
        __ASSERT(zmk_behavior_invoke_binding(&mode, event, true) == -EINVAL,
                 "Runtime unused mode parameter accepted");
        struct zmk_behavior_binding settings = {.behavior_dev =
                                                    DEVICE_DT_NAME(DT_NODELABEL(bsettings))};
        __ASSERT(zmk_behavior_validate_binding(&settings) == 0, "Settings metadata missing");
        settings.param1 = 1;
        __ASSERT(zmk_behavior_validate_binding(&settings) < 0,
                 "Unused settings parameter accepted");
        __ASSERT(zmk_behavior_invoke_binding(&settings, event, true) == -EINVAL,
                 "Runtime unused settings parameter accepted");
    }
    __ASSERT(zmk_keymap_highest_layer_active() == expected[i].layer, "Wrong active layer");
    __ASSERT(completed == expected[i].completed && pending == expected[i].pending,
             "Wrong typing state");
    __ASSERT(ui_chords == expected[i].ui_chords && resets == expected[i].resets,
             "Wrong settings routing");
    __ASSERT(notices == resets && !host_output, "Settings leaked output or reset notice");
    __ASSERT(braille_settings_active() == expected[i].active, "Wrong settings state");
    LOG_DBG("mode_settings_checkpoint: %u PASS", i);
    return 0;
}
static int exit_pressed(struct zmk_behavior_binding *binding,
                        struct zmk_behavior_binding_event event) {
    (void)binding;
    return braille_settings_exit(event);
}
static int released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    return 0;
}
static const struct behavior_driver_api check_api = {
    .binding_pressed = check_pressed,
    .binding_released = released,
};
static const struct behavior_driver_api exit_api = {
    .binding_pressed = exit_pressed,
    .binding_released = released,
};
BEHAVIOR_DT_DEFINE(DT_NODELABEL(check), NULL, NULL, NULL, NULL, POST_KERNEL,
                   CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &check_api);
BEHAVIOR_DT_DEFINE(DT_NODELABEL(exit_settings), NULL, NULL, NULL, NULL, POST_KERNEL,
                   CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &exit_api);
