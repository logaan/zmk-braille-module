// SPDX-License-Identifier: MIT
#ifndef BRAILLE_GRADE2_H
#define BRAILLE_GRADE2_H
#include <braille_rules.h>
#define BRAILLE_WORD_MAX_CELLS 32
/* Materialize the longest eligible contraction as ordinary behavior actions.
 * The complete buffered word supplies lookahead; no state is mutated. */
bool braille_grade2_match(const uint16_t *cells, uint8_t count, uint8_t offset,
                          const struct braille_context *ctx, bool after_letter,
                          struct braille_rule *out);
/* Computer8 shorthand supports only shortforms and contextual alphabetic wordsigns. */
bool braille_computer8_short_match(const uint16_t *cells, uint8_t count, uint8_t offset,
                                  bool after_letter, struct braille_rule *out);
bool braille_computer8_cell_is_letter(uint16_t cell);
bool braille_computer8_cell_is_opening(uint16_t cell);
#endif
