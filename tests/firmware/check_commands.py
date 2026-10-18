#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Require all real-ZMK command, sticky/HID and timeout checkpoints."""
import argparse
from pathlib import Path
import re


def check(log):
    if re.search(r"ASSERTION FAIL|ZEPHYR FATAL ERROR|LOG_DROPPED|messages dropped", log, re.I):
        raise AssertionError("Native test asserted, crashed or dropped log messages")
    actual = re.findall(r"factory_commands: (.+) PASS", log)
    assert actual == ["ueb1", "computer8", "keyboard-timeout", "globe-timeout", "ALL"], actual


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    args = parser.parse_args()
    check(args.log.read_text())
    print("PASS: both command tables, real ZMK sticky modifiers/Globe, HID and timeouts")
