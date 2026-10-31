/* SPDX-License-Identifier: MIT */
#ifndef ZMK_DT_BINDINGS_BRAILLE_H
#define ZMK_DT_BINDINGS_BRAILLE_H

/* Stable keymap arguments, independent of compiled registry order. */
#define BRAILLE_MODE_UEB1 0
#define BRAILLE_MODE_COMPUTER8 1
#define BRAILLE_MODE_UEB2 2
#define BRAILLE_MODE_NEXT 3
/* NEXT is a stable existing action; new modes append after it. */
#define BRAILLE_MODE_COMPUTER8_SHORT 4

#endif
