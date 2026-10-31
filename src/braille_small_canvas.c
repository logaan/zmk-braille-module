// SPDX-License-Identifier: MIT
#ifdef CONFIG_ZMK_BRAILLE_WIDGET
#include <braille_small_canvas.h>
#include <string.h>
#if LVGL_VERSION_MAJOR < 9
void braille_small_canvas(lv_obj_t *canvas, lv_color_t *draw,
                          lv_color_t *rotated, unsigned width, unsigned height,
                          int rotation, const char *text) {
  lv_canvas_set_buffer(canvas, draw, width, height, LV_IMG_CF_TRUE_COLOR);
  lv_canvas_fill_bg(canvas, lv_color_white(), LV_OPA_COVER);
  lv_draw_label_dsc_t d;
  lv_draw_label_dsc_init(&d);
  d.font = &lv_font_unscii_8;
  d.color = lv_color_black();
  d.line_space = 0;
  /* Draw pre-wrapped rows independently: LVGL otherwise wraps a trailing
   * space at the exact four-character width into an extra blank row. */
  unsigned row = 0;
  for (const char *p = text; *p && row * 8 < height; row++) {
    const char *end = strchr(p, '\n');
    size_t n = end ? (size_t)(end - p) : strlen(p);
    char line[65];
    if (n > 64)
      n = 64;
    memcpy(line, p, n);
    line[n] = 0;
    lv_canvas_draw_text(canvas, 0, row * 8, width + 8, &d, line);
    p = end ? end + 1 : p + strlen(p);
  }
  for (unsigned y = 0; y < height; y++)
    for (unsigned x = 0; x < width; x++)
      rotated[rotation < 0 ? (width - 1 - x) * height + y
                           : x * height + (height - 1 - y)] =
          draw[y * width + x];
  lv_canvas_set_buffer(canvas, rotated, height, width, LV_IMG_CF_TRUE_COLOR);
}
#endif
#endif
