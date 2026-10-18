// SPDX-License-Identifier: MIT

#include <errno.h>
#include <stddef.h>
#include <zephyr/toolchain.h>
#include <zmk/matrix.h>
#include <braille_chord.h>

/* Central behavior callbacks serialize input. No work item or timer owns state. */
static struct held_key {
    uint32_t position;
    uint16_t bit; /* Zero marks a free slot. */
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    uint8_t source;
#endif
} held_keys[ZMK_KEYMAP_LEN];
static size_t held_count;
static uint16_t chord_mask;

/* Matching/output is a later leaf; do not silently discard completed input. */
__weak int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event event) {
    (void)mask;
    (void)event;
    return -ENOTSUP;
}

static bool same_key(const struct held_key *key, struct zmk_behavior_binding_event event) {
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    if (key->source != event.source) {
        return false;
    }
#endif
    return key->position == event.position;
}

int braille_chord_key_event(uint16_t bit, bool pressed, struct zmk_behavior_binding_event event) {
    if (!bit || (bit & ~BRAILLE_CHORD_MASK) || (bit & (bit - 1))) {
        return -EINVAL;
    }

    struct held_key *free_slot = NULL;
    for (size_t i = 0; i < ZMK_KEYMAP_LEN; ++i) {
        struct held_key *key = &held_keys[i];
        if (!key->bit) {
            free_slot = key;
            continue;
        }
        if (!same_key(key, event)) {
            continue;
        }
        /* A layer change must not reinterpret the press-time binding. */
        if (key->bit != bit) {
            return -EINVAL;
        }
        if (pressed) {
            return 0; /* Duplicate notification, not another held key. */
        }
        key->bit = 0;
        if (--held_count) {
            return 0;
        }
        uint16_t completed = chord_mask;
        chord_mask = 0; /* Idle before dispatch, even on failure or reentry. */
        return braille_chord_resolved(completed, event);
    }

    if (!pressed) {
        return 0; /* An unmatched/duplicate release never resolves a chord. */
    }
    if (!free_slot) {
        return -ENOSPC; /* Bounded by the keyboard's physical key count. */
    }
    *free_slot = (struct held_key){
        .position = event.position,
        .bit = bit,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        .source = event.source,
#endif
    };
    ++held_count;
    chord_mask |= bit;
    return 0;
}
