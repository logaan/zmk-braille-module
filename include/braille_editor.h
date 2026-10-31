// SPDX-License-Identifier: MIT
#pragma once
#include <braille_rules.h>
#ifndef CONFIG_ZMK_BRAILLE_SEARCH_RESULTS
#define CONFIG_ZMK_BRAILLE_SEARCH_RESULTS 16
#endif
#define BRAILLE_EDITOR_QUERY_SIZE 32
struct braille_editor_candidate {
  const char *name, *id;
  uint32_t value;
  unsigned parameters;
  bool commands;
};
struct braille_editor_command {
  const char *name;
  uint32_t value;
  int min, max;
  bool profile;
};
/* Pure registry adapter; never invokes a behavior. */
size_t braille_editor_behavior_count(void);
bool braille_editor_behavior(size_t index,
                             struct braille_editor_candidate *out);
size_t braille_editor_key_count(void);
bool braille_editor_key(size_t index, struct braille_editor_candidate *out);
size_t braille_editor_command_count(const char *id);
bool braille_editor_command(const char *id, size_t index,
                            struct braille_editor_command *out);
struct braille_search_result {
  size_t index;
  int score;
};
typedef bool (*braille_search_source)(size_t,
                                      struct braille_editor_candidate *);
int braille_search_score(const char *query, const char *name);
size_t braille_search(const char *query, size_t count,
                      braille_search_source source,
                      struct braille_search_result *out, size_t capacity);
enum braille_editor_screen {
  BR_ED_HOME,
  BR_ED_MODES,
  BR_ED_CAPTURE,
  BR_ED_CAPTURE_MORE,
  BR_ED_COLLISION,
  BR_ED_POSITION,
  BR_ED_BEHAVIOR,
  BR_ED_KEY,
  BR_ED_MODIFIERS,
  BR_ED_COMMAND,
  BR_ED_PROFILE,
  BR_ED_ACTION_MORE,
  BR_ED_CONFIRM,
  BR_ED_REMOVE,
  BR_ED_REMOVE_CONFIRM,
  BR_ED_RESET_CONFIRM
};
struct braille_editor_snapshot {
  bool active;
  enum braille_editor_screen screen;
  char title[40];
  char body[320];
  char footer[80];
  char query[BRAILLE_EDITOR_QUERY_SIZE];
  int error;
};
void braille_editor_snapshot(struct braille_editor_snapshot *out);
