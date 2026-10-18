// SPDX-License-Identifier: MIT
#pragma once

/* Compile-only subset of ZMK v0.3.0 dt-bindings/zmk/keys.h. Tests assert the
 * emitted raw usages independently; firmware builds use the real header. */
#define TEST_KEY(id) ((0x07UL << 16) | (id))
#define RETURN TEST_KEY(0x28)
#define TAB TEST_KEY(0x2b)
#define SPACE TEST_KEY(0x2c)
#define HOME TEST_KEY(0x4a)
#define PAGE_UP TEST_KEY(0x4b)
#define END TEST_KEY(0x4d)
#define PAGE_DOWN TEST_KEY(0x4e)
#define RIGHT_ARROW TEST_KEY(0x4f)
#define LEFT_ARROW TEST_KEY(0x50)
#define DOWN_ARROW TEST_KEY(0x51)
#define UP_ARROW TEST_KEY(0x52)
