// SPDX-License-Identifier: MIT
#pragma once
#include <lvgl.h>
/* One central display. rotation: 0 normal, 1 clockwise, -1 anticlockwise. */
void braille_widget_init(lv_obj_t *parent, unsigned width, unsigned height,
                         int rotation);
