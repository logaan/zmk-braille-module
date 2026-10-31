// SPDX-License-Identifier: MIT
/* Deterministic hostile cell streams exercise bounded pending/preview paths. */
#include <assert.h>
#include <stdint.h>
#include <braille_engine.h>
#include <braille_chord.h>
#include <braille_tables.h>
static uint32_t seed=0x12345678;
static uint32_t next(void) {seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return seed;}
int braille_engine_commit_rule(const struct braille_rule *r, enum braille_commit_cause cause,
 struct zmk_behavior_binding_event e) {
 (void)cause;(void)e;assert(r->chord_count<=BRAILLE_RULE_MAX_CHORDS);
 assert(r->action_count<=BRAILLE_RULE_MAX_ACTIONS);return 0;
}
int main(void) {
 struct zmk_behavior_binding_event e={0};
 const char *modes[]={"ueb2","ueb1","computer8"};
 for(unsigned batch=0;batch<400;batch++) {
  assert(!braille_tables_select(modes[batch%3]));
  for(unsigned i=0;i<96;i++) {
   uint16_t cell=(next()%4)?(next()%63)+1:(next()%511)+1;
   assert(!braille_chord_resolved(cell,e));
   assert(braille_engine_pending_count()<=BRAILLE_WORD_MAX_CELLS);
   struct braille_rule preview;bool waiting;
   (void)braille_engine_preview_rule((next()%511)+1,&preview,&waiting);
   if(!(i%19))braille_engine_reset();
  }
  assert(!braille_chord_resolved(256,e));
 }
 return 0;
}
