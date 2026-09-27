---
author: user
cargo:
  status: draft
  type: task
created: Sun 27 Sep 14h42
subject: Draft
---

# Find macbook keys

1.  Find out which keys are able to be typed on any Apple Macbook.
    1.  You may find the `kagi` tool helpful.
    2.  It doesn't matter which macbook, they'll all largely be the same.
2.  Write The full list as a yaml file.
    1.  Each key should have 3 values.
        1.  `base` What happens when the key is pressed without shift.
        2.  `shifted` What happens when the is pressed with shift.
        3.  `character` Whether the key produces a printable character
        4.  Examples
            1.  `{base: 'h', shifted: 'H', character: true}`
            2.  `{base: ';', shifted: ':', character: true}`
            3.  `{base: 'F1', shifted: 'F1', character: false}`
3.  The yaml file should be in `references/mba-keys.yaml`
