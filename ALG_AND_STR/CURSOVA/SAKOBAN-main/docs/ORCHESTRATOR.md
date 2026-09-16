# SOKOBAN Orchestrator Playbook

Instructions for the chief orchestrator and any future agent orchestrator working in this repository.

This file explains how to coordinate Orca work correctly: Runs, tasks, supervised workers,
messages, worktrees, terminals, subagents, recovery, and project-specific boundaries.

Authoritative project rules remain in:

- `AGENTS.md` — agent hierarchy, workstream ownership, bypass-launch rules, Git boundaries.
- `PLAN.md` — architecture and development stages.
- `ALGORITHMS.md` — game rules, solver contracts, and timing methodology.
- `Індивідуальне завдання на КР - Сокобан.pdf` — primary source of requirements.

If this playbook ever conflicts with the PDF, treat the PDF as primary and report the
conflict before changing architecture. If it conflicts with `AGENTS.md` on responsibilities
or ownership, follow `AGENTS.md` and fix this playbook.

Source for Orca mechanics: `https://www.onorca.dev/docs/cli/orchestration`, plus the
version-matched local guides:

```bash
orca skills get orchestration --full
orca skills get orca-cli
```

Command flags change between Orca releases. Do not guess flags from memory. Prefer `--json`
for agent-driven calls.

## 1. Roles

- The agent in root worktree `main` is the chief orchestrator.
- Agents in child Orca worktrees are subagents.
- During orchestrated development, the chief orchestrator coordinates but does not
  personally implement program code, tests, game resources, or interfaces.
- The chief orchestrator owns:
  - work decomposition;
  - task specifications;
  - dependency ordering;
  - conflict avoidance;
  - result verification;
  - integration of finished branches.
- A subagent performs only its explicitly assigned task in its own worktree/branch.
- A subagent does not delegate its task to another agent without an explicit instruction
  from the orchestrator.
- A dispatched worker normally cannot dispatch sub-workers. Default nested-worker depth is
  `1`: the coordinator dispatches workers, and those workers do the work themselves.

## 2. Preconditions

Before orchestrating:

```bash
orca status --json
```

Requirements:

- Orca runtime is running.
- Orchestration is enabled under Settings → Experimental.
- The same Orca executable is used for the whole session.
- If `ORCA_CLI_COMMAND` is set, use it.
- On Linux outside an Orca-managed terminal, use `orca-ide`, not bare `orca`.
- In developer checkouts exposing `ORCA_DEV_REPO_ROOT`, use `orca-dev` where applicable.

If the runtime is not reachable, start it first and recheck status.

## 3. Supervised orchestration versus full handoff

Use supervised Orca orchestration when coordination state matters:

- task ownership;
- `worker_done` tracking;
- blocking questions and answers;
- escalations;
- task DAGs;
- decision gates;
- coordinator waiting for results.

Use a full ownership handoff instead when the user says:

- “hand off”;
- “handoff”;
- “handover”;
- “give this to another agent”;
- “give this to another worktree”;
- “another agent”;
- “another worktree”.

A full handoff transfers ownership and stops monitoring. Do not create an orchestration
task, inject a lifecycle preamble, wait for `worker_done`, or inspect the new worker
after delivering the prompt unless the user explicitly asked for supervision.

Use `terminal send` for a lightweight prompt to an agent already being watched.

## 4. Preferred supervised loop

Create the Run and independent tasks first. Start independent workers before waiting.

```bash
orca orchestration run-create --objective "<objective>" --json
orca orchestration task-create --spec "<worker A task>" --json
orca orchestration task-create --spec "<worker B task>" --json
orca orchestration worker-start --task <task_a> --worktree current --agent codex --json
orca orchestration worker-start --task <task_b> --worktree current --agent claude --json
```

Wait for completion:

```bash
orca orchestration check --wait --types worker_done,escalation,question --timeout-ms 900000 --json
```

Process every message in the returned Delivery, handle terminal ownership, then acknowledge:

```bash
orca orchestration check --ack <deliveryId> --wait --types worker_done,escalation,question --timeout-ms 900000 --json
```

Default `check` returns the bound Run’s oldest unacknowledged Delivery. Replay with `--ack`
until all expected dispatches settle. `--peek` and `--all` do not consume mail.

For an existing Run:

```bash
orca orchestration run-list --json
orca orchestration run-use --id <run_id> --json
orca orchestration task-list --json
```

## 5. Writing tasks

Make every task independently executable:

- exact worktree/branch;
- required prerequisite documents;
- files or directories the worker may change;
- files or directories the worker must not touch;
- tests or checks to run;
- required final report fields;
- definition of done.

Example task-creation commands:

```bash
orca orchestration task-create --spec "<complete worker task>" --json
orca orchestration task-create --spec "<dependent task>" --deps '["<parent_task_id>"]' --json
orca orchestration task-list --ready --json
orca orchestration task-list --brief --json
orca orchestration task-update --id <task_id> --status blocked --result '{"reason":"waiting on input"}' --json
```

Task statuses:

- `pending`
- `ready`
- `dispatched`
- `completed`
- `failed`
- `blocked`

Rules:

- Create independent tasks before starting workers.
- Start parallel workers in waves.
- Keep dependency chains shallow; avoid more than 3–4 levels.
- Use `task-list --ready` as coordinator memory.
- Do not mark a task complete based only on prose; require the injected `worker_done`
  lifecycle message with matching task and dispatch IDs.
- After three consecutive failures, a task circuit-breaks to failed. Inspect state before
  replacing it.

## 6. Starting supervised workers

Preferred composition:

```bash
orca orchestration worker-start --task <task_id> --worktree current --agent codex --json
```

New child worktree:

```bash
orca orchestration worker-start --task <task_id> --worktree new-child --name <name> --agent codex --setup run --json
```

Independent top-level worktree:

```bash
orca orchestration worker-start --task <task_id> --worktree new-top-level --name <name> --agent codex --setup run --json
```

Reuse an existing agent terminal only for an immediate follow-up:

```bash
orca orchestration worker-start --task <next_task_id> --terminal <handle> --json
```

Optional model/effort overrides apply only to fresh Claude, Codex, or Cursor terminals:

```bash
orca orchestration worker-start --task <task_id> --worktree current --agent claude --model <opaque-model-id> --effort high --json
```

Rules:

- `--effort` requires `--model`.
- Model/effort overrides cannot be combined with `--terminal`.
- Overrides apply only to that launch.
- Read the start receipt before continuing.
- `ready` is normal; inspect `stage`, `effects`, and residual resources on failure.
- Do not retry a failed start unchanged.
- Retry placement explicitly; `--retry-of` does not inherit worktree placement:

```bash
orca orchestration worker-start --task <task_id> --retry-of <dispatch_id> --worktree current --agent codex --json
```

Remote worker on another connected Orca server:

```bash
orca orchestration worker-start --task <task_id> --on <saved-environment> --worktree new-top-level --repo <exact_remote_repo_selector> --name <name> --agent codex --setup run --json
```

Later commands route by dispatch ID; do not repeat `--on`. Remote `current` and
`new-child` are invalid because they are ambiguous across servers.

## 7. Worktrees

Choose worker placement before creating a terminal. A “fresh worker” means a fresh agent
session, not necessarily a new Git worktree.

- Use the current worktree when work depends on uncommitted files, local artifacts, or the
  current branch.
- Use a child worktree for isolated work stacked on or dependent on the active worktree.
- Use a top-level worktree for independent features, unrelated fixes, or unrelated
  follow-up work.
- Do not create a new worktree merely for convenience or parallelism.
- For every allowed new worktree, pass `--setup run` unless there is a concrete reason to
  skip or inherit setup.
- Preserve repository startup policy; do not bypass `wait-for-setup`.
- Use agent-first creation when available so the worker does not get an unnecessary
  fallback shell plus an agent terminal.
- Resolve the exact full worktree ID in `<repo-id>::<path>` form when targeting a newly
  created worktree.

Project-specific Git boundaries:

- Each subagent works only in its own worktree and branch.
- Never edit files directly in another worktree.
- Never delete or overwrite someone else’s changes.
- Never use `git reset --hard`, forced push, or another destructive Git operation.
- Split changes into understandable commits with verified build state.
- If a task needs a file owned by another direction, the worker must ask the orchestrator
  first.

Project workstream ownership:

- `sokoban-cli`: C++20 core, solvers, CLI, benchmarks, tests, control XSB levels.
- `sokoban-web`: only after stable core integration; owns `web/` and Web bindings.
- `sokoban-godot-blender`: only after stable core integration; owns Godot integration,
  scenes/scripts/UI/assets and desktop packaging.
- Web and Godot must not duplicate Sokoban rules or solver algorithms.

## 8. Terminals

Useful terminal commands:

```bash
orca terminal list --json
orca terminal create --worktree active --title <task-name> --command "codex" --json
orca terminal wait --terminal <handle> --for tui-idle --timeout-ms 60000 --json
orca terminal read --terminal <handle> --json
orca terminal send --terminal <handle> --text "<task brief>" --enter --json
orca terminal split --terminal <handle> --direction horizontal --command "<cmd>" --json
```

Rules:

- Wait for `tui-idle` before injecting a dispatch into a newly created agent terminal.
- Always pass `--timeout-ms`.
- Read before sending unless the next input is obvious.
- Use `terminal send` only for direct input or untracked prompts.
- Do not use `terminal send` to simulate supervised lifecycle messages.
- For custom topology, create worktree/terminal manually, wait for readiness, then dispatch:

```bash
orca worktree create --name <task-name> --agent codex --setup run --json
orca terminal wait --terminal <handle> --for tui-idle --timeout-ms 60000 --json
orca orchestration dispatch --task <task_id> --to <handle> --inject --json
```

If the target is a bare shell, omit `--inject`, then send the prompt manually.

`dispatch --inject` into an operator-started terminal may remain unsupervised. Use
`worker-start --terminal <handle>` when full worker supervision and lifecycle ownership
are required.

## 9. Messages, questions, and gates

Worker completion must include task and dispatch IDs:

```bash
orca orchestration send --type worker_done --subject "<short status>" --body "<what changed, findings, and what remains>" --task-id <task_id> --dispatch-id <dispatch_id> --outcome succeeded --files-modified "path/a,path/b" --json
```

Failure reporting:

```bash
orca orchestration send --type worker_done --subject "<short status>" --body "<what failed and what remains>" --task-id <task_id> --dispatch-id <dispatch_id> --outcome failed --files-modified "path/a,path/b" --json
```

Rules:

- Workers send `worker_done` exactly once.
- `--outcome succeeded|failed` is mandatory.
- Never encode failure only in prose.
- Include both task and dispatch IDs so a stale retry cannot settle the wrong dispatch.
- Heartbeats and `worker_done` come from the worker’s own terminal.
- Never use group addresses for `worker_done` or heartbeat.

Heartbeat during long active work:

```bash
orca orchestration send --type heartbeat --subject "alive" --payload '{"taskId":"<task_id>","dispatchId":"<dispatch_id>","phase":"implementing"}' --json
```

Worker question:

```bash
orca orchestration ask --question "<question>" --options "yes,no" --timeout-ms 600000 --json
orca orchestration ask --resume <message_id> --timeout-ms 600000 --json
```

Coordinator answer:

```bash
orca orchestration reply --id <message_id> --body "<answer>" --json
```

Workers must use `ask`, not a local interactive prompt invisible to the coordinator.

Broadcast or targeted status mail:

```bash
orca orchestration send --to "@all" --subject "Heads up" --body "Pausing dispatches for review." --json
orca orchestration send --to "@idle" --subject "Anyone free?" --json
orca orchestration send --to "@codex" --subject "Codex agents only" --json
```

In PowerShell, quote group addresses such as `"@all"`.

Coordinator-owned decision gates:

```bash
orca orchestration gate-create --task <task_id> --question "<decision>" --options '["yes","no"]' --json
orca orchestration gate-resolve --id <gate_id> --resolution "yes" --json
orca orchestration gate-list --task <task_id> --json
```

Use `ask`/`reply` for worker-initiated questions. Use gates for coordinator-managed DAG
decisions that block a task until recorded.

## 10. Worker lifecycle and cleanup

Inspect workers:

```bash
orca orchestration worker-show --dispatch <dispatch_id> --json
orca orchestration worker-read --dispatch <dispatch_id> --limit 50 --json
orca orchestration dispatch-show --task <task_id> --json
```

After every accepted `worker_done`:

1. Decide the terminal’s next owner.
2. For an immediate follow-up with the same agent, start the next dispatch with
   `--terminal <worker-handle>`.
3. Otherwise release the terminal.
4. Only then acknowledge the Delivery or wait again.

Release:

```bash
orca orchestration worker-release --dispatch <dispatch_id> --json
```

Retain only when the user explicitly asks to keep a settled worker live:

```bash
orca orchestration worker-retain --dispatch <dispatch_id> --json
```

Rules:

- Release after both successful and failed completions unless retention was requested.
- Do not leave completed terminals open merely for rereading output; use `worker-read`.
- Do not release because of timeout, TUI idle state, heartbeat, question, escalation, or
  stale completion.
- If release returns `release_pending` or `release_unknown`, follow the receipt’s recovery
  action; do not substitute a broad terminal close.
- A replayed Delivery may repeat `worker-release` safely.

Cancellation and abandonment:

```bash
orca orchestration worker-stop --dispatch <dispatch_id> --json
orca orchestration worker-abandon --dispatch <dispatch_id> --json
```

- `worker-show ready`: keep waiting or read bounded output.
- Proven `failed` or `stopped`: create an explicit replacement with `--retry-of`.
- `outcome_unknown`: stop and inspect, or explicitly abandon while accepting that
  resources may remain live.
- `worker-stop` closes only the exact supervised agent terminal.
- `worker-abandon` performs no remote, process, or filesystem action.
- Never stop or restart a worker the user explicitly said to leave alone.

Lost-response recovery:

```bash
orca orchestration request-show --request <request_id> --json
```

- `completed`: the mutation already took effect.
- `pending`: the original operation may still be running.
- `absent`: this runtime has no receipt under the caller identity; inspect affected state
  before retrying.
- Replay with `--retry-request <request_id>` when appropriate instead of creating a
  duplicate operation.

Preflight errors have stable codes:

- `task_not_found`
- `task_not_startable`
- `inject_rejected`
- `runtime_error`
- `nested_worker_depth_exceeded`

Read `error.code` and `error.data.nextSteps` before recovery.

Never run orchestration reset during active coordination unless intentionally abandoning
that state:

```bash
orca orchestration reset --tasks --json
orca orchestration reset --messages --json
orca orchestration reset --all --json
```

Reset affects runtime-global orchestration state and can disturb another coordinator.

## 11. Subagent launch and bypass mode

Project bypass rules apply when the user has directly ordered bypass operation:

- Antigravity CLI: `--dangerously-skip-permissions`.
- Codex CLI: `--dangerously-bypass-approvals-and-sandbox`.
- OpenCode or another agent CLI: check its current `--help` first and use the exact
  official full-bypass flag.
- Never invent a bypass flag name.
- Verify that the effective new agent-terminal command actually contains the required flag
  before assigning work.
- Bypass mode does not expand task scope.
- The subagent still works only in its own worktree.
- The subagent still must not change other directories or perform destructive operations.

Before implementation, every SOKOBAN subagent must read:

- `PLAN.md`;
- `ALGORITHMS.md`;
- root `AGENTS.md`;
- the assigned individual task from the PDF.

## 12. Required worker report

Every SOKOBAN worker’s final `worker_done` report must state:

- what was implemented;
- which files changed;
- which tests were run, with exact commands and results;
- remaining limitations;
- final commit hash.

Blockers must be reported openly. Architecture must not be changed to hide a blocker.

After `worker_done`, the worker does not start another task independently.

## 13. SOKOBAN acceptance checklist

Before integration to `main`:

- Rules and solver behavior are correct.
- Shared logic remains independent of CLI, Web, and Godot APIs.
- Human, AI, replay, and tests use the same public `GameSession`/`GameRules` API.
- BFS optimality claims use the step-by-step move graph.
- A* states whether it optimizes `Moves` or `Pushes` and uses an admissible heuristic.
- Safe-mode deadlock detection does not discard potentially solvable states.
- Pure search time is separated from loading, preprocessing, reconstruction, validation,
  UI, and animation.
- Every discovered solution is rechecked through game rules before replay.
- Tests are runnable and their commands/results are included in the worker report.
- Completed work is integrated without overwriting unrelated changes.

Priority order:

1. Correctness of rules and solutions.
2. No data loss or worktree conflicts.
3. Verifiable, reproducible tests.
4. Correct timing and statistics.
5. Performance.
6. Interface convenience and visual quality.
