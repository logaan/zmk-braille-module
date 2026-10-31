// SPDX-License-Identifier: MIT
#include <assert.h>
#include <string.h>
#include <braille_display.h>
#include <braille_tables.h>
#include <braille_chord.h>
#include <braille_settings.h>
#include <dt-bindings/zmk/keys.h>
static unsigned output;
int braille_engine_commit_rule(const struct braille_rule *rule, enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event) {
    (void)rule; (void)cause; (void)event; ++output; return 0;
}
int main(void) {
    struct braille_display_snapshot s;
    struct zmk_behavior_binding_event e = {.position=0};
    assert(braille_tables_select("computer8") == 0);
    assert(braille_chord_key_event(1, true, e) == 0);
    braille_display_snapshot(&s);
    assert(s.held == 1 && !strcmp(s.preview, "a") && output == 0);
    struct zmk_behavior_binding_event second = {.position=1};
    assert(braille_chord_key_event(2, true, second) == 0);
    braille_display_snapshot(&s);
    assert(s.held == 3 && !strcmp(s.preview, "b"));
    assert(braille_chord_key_event(2, false, second) == 0);
    braille_display_snapshot(&s);
    assert(s.held == 1 && !strcmp(s.preview, "a") && output == 0);
    assert(braille_chord_key_event(1, false, e) == 0);
    assert(output == 1);
    assert(braille_tables_select("ueb1") == 0);
    assert(braille_chord_resolved(0x3c, e) == 0); /* numeric indicator */
    unsigned saved_output = output;
    assert(braille_chord_key_event(1, true, e) == 0);
    struct braille_context before = *braille_engine_context();
    for (int i=0; i<100; ++i) braille_display_snapshot(&s);
    assert(!strcmp(s.preview, "1") && output == saved_output);
    assert(!memcmp(&before, braille_engine_context(), sizeof(before)));
    braille_chord_discard();
    braille_display_snapshot(&s);
    assert(s.held == 0 && !s.preview[0]);
    assert(braille_chord_key_event(1, false, e) == 0);
    char small[5]; uint16_t cells[] = {0xff, 1, 3};
    braille_display_cells(cells, 3, small, sizeof(small));
    assert(!strcmp(small,"1234"));
    assert(braille_tables_select("ueb1") == 0);
    assert(braille_chord_resolved(0x10, e) == 0); /* multi-cell sign prefix */
    braille_display_snapshot(&s);
    assert(s.pending_count == 1 && !strcmp(s.pending,"5"));
    braille_engine_reset();
    braille_display_snapshot(&s);
    assert(!s.pending_count && !s.pending[0]);
    return 0;
}
