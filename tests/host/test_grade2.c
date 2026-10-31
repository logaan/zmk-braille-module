// SPDX-License-Identifier: MIT
#define main ueb1_test_main
#include "test_ueb1.c"
#undef main
int main(int argc, char **argv) {
    assert(argc > 1);
    assert(!braille_tables_select(argv[1]));
    for (int i = 2; i < argc; ++i) {
        if (!strcmp(argv[i], "override")) {
            assert(i + 2 < argc);
            struct braille_override_layer layer = braille_tables_overrides();
            struct braille_rule *r = &layer.rules[(*layer.count)++];
            memset(r, 0, sizeof(*r));
            r->class = BRAILLE_CLASS_NEUTRAL;
            for (char *token = strtok(argv[++i], ","); token; token = strtok(NULL, ","))
                r->chords[r->chord_count++] = strtoul(token, NULL, 0);
            r->action_count = 1;
            r->actions[0] = (struct zmk_behavior_binding){.behavior_dev = "kp",
                                                          .param1 = KEY(argv[++i][0] - 'a' + 4)};
            continue;
        }
        if (!strcmp(argv[i], "preview")) {
            assert(i + 1 < argc);
            uint8_t before = braille_engine_pending_count();
            struct braille_context ctx = *braille_engine_context();
            struct braille_rule rule;
            bool waiting = false;
            size_t old_keys = key_count;
            int found = braille_engine_preview_rule(strtoul(argv[++i], NULL, 0), &rule, &waiting);
            assert(braille_engine_pending_count() == before);
            assert(!memcmp(&ctx, braille_engine_context(), sizeof(ctx)));
            assert(key_count == old_keys);
            if (found > 0)
                for (unsigned a = 0; a < rule.action_count; ++a)
                    keys[key_count++] = rule.actions[a].param1;
            continue;
        }
        if (!strcmp(argv[i], "reset")) {
            braille_engine_reset();
            continue;
        }
        if (!strcmp(argv[i], "ueb1") || !strcmp(argv[i], "computer8") || !strcmp(argv[i], "ueb2")) {
            assert(!braille_tables_select(argv[i]));
            continue;
        }
        chord(strtoul(argv[i], NULL, 0));
    }
    print_observation();
}
