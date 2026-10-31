// SPDX-License-Identifier: MIT
#include <assert.h>
#include <string.h>
#include <braille_display.h>
#include <braille_engine.h>
#include <braille_chord.h>
#include <braille_tables.h>
#include <braille_storage.h>
static uint16_t held=1, cells[32];
static uint8_t pending;
static uint32_t revision;
static unsigned calls;
static const char *mode="ueb2";
static struct braille_context context=BRAILLE_CONTEXT_INIT;
static struct braille_rule rules[8];
static size_t count;
uint16_t braille_chord_held_mask(void) { return held; }
const char *braille_tables_active(void) { return mode; }
uint8_t braille_engine_pending_count(void) { return pending; }
const uint16_t *braille_engine_pending_cells(void) { return cells; }
const struct braille_context *braille_engine_context(void) { return &context; }
uint32_t braille_storage_revision(void) { return revision; }
struct braille_override_layer braille_tables_overrides(void) {
    return (struct braille_override_layer){rules,&count,8};
}
int braille_engine_preview_rule(uint16_t h,struct braille_rule *out,bool *waiting) {
    (void)h; ++calls; *waiting=true;
    *out=(struct braille_rule){.action_count=1,.actions={{.behavior_dev=DEVICE_DT_NAME(DT_NODELABEL(kp)),.param1=(7<<16)|4}}};
    return 1;
}
int main(void) {
    struct braille_display_snapshot out;
    for(unsigned i=0;i<100;i++) braille_display_snapshot(&out);
    assert(calls==1 && !strcmp(out.preview,"a") && out.waiting);
    held=3;braille_display_snapshot(&out);assert(calls==2);
    pending=1;cells[0]=1;braille_display_snapshot(&out);assert(calls==3);
    cells[0]=3;braille_display_snapshot(&out);assert(calls==4);
    context.numeric=true;braille_display_snapshot(&out);assert(calls==5);
    mode="ueb1";braille_display_snapshot(&out);assert(calls==6);
    ++revision;braille_display_snapshot(&out);assert(calls==7);
    count=1;braille_display_snapshot(&out);assert(calls==8);
    rules[0].chords[0]=5;braille_display_snapshot(&out);assert(calls==9);
    held=0;braille_display_snapshot(&out);assert(!out.preview[0]);
    held=3;braille_display_snapshot(&out);assert(calls==10);
    return 0;
}
