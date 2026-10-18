#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Require every assertion checkpoint from the real ZMK mode/settings fixture."""
import argparse
from pathlib import Path
import re


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    args = parser.parse_args()
    log = args.log.read_text()
    if re.search(r"ASSERTION FAIL|ZEPHYR FATAL ERROR|LOG_DROPPED|messages dropped", log, re.I):
        raise SystemExit("Native test asserted, crashed, or dropped log messages")
    actual = re.findall(r"mode_settings_checkpoint: (\d+) PASS", log)
    assert actual == [str(i) for i in range(12)], actual
    print("PASS: 12 native ZMK mode/settings checkpoints; no host keycode events")


if __name__ == "__main__":
    main()
