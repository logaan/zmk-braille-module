// SPDX-License-Identifier: MIT
#include <stdio.h>
#include <string.h>
#include <braille_display.h>
#include <braille_chord.h>
#include <braille_engine.h>
#include <braille_tables.h>
#include <dt-bindings/zmk/keys.h>

static void append(char *out, size_t size, const char *text) {
    size_t used = strlen(out);
    if (used < size) snprintf(out + used, size - used, "%s", text);
}

void braille_display_cells(const uint16_t *cells, size_t count, char *out, size_t size) {
    if (!size) return;
    out[0] = 0;
    for (size_t i = 0; i < count; ++i) {
        char cell[11];
        size_t n = 0;
        if (i) append(out, size, " ");
        for (unsigned dot = 0; dot < 8; ++dot)
            if (cells[i] & (1u << dot)) cell[n++] = '1' + dot;
        if (cells[i] & BRAILLE_CHORD_SPACE_BIT) cell[n++] = 's';
        cell[n] = 0;
        append(out, size, cell);
    }
}

void braille_display_rule(const struct braille_rule *rule, char *out, size_t size) {
    if (!size) return;
    out[0] = 0;
    if (!rule->action_count) { append(out, size, "indicator"); return; }
    for (unsigned i = 0; i < rule->action_count; ++i) {
        const struct zmk_behavior_binding *action = &rule->actions[i];
        if (!action->behavior_dev || strcmp(action->behavior_dev, DEVICE_DT_NAME(DT_NODELABEL(kp)))) {
            append(out, size, "[action]"); continue;
        }
        uint32_t key = action->param1;
        unsigned usage = key & 0xffff;
        unsigned mods = key >> 24;
        bool shift = mods & 0x22;
        if (((key >> 16) & 0xff) != 7 || (mods & ~0x22u)) {
            append(out, size, "[key]"); continue;
        }
        char c = 0;
        if (usage >= 4 && usage <= 29) c = (shift ? 'A' : 'a') + usage - 4;
        else if (usage >= 30 && usage <= 39) c = (shift ? "!@#$%^&*()" : "1234567890")[usage - 30];
        else if (usage == 44) c = ' ';
        else if (usage >= 45 && usage <= 56) c = (shift ? "_+{}|~:\"~<>?" : "-=[]\\#;'`,./")[usage - 45];
        if (c) { char text[2] = {c, 0}; append(out, size, text); }
        else {
            const char *name = usage == 40 ? "[Return]" : usage == 42 ? "[Delete]" :
                usage == 43 ? "[Tab]" : usage == 41 ? "[Escape]" : "[key]";
            append(out, size, name);
        }
    }
}

void braille_display_snapshot(struct braille_display_snapshot *out) {
    memset(out, 0, sizeof(*out));
    const char *mode = braille_tables_active();
    snprintf(out->mode, sizeof(out->mode), "%s", mode ? mode : "--");
    out->held = braille_chord_held_mask();
    out->pending_count = braille_engine_pending_count();
    braille_display_cells(braille_engine_pending_cells(), out->pending_count,
                          out->pending, sizeof(out->pending));
    if (out->held) {
        struct braille_rule rule;
        if (braille_engine_preview_rule(out->held, &rule, &out->waiting) > 0)
            braille_display_rule(&rule, out->preview, sizeof(out->preview));
        else snprintf(out->preview, sizeof(out->preview), "%s", out->waiting ? "prefix..." : "unmapped");
    }
}
