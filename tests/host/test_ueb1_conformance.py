#!/usr/bin/env python3
# SPDX-License-Identifier: MIT
"""Rulebook-derived cross-rule conformance, beyond individual mapping tests.

Uses the real C capture, registry, matcher and dispatcher, not a Python
translator. Recovered research fixtures retain their printed section/page;
constructed examples remain explicitly labelled. The PDF is authoritative.
"""
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

import yaml
from test_computer8 import HOST_KEYS, cell_mask
from test_ueb1 import build, observe, ZMK

ROOT = Path(__file__).resolve().parents[2]
RESEARCH = ROOT / "references/research"
RESEARCH_NAMES = ("ueb-literary-signs", "ueb-numeric-mode", "ueb-capitalisation", "ueb-grade-1-indicators")


class ICEBGrade1Conformance(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.tmp.cleanup)
        cls.binary = build(cls.tmp.name, real_keycodes=(ZMK / "include/dt-bindings/zmk/keys.h").exists())

    def test_all_existing_ueb1_fixtures(self):
        count = 0
        for path in sorted((ROOT / "tests/braille/ueb1").glob("*.yaml")):
            data = yaml.safe_load(path.read_text())
            self.assertEqual(data["mode"], "ueb1")
            for case in data["cases"]:
                with self.subTest(file=path.name, fixture=case["name"]):
                    self.assertTrue(case["source"])
                    actual = observe(self.binary, [cell_mask(c) for c in case["inputs"]])
                    self.assertEqual(actual["keys"], [HOST_KEYS[c] for c in case["outputs"]])
                    # A bare double capitals indicator is intentionally still
                    # ambiguous with the triple; no timer or end-of-test flush.
                    pending = case.get("pending", [[6], [6]] if case["name"] ==
                                       "capitalised word indicator alone has no output" else [])
                    self.assertEqual(actual["pending"], [cell_mask(c) for c in pending])
                    if "context" in case:
                        numeric, caps, start = case["context"]
                        self.assertEqual(actual["context"], [int(numeric), ["none", "word", "passage"].index(caps), int(start)])
                    if "dropped" in case:
                        self.assertEqual(actual["drops"], [[cell_mask(c) for c in seq] for seq in case["dropped"]])
                    if path.stem == "pending":
                        self.assertEqual(actual["drops"], [[cell_mask(case["inputs"][0])]])
                    count += 1
        self.assertEqual(count, 456)

    def test_recovered_rulebook_and_constructed_examples(self):
        count = 0
        for name in RESEARCH_NAMES:
            data = yaml.safe_load((RESEARCH / f"{name}.yaml").read_text())
            for group in ("from_rulebook", "constructed"):
                for case in data["fixtures"][group]:
                    with self.subTest(research=name, kind=group, example=case["print"]):
                        if group == "from_rulebook":
                            self.assertTrue(case["ref"]["section"])
                            self.assertGreater(case["ref"]["page"], 0)
                        else:
                            self.assertTrue(case["note"])  # labelled constructed, never an ICEB quotation
                        actual = observe(self.binary, [cell_mask(c) for c in case["cells"]])
                        self.assertEqual(actual["keys"], [HOST_KEYS[c] for c in case["print"]])
                        self.assertEqual(actual["pending"], [])
                        self.assertEqual(len(actual["drops"]), int("notice" in case))
                        count += 1
        self.assertEqual(count, 222)

    @unittest.skipUnless(shutil.which("pdftotext") and
                         (ROOT / "references/iceb/rules-of-unified-english-braille-2024.pdf").exists(),
                         "Poppler or the local ICEB rulebook PDF is unavailable")
    def test_normative_patterns_on_cited_pdf_pages(self):
        pages = subprocess.check_output([
            "pdftotext", "-layout", str(ROOT / "references/iceb/rules-of-unified-english-braille-2024.pdf"), "-",
        ], text=True).split("\f")
        literary = yaml.safe_load((RESEARCH / "ueb-literary-signs.yaml").read_text())["normative"]
        numeric = yaml.safe_load((RESEARCH / "ueb-numeric-mode.yaml").read_text())["normative"]
        caps = yaml.safe_load((RESEARCH / "ueb-capitalisation.yaml").read_text())["normative"]
        grade1 = yaml.safe_load((RESEARCH / "ueb-grade-1-indicators.yaml").read_text())["normative"]
        signs = [(s, literary["alphabet"]["ref"]) for s in literary["alphabet"]["signs"]]
        signs += [(s, {"section": "6.1", "page": 65}) for s in numeric["numeric_indicators"]["signs"]]
        signs += [(s, {"section": "4.1", "page": 45}) for s in caps["capital_letters"]["signs"]]
        signs += [(s, s["ref"]) for s in grade1["signs"]]
        additional = literary["additional_signs"]
        signs += [(s, s["ref"]) for s in additional["quotation_marks"]["signs"]]
        for name in ("vertical_line", "grave_accent_alone"):
            signs += [(s, additional[name]["refs"][0]) for s in additional[name]["sign"]]
        for sign, ref in signs:
            with self.subTest(sign=sign["braille"], section=ref["section"]):
                masks = [cell_mask(c) for c in sign["dots"]]
                self.assertEqual(sign["braille"], "".join(chr(0x2800 + m) for m in masks))
                page = re.sub(r"\s+", "", pages[ref["page"] + 27])
                self.assertIn(sign["braille"], page)


if __name__ == "__main__":
    unittest.main(verbosity=2)
