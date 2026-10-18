#!/usr/bin/env python3
"""Coordinator tests: no model calls, live workers, or firmware builds."""
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


def snapshot(done=12):
    return [{"task": f"software-{i}", "status": "delivered" if i < done else "draft",
             "status_message": f"delivery-{i}.md", "latest_message": f"delivery-{i}.md"}
            for i in range(12)] + [{"task": str(coordinator.HARDWARE), "status": "draft"}]


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
            rows = coordinator.task_snapshot(root)
            self.assertEqual(len(rows), 1)
            self.assertEqual(rows[0]["status"], "delivered")
            self.assertTrue(rows[0]["status_message"].endswith("02.md"))
            self.assertTrue(rows[0]["latest_message"].endswith("03.md"))

    def test_completion_requires_twelve_software_leaves_not_hardware_pass(self):
        self.assertTrue(coordinator.software_complete(snapshot()))
        self.assertFalse(coordinator.software_complete(snapshot(11)))
        for rows in [[], snapshot()[1:]]:
            with self.assertRaises(ValueError):
                coordinator.software_complete(rows)

    @patch.object(coordinator, "workers", return_value=[{"pane": "%2", "task": "software-0"}])
    @patch.object(coordinator, "task_snapshot", side_effect=lambda: snapshot())
    def test_luna_wait_does_not_wake_sol(self, rows, workers):
        with patch.object(coordinator, "run_pi", return_value='{"action":"wait"}') as run:
            self.assertEqual(coordinator.check_once(self.args, Path("unused")), "wait")
            self.assertEqual(run.call_count, 1)
            self.assertEqual(run.call_args.args[:2], ("gpt-6-luna", "low"))

    @patch.object(coordinator, "workers", return_value=[])
    @patch.object(coordinator, "task_snapshot", side_effect=lambda: snapshot())
    def test_sol_verifies_or_returns_control(self, rows, workers):
        for marker, expected in [("ALPHABET_VERIFIED", "done"),
                                 ("COORDINATOR_CONTINUE", "wait"),
                                 ("COORDINATOR_BLOCKED", "blocked")]:
            with self.subTest(marker=marker), patch.object(coordinator, "run_pi", side_effect=[
                '{"action":"verify"}', marker]) as run:
                self.assertEqual(coordinator.check_once(self.args, Path("unused")), expected)
                self.assertEqual(run.call_args.args[:2], ("gpt-6.1-sol", "high"))

    @patch.object(coordinator, "task_snapshot", side_effect=lambda: snapshot())
    def test_active_worker_prevents_even_luna_requested_sol_handoff(self, rows):
        with patch.object(coordinator, "workers", return_value=[{"pane": "%2", "task": "software-0"}]), \
             patch.object(coordinator, "run_pi", return_value='{"action":"verify"}') as run:
            self.assertEqual(coordinator.check_once(self.args, Path("unused")), "wait")
            self.assertEqual(run.call_count, 1)

    @patch.object(coordinator, "workers", return_value=[])
    def test_incomplete_software_prevents_sol_handoff(self, workers):
        with patch.object(coordinator, "task_snapshot", side_effect=lambda: snapshot(11)), \
             patch.object(coordinator, "run_pi", return_value='{"action":"verify"}') as run:
            self.assertEqual(coordinator.check_once(self.args, Path("unused")), "wait")
            self.assertEqual(run.call_count, 1)

    def test_active_worker_prevents_assignment_even_if_luna_requests_start(self):
        with patch.object(coordinator, "workers", return_value=[{"pane": "%2", "task": "software-0"}]), \
             patch.object(coordinator, "td") as td:
            self.assertIn("ignored", coordinator.start_leaf(self.args, snapshot(0)))
            td.assert_not_called()

    @patch.object(coordinator, "workers", return_value=[])
    @patch.object(coordinator, "committed", return_value=True)
    def test_scheduler_selects_only_first_unfinished_leaf_and_embeds_its_path(self, committed, workers):
        with patch.object(coordinator, "task_snapshot", return_value=snapshot(5)), \
             patch.object(coordinator, "td", side_effect=["ready.md", "assigned.md"]) as td, \
             patch.object(coordinator, "commit_messages"), \
             patch.object(coordinator.subprocess, "check_output", return_value="%9\n") as launch, \
             patch.object(coordinator.subprocess, "run"):
            coordinator.start_leaf(self.args, snapshot(12))  # Stale input must not matter.
            self.assertTrue(all(call.args[1] == "software-5" for call in td.call_args_list))
            command = launch.call_args.args[0][-1]
            self.assertIn("--print", command)
            self.assertIn("software-5", command)
            self.assertNotIn("software-6", command)

    @patch.object(coordinator, "workers", return_value=[])
    @patch.object(coordinator, "committed", return_value=False)
    @patch.object(coordinator, "task_snapshot", side_effect=lambda: snapshot(5))
    def test_uncommitted_predecessor_delivery_prevents_assignment(self, rows, committed, workers):
        with patch.object(coordinator, "td") as td:
            self.assertIn("commits", coordinator.start_leaf(self.args, snapshot(5)))
            td.assert_not_called()

    def test_multiple_workers_fail_before_any_model_call(self):
        with patch.object(coordinator, "task_snapshot", return_value=snapshot()), \
             patch.object(coordinator, "workers", return_value=[{}, {}]), \
             patch.object(coordinator, "run_pi") as run:
            with self.assertRaises(RuntimeError):
                coordinator.check_once(self.args, Path("unused"))
            run.assert_not_called()

    def test_luna_has_no_tools_or_accumulating_session(self):
        with tempfile.TemporaryDirectory() as directory, patch.object(coordinator.subprocess, "run") as run:
            run.return_value = subprocess.CompletedProcess([], 0, '{"action":"wait"}', "")
            coordinator.run_pi("gpt-6-luna", "low", "prompt", Path(directory), "luna")
            command = run.call_args.args[0]
            for flag in ["--no-session", "--no-tools", "--no-context-files", "--print"]:
                self.assertIn(flag, command)
            run.return_value = subprocess.CompletedProcess([], 1, "", "API failed")
            with self.assertRaises(RuntimeError):
                coordinator.run_pi("gpt-6-luna", "low", "prompt", Path(directory), "luna")


if __name__ == "__main__":
    unittest.main(verbosity=2)
