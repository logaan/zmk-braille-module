"""Check exhaustive collision evidence against the actual recorded tables."""
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
        self.assertEqual(saved['counts']['unchanged_spelling_compatible_subset'], 0)
        self.assertEqual(sum(saved['counts'][key] for key in
                             ('existing_character_conflicts', 'literal_sequence_conflicts')), 182)

    def test_source_cell_coverage_matches_audited_reference(self):
        source = (ROOT / 'src/braille_rules_computer8.c').read_text()
        cells = [int(mask, 16) for mask in re.findall(r'COMPUTER8_CHARACTER\((0x[0-9a-f]+),', source)]
        table = audit_module.patterns()
        self.assertEqual(len(cells), 94)
        self.assertEqual(set(cells), {i for i, value in enumerate(table) if isinstance(value, int)})
        self.assertTrue(all(cell in cells for cell in range(1, 64)))

    def test_single_and_multi_cell_conflicts_are_not_conflated(self):
        rows = {row['contraction']: row for row in audit_module.audit()['conflicts']}
        self.assertEqual((rows['but']['computer8_literal'], rows['but']['conflict']), ('b', 'existing_character'))
        self.assertEqual((rows['about']['computer8_literal'], rows['about']['conflict']), ('ab', 'literal_sequence'))
        self.assertEqual(rows['and']['computer8_literal'], '&')
        self.assertEqual(rows['but']['decision'], 'exclude_from_proposed_automatic_mode')
        self.assertEqual(rows['about']['decision'], 'proposed_automatic_sequence_expansion_pending_review')
        for row in rows.values():
            self.assertNotEqual(row['computer8_literal'], row['contraction'])


if __name__ == '__main__':
    unittest.main()
