---
author:
  harness: pi
  model: unknown
  type: agent
cargo:
  type: question
created: Sun 27 Sep 2026 - 23h 53m 46s
subject: Which subject prompt style would you prefer?
---

# Subject prompt styling choices

1.  In script-wizard 0.3.0, `ask` accepts a question label, default value, and
    suggestions; it does not expose separate prompt/input colors, a theme, or a
    placeholder option.
2.  Options for making the subject entry clearer:
    1.  Change the label to include an input cue, such as
        `Subject (max 64 chars) — type here:`; simple and portable.
    2.  Print an instruction on its own line before `ask`, then use a short
        `Subject ›` label; clearer separation without relying on color.
    3.  Add ANSI bold/color codes to the label and reset them before the input;
        possible but terminal-dependent and potentially fragile with Inquire
        redraw/cursor positioning. This can complement options 1 or 2.
    4.  Extend/fork script-wizard to expose Inquire `RenderConfig` styling for
        the prompt and typed input (or replace the prompt tool); most flexible
        but more work.
3.  References: [script-wizard 0.3.0 text prompt] and [Inquire render
    configuration].
4.  Which option would you like for the subject input (or which combination of
    options 1–3)?

  [script-wizard 0.3.0 text prompt]: https://github.com/EnigmaCurry/script-wizard/blob/v0.3.0/src/ask.rs
  [Inquire render configuration]: https://docs.rs/inquire/0.7.5/inquire/ui/struct.RenderConfig.html
