#!/usr/bin/env python3
"""Compare raw outputs without treating intended shorthand as a conflict."""
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
    grade1 = (root / 'src/braille_rules_grade1.c').read_text()
    letters = [(int(cell, 16), chr(int(usage, 16) - 4 + ord('a')))
               for cell, usage in re.findall(
                   r'BRAILLE_LETTER_RULE\((0x[0-9A-Fa-f]+), (0x[0-9A-Fa-f]+)\)', grade1)]
    if len(letters) != 26 or any(table[cell] != ord(letter) for cell, letter in letters):
        raise ValueError('Reassess this audit: lowercase base mappings changed')
    rows = []
    for item in contractions:
        cells = item['cells']
        values = [table[cell] for cell in cells]
        if not all(isinstance(value, int) for value in values):
            raise ValueError('Reassess this audit: a contraction uses an unmapped cell')
        literal = ''.join(chr(value) for value in values)
        shared_letter = len(cells) == 1 and item['condition'] == 'ALPHA' and literal.isalpha()
        symbol_conflict = len(cells) == 1 and not shared_letter
        shortform = item['condition'] == 'SHORT'
        enabled = shared_letter or shortform
        rows.append({
            'contraction': item['text'], 'cells': cells,
            'dots': [dots(cell) for cell in cells],
            'condition': item['condition'], 'iceb_section': item['section'],
            'computer8_literal': literal,
            'raw_output_difference': literal != item['text'],
            'input_kind': 'shared_letters' if literal.isalpha() else 'includes_computer8_symbols',
            'semantic_conflict': ('related_meaning_distinct_symbol_output' if symbol_conflict and literal == '&' and item['text'] == 'and'
                                  else 'different_symbol_definition' if symbol_conflict
                                  else ('enabled_symbol_sequence_ambiguity' if enabled else 'excluded_symbol_sequence_ambiguity') if not literal.isalpha()
                                  else 'none_expected_contextual_shorthand'),
            'decision': ('retain_computer8_symbol' if symbol_conflict
                         else 'enabled_contextual_letter_wordsign' if shared_letter
                         else 'enabled_contextual_shortform' if shortform
                         else 'excluded_category_initial_or_final'),
        })
    sources = ['references/research/computer8.yaml', source,
               'src/braille_rules_computer8.c', 'src/braille_rules_nav.c', 'src/braille_rules_grade1.c']
    return {
        'scope': 'Raw, unchanged UEB2 contraction spellings in the project Computer8 mapping',
        'source_sha256': {name: hashlib.sha256((root / name).read_bytes()).hexdigest()
                          for name in sources},
        'comparison_policy': 'Equal definitions are compatible. A shorthand expansion differing from literal typing is expected and is not by itself a semantic conflict. Symbol and indicator differences require contextual review.',
        'shared_lowercase_letters': [{'letter': letter, 'cell': cell, 'dots': dots(cell)}
                                     for cell, letter in letters],
        'implementation_interpretation': {
            'user_clarification': 'lets only keep Alphabetic wordsigns and shortforms in 8-dot shorthand',
            'category_policy': 'Enable only the 23 alphabetic wordsigns and 75 shortforms. Exclude 33 initial-letter contractions, 12 final-letter groupsigns and 39 single-cell symbol entries. Retain Computer8 indicator meanings. Original UEB2 and Computer8 modes stay unchanged.',
            'remaining_ambiguities': '26 enabled shortforms use Computer8 symbol cells. They intentionally expand in eligible contexts; ordinary Computer8 provides literal entry.',
            'currently_enabled_multi_cell_expansions': 75,
            'currently_enabled_single_cell_wordsigns': 23,
            'currently_excluded_single_cell_contractions': 39,
            'currently_excluded_multi_cell_contractions': 45,
            'explicit_prefix_required': False,
        },
        'counts': {
            'contractions': len(rows),
            'enabled_contractions': sum(r['decision'].startswith('enabled_') for r in rows),
            'excluded_contractions': sum(not r['decision'].startswith('enabled_') for r in rows),
            'enabled_symbol_sequence_ambiguities': sum(r['semantic_conflict'] == 'enabled_symbol_sequence_ambiguity' for r in rows),
            'excluded_category_contractions': sum(r['decision'] == 'excluded_category_initial_or_final' for r in rows),
            'excluded_symbol_contractions': sum(r['decision'] == 'retain_computer8_symbol' for r in rows),
            'distinct_excluded_symbol_cells': len({r['cells'][0] for r in rows if r['decision'] == 'retain_computer8_symbol'}),
            'single_cell_contractions': sum(len(r['cells']) == 1 for r in rows),
            'multi_cell_contractions': sum(len(r['cells']) > 1 for r in rows),
            'identical_raw_outputs': sum(not r['raw_output_difference'] for r in rows),
            'shared_lowercase_letter_definitions': len(letters),
            'single_cell_shared_letter_wordsigns': sum(len(r['cells']) == 1 and r['input_kind'] == 'shared_letters' for r in rows),
            'multi_cell_shared_letter_shortforms': sum(len(r['cells']) > 1 and r['input_kind'] == 'shared_letters' for r in rows),
            'single_cell_symbol_uses': sum(len(r['cells']) == 1 and r['input_kind'] == 'includes_computer8_symbols' for r in rows),
            'multi_cell_symbol_uses': sum(len(r['cells']) > 1 and r['input_kind'] == 'includes_computer8_symbols' for r in rows),
            'mapped_nonzero_six_dot_cells': sum(isinstance(table[i], int) for i in range(1, 64)),
        },
        'context_note': 'UEB2 letters can act as wordsigns in context. For example b and ab expanding to but and about is intentional shorthand, not evidence of incompatible base letter definitions.',
        'timing_note': 'Computer8 emits mapped cells immediately. Buffering a literal prefix to wait for shorthand is also an observable change.',
        'indicator_meaning_differences': [
            {'ueb': 'capital indicator', 'dots': '6', 'computer8': ','},
            {'ueb': 'numeric indicator', 'dots': '3456', 'computer8': '#'},
            {'ueb': 'grade-1 indicator', 'dots': '56', 'computer8': ';'},
        ],
        'commands_note': 'Dot7 Delete, dot8 Return, and space-bearing factory commands are existing definitions and must remain intact.',
        'mappings': rows,
    }


if __name__ == '__main__':
    print(json.dumps(audit(), indent=2, ensure_ascii=False))
