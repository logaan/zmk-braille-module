// SPDX-License-Identifier: MIT

#include <braille_dispatch.h>

/*
 * Ordered ZMK behavior dispatch (see include/braille_dispatch.h). Every
 * committed rule — letter, space, navigation, or custom — runs the same
 * loop: each binding pressed then released, in rule order, with its
 * behavior device and both parameters and the chord-resolution event
 * forwarded unchanged. Modifiers are ordinary ZMK `&sk` sticky-key actions:
 * a sticky key's own release leaves it armed until the next host key
 * release, so a modifier action naturally applies to the next action's or
 * the next rule's output with no engine-specific modifier lifecycle.
 *
 * Nothing here branches on characters, commands, or chord masks: the rule's
 * input cells are match-time data and dispatch never reads them, so
 * space-only, dot-only, and space-plus-dot sequences are handled
 * identically.
 */

int braille_dispatch_rule(const struct braille_rule *rule,
                          struct zmk_behavior_binding_event event) {
    for (uint8_t i = 0; i < rule->action_count; ++i) {
        int ret = zmk_behavior_invoke_binding(&rule->actions[i], event, true);
        if (ret < 0) {
            return ret;
        }
        ret = zmk_behavior_invoke_binding(&rule->actions[i], event, false);
        if (ret < 0) {
            return ret;
        }
    }
    return 0;
}

/* Strong override of the matcher's weak commit hook: the engine's single
 * host-output path. The phase-05 settings/search flow captures rules
 * without ever reaching this function, so captured input never executes on
 * the host. */
int braille_engine_commit_rule(const struct braille_rule *rule, enum braille_commit_cause cause,
                               struct zmk_behavior_binding_event event) {
    (void)cause; /* Context/lifecycle are owned by the shared matcher. */
    return braille_dispatch_rule(rule, event);
}
