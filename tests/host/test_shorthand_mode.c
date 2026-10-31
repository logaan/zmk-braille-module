// SPDX-License-Identifier: MIT
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <braille_chord.h>
#include <braille_display.h>
#include <braille_storage.h>
#include <braille_tables.h>
static char saved_mode[32];
static unsigned writes, outputs;
int braille_storage_write_mode(const char *name) {
    strcpy(saved_mode, name); ++writes; return 0;
}
int braille_engine_commit_rule(const struct braille_rule *rule,
                               enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event) {
    (void)cause; (void)event; outputs += rule->action_count; return 0;
}
int main(void) {
    struct zmk_behavior_binding_event event = {0}, second = {.position = 1};
    struct braille_display_snapshot display;
    braille_storage_begin_load();
    assert(!braille_storage_load_mode("computer8_short"));
#ifdef CONFIG_ZMK_BRAILLE_COMPUTER8_SHORT
    assert(!strcmp(braille_tables_active(), "computer8_short"));
    assert(braille_tables_count() == 4);
    assert(braille_tables_find("computer8_short")->pending_limit == 32);
#else
    assert(!strcmp(braille_tables_active(), "ueb1"));
    assert(!braille_tables_find("computer8_short"));
#endif
    assert(!writes); /* Boot restoration never writes flash. */
    braille_storage_finish_load();
#ifdef CONFIG_ZMK_BRAILLE_COMPUTER8_SHORT
    assert(!braille_tables_select("computer8"));
    assert(!braille_tables_select("computer8_short"));
    assert(writes == 2 && !strcmp(saved_mode, "computer8_short"));
    /* Preview the held b after pending a: no host output or context mutation. */
    assert(!braille_chord_resolved(1, event));
    assert(!braille_chord_key_event(2, true, second));
    braille_display_snapshot(&display);
    assert(!strcmp(display.mode, "computer8_short"));
    assert(strlen(display.mode) == 15);
    assert(!strcmp(braille_display_mode_label(display.mode), "8dot+short"));
    assert(display.held == 2 && display.pending_count == 1 && !outputs);
    /* Add dot1 while dot2 is held, producing held b. */
    assert(!braille_chord_key_event(1, true, event));
    braille_display_snapshot(&display);
    assert(display.held == 3 && !strcmp(display.preview, "about") && display.waiting);
    assert(braille_engine_pending_count() == 1 && !outputs);
    unsigned previous_writes = writes;
    char persisted[32]; strcpy(persisted, saved_mode);
    braille_chord_discard();
    braille_storage_begin_load();
    assert(!braille_storage_load_mode(persisted));
    braille_storage_finish_load();
    assert(writes == previous_writes && !strcmp(braille_tables_active(), persisted));
    assert(braille_engine_pending_count() == 0);
    assert(!braille_chord_key_event(1, false, event));
    assert(!braille_chord_key_event(2, false, second));
    assert(!outputs);
#else
    (void)event; (void)second;
    braille_display_snapshot(&display);
    assert(!strcmp(display.mode, "ueb1"));
    assert(braille_tables_select("computer8_short") == -ENOENT);
    assert(!writes && !outputs);
#endif
    return 0;
}
