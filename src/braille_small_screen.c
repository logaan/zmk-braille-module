// SPDX-License-Identifier: MIT
#include <braille_small_screen.h>
#include <stdio.h>
#include <string.h>
static void add(char *out, size_t size, const char *s) {
  size_t n = strlen(out);
  if (n < size)
    snprintf(out + n, size - n, "%s", s);
}
static void row(char *out, size_t size, const char *s, size_t n,
                unsigned columns) {
  char line[65];
  if (n > columns)
    n = columns;
  if (n > 64)
    n = 64;
  memcpy(line, s, n);
  line[n] = 0;
  add(out, size, line);
  add(out, size, "\n");
}
unsigned braille_small_screen(const struct braille_editor_snapshot *editor,
                              const struct braille_display_snapshot *braille,
                              unsigned columns, unsigned rows, unsigned page,
                              char *out, size_t size) {
  if (!size)
    return 1;
  out[0] = 0;
  if (columns < 4 || columns > 64 || rows < 4)
    return 1;
  char title[64], body[512] = {0};
  if (editor->active) {
    snprintf(title, sizeof(title), "%s", editor->title);
    const char *selected = strstr(editor->body, "> ");
    if (selected) {
      const char *end = strchr(selected, '\n');
      size_t n = end ? (size_t)(end - selected) : strlen(selected);
      snprintf(body, sizeof(body), "%.*s\n", (int)n, selected);
    }
    /* Keep contextual input/search/confirmation information, omit unselected
     * rows. */
    for (const char *p = editor->body; *p;) {
      const char *end = strchr(p, '\n');
      size_t n = end ? (size_t)(end - p) : strlen(p);
      if (*p != '>' && *p != ' ') {
        char line[321];
        snprintf(line, sizeof(line), "%.*s\n", (int)n, p);
        add(body, sizeof(body), line);
      }
      p += n;
      if (*p == '\n')
        ++p;
    }
    if (editor->error)
      add(body, sizeof(body), editor->footer);
  } else {
    snprintf(title, sizeof(title), "%s", braille->mode);
    snprintf(body, sizeof(body), "%s%s\n",
             braille->preview[0] ? braille->preview : "Ready",
             braille->waiting ? "?" : "");
    char held[12], line[64];
    braille_display_cells(&braille->held, braille->held ? 1 : 0, held,
                          sizeof(held));
    snprintf(line, sizeof(line), "Held:%s\nBuf:%u\n", *held ? held : "--",
             braille->pending_count);
    add(body, sizeof(body), line);
    /* Newest cells come first on a narrow screen; older cells remain in the
     * engine. The pending count makes truncation explicit. */
    const char *pending = braille->pending;
    size_t n = strlen(pending);
    if (n > columns * 4) {
      pending += n - columns * 4;
      /* Never display a suffix of a cell as though it were a complete chord. */
      while (*pending && *pending != ' ')
        ++pending;
      if (*pending)
        ++pending;
      add(body, sizeof(body), "... ");
    }
    add(body, sizeof(body), pending);
  }
  /* The four-column OLED uses a one-row stage code, leaving fourteen rows
   * for the selected action and captured input. Wider displays keep titles. */
  if (columns < 8) {
    static const char *const stages[] = {
        "MENU", "MODE", "HOLD", "INPT", "EXST", "POS", "ACT",  "KEY",
        "MODS", "CMD",  "BT",   "MORE", "SAVE", "DEL", "DEL?", "RST?"};
    if (editor->active &&
        (unsigned)editor->screen < sizeof(stages) / sizeof(*stages))
      snprintf(title, sizeof(title), "%s", stages[editor->screen]);
    else if (!strcmp(braille->mode, "computer8"))
      snprintf(title, sizeof(title), "8DOT");
  }
  unsigned heading = columns < 8 ? 1 : 2, content = rows - heading - 1;
  if (!content)
    return 1;
  for (unsigned i = 0; i < heading; i++) {
    size_t start = i * columns, n = strlen(title);
    row(out, size, start < n ? title + start : "", start < n ? n - start : 0,
        columns);
  }
  /* Flatten existing newlines to spaces before wrapping; no data disappears
   * at an accidental short line or control-character boundary. */
  for (char *p = body; *p; p++)
    if (*p == '\n')
      *p = ' ';
  size_t len = strlen(body);
  unsigned capacity = columns * content,
           pages = (len + capacity - 1) / capacity;
  if (!pages)
    pages = 1;
  page %= pages;
  for (unsigned i = 0; i < content; i++) {
    size_t start = page * capacity + i * columns;
    row(out, size, start < len ? body + start : "",
        start < len ? len - start : 0, columns);
  }
  char footer[65];
  snprintf(footer, sizeof(footer), "%u/%u", page + 1, pages);
  row(out, size, footer, strlen(footer), columns);
  return pages;
}
