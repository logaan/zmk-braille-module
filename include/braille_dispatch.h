// SPDX-License-Identifier: MIT

#ifndef BRAILLE_DISPATCH_H
#define BRAILLE_DISPATCH_H

#include <braille_engine.h>
#include <braille_rules.h>
#include <zmk/behavior.h>

/*
 * Ordered ZMK behavior dispatch: the single boundary where a committed rule
 * becomes host-visible behavior events.
 *
 * `braille_dispatch_rule` invokes the rule's ordered actions one at a time,
 * press then release, through `zmk_behavior_invoke_binding`, forwarding each
 * binding's behavior device and both parameters and the chord-resolution
 * event unchanged. ZMK gives each behavior its own press/release semantics:
 * `&kp` emits ordinary host keystrokes (letters, space, digits,
 * punctuation), `&sk` sticky-key actions arm through the next host key
 * release whether that key arrives in this rule or a later one, and
 * navigation or custom behaviors run their own bindings. There is no
 * engine-specific modifier lifecycle, no per-character or per-command
 * branching, and no second action representation: letters, space,
 * navigation, and custom actions are all plain `struct
 * zmk_behavior_binding` rows dispatched identically, and nothing here
 * inspects the rule's chord masks (space-only, dot-only, and space-plus-dot
 * sequences dispatch the same way).
 *
 * This unit is also the integration boundary for phase 05's local Grade 1
 * search input: the settings interface captures and reviews rules through
 * the same `struct braille_rule` representation and simply never calls this
 * function, so captured/settings input never executes on the host.
 *
 * The engine integration is the strong `braille_engine_commit_rule` override
 * in braille_dispatch.c: the matcher's commits flow through here. `cause`
 * carries word-end/discard evidence for phase 02's context transitions;
 * dispatch itself ignores it.
 */

/* Invoke `rule`'s actions[0..action_count) in order, each pressed then
 * released, with `event` forwarded unchanged. A rule with zero actions (an
 * indicator) invokes nothing and returns 0. On the first failing invocation
 * the remaining actions are skipped and the error is returned. */
int braille_dispatch_rule(const struct braille_rule *rule,
                          struct zmk_behavior_binding_event event);

#endif /* BRAILLE_DISPATCH_H */
