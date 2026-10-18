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
 * Until that implementation lands, the weak placeholder returns -ENOTSUP.
 */
int braille_chord_key_event(uint16_t bit, bool pressed, struct zmk_behavior_binding_event event);

#endif /* BRAILLE_CHORD_H */
