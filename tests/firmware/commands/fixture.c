// SPDX-License-Identifier: MIT
/* Native integration: production bindings/capture/matcher/dispatch/tables and
 * unmodified ZMK kp/sk/HID. Only report transmission is observed, not replaced.
 * No alternate matcher, rule table, sticky-key state machine or HID mock. */
#include <string.h>
#include <drivers/behavior.h>
#include <dt-bindings/zmk/keys.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_tables.h>
#include <zmk/hid.h>
#include <zmk/keymap.h>
#include <zmk/event_manager.h>
#include <zmk/events/position_state_changed.h>
#include <braille_settings.h>

LOG_MODULE_REGISTER(braille_commands_test, LOG_LEVEL_INF);

struct observation {
    uint16_t page;
    uint16_t key;
    uint8_t mods;
    bool globe;
};
static struct observation reports[32];
static size_t report_count;
static bool observing;
static unsigned checkpoint;

int __real_zmk_endpoints_send_report(uint16_t page);

int __wrap_zmk_endpoints_send_report(uint16_t page) {
    if (observing) {
        /* A keyboard report may hold ordinary keys or only modifiers. Only
         * ordinary key-down reports count as output; release reports must
         * still clear the real HID state, checked after every command. */
        for (uint16_t key = 1; key <= (page == HID_USAGE_KEY ? 0x65 : 0x29f); ++key) {
            uint32_t usage = ZMK_HID_USAGE(page, key);
            if (!zmk_hid_is_pressed(usage) || usage == GLOBE) {
                continue;
            }
            __ASSERT(report_count < ARRAY_SIZE(reports), "Report observation overflow");
            reports[report_count++] = (struct observation){
                .page = page, .key = key,
                .mods = zmk_hid_get_keyboard_report()->body.modifiers,
                .globe = zmk_hid_is_pressed(GLOBE),
            };
        }
    }
    /* Native POSIX has no physical USB endpoint; keep the real call so this
     * test does not replace any production behavior or HID implementation. */
    return __real_zmk_endpoints_send_report(page);
}

static void begin_observation(void) {
    report_count = 0;
    observing = true;
}

static void chord(uint16_t mask) {
    for (unsigned state = 0; state < 2; ++state) {
        for (unsigned bit = 0; bit < 9; ++bit) {
            if (!(mask & BIT(bit))) {
                continue;
            }
            struct zmk_behavior_binding binding = {
                .behavior_dev = bit == 8 ? DEVICE_DT_NAME(DT_NODELABEL(bspc))
                                         : DEVICE_DT_NAME(DT_NODELABEL(bc)),
                .param1 = bit == 8 ? 0 : bit + 1,
            };
            struct zmk_behavior_binding_event event = {
                .layer = 0, .position = bit, .timestamp = k_uptime_get(),
            };
            __ASSERT(zmk_behavior_invoke_binding(&binding, event, state == 0) >= 0,
                     "Braille binding failed");
        }
    }
}

static void expect_output(uint32_t usage, uint8_t mods, bool globe) {
    __ASSERT(braille_engine_pending_count() == 0, "Complete output left pending input");
    __ASSERT(report_count == 1, "Expected exactly one key, got %u", (unsigned)report_count);
    __ASSERT(reports[0].page == ZMK_HID_USAGE_PAGE(usage) &&
                 reports[0].key == ZMK_HID_USAGE_ID(usage), "Wrong host usage");
    __ASSERT(reports[0].mods == mods && reports[0].globe == globe,
             "Wrong modifier scope: mods %u, Globe %u", reports[0].mods, reports[0].globe);
    __ASSERT(!zmk_hid_is_pressed(usage), "Host key left held");
    __ASSERT(zmk_hid_get_explicit_mods() == 0 && !zmk_hid_is_pressed(GLOBE),
             "Sticky modifier left armed after its next key");
    observing = false;
}

static void test_direct_commands(void) {
    static const struct { uint16_t mask; uint32_t usage; } expected[] = {
        {0x100, SPACE}, {0x101, LEFT_ARROW}, {0x108, RIGHT_ARROW},
        {0x104, UP_ARROW}, {0x120, DOWN_ARROW}, {0x107, HOME}, {0x138, END},
        {0x13c, PAGE_UP}, {0x139, PAGE_DOWN}, {0x11e, TAB}, {0x111, RETURN},
        {0x103, ESC}, {0x119, BSPC}, {0x040, BSPC}, {0x140, BSPC},
        {0x080, RETURN}, {0x180, RETURN},
    };
    for (size_t i = 0; i < ARRAY_SIZE(expected); ++i) {
        begin_observation();
        chord(expected[i].mask);
        expect_output(expected[i].usage, 0, false);
    }
    begin_observation();
    chord(0x118); /* Removed fixture Tab: never a factory output. */
    __ASSERT(report_count == 0, "Old Tab fixture survived");
    observing = false;
}

static void test_sticky_modifiers(void) {
    static const struct { uint16_t mask; uint32_t usage; uint8_t mods; } sticky[] = {
        {0x148, LSHFT, 0x02}, {0x144, LCTRL, 0x01},
        {0x142, LALT, 0x04}, {0x141, LGUI, 0x08}, {0x150, GLOBE, 0x00},
    };
    for (size_t i = 0; i < ARRAY_SIZE(sticky); ++i) {
        /* A real sticky release keeps the usage armed until the following
         * letter/Tab release; the module does not implement that lifecycle. */
        for (unsigned tab = 0; tab < 2; ++tab) {
            chord(sticky[i].mask);
            __ASSERT(zmk_hid_is_pressed(sticky[i].usage), "Sticky did not stay armed");
            begin_observation();
            chord(tab ? 0x11e : 0x001);
            expect_output(tab ? TAB : A, sticky[i].mods, sticky[i].usage == GLOBE);
            begin_observation();
            chord(tab ? 0x11e : 0x001);
            expect_output(tab ? TAB : A, 0, false); /* One-shot, never held. */
        }
    }

    /* Real macOS line-start/end and previous/next-UI compositions, NOT
     * assumptions that Home/End produce Command+Left/Right. */
    chord(0x141);
    begin_observation();
    chord(0x101);
    expect_output(LEFT_ARROW, 0x08, false);
    chord(0x141);
    begin_observation();
    chord(0x108);
    expect_output(RIGHT_ARROW, 0x08, false);
    chord(0x148);
    begin_observation();
    chord(0x11e);
    expect_output(TAB, 0x02, false);
    begin_observation();
    chord(0x11e);
    expect_output(TAB, 0, false);

    /* Keyboard modifiers stack because default &sk ignores modifiers. */
    chord(0x148);
    chord(0x141);
    begin_observation();
    chord(0x101);
    expect_output(LEFT_ARROW, 0x0a, false);

    /* Actual GLOBE+e usage combination; this checks HID, not macOS's emoji UI. */
    chord(0x150);
    begin_observation();
    chord(0x011);
    expect_output(E, 0, true);
}

static void test_computer8_characters(void) {
    /* Real registered cells, not override fixtures: exercise implicit Shift
     * through unmodified ZMK kp/HID and verify it releases before the next cell. */
    static const struct { uint16_t mask; uint32_t usage; uint8_t mods; } expected[] = {
        {0x001, A, 0}, {0x041, A, 0x02}, {0x002, N1, 0}, {0x026, N8, 0},
        {0x008, GRAVE, 0}, {0x033, BSLH, 0x02}, {0x038, MINUS, 0x02},
        {0x039, FSLH, 0x02}, {0x018, GRAVE, 0x02}, {0x02a, LBKT, 0x02},
        {0x03b, RBKT, 0x02}, {0x073, BSLH, 0}, {0x010, SQT, 0x02},
        {0x020, COMMA, 0}, {0x03c, N3, 0x02}, {0x030, SEMI, 0},
    };
    __ASSERT(braille_rules_computer8_characters.count == 94, "Computer8 table incomplete");
    for (size_t i = 0; i < ARRAY_SIZE(expected); ++i) {
        begin_observation();
        chord(expected[i].mask);
        expect_output(expected[i].usage, expected[i].mods, false);
    }
    /* No dot-8 masking, no newline alias for dots 78, no stale key state. */
    static const uint16_t reserved[] = {0x042, 0x081, 0x0c0, 0x0ff};
    for (size_t i = 0; i < ARRAY_SIZE(reserved); ++i) {
        begin_observation();
        chord(reserved[i]);
        __ASSERT(report_count == 0, "Reserved cell emitted a host key");
        __ASSERT(zmk_hid_get_keyboard_report()->body.modifiers == 0, "Modifier leaked");
        observing = false;
    }
    begin_observation();
    chord(0x001);
    expect_output(A, 0, false);
    LOG_INF("computer8_characters: PASS");
}

static void test_ueb1_characters(void) {
    __ASSERT(braille_tables_select("ueb1") == 0, "UEB1 selection");
    chord(0x20);
    begin_observation(); chord(0x01); expect_output(A, 0x02, false);
    begin_observation(); chord(0x03); expect_output(B, 0, false);
    chord(0x20); chord(0x20);
    begin_observation(); chord(0x01); expect_output(A, 0x02, false);
    begin_observation(); chord(0x03); expect_output(B, 0x02, false);
    begin_observation(); chord(0x02); expect_output(COMMA, 0, false);
    begin_observation(); chord(0x03); expect_output(B, 0, false);
    chord(0x20); chord(0x20); chord(0x20);
    begin_observation(); chord(0x01); expect_output(A, 0x02, false);
    begin_observation(); chord(0x100); expect_output(SPACE, 0, false);
    begin_observation(); chord(0x03); expect_output(B, 0x02, false);
    chord(0x3c);
    begin_observation(); chord(0x01); expect_output(N1, 0, false);
    begin_observation(); chord(0x03); expect_output(N2, 0, false);
    chord(0x10);
    begin_observation(); chord(0x09); /* numeric space then 3: two ordered host keys */
    __ASSERT(report_count == 2 && reports[0].key == ZMK_HID_USAGE_ID(SPACE) &&
                 reports[1].key == ZMK_HID_USAGE_ID(N3) && !reports[0].mods && !reports[1].mods,
             "Numeric space dispatch order/modifiers");
    __ASSERT(braille_engine_context()->numeric && braille_engine_context()->caps == BRAILLE_CAPS_PASSAGE,
             "Numeric space lost passage or numeric mode");
    observing = false;
    chord(0x30);
    begin_observation(); chord(0x03); expect_output(B, 0x02, false);
    __ASSERT(!braille_engine_context()->numeric, "Grade 1 failed to end numeric mode");
    chord(0x20); chord(0x04);
    begin_observation(); chord(0x03); expect_output(B, 0, false);
    braille_engine_reset();
    begin_observation(); chord(0x26); expect_output(SQT, 0x02, false);
    begin_observation(); chord(0x01); expect_output(A, 0, false);
    begin_observation(); chord(0x26); expect_output(FSLH, 0x02, false);
    chord(0x18);
    begin_observation(); chord(0x26); expect_output(SQT, 0x02, false);
    chord(0x38);
    begin_observation(); chord(0x33); expect_output(BSLH, 0x02, false);
    chord(0x28);
    begin_observation(); chord(0x21); expect_output(GRAVE, 0, false);
    chord(0x20);
    begin_observation(); chord(0x24); expect_output(MINUS, 0, false);
    LOG_INF("ueb1_characters: PASS");
}

static void live_context_and_prefix(void) {
    braille_engine_reset();
    chord(0x20); chord(0x20); chord(0x20); /* caps passage */
    chord(0x3c); chord(0x01); /* numeric on */
    chord(0x008); /* actual strict literary prefix */
    __ASSERT(braille_engine_pending_count() == 1 && braille_engine_context()->numeric &&
                 braille_engine_context()->caps == BRAILLE_CAPS_PASSAGE &&
                 !braille_engine_context()->word_start, "Live context setup failed");
}
static void expect_reset_context(void) {
    __ASSERT(!braille_engine_pending_count() && !braille_engine_context()->numeric &&
                 braille_engine_context()->caps == BRAILLE_CAPS_NONE &&
                 braille_engine_context()->word_start, "Boundary kept pending cells/context");
}

static void position(uint32_t pos, bool pressed) {
    __ASSERT(raise_zmk_position_state_changed((struct zmk_position_state_changed){
        .source = ZMK_POSITION_STATE_CHANGE_SOURCE_LOCAL, .position = pos,
        .state = pressed, .timestamp = k_uptime_get()}) >= 0, "Position event failed");
}

static void test_context_boundaries(void) {
    __ASSERT(braille_tables_select("ueb1") == 0, "Context fixture mode");
    live_context_and_prefix();
    /* No output from a reset, and a conventional release alone does not reset. */
    position(15, false);
    __ASSERT(braille_engine_pending_count() == 1, "Conventional release reset context");
    position(15, true); /* &none counts as a non-Braille key, not only &kp */
    position(15, false);
    expect_reset_context();
    live_context_and_prefix();
    __ASSERT(zmk_keymap_layer_activate(1) == 0, "Layer activation");
    expect_reset_context();
    live_context_and_prefix();
    position(1, true); position(1, false); /* opaque &none shadows base &bc 2 */
    expect_reset_context();
    /* Transparent lookup and a custom compatible device must not reset input.
     * Observe only releases: every generated host key must also preserve UEB context. */
    chord(0x008);
    position(0, true); position(0, false); /* transparent -> base &bc 1: @ */
    __ASSERT(!braille_engine_pending_count() && !braille_engine_context()->word_start,
             "Transparent Braille press or generated &kp reset context");
    position(12, true); position(12, false); /* custom &bc 1: a */
    __ASSERT(!braille_engine_context()->word_start, "Custom dot device reset context");
    /* Preserve phase-01 capture semantics: a held key keeps its press-time
     * release owner across a layer change, but resolves in fresh UEB context. */
    position(0, true);
    __ASSERT(zmk_keymap_layer_deactivate(1) == 0, "Layer deactivation");
    expect_reset_context();
    begin_observation();
    position(0, false);
    expect_output(A, 0, false);
    /* The actual settings gate must reach the engine's strong reset hook. */
    live_context_and_prefix();
    struct zmk_behavior_binding_event event = {.timestamp = k_uptime_get()};
    __ASSERT(braille_settings_enter(event) == 0 && !braille_engine_pending_count(),
             "Settings entry kept pending prefix");
    __ASSERT(braille_settings_exit(event) == 0, "Settings exit");
    expect_reset_context();
    LOG_INF("context_boundaries: PASS");
}

static void run_suite(void) {
    BUILD_ASSERT(IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL),
                 "Native command acceptance requires FULL consumer usages");
    __ASSERT(braille_tables_count() == 2, "Initial mode registry changed");
    for (size_t m = 0; m < braille_tables_count(); ++m) {
        const struct braille_mode *mode = braille_tables_mode(m);
        __ASSERT(mode->commands->count == 22, "Verified command count changed");
        __ASSERT(braille_tables_select(mode->name) == 0, "Mode selection/collision failure");
        struct braille_override_layer overrides = braille_tables_overrides();
        *overrides.count = 0;
        __ASSERT(mode->characters->count > 0, "Real character table required");
        test_direct_commands();
        test_sticky_modifiers();
        if (strcmp(mode->name, "computer8") == 0) {
            test_computer8_characters();
        }
        *overrides.count = 0;
        LOG_INF("factory_commands: %s PASS", mode->name);
    }
    test_ueb1_characters();
    test_context_boundaries();
    __ASSERT(braille_tables_select("ueb1") == 0, "Unable to restore UEB1");
    /* Let the real workqueue timer expire between mock scan checkpoints. */
    chord(0x148);
    __ASSERT(zmk_hid_is_pressed(LSHFT), "Shift timeout test not armed");
}

static int pressed(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    (void)event;
    __ASSERT(binding->param1 == checkpoint++, "Unexpected native checkpoint order");
    switch (binding->param1) {
    case 0:
        run_suite();
        break;
    case 1:
        __ASSERT(!zmk_hid_is_pressed(LSHFT), "Unused sticky Shift did not time out");
        begin_observation();
        chord(0x001);
        expect_output(A, 0, false);
        LOG_INF("factory_commands: keyboard-timeout PASS");
        chord(0x150);
        __ASSERT(zmk_hid_is_pressed(GLOBE), "Globe timeout test not armed");
        break;
    case 2:
        __ASSERT(!zmk_hid_is_pressed(GLOBE), "Unused sticky Globe did not time out");
        begin_observation();
        chord(0x011);
        expect_output(E, 0, false);
        LOG_INF("factory_commands: globe-timeout PASS");
        LOG_INF("factory_commands: ALL PASS");
        break;
    default:
        __ASSERT(false, "Extra native checkpoint");
    }
    return 0;
}

static int released(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    (void)binding;
    (void)event;
    return 0;
}
static const struct behavior_driver_api api = {
    .binding_pressed = pressed, .binding_released = released,
};
BEHAVIOR_DT_DEFINE(DT_NODELABEL(run_commands), NULL, NULL, NULL, NULL, POST_KERNEL,
                   CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &api);
