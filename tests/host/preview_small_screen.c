#include <assert.h>
#include <braille_small_canvas.h>
#include <braille_small_screen.h>
#include <stdio.h>
void braille_display_cells(const uint16_t *cells, size_t count, char *out,
                           size_t size) {
  (void)cells;
  snprintf(out, size, "%s", count ? "13456" : "");
}
int main(int argc, char **argv) {
  assert(argc == 2);
  lv_init();
  static lv_color_t fb[128 * 32];
  static lv_disp_draw_buf_t draw_buf;
  lv_disp_draw_buf_init(&draw_buf, fb, NULL, 128 * 32);
  static lv_disp_drv_t drv;
  lv_disp_drv_init(&drv);
  drv.hor_res = 128;
  drv.ver_res = 32;
  drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&drv);
  lv_obj_t *screen = lv_obj_create(NULL), *canvas = lv_canvas_create(screen);
  static lv_color_t portrait[4096], rotated[4096];
  struct braille_editor_snapshot e = {.active = true};
  struct braille_display_snapshot b = {0};
  snprintf(e.title, sizeof(e.title), "Confirm binding");
  snprintf(e.body, sizeof(e.body),
           "Input: 123 456\nPosition 0; 2 actions\n> key_press Left Arrow\n  "
           "sticky_key Shift\n");
  char text[512];
  unsigned pages = braille_small_screen(&e, &b, 4, 16, 0, text, sizeof(text));
  assert(pages >= 2);
  FILE *f = fopen(argv[1], "wb");
  assert(f);
  fprintf(f, "P6\n%u 128\n255\n", pages * 36);
  static lv_color_t all[4096 * 16];
  assert(pages < 16);
  for (unsigned p = 0; p < pages; p++) {
    braille_small_screen(&e, &b, 4, 16, p, text, sizeof(text));
    braille_small_canvas(canvas, portrait, rotated, 32, 128, 1, text);
    for (unsigned y = 0; y < 128; y++)
      for (unsigned x = 0; x < 32; x++)
        assert(rotated[x * 128 + 127 - y].full == portrait[y * 32 + x].full);
    for (unsigned i = 0; i < 4096; i++)
      all[p * 4096 + i] = portrait[i];
  }
  for (unsigned y = 0; y < 128; y++)
    for (unsigned p = 0; p < pages; p++)
      for (unsigned x = 0; x < 36; x++) {
        unsigned char pixel =
            x < 32 ? (all[p * 4096 + y * 32 + x].full ? 255 : 0) : 128;
        for (unsigned c = 0; c < 3; c++)
          fwrite(&pixel, 1, 1, f);
      }
  fclose(f);
  puts("Actual LVGL8 portrait pages and pixel-exact rotation: PASS");
}
