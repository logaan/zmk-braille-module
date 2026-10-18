#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Compare real native ZMK routing/keystroke events with the reviewed snapshot."""
import argparse
import difflib
from pathlib import Path
import re


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    args = parser.parse_args()
    log = args.log.read_text()
    # Strip only logger prefixes, not fields, ordering, or unexpected events.
    markers = r"(?:braille_edge:|conventional_key:|routing_metadata:)"
    actual = [match.group(0) for match in re.finditer(markers + r"[^\r\n]*", log)]
    expected = (Path(__file__).parent / "routing/routing.snapshot").read_text().splitlines()
    if actual != expected:
        print("\n".join(difflib.unified_diff(expected, actual, fromfile="expected", tofile="actual")))
        raise SystemExit(1)
    if re.search(r"ASSERTION FAIL|ZEPHYR FATAL ERROR|LOG_DROPPED|messages dropped", log, re.I):
        raise SystemExit("Native test asserted, crashed, or dropped log messages")
    print(f"PASS: {len(actual)} native ZMK routing/keystroke events match")


if __name__ == "__main__":
    main()
