// SPDX-License-Identifier: MIT
#ifdef CONFIG_ZMK_BRAILLE_WIDGET
#include <braille_engine.h>
#include <braille_small_canvas.h>
#include <braille_small_screen.h>
#include <braille_storage.h>
#include <braille_tables.h>
#include <braille_widget.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zmk/keymap.h>
static struct {
  struct braille_editor_snapshot editor;
  struct braille_display_snapshot braille;
  bool active;
} state, rendered, next;
static struct k_spinlock lock;
static lv_obj_t *panel, *label;
static unsigned width, height, page, pages = 1;
static int rotation;
static char text[512], last[512];
static int64_t page_at, notice_until;
static unsigned last_skipped;
static char notice[48];
void braille_engine_notify_dropped_prefix(
    const uint16_t *cells, uint8_t count,
    struct zmk_behavior_binding_event event) {
  (void)cells;
  (void)count;
  (void)event;
  const char *mode = braille_tables_active();
  snprintf(notice, sizeof(notice), "%s",
           braille_display_computer8_mode(mode) ? "Reserved cell"
                                              : "Incomplete input dropped");
  notice_until = k_uptime_get() + 3000;
}
#if LVGL_VERSION_MAJOR < 9
/* QBF portrait32x128 rotates into its native128x32 framebuffer. */
static lv_color_t draw[4096], rotated[4096];
#endif
static void collect(struct k_work *work) {
  (void)work;
  braille_editor_snapshot(&next.editor);
  braille_display_snapshot(&next.braille);
  next.active = next.editor.active ||
                zmk_keymap_layer_active(CONFIG_ZMK_BRAILLE_WIDGET_LAYER);
  unsigned skipped = braille_storage_skipped_count();
  if (skipped != last_skipped) {
    last_skipped = skipped;
    if (skipped) {
      snprintf(notice, sizeof(notice), "Skipped:%u", skipped);
      notice_until = k_uptime_get() + 4000;
    }
  }
  if (!next.editor.active && k_uptime_get() < notice_until)
    snprintf(next.braille.preview, sizeof(next.braille.preview), "%s", notice);
  k_spinlock_key_t key = k_spin_lock(&lock);
  state = next;
  k_spin_unlock(&lock, key);
}
K_WORK_DEFINE(collect_work, collect);
static void refresh(lv_timer_t *timer) {
  (void)timer;
  k_spinlock_key_t key = k_spin_lock(&lock);
  rendered = state;
  k_spin_unlock(&lock, key);
  if (!rendered.active) {
    lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
    k_work_submit(&collect_work);
    return;
  }
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);
  unsigned columns = width / 8, rows = height / 8;
  braille_small_screen(&rendered.editor, &rendered.braille, columns, rows, 0,
                       text, sizeof(text));
  if (strcmp(last, text)) {
    snprintf(last, sizeof(last), "%s", text);
    page = 0;
    page_at = k_uptime_get();
  } else if (k_uptime_get() - page_at >= 4000) {
    page = (page + 1) % pages;
    page_at = k_uptime_get();
  }
  pages = braille_small_screen(&rendered.editor, &rendered.braille, columns,
                               rows, page, text, sizeof(text));
#if LVGL_VERSION_MAJOR < 9
  if (rotation) {
    braille_small_canvas(panel, draw, rotated, width, height, rotation, text);
  } else
#endif
    lv_label_set_text(label, text);
  k_work_submit(&collect_work);
}
void braille_widget_init(lv_obj_t *parent, unsigned w, unsigned h, int turn) {
  width = w;
  height = h;
  rotation = turn;
#if LVGL_VERSION_MAJOR < 9
  if (turn)
    panel = lv_canvas_create(parent);
  else
#endif
  {
    panel = lv_obj_create(parent);
    lv_obj_set_size(panel, w, h);
    lv_obj_set_style_bg_color(panel, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    label = lv_label_create(panel);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    lv_obj_set_pos(label, 0, 0);
    lv_obj_set_width(label, w);
    lv_obj_set_style_text_color(label, lv_color_black(), 0);
    lv_obj_set_style_text_font(label, &lv_font_unscii_8, 0);
    lv_obj_set_style_text_line_space(label, 0, 0);
  }
  lv_obj_clear_flag(panel, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_pos(panel, 0, 0);
  lv_obj_add_flag(panel, LV_OBJ_FLAG_HIDDEN);
  k_work_submit(&collect_work);
  lv_timer_create(refresh, 100, NULL);
}

#endif /* CONFIG_ZMK_BRAILLE_WIDGET */
