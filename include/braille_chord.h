// SPDX-License-Identifier: MIT

#ifndef BRAILLE_CHORD_H
#define BRAILLE_CHORD_H

#include <stdbool.h>
#include <stdint.h>
#include <zmk/behavior.h>

/* Dots 1..8 occupy bits 0..7; Braille space is bit 8 (not HID SPACE). */
#define BRAILLE_CHORD_SPACE_BIT UINT16_C(0x0100)
#define BRAILLE_CHORD_MASK UINT16_C(0x01ff)

/*
 * Input boundary for the release-terminated detector. `bit` is exactly one
 * validated dot/space bit. Each callback forwards the original ZMK event,
 * including position, layer, timestamp, and split source when present.
 * Position/source identify the key; layer is not a filter or an on/off state.
 *
 * Return 0 (opaque/handled) or a negative errno, never a transparent result.
 * The detector owns held-key/chord state and finalization, not the driver.
 * Callbacks must be serialized, as in ZMK's central behavior event path.
 * Held keys are identified by position and (on splits) source, not layer.
 * Duplicate edges are ignored; inconsistent bits for a held key are rejected.
 * At most ZMK_KEYMAP_LEN distinct keys may be held; overflow returns -ENOSPC
 * without changing the chord. Invalid/non-single-bit input returns -EINVAL.
 */
int braille_chord_key_event(uint16_t bit, bool pressed, struct zmk_behavior_binding_event event);

/*
 * One resolution per last release, with all bits pressed since idle and the
 * unchanged last-release event. State is idle before this synchronous call.
 * Return 0 or a negative errno (propagated to the releasing behavior); failures
 * are not retried and cannot contaminate the next chord. The weak default
 * returns -ENOTSUP until the shared engine supplies this completion hook.
 * This is a capture boundary, not an alternate character/action representation.
 */
int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event event);

#endif /* BRAILLE_CHORD_H */
