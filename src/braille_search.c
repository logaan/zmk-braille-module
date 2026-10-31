// SPDX-License-Identifier: MIT
#include <braille_editor.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
static bool boundary(char c) { return c == '_' || c == '-' || c == ' '; }
static int fold(char c) { return tolower((unsigned char)c); }
int braille_search_score(const char *query, const char *name) {
  if (!*query)
    return 0;
  size_t end = 0, q = 0, prev = 0;
  bool word = false;
  while (query[q]) {
    if (boundary(query[q])) {
      word = true;
      ++q;
      continue;
    }
    while (name[end] && (fold(name[end]) != fold(query[q]) ||
                         (word && end && !boundary(name[end - 1]))))
      ++end;
    if (!name[end])
      return INT_MIN;
    prev = end++;
    ++q;
    word = false;
  }
  /* Backward greedy scan tightens the first forward match. */
  size_t begin = prev, at = prev + 1;
  q = strlen(query);
  while (q) {
    char c = query[--q];
    if (boundary(c))
      continue;
    /* Tightening must retain the word-start requirement of the forward
     * match. Otherwise an interior repeated letter can hide a valid match
     * (for example "a ab" in "a aab"). */
    bool word_start = q && boundary(query[q - 1]);
    while (at && (fold(name[at - 1]) != fold(c) ||
                  (word_start && at > 1 && !boundary(name[at - 2]))))
      --at;
    if (!at)
      return INT_MIN;
    begin = --at;
  }
  int score = 0;
  size_t i = begin, last = SIZE_MAX;
  q = 0;
  word = false;
  while (query[q]) {
    if (boundary(query[q])) {
      word = true;
      ++q;
      continue;
    }
    while (name[i] && (fold(name[i]) != fold(query[q]) ||
                       (word && i && !boundary(name[i - 1]))))
      ++i;
    if (!name[i])
      return INT_MIN;
    score += 16;
    if (!i || boundary(name[i - 1]))
      score += 12;
    if (last != SIZE_MAX)
      score += i == last + 1 ? 10 : -(int)(i - last - 1);
    last = i++;
    ++q;
    word = false;
  }
  if (!begin)
    score += 24;
  if (!begin && i == strlen(name))
    score += 32;
  return score - (int)begin;
}
static bool precedes(const struct braille_search_result *a,
                     const struct braille_search_result *b,
                     braille_search_source source, bool empty) {
  struct braille_editor_candidate x, y;
  source(a->index, &x);
  source(b->index, &y);
  if (a->score != b->score)
    return a->score > b->score;
  if (!empty && strlen(x.name) != strlen(y.name))
    return strlen(x.name) < strlen(y.name);
  return strcmp(x.name, y.name) < 0;
}
size_t braille_search(const char *query, size_t count,
                      braille_search_source source,
                      struct braille_search_result *out, size_t capacity) {
  size_t n = 0;
  for (size_t i = 0; i < count; ++i) {
    struct braille_editor_candidate c;
    if (!source(i, &c))
      continue;
    int score = braille_search_score(query, c.name),
        alias = braille_search_score(query, c.id ? c.id : c.name);
    if (alias > score)
      score = alias;
    if (score == INT_MIN)
      continue;
    struct braille_search_result r = {i, score};
    size_t p = 0;
    while (p < n && !precedes(&r, &out[p], source, !*query))
      ++p;
    if (p >= capacity)
      continue;
    if (n < capacity)
      ++n;
    for (size_t j = n - 1; j > p; --j)
      out[j] = out[j - 1];
    out[p] = r;
  }
  return n;
}
