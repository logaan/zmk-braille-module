// SPDX-License-Identifier: MIT

#include <zephyr/devicetree.h>
#include <zephyr/sys/util.h>

/*
 * Zephyr 3.5 binding schemas constrain cell count, but cannot constrain a
 * phandle-array argument's value. Check literal dot arguments in every enabled
 * node's `bindings` property (keymap layers, combos, macros, etc.). This also
 * covers custom dot devices, not just the default bc label. Dynamic/rule
 * bindings will still need runtime validation in the input driver.
 */
#define BRAILLE_CHECK_DOT(node_id, prop, idx)                                                       \
    COND_CODE_1(DT_NODE_HAS_COMPAT(DT_PHANDLE_BY_IDX(node_id, prop, idx),                             \
                                  zmk_behavior_braille_dot),                                       \
                (BUILD_ASSERT(DT_PHA_BY_IDX(node_id, prop, idx, param1) >= 1 &&                      \
                                  DT_PHA_BY_IDX(node_id, prop, idx, param1) <= 8,                   \
                              "Braille dot parameter must be in 1..8");), ())

#define BRAILLE_CHECK_NODE(node_id)                                                               \
    COND_CODE_1(DT_NODE_HAS_PROP(node_id, bindings),                                                \
                (DT_FOREACH_PROP_ELEM(node_id, bindings, BRAILLE_CHECK_DOT)), ())

DT_FOREACH_STATUS_OKAY_NODE(BRAILLE_CHECK_NODE)
