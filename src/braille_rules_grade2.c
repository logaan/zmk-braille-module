// SPDX-License-Identifier: MIT
#include <braille_grade2.h>
#include <braille_tables.h>
#include <dt-bindings/zmk/keys.h>
#include <string.h>

/* The marker selects word buffering in the common engine. Uncontracted
 * fallback uses the same UEB1 rules, commands, overrides and dispatch. */
/* Formatting indicators have no US-keyboard glyph; they preserve reading
 * context while allowing standing-alone contractions inside their scopes. */
#define SILENT2(a, b) {.chords = {a, b}, .chord_count = 2, .class = BRAILLE_CLASS_NEUTRAL}
#define SILENT3(a, b, c) {.chords = {a, b, c}, .chord_count = 3, .class = BRAILLE_CLASS_NEUTRAL}
#define TYPEFORM(p) SILENT2(p, 2), SILENT2(p, 6), SILENT2(p, 54), SILENT2(p, 4)
static const struct braille_rule formatting[] = {
    TYPEFORM(8),        TYPEFORM(24),       TYPEFORM(40),       TYPEFORM(56),
    SILENT3(8, 60, 2),  SILENT3(8, 60, 6),  SILENT3(8, 60, 54), SILENT3(8, 60, 4),
    SILENT3(8, 40, 35), SILENT3(8, 40, 28),
};
const struct braille_rule_table braille_rules_ueb2_characters = {
    .rules = formatting, .count = sizeof(formatting) / sizeof(*formatting)};
enum condition {
    G2_ALPHA,
    G2_WORD,
    G2_LOWER_WORD,
    G2_ANY,
    G2_START,
    G2_MIDDLE,
    G2_FINAL,
    G2_SHORT
};
struct contraction {
    uint16_t cells[5];
    uint8_t count;
    enum condition condition;
    const char *text;
};
static const struct contraction contractions[] = {
#include "braille_grade2_data.inc"
};
static const char *const longer_words[] = {
#include "braille_grade2_shortforms.inc"
};
static const uint16_t letters[] = {1,  3,  9,  25, 17, 11, 27, 19, 10, 26, 5,  7,  13,
                                   29, 21, 15, 31, 23, 14, 30, 37, 39, 58, 45, 61, 53};
static char letter(uint16_t c) {
    for (unsigned i = 0; i < 26; ++i)
        if (letters[i] == c)
            return 'a' + i;
    return 0;
}
static bool matches(const struct contraction *r, const uint16_t *s, unsigned n) {
    return n >= r->count && !memcmp(r->cells, s, r->count * sizeof(*s));
}
static bool lower(uint16_t c) {
    return c == 4 || c == 2 || c == 6 || c == 18 || c == 50 || c == 22 || c == 38 || c == 52 ||
           c == 54 || c == 36;
}
/* ICEB 2.6: punctuation is accepted only when it reaches a real boundary.
 * A slash or another letter after punctuation must not create a wordsign. */
static bool right_boundary(const uint16_t *s, unsigned n) {
    unsigned i = 0;
    while (i < n) {
        uint16_t c = s[i];
        if (c == 36)
            return true;
        if (c == 32 && i + 1 < n && s[i + 1] == 36)
            return true;
        if (c == 16 && i + 2 < n && s[i + 1] == 32 && s[i + 2] == 36)
            return true;
        if (i + 1 < n && (c == 8 || c == 24 || c == 40 || c == 56) && s[i + 1] == 4) {
            i += 2;
            continue;
        }
        if (i + 2 < n && c == 8 &&
            ((s[i + 1] == 60 && s[i + 2] == 4) || (s[i + 1] == 40 && s[i + 2] == 28))) {
            i += 3;
            continue;
        }
        if (lower(c) || c == 4) {
            ++i;
            continue;
        }
        if (i + 1 < n && ((c == 16 || c == 40 || c == 56) && s[i + 1] == 28)) {
            i += 2;
            continue;
        }
        if (i + 1 < n && c == 32 && (s[i + 1] == 4 || s[i + 1] == 52 || s[i + 1] == 54)) {
            i += 2;
            continue;
        }
        return false;
    }
    return true;
}
static bool left_boundary(const uint16_t *s, unsigned pos) {
    unsigned i = pos;
    while (i) {
        uint16_t c = s[i - 1];
        if (c == 36)
            return true;
        if (i >= 3 && s[i - 3] == 8 &&
            ((s[i - 2] == 60 && (c == 2 || c == 6 || c == 54)) || (s[i - 2] == 40 && c == 35))) {
            i -= 3;
            continue;
        }
        if (i >= 2 && (s[i - 2] == 8 || s[i - 2] == 24 || s[i - 2] == 40 || s[i - 2] == 56) &&
            (c == 2 || c == 6 || c == 54)) {
            i -= 2;
            continue;
        }
        if (c == 32 || c == 38 || c == 4) {
            --i;
            continue;
        }
        if (i >= 2 && (s[i - 2] == 16 || s[i - 2] == 40 || s[i - 2] == 56) && c == 35) {
            i -= 2;
            continue;
        }
        if (i >= 2 && s[i - 2] == 32 && (c == 38 || c == 54)) {
            i -= 2;
            continue;
        }
        return false;
    }
    return true;
}
static bool alphabetic(const uint16_t *s, unsigned n) {
    while (n && *s == 32) {
        ++s;
        --n;
    }
    if (!n)
        return false;
    if (letter(*s))
        return true;
    for (unsigned i = 0; i < sizeof(contractions) / sizeof(*contractions); ++i) {
        const struct contraction *r = &contractions[i];
        if ((r->condition == G2_ANY || r->condition == G2_SHORT) && matches(r, s, n))
            return true;
    }
    return false;
}
static bool apostrophe_suffix(const uint16_t *s, unsigned n, bool shortform) {
    if (n < 2 || s[0] != 4)
        return false;
    static const char *const endings[] = {"d", "ll", "re", "s", "t", "ve"};
    for (unsigned k = 0; k < sizeof(endings) / sizeof(*endings); ++k) {
        if (shortform && strcmp(endings[k], "s"))
            continue;
        unsigned j = 0;
        while (j + 1 < n && endings[k][j] && letter(s[j + 1]) == endings[k][j])
            ++j;
        if (!endings[k][j] && right_boundary(s + j + 1, n - j - 1))
            return true;
    }
    return false;
}
/* Match a known Appendix 1 word without greedily expanding unrelated
 * shortform-looking letters. The bounded DP explores both literal letters
 * and contractions, requiring the candidate under consideration. */
static bool dictionary_match(const uint16_t *s, unsigned start, unsigned end, unsigned wanted_pos,
                             const struct contraction *wanted, const char *text) {
    size_t length = strlen(text);
    if (length > 63 || end > 32)
        return false;
    uint64_t reachable[33] = {0};
    reachable[start] = 1;
    for (unsigned i = start; i < end; ++i)
        for (unsigned j = 0; j <= length; ++j) {
            if (!(reachable[i] & (UINT64_C(1) << j)))
                continue;
            unsigned p = i;
            bool upper = false;
            if (s[p] == 32 && p + 1 < end) {
                upper = p > start;
                ++p;
            }
            for (unsigned k = 0; k <= sizeof(contractions) / sizeof(*contractions); ++k) {
                const struct contraction *r =
                    k < sizeof(contractions) / sizeof(*contractions) ? &contractions[k] : NULL;
                char one[2] = {letter(s[p]), 0};
                if (s[p] == 4)
                    one[0] = '\'';
                if (s[p] == 36)
                    one[0] = '-';
                const char *out = r ? r->text : one;
                unsigned n = r ? r->count : 1;
                if (p == wanted_pos && r != wanted)
                    continue;
                if (p < wanted_pos && p + n > wanted_pos)
                    continue;
                if (r) {
                    if (!matches(r, s + p, end - p))
                        continue;
                    if ((r->condition == G2_ALPHA || r->condition == G2_WORD ||
                         r->condition == G2_LOWER_WORD) &&
                        !(left_boundary(s, p) && right_boundary(s + p + n, end - p - n)))
                        continue;
                    if (r->condition == G2_FINAL && p == start)
                        continue;
                    if (r->condition == G2_START && p != start)
                        continue;
                    if (r->condition == G2_MIDDLE && (p == start || p + n == end))
                        continue;
                } else if (!one[0])
                    continue;
                unsigned len = strlen(out);
                if (j + len > length)
                    continue;
                bool same = true;
                for (unsigned q = 0; q < len; ++q) {
                    char expected = out[q], actual = text[j + q];
                    if (upper && q == 0)
                        expected -= 32;
                    if (j + q == 0 && actual >= 'A' && actual <= 'Z')
                        actual += 32;
                    if (expected != actual) {
                        same = false;
                        break;
                    }
                }
                if (same)
                    reachable[p + n] |= UINT64_C(1) << (j + len);
            }
        }
    return (reachable[end] & (UINT64_C(1) << length)) != 0;
}

/* Build a tentative letters-sequence for Appendix 1 membership. This pass
 * expands shortforms provisionally; eligibility is checked before emission.
 * The 328 entries remain lexical data, never oversized input rules. */
static bool shortform_longer(const uint16_t *s, unsigned n, unsigned pos,
                             const struct contraction *wanted) {
    unsigned start = pos, end = pos + wanted->count;
    while (start && s[start - 1] != 36)
        --start;
    while (end < n && !right_boundary(s + end, n - end))
        ++end;
    for (unsigned i = start; i <= pos; ++i)
        if (left_boundary(s, i) && !(i && s[i - 1] == 4))
            start = i;
    if (!left_boundary(s, start) || !right_boundary(s + end, n - end))
        return false;
    /* Exact list membership also permits the rule 10.9.5 suffixes. */
    for (unsigned k = 0; k < sizeof(longer_words) / sizeof(*longer_words); ++k) {
        char folded[64];
        size_t fold_len = strlen(longer_words[k]);
        if (fold_len >= sizeof(folded))
            continue;
        for (unsigned f = 0; f <= fold_len; ++f) {
            char c = longer_words[k][f];
            folded[f] = (c >= 'A' && c <= 'Z') ? c + 32 : c;
        }
        if (!strstr(folded, wanted->text))
            continue;
        if (dictionary_match(s, strchr(longer_words[k], '-') ? 0 : start, end, pos, wanted,
                             longer_words[k]))
            return true;
        char extended[64];
        size_t len = strlen(longer_words[k]);
        if (len + 3 < sizeof(extended)) {
            memcpy(extended, longer_words[k], len);
            strcpy(extended + len, "s");
            if (dictionary_match(s, start, end, pos, wanted, extended))
                return true;
            strcpy(extended + len, "'s");
            if (dictionary_match(s, start, end, pos, wanted, extended))
                return true;
        }
    }
    char text[192];
    unsigned used = 0, at = 0, after = 0;
    bool interior_caps = false;
    for (unsigned i = start; i < end;) {
        if (s[i] == 32) {
            if (i > start)
                interior_caps = true;
            ++i;
            continue;
        }
        if (s[i] == 4) {
            text[used++] = '\'';
            ++i;
            continue;
        }
        const struct contraction *best = NULL;
        for (unsigned k = 0; k < sizeof(contractions) / sizeof(*contractions); ++k) {
            const struct contraction *r = &contractions[k];
            if (!matches(r, s + i, end - i))
                continue;
            if (r->condition == G2_ALPHA || r->condition == G2_WORD ||
                r->condition == G2_LOWER_WORD)
                continue;
            if (r->condition == G2_FINAL && i == start)
                continue;
            if (r->condition == G2_START && i != start)
                continue;
            if (r->condition == G2_MIDDLE && (i == start || i + r->count == end))
                continue;
            if (!best || r->count > best->count ||
                (r->count == best->count && r->condition == G2_SHORT))
                best = r;
        }
        if (i == pos)
            best = wanted;
        if (i == pos)
            at = used;
        const char *out = best ? best->text : NULL;
        char ch = letter(s[i]);
        if (!out && !ch)
            return false;
        unsigned len = out ? strlen(out) : 1;
        if (used + len >= sizeof(text))
            return false;
        if (out)
            memcpy(text + used, out, len);
        else
            text[used] = ch;
        used += len;
        if (i == pos)
            after = used;
        i += best ? best->count : 1;
    }
    text[used] = 0;
    /* 10.9.3 unrestricted and positional extensions. */
    const char *word = wanted->text;
    bool vowel = strchr("aeiouy", text[after]) != NULL && text[after];
    if (!strcmp(word, "braille") || !strcmp(word, "great"))
        return true;
    if (!strcmp(word, "children") && !vowel)
        return true;
    if (at == 0 && !vowel &&
        (!strcmp(word, "blind") || !strcmp(word, "first") || !strcmp(word, "friend") ||
         !strcmp(word, "good") || !strcmp(word, "letter") || !strcmp(word, "little") ||
         !strcmp(word, "quick")))
        return true;
    /* 10.9.5 suffixes, with the three explicit exceptions. */
    if (!strcmp(text, "abouts") || !strcmp(text, "almosts") || !strcmp(text, "hims"))
        return false;
    if (at == 0 && (!strcmp(text + after, "s") || !strcmp(text + after, "'s")))
        return true;
    if (interior_caps)
        return false; /* conservative except the 10.9.3 cases above */
    for (unsigned k = 0; k < sizeof(longer_words) / sizeof(*longer_words); ++k) {
        size_t len = strlen(longer_words[k]);
        if (!strncmp(text, longer_words[k], len) &&
            (!text[len] || !strcmp(text + len, "s") || !strcmp(text + len, "'s")))
            return true;
    }
    return false;
}

bool braille_grade2_match(const uint16_t *s, uint8_t count, uint8_t offset,
                          const struct braille_context *ctx, bool after_letter,
                          struct braille_rule *out) {
    if (ctx->grade1 || ctx->grade1_symbol || ctx->numeric || offset >= count)
        return false;
    unsigned pos = offset, prefix = 0;
    if (s[pos] == 32 && pos + 1 < count && s[pos + 1] != 32 && s[pos + 1] != 4) {
        ++pos;
        prefix = 1;
    }
    const struct contraction *best = NULL;
    for (unsigned i = 0; i < sizeof(contractions) / sizeof(*contractions); ++i) {
        const struct contraction *r = &contractions[i];
        if (!matches(r, s + pos, count - pos))
            continue;
        unsigned next = pos + r->count;
        bool left = left_boundary(s, offset), right = right_boundary(s + next, count - next);
        bool alone = left && right;
        bool before = alphabetic(s + next, count - next);
        bool eligible = false;
        switch (r->condition) {
        case G2_ALPHA:
        case G2_WORD:
            eligible = left && (right || apostrophe_suffix(s + next, count - next, false));
            break;
        case G2_LOWER_WORD:
            eligible = alone;
            if (strcmp(r->text, "enough") && strcmp(r->text, "in"))
                eligible = eligible && (!offset || !lower(s[offset - 1])) &&
                           (next == count || !lower(s[next]));
            break;
        case G2_ANY:
            eligible = true;
            break;
        case G2_START:
            eligible = left && before;
            break;
        case G2_MIDDLE:
            eligible = after_letter && before;
            break;
        case G2_FINAL:
            eligible = after_letter && !prefix;
            break;
        case G2_SHORT:
            eligible = alone || (left && apostrophe_suffix(s + next, count - next, true)) ||
                       shortform_longer(s, count, pos, r);
            break;
        }
        if (eligible && (!best || r->count > best->count ||
                         (r->count == best->count && r->condition == G2_SHORT)))
            best = r;
    }
    if (!best)
        return false;
    *out = (struct braille_rule){.chord_count = best->count + prefix,
                                 .class =
                                     prefix ? BRAILLE_CLASS_CAPITAL_LETTER : BRAILLE_CLASS_LETTER};
    memcpy(out->chords, s + offset, out->chord_count * sizeof(*s));
    out->action_count = strlen(best->text);
    for (unsigned i = 0; i < out->action_count; ++i) {
        uint32_t key = (0x07UL << 16) | (best->text[i] - 'a' + 4);
        if (ctx->caps != BRAILLE_CAPS_NONE || (prefix && i == 0))
            key = LS(key);
        out->actions[i] = (struct zmk_behavior_binding){
            .behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)), .param1 = key};
    }
    return true;
}

#ifdef CONFIG_ZMK_BRAILLE_COMPUTER8_SHORT
/* Separate marker: the common word resolver falls back to Computer8, not UEB1.
 * The raw 120 multi-cell spellings are intentional sequence ambiguities; all
 * 62 single-cell contractions and every silent UEB indicator are excluded. */
const struct braille_rule_table braille_rules_computer8_short_characters = {0};

static char computer8_ascii(uint16_t cell) {
    /* The production table is in complete printable ASCII order (33..126).
     * Reuse its actual masks rather than introducing a second dot mapping. */
    for (size_t i = 0; i < braille_rules_computer8_characters.count; ++i)
        if (braille_rules_computer8_characters.rules[i].chords[0] == cell)
            return (char)(33 + i);
    return 0;
}
bool braille_computer8_cell_is_letter(uint16_t cell) {
    char c = computer8_ascii(cell);
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
bool braille_computer8_cell_is_opening(uint16_t cell) {
    char c = computer8_ascii(cell);
    return c && strchr("\"'([{", c);
}
static bool c8_left(const uint16_t *s, unsigned pos) {
    while (pos) {
        char c = computer8_ascii(s[--pos]);
        if (c == '-') return true;
        if (!c || !strchr("\"'([{", c)) return false;
    }
    return true;
}
static bool c8_right(const uint16_t *s, unsigned n) {
    for (unsigned i = 0; i < n; ++i) {
        char c = computer8_ascii(s[i]);
        if (c == '-') return true;
        if (!c || !strchr("\"')]}.,;:!?", c)) return false;
    }
    return true;
}
static bool c8_possessive(const uint16_t *s, unsigned n) {
    return n >= 2 && computer8_ascii(s[0]) == '\'' && computer8_ascii(s[1]) == 's' &&
           c8_right(s + 2, n - 2);
}
/* Appendix membership explores only the same multi-cell spellings the hybrid
 * may emit. It never erases commas as UEB capitals or reads digits as lower
 * groupsigns. Fixed 33x64-bit reachability bounds memory and execution. */
static bool c8_dictionary(const uint16_t *s, unsigned start, unsigned end,
                          unsigned wanted_pos, const struct contraction *wanted,
                          const char *text) {
    size_t length = strlen(text);
    if (length > 63 || end > BRAILLE_WORD_MAX_CELLS) return false;
    uint64_t reachable[BRAILLE_WORD_MAX_CELLS + 1] = {0};
    reachable[start] = 1;
    for (unsigned i = start; i < end; ++i)
        for (unsigned j = 0; j <= length; ++j) {
            if (!(reachable[i] & (UINT64_C(1) << j))) continue;
            for (unsigned k = 0; k <= sizeof(contractions) / sizeof(*contractions); ++k) {
                const struct contraction *r = k < sizeof(contractions) / sizeof(*contractions)
                    ? &contractions[k] : NULL;
                unsigned n = r ? r->count : 1;
                if (i == wanted_pos && r != wanted) continue;
                if (i < wanted_pos && i + n > wanted_pos) continue;
                if (r && (r->count < 2 || !matches(r, s + i, end - i))) continue;
                if (r && r->condition == G2_FINAL && i == start) continue;
                char one[2] = {computer8_ascii(s[i]), 0};
                if (!r && !one[0]) continue;
                const char *out = r ? r->text : one;
                size_t len = strlen(out);
                if (j + len <= length && !memcmp(text + j, out, len))
                    reachable[i + n] |= UINT64_C(1) << (j + len);
            }
        }
    return (reachable[end] & (UINT64_C(1) << length)) != 0;
}
static bool c8_short_longer(const uint16_t *s, unsigned count, unsigned pos,
                            const struct contraction *wanted) {
    unsigned start = pos, end = pos + wanted->count;
    while (start && computer8_ascii(s[start - 1]) != '-') --start;
    while (start < pos && c8_left(s, start + 1)) ++start;
    while (end < count && !c8_right(s + end, count - end)) ++end;
    if (!c8_left(s, start) || !c8_right(s + end, count - end)) return false;
    /* Exact Appendix entries and their plural/possessive suffixes. */
    for (unsigned k = 0; k < sizeof(longer_words) / sizeof(*longer_words); ++k) {
        if (!strstr(longer_words[k], wanted->text)) continue;
        unsigned begin = strchr(longer_words[k], '-') || longer_words[k][0] == '\'' ? 0 : start;
        if (c8_dictionary(s, begin, end, pos, wanted, longer_words[k])) return true;
        char extended[64];
        size_t len = strlen(longer_words[k]);
        if (len + 3 >= sizeof(extended)) continue;
        memcpy(extended, longer_words[k], len);
        strcpy(extended + len, "s");
        if (c8_dictionary(s, begin, end, pos, wanted, extended)) return true;
        strcpy(extended + len, "'s");
        if (c8_dictionary(s, begin, end, pos, wanted, extended)) return true;
    }
    /* 10.9.3 productive extensions are eligible with Computer8 literal
     * suffixes. No case folding changes the meaning of 8-dot capitals. */
    char text[192];
    unsigned used = 0, at = 0, after = 0;
    for (unsigned i = start; i < end;) {
        const struct contraction *best = NULL;
        for (unsigned k = 0; k < sizeof(contractions) / sizeof(*contractions); ++k) {
            const struct contraction *r = &contractions[k];
            if (r->count < 2 || !matches(r, s + i, end - i) ||
                (r->condition == G2_FINAL && i == start)) continue;
            if (!best || r->count > best->count ||
                (r->count == best->count && r->condition == G2_SHORT)) best = r;
        }
        if (i == pos) best = wanted;
        char one[2] = {computer8_ascii(s[i]), 0};
        const char *out = best ? best->text : one;
        if (!out[0]) return false;
        size_t len = strlen(out);
        if (used + len >= sizeof(text)) return false;
        if (i == pos) at = used;
        memcpy(text + used, out, len); used += len;
        if (i == pos) after = used;
        i += best ? best->count : 1;
    }
    text[used] = 0;
    bool vowel = text[after] && strchr("aeiouyAEIOUY", text[after]);
    const char *word = wanted->text;
    if (!strcmp(word, "braille") || !strcmp(word, "great")) return true;
    if (!strcmp(word, "children") && !vowel) return true;
    if (at == 0 && !vowel && (!strcmp(word, "blind") || !strcmp(word, "first") ||
        !strcmp(word, "friend") || !strcmp(word, "good") || !strcmp(word, "letter") ||
        !strcmp(word, "little") || !strcmp(word, "quick"))) return true;
    if (!strcmp(text, "abouts") || !strcmp(text, "almosts") || !strcmp(text, "hims")) return false;
    return at == 0 && (!strcmp(text + after, "s") || !strcmp(text + after, "'s"));
}

bool braille_computer8_short_match(const uint16_t *s, uint8_t count, uint8_t offset,
                                  bool after_letter, struct braille_rule *out) {
    if (offset >= count) return false;
    const struct contraction *best = NULL;
    for (unsigned i = 0; i < sizeof(contractions) / sizeof(*contractions); ++i) {
        const struct contraction *r = &contractions[i];
        if (r->count < 2 || !matches(r, s + offset, count - offset)) continue;
        unsigned next = offset + r->count;
        bool eligible = r->condition == G2_ANY ||
            (r->condition == G2_FINAL && after_letter);
        if (r->condition == G2_SHORT)
            eligible = (c8_left(s, offset) && (c8_right(s + next, count - next) ||
                         c8_possessive(s + next, count - next))) ||
                       c8_short_longer(s, count, offset, r);
        if (eligible && (!best || r->count > best->count ||
            (r->count == best->count && r->condition == G2_SHORT))) best = r;
    }
    if (!best) return false;
    *out = (struct braille_rule){.chord_count = best->count,
                                 .class = BRAILLE_CLASS_LETTER};
    memcpy(out->chords, s + offset, out->chord_count * sizeof(*s));
    out->action_count = strlen(best->text);
    for (unsigned i = 0; i < out->action_count; ++i)
        out->actions[i] = (struct zmk_behavior_binding){
            .behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)),
            .param1 = (0x07UL << 16) | (best->text[i] - 'a' + 4)};
    return true;
}
#endif /* CONFIG_ZMK_BRAILLE_COMPUTER8_SHORT */
