// SPDX-License-Identifier: MIT
#pragma once
#include <braille_display.h>
#include <braille_editor.h>
#include <stddef.h>
/* Pure, bounded presentation. Page zero always starts at the selected item;
 * subsequent pages retain the complete item and capture/confirmation context.
 */
unsigned braille_small_screen(const struct braille_editor_snapshot *editor,
                              const struct braille_display_snapshot *braille,
                              unsigned columns, unsigned rows, unsigned page,
                              char *out, size_t size);
