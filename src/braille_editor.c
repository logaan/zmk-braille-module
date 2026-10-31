// SPDX-License-Identifier: MIT
#include <braille_display.h>
#include <braille_editor.h>
#include <braille_settings.h>
#include <braille_storage.h>
#include <braille_tables.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
#include <braille_grade2.h>
#endif

static enum braille_editor_screen screen;
static size_t selected, result_count;
static struct braille_search_result results[CONFIG_ZMK_BRAILLE_SEARCH_RESULTS];
static struct braille_rule draft;
static struct braille_editor_candidate behavior;
static struct braille_editor_command command;
static char query[BRAILLE_EDITOR_QUERY_SIZE];
static unsigned parameter, mods;
static int error;
static uint16_t query_cells[3];
static uint8_t query_count;
static struct braille_context query_context;
static struct zmk_behavior_binding_event last_event;
static const uint16_t letters[] = {1,  3,  9,  25, 17, 11, 27, 19, 10,
                                   26, 5,  7,  13, 29, 21, 15, 31, 23,
                                   14, 30, 37, 39, 58, 45, 61, 53};
static char letter(uint16_t mask) {
  for (unsigned i = 0; i < 26; ++i)
    if (mask == letters[i])
      return 'a' + i;
  return 0;
}
static void go(enum braille_editor_screen next) {
  screen = next;
  selected = 0;
  error = 0;
}
static void search_update(void) {
  bool keys = screen == BR_ED_KEY;
  result_count = braille_search(
      query,
      keys ? braille_editor_key_count() : braille_editor_behavior_count(),
      keys ? braille_editor_key : braille_editor_behavior, results,
      CONFIG_ZMK_BRAILLE_SEARCH_RESULTS);
  selected = 0;
}
static void search_start(enum braille_editor_screen next) {
  go(next);
  query[0] = 0;
  query_count = 0;
  query_context = BRAILLE_CONTEXT_INIT;
  search_update();
}
static void new_rule(void) {
  memset(&draft, 0, sizeof(draft));
  draft.class = BRAILLE_CLASS_NEUTRAL;
  go(BR_ED_CAPTURE);
}
static bool has_mapping(void) {
  struct braille_rule r;
#ifdef CONFIG_ZMK_BRAILLE_GRADE2
  struct braille_context context = BRAILLE_CONTEXT_INIT;
  if (braille_grade2_match(draft.chords, draft.chord_count, 0, &context, false,
                           &r) &&
      r.chord_count == draft.chord_count)
    return true;
#endif
  for (size_t i = 0; i < braille_storage_count(); ++i)
    if (!braille_storage_rule(i, &r) && r.chord_count == draft.chord_count &&
        !memcmp(r.chords, draft.chords, draft.chord_count * 2))
      return true;
  for (size_t m = 0; m < braille_tables_count(); ++m) {
    const struct braille_mode *mode = braille_tables_mode(m);
    const struct braille_rule_table *tables[] = {mode->characters,
                                                 mode->commands};
    for (unsigned t = 0; t < 2; ++t)
      for (size_t i = 0; i < tables[t]->count; ++i) {
        const struct braille_rule *p = &tables[t]->rules[i];
        if (p->chord_count == draft.chord_count &&
            !memcmp(p->chords, draft.chords, draft.chord_count * 2))
          return true;
      }
  }
  return false;
}
static void finish_capture(void) {
  go(has_mapping() ? BR_ED_COLLISION : BR_ED_POSITION);
}
static void append_query(const char *s) {
  size_t n = strlen(query), len = strlen(s);
  if (n + len >= sizeof(query)) {
    error = -ENOSPC;
    return;
  }
  memcpy(query + n, s, len + 1);
  search_update();
}
/* Read the same UEB1 rules without changing the live engine, mode, overrides,
 * or dispatch. Search accepts textual signs only; indicators remain silent. */
static void query_chord(uint16_t mask) {
  if (mask == 0x100) {
    append_query(" ");
    query_count = 0;
    query_context = BRAILLE_CONTEXT_INIT;
    return;
  }
  if (query_count == 3)
    query_count = 0;
  query_cells[query_count++] = mask;
  const struct braille_rule *best = NULL;
  bool prefix = false;
  for (size_t i = 0; i < braille_rules_ueb1_characters.count; ++i) {
    const struct braille_rule *r = &braille_rules_ueb1_characters.rules[i];
    if (!braille_rule_when_holds(&r->when, &query_context) ||
        r->chord_count < query_count ||
        memcmp(r->chords, query_cells, query_count * 2))
      continue;
    if (r->chord_count > query_count)
      prefix = true;
    else
      best = r;
  }
  if (prefix && !best)
    return;
  if (best) {
    char text[96];
    braille_display_rule(best, text, sizeof(text));
    if (best->action_count && text[0] != '[')
      append_query(text);
    if (best->class == BRAILLE_CLASS_DIGIT)
      query_context.numeric = true;
    else if (best->class == BRAILLE_CLASS_LETTER ||
             best->class == BRAILLE_CLASS_CAPITAL_LETTER)
      query_context.numeric = false;
    if (best->class == BRAILLE_CLASS_CAPS_INDICATOR)
      query_context.caps = best->sets;
    query_count = 0;
    return;
  }
  query_count = 0;
  error = -EINVAL;
}
static size_t choices(void) {
  switch (screen) {
  case BR_ED_HOME:
    return 5;
  case BR_ED_MODES:
    return braille_tables_count();
  case BR_ED_CAPTURE_MORE:
    return 3;
  case BR_ED_COLLISION:
    return 3;
  case BR_ED_POSITION:
    return 4;
  case BR_ED_BEHAVIOR:
  case BR_ED_KEY:
    return result_count;
  case BR_ED_MODIFIERS:
    return 5;
  case BR_ED_COMMAND:
    return braille_editor_command_count(behavior.id);
  case BR_ED_PROFILE:
    return command.max >= command.min ? (size_t)(command.max - command.min + 1)
                                      : 0;
  case BR_ED_ACTION_MORE:
    return draft.action_count < BRAILLE_RULE_MAX_ACTIONS ? 3 : 2;
  case BR_ED_CONFIRM:
    return draft.action_count + 2;
  case BR_ED_REMOVE_CONFIRM:
  case BR_ED_RESET_CONFIRM:
    return 2;
  case BR_ED_REMOVE:
    return braille_storage_count();
  default:
    return 0;
  }
}
static void action_done(void) {
  ++draft.action_count;
  go(BR_ED_ACTION_MORE);
}
static void key_done(void) {
  struct zmk_behavior_binding *a = &draft.actions[draft.action_count];
  uint32_t *p = parameter == 0 ? &a->param1 : &a->param2;
  *p |= (uint32_t)mods << 24;
  if (++parameter < behavior.parameters)
    search_start(BR_ED_KEY);
  else
    action_done();
}
static void activate(void) {
  size_t count = choices();
  if (!count || selected >= count) {
    error = -ENOENT;
    return;
  }
  switch (screen) {
  case BR_ED_HOME:
    if (selected == 0)
      go(BR_ED_MODES);
    else if (selected == 1)
      new_rule();
    else if (selected == 2)
      go(BR_ED_REMOVE);
    else if (selected == 3)
      go(BR_ED_RESET_CONFIRM);
    else
      braille_settings_exit(last_event);
    break;
  case BR_ED_MODES:
    error = braille_tables_select(braille_tables_mode(selected)->name);
    if (!error)
      error = braille_storage_last_error();
    break;
  case BR_ED_CAPTURE_MORE:
    if (selected == 0)
      go(BR_ED_CAPTURE);
    else if (selected == 1)
      finish_capture();
    else
      go(BR_ED_HOME);
    break;
  case BR_ED_COLLISION:
    if (selected == 0)
      go(BR_ED_POSITION);
    else if (selected == 1)
      new_rule();
    else
      go(BR_ED_HOME);
    break;
  case BR_ED_POSITION:
    draft.word_position = (enum braille_word_position)selected;
    search_start(BR_ED_BEHAVIOR);
    break;
  case BR_ED_BEHAVIOR:
    if (!braille_editor_behavior(results[selected].index, &behavior)) {
      error = -ENOENT;
      break;
    }
    draft.actions[draft.action_count] =
        (struct zmk_behavior_binding){.behavior_dev = behavior.id};
    parameter = 0;
    if (behavior.commands)
      go(BR_ED_COMMAND);
    else if (behavior.parameters)
      search_start(BR_ED_KEY);
    else
      action_done();
    break;
  case BR_ED_KEY: {
    struct braille_editor_candidate c;
    braille_editor_key(results[selected].index, &c);
    struct zmk_behavior_binding *a = &draft.actions[draft.action_count];
    if (parameter == 0)
      a->param1 = c.value;
    else
      a->param2 = c.value;
    mods = 0;
    go(BR_ED_MODIFIERS);
    break;
  }
  case BR_ED_MODIFIERS:
    if (selected == 4)
      key_done();
    else
      mods ^= 1u << selected;
    break;
  case BR_ED_COMMAND:
    if (!braille_editor_command(behavior.id, selected, &command)) {
      error = -EINVAL;
      break;
    }
    draft.actions[draft.action_count].param1 = command.value;
    if (command.profile)
      go(BR_ED_PROFILE);
    else
      action_done();
    break;
  case BR_ED_PROFILE:
    draft.actions[draft.action_count].param2 = command.min + selected;
    action_done();
    break;
  case BR_ED_ACTION_MORE:
    if (selected == 0)
      go(BR_ED_CONFIRM);
    else if (selected == 1 && draft.action_count < BRAILLE_RULE_MAX_ACTIONS)
      search_start(BR_ED_BEHAVIOR);
    else
      go(BR_ED_HOME);
    break;
  case BR_ED_CONFIRM:
    if (selected < draft.action_count)
      break;
    if (selected > draft.action_count) {
      go(BR_ED_HOME);
      break;
    }
    error = braille_storage_save(&draft);
    if (!error)
      braille_settings_exit(last_event);
    break;
  case BR_ED_REMOVE:
    error = braille_storage_rule(selected, &draft);
    if (!error)
      go(BR_ED_REMOVE_CONFIRM);
    break;
  case BR_ED_REMOVE_CONFIRM:
    if (selected) {
      go(BR_ED_HOME);
      break;
    }
    error = braille_storage_remove(draft.chords, draft.chord_count,
                                   draft.word_position);
    if (!error)
      go(BR_ED_HOME);
    break;
  case BR_ED_RESET_CONFIRM:
    if (selected) {
      go(BR_ED_HOME);
      break;
    }
    error = braille_storage_reset();
    if (!error)
      go(BR_ED_HOME);
    break;
  default:
    break;
  }
}
void braille_settings_changed(bool active,
                              struct zmk_behavior_binding_event event) {
  last_event = event;
  if (active) {
    go(BR_ED_HOME);
    query[0] = 0;
    query_count = 0;
  }
}
int braille_settings_chord(uint16_t mask,
                           struct zmk_behavior_binding_event event) {
  last_event = event;
  error = 0;
  /* Capture is deliberately literal, even for escape and navigation. */
  if (screen == BR_ED_CAPTURE) {
    draft.chords[draft.chord_count++] = mask;
    if (draft.chord_count == BRAILLE_RULE_MAX_CHORDS)
      finish_capture();
    else
      go(BR_ED_CAPTURE_MORE);
    return 0;
  }
  /* VoiceOver Mac Navigation: space+4 next, space+1 previous,
   * space+15 Return/activate, space+12 Escape/back, space+145 Delete.
   * Project-local space+123456 toggles/finishes settings (not a VO claim). */
  if (mask == 0x13f) {
    return braille_settings_exit(event);
  }
  if (mask == 0x103) {
    if (screen == BR_ED_HOME)
      return braille_settings_exit(event);
    go(BR_ED_HOME);
    return 0;
  }
  if (mask == 0x108 || mask == 0x101) {
    size_t n = choices();
    if (n)
      selected = (selected + n + (mask == 0x108 ? 1 : -1)) % n;
    return 0;
  }
  if (mask == 0x111 || mask == 0x180 || mask == 0x80) {
    activate();
    return 0;
  }
  if (screen == BR_ED_BEHAVIOR || screen == BR_ED_KEY) {
    if (mask == 0x119 || mask == 0x40 || mask == 0x140) {
      if (query_count)
        query_count = 0;
      else if (*query)
        query[strlen(query) - 1] = 0;
      search_update();
      return 0;
    }
    query_chord(mask);
    return 0;
  }
  char c = letter(mask);
  if (screen == BR_ED_MODES) {
    const char *name = c == 'c'   ? "computer8"
                       : c == 'u' ? "ueb1"
                       : c == 't' ? "ueb2"
                       : c == 's' ? "computer8_short"
                                  : NULL;
    for (size_t i = 0; name && i < braille_tables_count(); ++i) {
      if (!strcmp(name, braille_tables_mode(i)->name)) {
        selected = i;
        activate();
        return 0;
      }
    }
  }
  const char *shortcuts = NULL;
  switch (screen) {
  case BR_ED_HOME:
    shortcuts = "mnurd";
    break; /* mode,new,unbind,reset,done */
  case BR_ED_CAPTURE_MORE:
    shortcuts = "afc";
    break;
  case BR_ED_COLLISION:
    shortcuts = "orc";
    break;
  case BR_ED_POSITION:
    shortcuts = "asme";
    break;
  case BR_ED_MODIFIERS:
    shortcuts = "csogd";
    break; /* control,shift,option,gui,done */
  case BR_ED_ACTION_MORE:
    shortcuts = draft.action_count < 16 ? "fac" : "fc";
    break;
  case BR_ED_CONFIRM:
    shortcuts = "sc";
    break;
  case BR_ED_REMOVE_CONFIRM:
  case BR_ED_RESET_CONFIRM:
    shortcuts = "rc";
    break;
  default:
    break;
  }
  if (screen == BR_ED_CONFIRM && (c == 's' || c == 'c')) {
    selected = draft.action_count + (c == 'c');
    activate();
    return 0;
  }
  if (shortcuts && c) {
    const char *p = strchr(shortcuts, c);
    if (p) {
      selected = p - shortcuts;
      activate();
    }
  }
  return 0;
}
static void append(char *out, size_t size, const char *format, ...) {
  size_t used = strlen(out);
  if (used >= size)
    return;
  va_list args;
  va_start(args, format);
  vsnprintf(out + used, size - used, format, args);
  va_end(args);
}
static const char *choice(size_t i, char *tmp, size_t size) {
  static const char *home[] = {"m Mode", "n New binding", "u Unbind",
                               "r Reset overrides", "d Done"};
  static const char *more[] = {"a Add chord", "f Finish input", "c Cancel"};
  static const char *collision[] = {"o Override", "r Re-record", "c Cancel"};
  static const char *position[] = {"a Any position", "s Word start",
                                   "m Word middle", "e Word end"};
  static const char *modifiers[] = {"c Control", "s Shift", "o Option",
                                    "g Command", "d Done"};
  struct braille_editor_candidate c;
  struct braille_editor_command cmd;
  struct braille_rule r;
  switch (screen) {
  case BR_ED_HOME:
    return home[i];
  case BR_ED_MODES: {
    const char *name = braille_tables_mode(i)->name;
    char shortcut = !strcmp(name, "computer8") ? 'c'
                    : !strcmp(name, "ueb1")    ? 'u'
                    : !strcmp(name, "computer8_short") ? 's'
                                               : 't';
    snprintf(tmp, size, "%c %s", shortcut, name);
    return tmp;
  }
  case BR_ED_CAPTURE_MORE:
    return more[i];
  case BR_ED_COLLISION:
    return collision[i];
  case BR_ED_POSITION:
    return position[i];
  case BR_ED_BEHAVIOR:
  case BR_ED_KEY:
    (screen == BR_ED_KEY ? braille_editor_key
                         : braille_editor_behavior)(results[i].index, &c);
    return c.name;
  case BR_ED_MODIFIERS:
    snprintf(tmp, size, "%s %s", modifiers[i],
             i < 4 && (mods & (1u << i)) ? "ON" : "");
    return tmp;
  case BR_ED_COMMAND:
    braille_editor_command(behavior.id, i, &cmd);
    return cmd.name;
  case BR_ED_PROFILE:
    snprintf(tmp, size, "Profile %u", (unsigned)(command.min + i + 1));
    return tmp;
  case BR_ED_ACTION_MORE:
    return i == 0                              ? "f Finish actions"
           : i == 1 && draft.action_count < 16 ? "a Add action"
                                               : "c Cancel";
  case BR_ED_CONFIRM:
    if (i < draft.action_count) {
      snprintf(tmp, size, "%u %s %lx %lx", (unsigned)i + 1,
               draft.actions[i].behavior_dev,
               (unsigned long)draft.actions[i].param1,
               (unsigned long)draft.actions[i].param2);
      return tmp;
    }
    return i == draft.action_count ? "s Save" : "c Cancel";
  case BR_ED_REMOVE:
    braille_storage_rule(i, &r);
    braille_display_cells(r.chords, r.chord_count, tmp, size);
    return tmp;
  case BR_ED_REMOVE_CONFIRM:
    return i ? "c Cancel" : "r Remove";
  case BR_ED_RESET_CONFIRM:
    return i ? "c Cancel" : "r Reset all overrides";
  default:
    return "";
  }
}
void braille_editor_snapshot(struct braille_editor_snapshot *out) {
  memset(out, 0, sizeof(*out));
  out->active = braille_settings_active();
  out->screen = screen;
  out->error = error;
  if (!out->active)
    return;
  static const char *titles[] = {
      "Braille settings", "Character mode",    "Hold input chord",
      "Captured input",   "Mapping exists",    "Rule position",
      "Find behavior",    "Find keycode",      "Key modifiers",
      "Command",          "Bluetooth profile", "Action sequence",
      "Confirm binding",  "Remove binding",    "Confirm removal",
      "Reset overrides"};
  snprintf(out->title, sizeof(out->title), "%s", titles[screen]);
  snprintf(out->query, sizeof(out->query), "%s", query);
  if (screen == BR_ED_BEHAVIOR || screen == BR_ED_KEY)
    append(out->body, sizeof(out->body), "Search: %s\n", query);
  if (screen == BR_ED_CAPTURE || screen == BR_ED_CAPTURE_MORE ||
      screen == BR_ED_COLLISION || screen == BR_ED_CONFIRM ||
      screen == BR_ED_REMOVE_CONFIRM) {
    char cells[90];
    braille_display_cells(draft.chords, draft.chord_count, cells,
                          sizeof(cells));
    append(out->body, sizeof(out->body), "Input: %s\n", *cells ? cells : "--");
  }
  if (screen == BR_ED_CONFIRM || screen == BR_ED_REMOVE_CONFIRM) {
    append(out->body, sizeof(out->body), "Position %u; %u actions\n",
           draft.word_position, draft.action_count);
    /* Navigate the action list to review every binding without truncation. */
  }
  size_t n = choices(), start = selected > 1 ? selected - 1 : 0;
  for (size_t i = start; i < n && i < start + 3; ++i) {
    char tmp[96];
    append(out->body, sizeof(out->body), "%c %s\n", i == selected ? '>' : ' ',
           choice(i, tmp, sizeof(tmp)));
  }
  if (screen == BR_ED_CAPTURE)
    append(out->body, sizeof(out->body), "Every chord is recorded.\n");
  if (!n && (screen == BR_ED_BEHAVIOR || screen == BR_ED_KEY ||
             screen == BR_ED_REMOVE))
    append(out->body, sizeof(out->body), "No matches\n");
  if (error)
    snprintf(out->footer, sizeof(out->footer),
             "Failed %d: edit/retry or Escape", error);
  else
    snprintf(out->footer, sizeof(out->footer),
             "Sp+1/4 select; Sp+15 enter; Sp+12 back");
}
