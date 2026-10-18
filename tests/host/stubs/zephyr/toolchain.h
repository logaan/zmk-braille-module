// SPDX-License-Identifier: MIT
#pragma once
#ifdef __weak
#undef __weak
#endif
#define __weak __attribute__((weak))
