// SPDX-License-Identifier: MIT

#include <errno.h>
#include <zephyr/toolchain.h>
#include <braille_chord.h>

/*
 * Explicit detector boundary, not chord capture. Replace this placeholder in
 * the detector leaf. Weak linkage lets routing tests substitute an edge spy
 * without a test-only switch in the production driver or ZMK-core patches.
 */
__weak int braille_chord_key_event(uint16_t bit, bool pressed,
                                   struct zmk_behavior_binding_event event) {
    (void)bit;
    (void)pressed;
    (void)event;
    return -ENOTSUP;
}
