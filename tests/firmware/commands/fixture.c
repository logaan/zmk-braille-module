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
    __ASSERT(braille_engine_pending_count() == 0, "Direct command left pending input");
}

static void expect_output(uint32_t usage, uint8_t mods, bool globe) {
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
        {0x123, C_VOL_DN}, {0x11c, C_VOL_UP}, {0x1c0, C_PP},
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

static void run_suite(void) {
    BUILD_ASSERT(IS_ENABLED(CONFIG_ZMK_HID_CONSUMER_REPORT_USAGES_FULL),
                 "Native command acceptance requires FULL consumer usages");
    __ASSERT(braille_tables_count() == 2, "Initial mode registry changed");
    for (size_t m = 0; m < braille_tables_count(); ++m) {
        const struct braille_mode *mode = braille_tables_mode(m);
        __ASSERT(mode->commands->count == 25, "Verified command count changed");
        __ASSERT(braille_tables_select(mode->name) == 0, "Mode selection/collision failure");
        struct braille_override_layer overrides = braille_tables_overrides();
        *overrides.count = 0;
        if (mode->characters->count == 0) {
            /* computer8's real text arrives in Phase 03. Only these fixture
             * letters use the real shared override layer; commands are real. */
            overrides.rules[0] = (struct braille_rule){
                .chords = {0x001}, .chord_count = 1, .action_count = 1,
                .actions = {{.behavior_dev = DEVICE_DT_NAME(DT_NODELABEL(kp)), .param1 = A}},
            };
            overrides.rules[1] = overrides.rules[0];
            overrides.rules[1].chords[0] = 0x011;
            overrides.rules[1].actions[0].param1 = E;
            *overrides.count = 2;
        }
        test_direct_commands();
        test_sticky_modifiers();
        *overrides.count = 0;
        LOG_INF("factory_commands: %s PASS", mode->name);
    }
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
