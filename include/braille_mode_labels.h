// SPDX-License-Identifier: MIT
#pragma once
#include <stdbool.h>
#include <string.h>

/* Keep stable storage identifiers separate from labels sized for the panel. */
static inline const char *braille_display_mode_label(const char *name) {
    return name && !strcmp(name, "computer8_short") ? "8dot+short" : name ? name : "--";
}
static inline bool braille_display_computer8_mode(const char *name) {
    return name && (!strcmp(name, "computer8") || !strcmp(name, "computer8_short"));
}
