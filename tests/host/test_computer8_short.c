// SPDX-License-Identifier: MIT
#include <errno.h>
#define zmk_behavior_invoke_binding record_key_binding
#define main ueb1_test_main
#include "test_ueb1.c"
#undef main
#undef zmk_behavior_invoke_binding
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                               struct zmk_behavior_binding_event event, bool pressed) {
    struct zmk_behavior_binding copy = *binding;
    if (!strcmp(copy.behavior_dev, "sk")) {
        copy.behavior_dev = "kp";
        copy.param1 |= 0x80000000u; /* distinguish sticky actions in observations */
    }
    return record_key_binding(&copy, event, pressed);
}
int main(int argc, char **argv) {
    assert(argc > 1);
    assert(!braille_tables_select(argv[1]));
    if ((!strcmp(argv[1], "ueb2") || !strcmp(argv[1], "computer8_short"))) {
        assert(!braille_engine_set_pending_limit(32));
        assert(braille_engine_set_pending_limit(33) == -EINVAL);
    } else {
        assert(braille_engine_set_pending_limit(9) == -EINVAL);
        assert(braille_engine_set_pending_limit(32) == -EINVAL);
    }
    for (int i = 2; i < argc; ++i) {
        if (!strncmp(argv[i], "override", 8)) {
            assert(i + 2 < argc);
            struct braille_override_layer layer = braille_tables_overrides();
            struct braille_rule *r = &layer.rules[(*layer.count)++];
            memset(r, 0, sizeof(*r));
            r->class = BRAILLE_CLASS_NEUTRAL;
            r->word_position = !strcmp(argv[i], "override-start") ? BRAILLE_WORD_POSITION_START :
                !strcmp(argv[i], "override-middle") ? BRAILLE_WORD_POSITION_MIDDLE :
                !strcmp(argv[i], "override-end") ? BRAILLE_WORD_POSITION_END : BRAILLE_WORD_POSITION_ANY;
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
        if (!strcmp(argv[i], "ueb1") || !strcmp(argv[i], "computer8") || !strcmp(argv[i], "ueb2") || !strcmp(argv[i], "computer8_short")) {
            assert(!braille_tables_select(argv[i]));
            continue;
        }
        uint16_t cell = strtoul(argv[i], NULL, 0);
        if (cell) chord(cell);
        else assert(!braille_chord_resolved(0, (struct zmk_behavior_binding_event){0}));
    }
    print_observation();
}
