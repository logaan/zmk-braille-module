"""Check output comparisons and shared meanings against recorded tables."""
import importlib.util
import json
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('shorthand_audit', ROOT / 'scripts/audit-computer8-shorthand.py')
audit_module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(audit_module)


class ShorthandCompatibility(unittest.TestCase):
    def test_audit_is_exhaustive_and_reproducible(self):
        saved = json.loads((ROOT / 'references/research/computer8-shorthand-conflicts.json').read_text())
        self.assertEqual(saved, audit_module.audit())
        self.assertEqual(saved['counts']['contractions'], 182)
        self.assertEqual(saved['counts']['mapped_nonzero_six_dot_cells'], 63)
        self.assertEqual(saved['counts']['identical_raw_outputs'], 0)
        self.assertEqual(sum(saved['counts'][key] for key in
                             ('single_cell_contractions', 'multi_cell_contractions')), 182)

    def test_shared_letters_and_neutral_partitions(self):
        result = audit_module.audit()
        self.assertEqual(''.join(row['letter'] for row in result['shared_lowercase_letters']), 'abcdefghijklmnopqrstuvwxyz')
        self.assertEqual(result['counts']['shared_lowercase_letter_definitions'], 26)
        for key, expected in [('single_cell_shared_letter_wordsigns', 23),
                              ('multi_cell_shared_letter_shortforms', 49),
                              ('single_cell_symbol_uses', 39),
                              ('multi_cell_symbol_uses', 71)]:
            self.assertEqual(result['counts'][key], expected)

    def test_source_cell_coverage_matches_audited_reference(self):
        source = (ROOT / 'src/braille_rules_computer8.c').read_text()
        cells = [int(mask, 16) for mask in re.findall(r'COMPUTER8_CHARACTER\((0x[0-9a-f]+),', source)]
        table = audit_module.patterns()
        self.assertEqual(len(cells), 94)
        self.assertEqual(set(cells), {i for i, value in enumerate(table) if isinstance(value, int)})
        self.assertTrue(all(cell in cells for cell in range(1, 64)))

    def test_shorthand_expansion_is_not_automatically_a_conflict(self):
        rows = {row['contraction']: row for row in audit_module.audit()['mappings']}
        self.assertEqual((rows['but']['computer8_literal'], rows['but']['input_kind']), ('b', 'shared_letters'))
        self.assertEqual((rows['about']['computer8_literal'], rows['about']['input_kind']), ('ab', 'shared_letters'))
        self.assertEqual(rows['and']['computer8_literal'], '&')
        self.assertEqual(rows['but']['decision'], 'currently_excluded_single_cell')
        self.assertEqual(rows['about']['decision'], 'currently_enabled_contextual_expansion')
        for row in rows.values():
            self.assertEqual(row['raw_output_difference'], row['computer8_literal'] != row['contraction'])
            self.assertEqual(row['semantic_conflict'], 'not_inferred_from_expansion')


if __name__ == '__main__':
    unittest.main()
