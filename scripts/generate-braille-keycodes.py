#!/usr/bin/env python3
"""Generate symbolic keycode search entries from the build's ZMK header."""
import re
import sys
from pathlib import Path
header, output = map(Path, sys.argv[1:])
names = sorted(set(re.findall(r'^#define\s+([A-Z][A-Z0-9_]*)\s+\S', header.read_text(), re.M)))
output.write_text('/* Generated from the selected ZMK keys.h; do not edit. */\n' +
                  ''.join('{"%s", %s},\n' % (name, name) for name in names))
