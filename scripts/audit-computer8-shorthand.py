#!/usr/bin/env python3
"""Reproduce the raw UEB2/Computer8 collision audit without optional packages."""
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]


def patterns(root=ROOT):
    text = (root / 'references/research/computer8.yaml').read_text()
    rows = re.findall(r'^  - \[([^\]]+)\] # 0x[0-9a-f]+\.\.0x[0-9a-f]+$', text, re.M)
    result = []
    for row in rows:
        result.extend(int(v.strip()) if v.strip().isdigit() else v.strip()
                      for v in row.split(','))
    if len(result) != 256:
        raise ValueError('Computer8 reference must enumerate all 256 cells')
    return result


def dots(cell):
    return ''.join(str(i + 1) for i in range(8) if cell & (1 << i))


def audit(root=ROOT):
    table = patterns(root)
    source = 'references/research/ueb-grade2-contractions.yaml'
    text = '\n'.join(line for line in (root / source).read_text().splitlines()
                     if not line.startswith('#'))
    contractions = json.loads(text)['contractions']
    rows = []
    for item in contractions:
        cells = item['cells']
        values = [table[cell] for cell in cells]
        if not all(isinstance(value, int) for value in values):
            raise ValueError('Reassess this audit: a contraction uses an unmapped cell')
        literal = ''.join(chr(value) for value in values)
        if literal == item['text']:
            raise ValueError('Reassess this audit: a contraction is already literal')
        rows.append({
            'contraction': item['text'], 'cells': cells,
            'dots': [dots(cell) for cell in cells],
            'condition': item['condition'], 'iceb_section': item['section'],
            'computer8_literal': literal,
            'conflict': 'existing_character' if len(cells) == 1 else 'literal_sequence',
            'decision': 'preserve_computer8_pending_user_review',
        })
    sources = ['references/research/computer8.yaml', source,
               'src/braille_rules_computer8.c', 'src/braille_rules_nav.c']
    return {
        'scope': 'Raw, unchanged UEB2 contraction spellings in the project Computer8 mapping',
        'source_sha256': {name: hashlib.sha256((root / name).read_bytes()).hexdigest()
                          for name in sources},
        'policy': 'Existing Computer8 characters, literal sequences, and commands win.',
        'counts': {
            'contractions': len(rows),
            'existing_character_conflicts': sum(len(r['cells']) == 1 for r in rows),
            'literal_sequence_conflicts': sum(len(r['cells']) > 1 for r in rows),
            'unchanged_spelling_compatible_subset': 0,
            'mapped_nonzero_six_dot_cells': sum(isinstance(table[i], int) for i in range(1, 64)),
        },
        'context_note': 'Word-position restrictions do not remove collisions: the same literal ASCII input remains valid at those positions.',
        'timing_note': 'Computer8 emits mapped cells immediately. Buffering a literal prefix to wait for shorthand is also an observable change.',
        'indicator_conflicts': [
            {'ueb': 'capital indicator', 'dots': '6', 'computer8': ','},
            {'ueb': 'numeric indicator', 'dots': '3456', 'computer8': '#'},
            {'ueb': 'grade-1 indicator', 'dots': '56', 'computer8': ';'},
        ],
        'commands_note': 'Dot7 Delete, dot8 Return, and space-bearing factory commands are existing definitions and must remain intact.',
        'additive_options_for_review': [
            {'name': 'explicit_prefix', 'dots': '78', 'mask': 192,
             'current_definition': table[192],
             'description': 'An unused prefix can explicitly enter a bounded shorthand sequence; raw Computer8 remains literal outside it.'},
            {'name': 'dot8_on_first_cell',
             'currently_reserved_first_cells': all(table[cell | 128] == 'reserved' for cell in range(1, 64)),
             'description': 'Adding dot8 to a nonzero six-dot first cell uses reserved input, but invents new spellings and needs a clear scope/termination policy.'},
        ],
        'conflicts': rows,
    }


if __name__ == '__main__':
    print(json.dumps(audit(), indent=2, ensure_ascii=False))
