// SPDX-License-Identifier: MIT
#pragma once
#include <stddef.h>
#include <stdint.h>
#include <braille_mode_labels.h>
#include <braille_rules.h>

/* Read-only rendering. Call in the same serialized context as braille input.
 * No behavior is invoked and no pending/context state is changed. */
#define BRAILLE_DISPLAY_TEXT_SIZE 96
struct braille_display_snapshot {
    uint16_t held;
    uint8_t pending_count;
    bool waiting;
    char mode[16];
    char preview[BRAILLE_DISPLAY_TEXT_SIZE];
    /* 32 cells, each eight dots plus space marker and separator. */
    char pending[321];
};
void braille_display_snapshot(struct braille_display_snapshot *out);
void braille_display_cells(const uint16_t *cells, size_t count, char *out, size_t size);
void braille_display_rule(const struct braille_rule *rule, char *out, size_t size);
