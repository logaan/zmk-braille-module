#!/usr/bin/env python3
"""Coordinator control-flow tests; no model calls, workers, or firmware builds."""
import argparse
import importlib.machinery
import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
loader = importlib.machinery.SourceFileLoader("coordinator", str(ROOT / "scripts/coordinate-alphabet"))
spec = importlib.util.spec_from_loader(loader.name, loader)
coordinator = importlib.util.module_from_spec(spec)
loader.exec_module(coordinator)


def complete():
    return [{"task": f"software-{i}", "status": "delivered"} for i in range(12)] + [
        {"task": str(coordinator.HARDWARE), "status": "draft"}]


class CoordinatorTests(unittest.TestCase):
    def setUp(self):
        self.args = argparse.Namespace(pane="%1", luna_model="gpt-6-luna", sol_model="gpt-6.1-sol")

    def test_snapshot_folds_status_across_questions_and_skips_containers(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            phase = root / coordinator.PHASE
            leaf = phase / "01-group/01-leaf"
            leaf.mkdir(parents=True)
            (phase / "01.md").write_text("---\ncargo:\n  status: draft\n---\n")
            (leaf / "01.md").write_text("---\ncargo:\n  type: task\n  status: started\n---\n")
            (leaf / "02.md").write_text("---\ncargo:\n  type: task\n  status: delivered\n---\n")
            (leaf / "03.md").write_text("---\ncargo:\n  type: question\n---\n1. Question?\n")
            snapshot = coordinator.task_snapshot(root)
            self.assertEqual(len(snapshot), 1)
            self.assertEqual(snapshot[0]["status"], "delivered")
            self.assertTrue(snapshot[0]["latest_message"].endswith("03.md"))

    def test_completion_requires_twelve_software_leaves_not_hardware_pass(self):
        self.assertTrue(coordinator.software_complete(complete()))
        self.assertFalse(coordinator.software_complete(complete()[1:]))
        snapshot = complete()
        snapshot[0]["status"] = "blocked"
        self.assertFalse(coordinator.software_complete(snapshot))
        self.assertFalse(coordinator.software_complete([]))

    @patch.object(coordinator.subprocess, "check_output", return_value="%2 command=node task=leaf\n")
    @patch.object(coordinator, "task_snapshot", side_effect=lambda: complete())
    def test_luna_wait_does_not_wake_sol(self, snapshot, panes):
        with patch.object(coordinator, "run_pi", return_value=("COORDINATOR_WAIT", "waiting")) as run:
            self.assertEqual(coordinator.check_once(self.args, Path("unused")), "wait")
            self.assertEqual(run.call_count, 1)
            self.assertEqual(run.call_args.args[:2], ("gpt-6-luna", "low"))

    @patch.object(coordinator.subprocess, "check_output", return_value="")
    @patch.object(coordinator, "task_snapshot", side_effect=lambda: complete())
    def test_sol_verifies_or_returns_control(self, snapshot, panes):
        for marker, expected in [("ALPHABET_VERIFIED", "done"),
                                 ("COORDINATOR_CONTINUE", "wait"),
                                 ("COORDINATOR_BLOCKED", "blocked")]:
            with self.subTest(marker=marker), patch.object(coordinator, "run_pi", side_effect=[
                ("COORDINATOR_VERIFY", "ready"), (marker, "checked")]) as run:
                self.assertEqual(coordinator.check_once(self.args, Path("unused")), expected)
                self.assertEqual(run.call_args.args[:2], ("gpt-6.1-sol", "high"))

    @patch.object(coordinator.subprocess, "check_output", return_value="")
    @patch.object(coordinator, "task_snapshot", return_value=[])
    def test_premature_handoff_and_unknown_markers_fail_closed(self, snapshot, panes):
        for marker in ["COORDINATOR_VERIFY", "looks done"]:
            with self.subTest(marker=marker), patch.object(coordinator, "run_pi", return_value=(marker, marker)) as run:
                with self.assertRaises(RuntimeError):
                    coordinator.check_once(self.args, Path("unused"))
                self.assertEqual(run.call_count, 1)

    def test_print_calls_are_stateless_explicit_models_and_fail_on_api_errors(self):
        with tempfile.TemporaryDirectory() as directory, patch.object(coordinator.subprocess, "run") as run:
            run.return_value = subprocess.CompletedProcess([], 0, "short report\nCOORDINATOR_WAIT\n", "")
            marker, _ = coordinator.run_pi("gpt-6-luna", "low", "prompt", Path(directory), "luna")
            self.assertEqual(marker, "COORDINATOR_WAIT")
            command = run.call_args.args[0]
            self.assertIn("--no-session", command)
            self.assertIn("--print", command)
            self.assertIn("gpt-6-luna", command)
            run.return_value = subprocess.CompletedProcess([], 1, "", "API failed")
            with self.assertRaises(RuntimeError):
                coordinator.run_pi("gpt-6-luna", "low", "prompt", Path(directory), "luna")


if __name__ == "__main__":
    unittest.main(verbosity=2)
