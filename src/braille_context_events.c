// SPDX-License-Identifier: MIT

/* Event observation only: the keymap still routes every edge and the Braille
 * behaviors alone feed capture. Never invoke a binding to classify a key,
 * listen to generated keycodes (our own &kp would reset context), or consume
 * an event. Read live bindings so transparent layers, custom dot/space devices
 * and Studio edits are respected. */
#ifdef __ZEPHYR__
#include <string.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <braille_engine.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/matrix.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/position_state_changed.h>

#define BRAILLE_DEVICE_NAME(node) DEVICE_DT_NAME(node),
static const char *const input_devices[] = {
    DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_dot, BRAILLE_DEVICE_NAME)
    DT_FOREACH_STATUS_OKAY(zmk_behavior_braille_space, BRAILLE_DEVICE_NAME)
    NULL,
};
static const char *const transparent_devices[] = {
    DT_FOREACH_STATUS_OKAY(zmk_behavior_transparent, BRAILLE_DEVICE_NAME)
    NULL,
};

static bool named_in(const char *name, const char *const *devices) {
    for (size_t i = 0; name && devices[i]; ++i) {
        if (strcmp(name, devices[i]) == 0) {
            return true;
        }
    }
    return false;
}

static bool position_is_braille(uint32_t position) {
    if (position >= ZMK_KEYMAP_LEN || position > UINT8_MAX) {
        return false;
    }
    for (int index = ZMK_KEYMAP_LAYERS_LEN - 1; index >= 0; --index) {
        zmk_keymap_layer_id_t id = zmk_keymap_layer_index_to_id(index);
        if (id == ZMK_KEYMAP_LAYER_ID_INVAL || !zmk_keymap_layer_active(id)) {
            continue;
        }
        const struct zmk_behavior_binding *binding = zmk_keymap_get_layer_binding_at_idx(id, position);
        if (!binding || !binding->behavior_dev) {
            return false;
        }
        if (named_in(binding->behavior_dev, transparent_devices)) {
            continue;
        }
        return named_in(binding->behavior_dev, input_devices);
    }
    return false;
}

static int context_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *position = as_zmk_position_state_changed(eh);
    if (as_zmk_layer_state_changed(eh) ||
        (position && position->state && !position_is_braille(position->position))) {
        /* Reset completed/pending engine input, not physical capture. Existing
         * press-time release ownership and release-terminated chord capture
         * survive layer changes; a later completed chord starts in reset context.
         * The settings gate separately discards live capture while it owns input. */
        braille_engine_reset();
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(braille_context, context_listener);
ZMK_SUBSCRIPTION(braille_context, zmk_position_state_changed);
ZMK_SUBSCRIPTION(braille_context, zmk_layer_state_changed);
#endif
