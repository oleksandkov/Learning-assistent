# OpenCode on this device

Inventory date: 2026-09-22. Sources: local config files under
`C:/Users/muaro/.config/opencode/`, this repository’s `.opencode/` and
`.agents/`, and the skill catalog exposed to this session. This documents
configuration, not private chats or credentials. It is an inventory, not a
portable restore package.

## Settings and installation

- CLI launcher: `C:/Users/muaro/AppData/Roaming/npm/opencode.ps1`.
- Global config directory: `C:/Users/muaro/.config/opencode/`.
- Two top-level configs exist side by side (both last written 2026-09-19):
  - `opencode.json` (~20 KB): `$schema`, provider `freellmapi` (large model
    list), `plugin: ["@dietrichgebert/ponytail"]`. No `mcp`, `model`,
    `instructions`, or `command` keys.
  - `opencode.jsonc` (~2 KB): `$schema`, default
    `model: opencode/muse-spark-1.3-contributor-free`, SecondBrain instruction
    globs, `plugin: ["@dietrichgebert/ponytail"]`, `mcp: {}`, permissions,
    `command.secondbrain`, experimental quota/mcp timeouts.
  - The two files diverge: providers live only in `.json`; model, instructions,
    MCP, permissions, and the slash command live only in `.jsonc`. Whichever
    file the running CLI prefers, the other half is not active. Treat this as
    a merge candidate, not two intentional layers.
- Project has no `opencode.json` / `opencode.jsonc` at the repo root.
- Project local dir: `C:/Github/Learning-assistent/.opencode/`
  (`package.json` for `opencode-pet-plugin` 0.2.0, empty `plugin/`,
  `tui.json` with `"plugin": []`, `node_modules/`, `.gitignore`).
- Global `package.json`: depends on `@opencode-ai/plugin` 1.15.13.
- Global `tui.json`: `"plugin": ["@slkiser/opencode-quota"]`.
- Global `dcp.jsonc`: DCP schema only; no options set.
- Global `AGENTS.md`: empty.
- Config backups on disk: `opencode.json.bak.add-mcp-orphan`,
  `opencode.jsonc.bak`, `opencode.jsonc.bak.20260604_063519`,
  `opencode.jsonc.bak.20260605_213353`, `opencode.jsonc.bak.pre-openrouter`,
  `opencode.jsonc.bak.secondbrain_wipe`, `opencode.jsonc.bak2`, and
  `opencode.before_secondbrain_mcps.jsonc`.

See [MCP.txt](MCP.txt), [plugins.txt](plugins.txt), [skills.txt](skills.txt),
and [MY PULL.txt](MY%20PULL.txt) for cross-agent inventories.

## Provider and model

- Active provider block in `opencode.json`: `freellmapi` via
  `@ai-sdk/openai-compatible`, `baseURL` `http://localhost:3001/v1`,
  `apiKey` from `{env:FREELLMAPI_API_KEY}`.
- Models include routing slots (`fusion`, `auto`, `default`, Claude-named
  slots) and many third-party ids (GPT, Gemini, DeepSeek, Kimi, GLM, Grok,
  Mistral, Qwen, uncensored variants, etc.). Limits are declared in config;
  they were not connectivity-tested in this scan.
- `opencode.jsonc` separately sets default `model` to
  `opencode/muse-spark-1.3-contributor-free` (not defined under `freellmapi`
  in `opencode.json`). Confirm which file the CLI loads before trusting either
  default.

## Instructions, permissions, and commands

- SecondBrain integration (only in `opencode.jsonc`):
  - `instructions` include `C:\SecondBrain\**\*.md` and rules treating the
    vault as source of truth (router `agents.md`, orientation
    `About the SecondBrain.md`, style `Formatting Guidelines.md`).
  - `permission`: `compress: allow`, read/external allow for
    `C:\SecondBrain\**`.
  - `command.secondbrain` templates
    `C:\Users\muaro\.config\opencode\commands\secondbrain.md` (agent `build`).
- Slash command file: `commands/secondbrain.md` — vault context/draft/link/tidy
  subcommands; ask-before-write and link-once guards; no raw secrets.
- Experimental: `quotaToast` disabled, `mcp_timeout` 30000.

## Plugins

Configured in both top-level configs:

- `@dietrichgebert/ponytail` — lazy senior-dev mode; skills exposed in this
  session (ponytail, ponytail-audit, ponytail-debt, ponytail-gain,
  ponytail-help, ponytail-review).

Present on disk but not listed in the `plugin` array:

- `plugin/voice-interface.js` — voice/Jarvis bridge; last modified 2026-06-02.
  Header says it may live in `~/.config/opencode/plugin/`.
- `plugins/opencode-bar/` — TUI status bar for token metrics, v0.1.0,
  `oc-plugin: ["tui"]`, source checkout with its own `node_modules`.
- `opencode-bar.json` — bar visibility config (speed, ttft, input, output,
  cache, elapsed, model).
- `opencode-quota/quota-toast.json` — quota toast state; paired with
  `@slkiser/opencode-quota` in global `tui.json`.
- Historical note from 2026-09-18 scan: `~/.local/share/opencode/plugins/worktree`.

Plugin files on disk are not proof the CLI loads them; only the `plugin` /
`tui.json` entries are configuration.

## MCP

- `opencode.jsonc`: `mcp` map with 6 servers (mirrored from Codex 2026-09-22):
  `playwright`, `chrome-devtools`, `context7` (npx), `markitdown`, `docling`,
  `excel` (local binaries, same commands/env as Codex `config.toml`).
- `opencode.json`: no `mcp` key (providers only).
- `node_repl` skipped: Codex-runtime internal, not portable.
- `experimental.mcp_timeout` raised 30000 → 120000 for docling/excel cold starts.
- Verification 2026-09-22: `opencode mcp list` → all 6 `connected`;
  stdio re-probe markitdown 1 tool, excel 7 tools (match 2026-09-21 counts).

## Skills

- User skill root `~/.config/opencode/skills/` created 2026-09-22 with
  `review-agent/` (copied from Codex `.system`, self-contained prompt).
- Repository skills (`skills-lock.json` + `.agents/skills/`):
  - `animate` — source `emilkowalski/skills`.
  - `frontend-design` — source `anthropics/skills`.
- Shared user skills used in this session: Microsoft Foundry set under
  `C:/Users/muaro/.agents/skills/` (`microsoft-foundry`, `finetuning`,
  `deploy-model`, `capacity`, `customize`, `preset`).
- Plugin skills: ponytail family (see Plugins).
- Codex-only skills (Documents, PDF, Spreadsheets, Presentations, imagegen,
  system OpenAI skills, etc.) stay Codex-only except `review-agent` above;
  imagegen needs Codex's built-in `image_gen` tool, the rest are Codex workflows.

Full path-level listing: [skills.txt](skills.txt).

## Repository context

- `AGENTS.md` at repo root: course layout, build/test commands, style, commit
  and credential rules (same file Codex reads).
- `AI_TOOLS.md`: Ukrainian catalog of recommended skills/MCPs/plugins for this
  repo (2026-09-20); recommendations only, not an installer.
- `skills-lock.json`: locked hashes for the two repository skills.
- No project-level OpenCode JSON config; behavior comes from global config +
  `AGENTS.md` + `.agents/` skills.

## Scan boundaries

Read: both top-level OpenCode configs, command/plugin/TUI/package files,
project `.opencode/` and `.agents/`, `skills-lock.json`, repo `AGENTS.md` and
`AI_TOOLS.md`, and the session skill catalog.

Not exported: `FREELLMAPI_API_KEY` and other env values, `auth.json` or
equivalent credential stores if present, chat history, SQLite/state under
`~/.local/share/opencode/` beyond tool-output logs, and backup config contents
beyond their filenames. Presence of a plugin source tree is not a load test.
Provider reachability was not probed.

## Effective configuration caveats

- Merge or pick one of `opencode.json` / `opencode.jsonc`; today the split can
  silently drop providers or SecondBrain/MCP/model settings.
- Codex MCPs mirrored into OpenCode 2026-09-22 (see MCP above);
  docling/excel/markitdown now verified for both agents.
  Node/npx servers report `connected` in `opencode mcp list`.
- OpenCode rescan date for cross-agent files: 2026-09-22. Codex rescan date
  remains 2026-09-21.
