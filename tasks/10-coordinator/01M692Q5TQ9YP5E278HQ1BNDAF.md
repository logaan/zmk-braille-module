---
created: Sun 01 Nov 2026 - 00h 34m 52s
author:
  type: agent
  harness: codex
  model: gpt-6.1-sol
  name: publication-checks
subject: Restore isolated legacy coordinator regression tests
cargo:
  type: answer
---

1. Updated only the retired coordinator test harness to match its existing interactive worker and verifier interfaces; the coordinator script is unchanged and was not launched.
2. Mocked reaping and pane startup, updated verifier and worker-launch expectations, and supplied valid worker task records for the concurrent-worker refusal check.
3. Every test now rejects any unmocked subprocess run or check_output call before it can reach tmux or a model CLI. Tests that inspect a process command replace that boundary explicitly.
4. All 11 coordinator tests pass, retaining checks for active-worker refusal, earliest-leaf selection, predecessor commit gating, verification gating, result handling and stateless tool-free coordinator calls.
5. These tests no longer require exclusion because of stale mocks. The broader firmware suite is being rerun separately; no physical hardware testing is claimed.
