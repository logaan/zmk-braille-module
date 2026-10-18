// SPDX-License-Identifier: MIT
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <braille_chord.h>
#include <zmk/matrix.h>

static unsigned resolutions;
static uint16_t resolved_mask;
static struct zmk_behavior_binding_event resolved_event;
static int consumer_result;
static bool reenter;

static struct zmk_behavior_binding_event event(uint32_t position, int layer) {
    return (struct zmk_behavior_binding_event){
        .position = position,
        .layer = layer,
        .timestamp = INT64_C(9876543210),
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
        .source = 0,
#endif
    };
}

int braille_chord_resolved(uint16_t mask, struct zmk_behavior_binding_event e) {
    ++resolutions;
    resolved_mask = mask;
    resolved_event = e;
    if (reenter) {
        reenter = false;
        /* The old chord must already be idle before a consumer is invoked. */
        assert(braille_chord_key_event(0x80, true, event(UINT32_MAX, 31)) == 0);
    }
    return consumer_result;
}

static void edge(uint16_t bit, bool pressed, uint32_t pos, int layer, unsigned expected_calls) {
    assert(braille_chord_key_event(bit, pressed, event(pos, layer)) == 0);
    assert(resolutions == expected_calls);
}

static void test_every_mask(void) {
    /* All 511 nonempty masks, including every dot, space, and space+dot mix. */
    for (uint16_t mask = 1; mask <= BRAILLE_CHORD_MASK; ++mask) {
        unsigned before = resolutions;
        for (unsigned b = 0; b < 9; ++b) {
            if (mask & (1U << b)) {
                edge(1U << b, true, b, b, before);
            }
        }
        for (int b = 8; b >= 0; --b) {
            if (mask & (1U << b)) {
                bool last = (mask & ((1U << b) - 1)) == 0;
                /* Layer is deliberately different at release. */
                edge(1U << b, false, b, 31 - b, before + last);
                if (last) {
                    assert(resolved_mask == mask);
                    assert(resolved_event.position == (uint32_t)b);
                    assert(resolved_event.layer == 31 - b);
                    assert(resolved_event.timestamp == INT64_C(9876543210));
                }
            }
        }
    }
}

static void test_overlap_and_repress(void) {
    unsigned n = resolutions;
    edge(1, true, 10, 0, n);
    edge(2, true, 11, 0, n);
    edge(1, false, 10, 1, n); /* Early release does not remove accumulated bit. */
    edge(4, true, 12, 1, n);  /* New key joins the partly released chord. */
    edge(1, true, 10, 2, n);  /* Re-press within the same chord. */
    edge(2, false, 11, 2, n);
    edge(4, false, 12, 2, n);
    edge(0x100, true, 13, 2, n);
    edge(1, false, 10, 2, n);
    edge(0x100, false, 13, 2, n + 1);
    assert(resolved_mask == 0x107);
    /* Next chord has no bits from the first and no dependence on elapsed time. */
    struct zmk_behavior_binding_event e = event(13, 5);
    e.timestamp = INT64_MIN;
    assert(braille_chord_key_event(8, true, e) == 0);
    e.timestamp = INT64_MAX;
    assert(braille_chord_key_event(8, false, e) == 0);
    assert(resolutions == n + 2 && resolved_mask == 8);
    assert(resolved_event.timestamp == INT64_MAX);
}

static void test_physical_ownership(void) {
    unsigned n = resolutions;
    /* Two different keys may share a dot binding; neither release ends early. */
    edge(2, true, 0, 0, n);
    edge(2, true, UINT32_MAX, 3, n);
    edge(2, true, 0, 4, n); /* Duplicate press is idempotent. */
    edge(2, false, 0, 5, n);
    edge(2, false, 0, 5, n);  /* Duplicate release is idempotent. */
    edge(2, false, 77, 5, n); /* Unknown key cannot release somebody else's bit. */
    edge(2, false, UINT32_MAX, 6, n + 1);
    assert(resolved_mask == 2);
    edge(2, false, UINT32_MAX, 6, n + 1);
#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    n = resolutions;
    struct zmk_behavior_binding_event local = event(3, 0), remote = local;
    local.source = 255;
    remote.source = 1;
    assert(braille_chord_key_event(1, true, local) == 0);
    assert(braille_chord_key_event(2, true, remote) == 0);
    local.layer = remote.layer = 7;
    assert(braille_chord_key_event(1, false, local) == 0);
    assert(resolutions == n);
    assert(braille_chord_key_event(2, false, remote) == 0);
    assert(resolutions == n + 1 && resolved_mask == 3);
    assert(resolved_event.source == 1);
#endif
}

static void test_invalid_and_capacity(void) {
    unsigned n = resolutions;
    const uint16_t invalid[] = {0, 3, 0x101, 0x200, 0xffff};
    edge(1, true, 0, 0, n);
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        assert(braille_chord_key_event(invalid[i], true, event(0, 0)) == -EINVAL);
        assert(braille_chord_key_event(invalid[i], false, event(0, 0)) == -EINVAL);
    }
    assert(braille_chord_key_event(2, true, event(0, 1)) == -EINVAL);
    assert(braille_chord_key_event(2, false, event(0, 1)) == -EINVAL);
    for (unsigned p = 1; p < ZMK_KEYMAP_LEN; ++p) {
        edge(1, true, p, 0, n);
    }
    assert(braille_chord_key_event(2, true, event(UINT32_MAX, 0)) == -ENOSPC);
    edge(2, false, UINT32_MAX, 0, n);
    for (unsigned p = 0; p < ZMK_KEYMAP_LEN; ++p) {
        edge(1, false, p, 1, n + (p == ZMK_KEYMAP_LEN - 1));
    }
    assert(resolved_mask == 1); /* Rejected bit never contaminated accumulation. */
}

static void test_consumer_failure_and_reentry(void) {
    unsigned n = resolutions;
    consumer_result = -EIO;
    edge(1, true, 0, 0, n);
    assert(braille_chord_key_event(1, false, event(0, 0)) == -EIO);
    assert(resolutions == n + 1 && resolved_mask == 1);
    consumer_result = 0;
    edge(2, true, 1, 0, n + 1);
    reenter = true;
    edge(2, false, 1, 0, n + 2);
    assert(resolved_mask == 2);
    edge(0x80, false, UINT32_MAX, 0, n + 3);
    assert(resolved_mask == 0x80);
}

int main(void) {
    test_every_mask();
    test_overlap_and_repress();
    test_physical_ownership();
    test_invalid_and_capacity();
    test_consumer_failure_and_reentry();
    return 0;
}
