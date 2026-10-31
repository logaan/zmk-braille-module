#include <assert.h>
#include <braille_small_screen.h>
#include <stdio.h>
#include <string.h>
void braille_display_cells(const uint16_t *cells, size_t count, char *out,
                           size_t size) {
  (void)cells;
  snprintf(out, size, "%s", count ? "13456" : "");
}
int main(void) {
  struct braille_editor_snapshot e = {.active = true, .screen = BR_ED_CONFIRM};
  struct braille_display_snapshot b = {0};
  char out[512], joined[1024] = {0};
  strcpy(e.title, "Confirm binding");
  strcpy(e.body, "Input: 123 456\nPosition 0; 3 actions\n  Previous\n> "
                 "key_press 1234567890 abcdefghijklmnopqrstuvwxyz\n  Next\n");
  unsigned pages = braille_small_screen(&e, &b, 4, 16, 0, out, sizeof(out));
  assert(pages >= 2);
  assert(strstr(out, "> ke"));
  for (unsigned p = 0; p < pages; p++) {
    braille_small_screen(&e, &b, 4, 16, p, out, sizeof(out));
    char *line = out;
    unsigned row = 0;
    while (*line) {
      char *end = strchr(line, '\n');
      assert(end);
      assert(end - line <= 4);
      if (row >= 1 && row < 15)
        strncat(joined, line, end - line);
      line = end + 1;
      row++;
    }
    assert(row == 16);
  }
  assert(strstr(joined, "key_press 1234567890 abcdefghijklmnopqrstuvwxyz"));
  assert(strstr(joined, "Input: 123 456"));
  assert(!strstr(joined, "Previous"));
  assert(!strstr(joined, "Next"));
  braille_small_screen(&e, &b, 18, 20, 0, out, sizeof(out));
  assert(strstr(out, "Confirm binding"));
  assert(strstr(out, "> key_press"));
  e.active = false;
  strcpy(b.mode, "computer8");
  strcpy(b.preview, "y");
  b.held = 61;
  b.pending_count = 3;
  strcpy(b.pending, "1 12 13456");
  braille_small_screen(&e, &b, 18, 20, 0, out, sizeof(out));
  assert(strstr(out, "computer8"));
  assert(strstr(out, "y"));
  assert(strstr(out, "Held:13456"));
  assert(strstr(out, "Buf:3"));
  assert(strstr(out, "1 12 13456"));
  strcpy(b.pending, "12345678 12345678 12345678");
  braille_small_screen(&e, &b, 4, 16, 0, out, sizeof(out));
  assert(strstr(out, "... "));
  char guard[2] = {'x', 'z'};
  braille_small_screen(&e, &b, 4, 16, 0, guard, 1);
  assert(!guard[0] && guard[1] == 'z');
  puts("small-screen pagination, selection, context, mode and buffer: PASS");
}
