// SPDX-License-Identifier: MIT
#pragma once
#include <lvgl.h>
#if LVGL_VERSION_MAJOR < 9
void braille_small_canvas(lv_obj_t *canvas, lv_color_t *draw,
                          lv_color_t *rotated, unsigned width, unsigned height,
                          int rotation, const char *text);
#endif
