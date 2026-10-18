// SPDX-License-Identifier: MIT
/*
 * Host tests for the ordered ZMK behavior dispatch (src/braille_dispatch.c),
 * compiled against the stub ZMK headers and run natively by
 * test_dispatch.py. `zmk_behavior_invoke_binding` is a recording stub here;
 * the real ZMK press/release semantics (including `&sk` sticky keys staying
 * armed until the next host key release) run in firmware.
 */
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <braille_dispatch.h>
#include <braille_engine.h>
#include <braille_rules.h>
#include <zmk/behavior.h>

/* --- Invocation log -------------------------------------------------------- */

struct invoke_entry {
    const char *dev;
    uint32_t param1;
    uint32_t param2;
    bool pressed;
    struct zmk_behavior_binding_event event;
};

static struct invoke_entry log_entries[64];
static size_t log_count;
/* When nonzero, the (1-based) invocation at this index fails with this. */
static size_t fail_at;
static int fail_err;

static void log_reset(void) {
    log_count = 0;
    fail_at = 0;
    fail_err = 0;
}

/* Recording stub; the real dispatcher is ZMK's behavior runtime. */
int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                struct zmk_behavior_binding_event event, bool pressed) {
    assert(log_count < sizeof(log_entries) / sizeof(log_entries[0]));
    log_entries[log_count] = (struct invoke_entry){
        .dev = binding->behavior_dev,
        .param1 = binding->param1,
        .param2 = binding->param2,
        .pressed = pressed,
        .event = event,
    };
    log_count++;
    if (fail_at == log_count) {
        return fail_err;
    }
    return 0;
}

static bool invoke_is(size_t i, const char *dev, uint32_t param1, uint32_t param2, bool pressed,
                      struct zmk_behavior_binding_event event) {
    const struct invoke_entry *e = &log_entries[i];
    return strcmp(e->dev, dev) == 0 && e->param1 == param1 && e->param2 == param2 &&
           e->pressed == pressed && memcmp(&e->event, &event, sizeof(event)) == 0;
}

/* --- Fixture rules ---------------------------------------------------------- */

/* Chord masks are match-time data; dispatch must never read them. The three
 * rules below cover space-only, dot-only, and space-plus-dot masks with
 * identical actions. */
#define DOT1 0x001
#define SPC 0x100
#define SPD 0x101

#define WHEN_ANY                                                                                   \
    { BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY, BRAILLE_CONDITION_ANY }

static const struct zmk_behavior_binding_event ev = {
    .layer = 2,
    .position = 7,
    .timestamp = 1234,
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    .source = 1,
#endif
};

/* --- Tests ------------------------------------------------------------------ */

/* 1. Ordering: each action is pressed then released, in rule order, with
 *    the binding's device and both parameters and the event forwarded
 *    unchanged. */
static void test_action_ordering(void) {
    log_reset();
    static const struct braille_rule rule = {
        .chords = {DOT1},
        .chord_count = 1,
        .actions =
            {
                {.behavior_dev = "&kp", .param1 = 0x04},
                {.behavior_dev = "&none"},
                {.behavior_dev = "&to", .param1 = 3},
            },
        .action_count = 3,
        .when = WHEN_ANY,
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    assert(braille_dispatch_rule(&rule, ev) == 0);
    assert(log_count == 6);
    assert(invoke_is(0, "&kp", 0x04, 0, true, ev));
    assert(invoke_is(1, "&kp", 0x04, 0, false, ev));
    assert(invoke_is(2, "&none", 0, 0, true, ev));
    assert(invoke_is(3, "&none", 0, 0, false, ev));
    assert(invoke_is(4, "&to", 3, 0, true, ev));
    assert(invoke_is(5, "&to", 3, 0, false, ev));
    printf("ok: action ordering\n");
}

/* 2. Zero, one, and two parameters are forwarded unchanged. */
static void test_parameter_counts(void) {
    log_reset();
    static const struct braille_rule rule = {
        .chords = {DOT1},
        .chord_count = 1,
        .actions =
            {
                {.behavior_dev = "&reset"},
                {.behavior_dev = "&sk", .param1 = 0x02 /* LSHIFT */},
                {.behavior_dev = "&kp", .param1 = 0xe1 /* LSHIFT */, .param2 = 0x04 /* A */},
            },
        .action_count = 3,
        .when = WHEN_ANY,
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    assert(braille_dispatch_rule(&rule, ev) == 0);
    assert(log_count == 6);
    assert(invoke_is(0, "&reset", 0, 0, true, ev));
    assert(invoke_is(1, "&reset", 0, 0, false, ev));
    assert(invoke_is(2, "&sk", 0x02, 0, true, ev));
    assert(invoke_is(3, "&sk", 0x02, 0, false, ev));
    assert(invoke_is(4, "&kp", 0xe1, 0x04, true, ev));
    assert(invoke_is(5, "&kp", 0xe1, 0x04, false, ev));
    printf("ok: zero/one/two parameters\n");
}

/* 3. A sticky modifier applies to the next rule's output: the engine's
 *    commit path dispatches the `&sk` rule's press/release like any other
 *    action — no engine-specific modifier lifecycle — and ZMK's sticky-key
 *    semantics keep it armed for the following rule's keystroke. */
static void test_sticky_modifier_applies_to_next_rule(void) {
    log_reset();
    static const struct braille_rule sk_rule = {
        .chords = {DOT1},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&sk", .param1 = 0x02 /* LSHIFT */}},
        .action_count = 1,
        .when = WHEN_ANY,
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    static const struct braille_rule letter_rule = {
        .chords = {DOT1},
        .chord_count = 1,
        .actions = {{.behavior_dev = "&kp", .param1 = 0x04 /* A */}},
        .action_count = 1,
        .when = WHEN_ANY,
        .class = BRAILLE_CLASS_LETTER,
    };
    /* Two separate commits, as the matcher produces them; the commit cause
     * is evidence for phase-02 transitions, not dispatch input. */
    assert(braille_engine_commit_rule(&sk_rule, BRAILLE_COMMIT_CLEAN, ev) == 0);
    assert(braille_engine_commit_rule(&letter_rule, BRAILLE_COMMIT_RULED_OUT, ev) == 0);
    assert(log_count == 4);
    assert(invoke_is(0, "&sk", 0x02, 0, true, ev));
    assert(invoke_is(1, "&sk", 0x02, 0, false, ev)); /* stays armed (ZMK &sk) */
    assert(invoke_is(2, "&kp", 0x04, 0, true, ev));  /* next rule: shifted A */
    assert(invoke_is(3, "&kp", 0x04, 0, false, ev));
    printf("ok: sticky modifier applies to the next rule's output\n");
}

/* 4. Space-only, dot-only, and space-plus-dot masks are handled identically:
 *    dispatch never inspects the rule's chords. */
static void test_mask_shapes_identical(void) {
    static const struct zmk_behavior_binding action = {.behavior_dev = "&kp", .param1 = 0x2c};
    static const uint16_t masks[] = {SPC, DOT1, SPD};
    for (size_t m = 0; m < sizeof(masks) / sizeof(masks[0]); ++m) {
        log_reset();
        const struct braille_rule rule = {
            .chords = {masks[m]},
            .chord_count = 1,
            .actions = {action},
            .action_count = 1,
            .when = WHEN_ANY,
            .class = BRAILLE_CLASS_SPACE,
        };
        assert(braille_dispatch_rule(&rule, ev) == 0);
        assert(log_count == 2);
        assert(invoke_is(0, "&kp", 0x2c, 0, true, ev));
        assert(invoke_is(1, "&kp", 0x2c, 0, false, ev));
    }
    printf("ok: space-only/dot-only/space-plus-dot identical\n");
}

/* 5. A rule with zero actions (an indicator) invokes nothing. */
static void test_zero_actions(void) {
    log_reset();
    static const struct braille_rule indicator = {
        .chords = {DOT1},
        .chord_count = 1,
        .action_count = 0,
        .when = WHEN_ANY,
        .class = BRAILLE_CLASS_CAPS_INDICATOR,
        .sets = BRAILLE_CAPS_WORD,
    };
    assert(braille_dispatch_rule(&indicator, ev) == 0);
    assert(log_count == 0);
    printf("ok: zero actions invoke nothing\n");
}

/* 6. A failing invocation stops dispatch: the failing action is not
 *    released and later actions are not pressed; the error propagates. */
static void test_error_stops_dispatch(void) {
    log_reset();
    static const struct braille_rule rule = {
        .chords = {DOT1},
        .chord_count = 1,
        .actions =
            {
                {.behavior_dev = "&kp", .param1 = 0x04},
                {.behavior_dev = "&kp", .param1 = 0x05},
                {.behavior_dev = "&kp", .param1 = 0x06},
            },
        .action_count = 3,
        .when = WHEN_ANY,
        .class = BRAILLE_CLASS_NEUTRAL,
    };
    fail_at = 3; /* Second action's press fails. */
    fail_err = -EIO;
    assert(braille_dispatch_rule(&rule, ev) == -EIO);
    assert(log_count == 3); /* press/release of action 1, press of action 2 */
    assert(invoke_is(2, "&kp", 0x05, 0, true, ev));
    printf("ok: a failing action stops dispatch\n");
}

int main(void) {
    test_action_ordering();
    test_parameter_counts();
    test_sticky_modifier_applies_to_next_rule();
    test_mask_shapes_identical();
    test_zero_actions();
    test_error_stops_dispatch();
    printf("test_dispatch: all passed\n");
    return 0;
}
