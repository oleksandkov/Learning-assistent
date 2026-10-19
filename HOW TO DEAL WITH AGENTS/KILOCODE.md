# Kilo Code on this device

Inventory date: 2026-10-14. Sources: local config files under
`C:/Users/muaro/.config/kilo/`, the installed npm package metadata, the
`kilo mcp list` / `kilo config check` / `kilo debug skill` output from this
session, and this repository's `AGENTS.md`. This documents configuration, not
private chats or credentials. It is an inventory, not a portable restore
package.

## Settings and installation

- CLI launcher: `C:/Users/muaro/AppData/Roaming/npm/kilo.ps1`.
- Global config directory: `C:/Users/muaro/.config/kilo/`.
- Installed npm package: `@kilocode/cli` **7.3.46**
  (`C:/Users/muaro/AppData/Roaming/npm/node_modules/@kilocode/cli/package.json`).
- Global config: `kilo.jsonc` (~4 KB after the 2026-10-14 update) — indexing,
  permissions, instructions, skills, commands, plugin, mcp, compaction.
- TUI config: `tui.jsonc` — activates the custom theme `secondbrain`.
- Custom theme: `themes/secondbrain.json` (Tokyo Night base with violet
  primary; dark + light palettes).
- Plugin cache: `C:/Users/muaro/.cache/kilo/packages/` — ponytail resolved to
  `@dietrichgebert/ponytail@latest` on first load.
- User home dir: `C:/Users/muaro/.kilo/` — `AGENT.md` (universal vault
  template), `commands/secondbrain.md`, `package.json`,
  `node_modules/`, `.gitignore`.
- Project config: `C:/Github/Learning-assistent/.kilo/` — `command/*.md`,
  `agent/*.md`, `kilo.json`; the repo `AGENTS.md` is read as instructions.
- Backup before the 2026-10-14 edit:
  `kilo.jsonc.bak-agent-inventory-20261014`.

See [MCP.txt](MCP.txt), [plugins.txt](plugins.txt), [skills.txt](skills.txt),
[MCP-VERIFICATION.md](MCP-VERIFICATION.md), and [MY PULL.txt](MY%20PULL.txt)
for cross-agent inventories.

## Configuration versus this session

This inventory was written from inside a running Kilo Code session (the
session that performed the configuration change). That session used
PowerShell with tool permissions from the config block below; it verified the
documented state with `kilo config check`, `kilo mcp list`, and
`kilo debug skill` on 2026-10-14.

## MCP

`kilo.jsonc` `mcp` map: 10 servers, mirrored 2026-10-14 from
`C:/Users/muaro/.config/opencode/opencode.jsonc` (same commands, args, and
environment; `node_repl` skipped — Codex-runtime internal, not portable).
Each local server carries `timeout: 120000` to survive npx/uvx cold starts
(OpenCode uses `experimental.mcp_timeout` 120000 for the same reason).

- `browser-use` — `uvx.exe --from browser-use[cli] browser-use --mcp`
- `playwright` — `npx -y @playwright/mcp@latest`
- `chrome-devtools` — `npx -y chrome-devtools-mcp@latest`
- `context7` — `npx -y @upstash/context7-mcp@latest`
- `markitdown` — `C:/Users/muaro/.local/bin/markitdown-mcp.exe`
- `docling` — `C:/Users/muaro/.local/bin/docling-mcp-server.exe --transport stdio conversion generation manipulation`
  (env: `DOCLING_MCP_CONVERSION_MODE`, `PYTHONIOENCODING`, `HF_HUB_DISABLE_SYMLINKS`, `CACHE_DIR`)
- `excel` — `C:/Users/muaro/.codex/mcps/excel/node_modules/@negokaz/excel-mcp-server/dist/excel-mcp-server_windows_amd64_v1/excel-mcp-server.exe`
  (env: `EXCEL_MCP_PAGING_CELLS_LIMIT`)
- `carbone` — `npx -y carbone-mcp` (env: `CARBONE_API_KEY`, value present in config, not recorded here)
- `office` — `npx -y office-mcp` (env: `DOTNET_ROLL_FORWARD=LatestMajor`)
- `docsmith` — `npx -y docsmith-mcp`

Verification 2026-10-14: `kilo mcp list` → all 10 `connected`
(browser-use ✓, playwright ✓, chrome-devtools ✓, context7 ✓, markitdown ✓,
docling ✓, excel ✓, carbone ✓, office ✓, docsmith ✓).
`kilo config check` → no warnings.

Tool counts observed in the 2026-09-29 cross-agent stdio probe (same binaries
and args): playwright 25, chrome-devtools 30, context7 2, markitdown 1,
docling 19, excel 7, carbone 11, office 17, docsmith 4 — see
[MCP-VERIFICATION.md](MCP-VERIFICATION.md).

## Plugins

Configured in `kilo.jsonc`:

- `@dietrichgebert/ponytail` — lazy senior-dev mode; added 2026-10-14 to match
  the OpenCode `plugin` array. The CLI resolved it from npm into
  `C:/Users/muaro/.cache/kilo/packages/@dietrichgebert/ponytail@latest/` and
  injected its 6 skills and 6 slash commands (see Skills, Commands).

Bundled (always active, visible in resolved config but not user entries):
`@kilocode/kilo-indexing`, `@kilocode/plugin-atomic-chat`.

OpenCode-only items with no Kilo equivalent: `@slkiser/opencode-quota`
(TUI plugin in opencode's `tui.json`), and the on-disk-but-unloaded
`voice-interface.js` / `opencode-bar` trees. Kilo's own TUI plugin slot
(`tui.jsonc` `plugin`) exists but is unused.

## Skills

18 skills exposed in this session (`kilo debug skill`, 2026-10-14).

Built-in (2):

- `customize-opencode` — configuring opencode itself (agent configs, subagents, skills, plugins, MCP, permissions).
- `kilo-config` — Kilo configuration reference (config paths, kilo.json fields, commands, agents, skills, permissions, MCPs, providers, TUI settings).

Shared user skills under `C:/Users/muaro/.agents/skills/` (7):

- `computer-use` — `C:/Users/muaro/.agents/skills/computer-use/SKILL.md`
- `microsoft-foundry` — Foundry agents/models end to end
- `finetuning` — SFT/DPO/RFT fine-tuning
- `deploy-model` — deployment router (preset/customize/capacity)
- `capacity` — quota/capacity discovery
- `customize` — custom deployment flow
- `preset` — quick deployment to optimal region

Repository skills under `C:/Github/Learning-assistent/.agents/skills/` (3):

- `animate` — animation build skill (emilkowalski/skills source)
- `frontend-design` — UI design guidance (anthropics/skills source)
- `gost-uml-diagram` — ГОСТ 19.701-90 / UML 2.5 block diagrams

Ponytail plugin skills (6): `ponytail`, `ponytail-audit`, `ponytail-debt`,
`ponytail-gain`, `ponytail-help`, `ponytail-review`.

Full path-level listing: [skills.txt](skills.txt). Codex-only skills
(Documents, PDF, Spreadsheets, Presentations, imagegen, system OpenAI skills)
stay Codex-only; the OpenCode `review-agent` skill is not configured for Kilo.

## Theme

Custom theme `secondbrain` at `~/.config/kilo/themes/secondbrain.json`,
activated through `tui.jsonc` (`"theme": "secondbrain"`). Tokyo Night derived:
dark surfaces (`#1a1b26` … `#292e42`), violet primary `#bb9af7` (the vault's
Obsidian identity), blue secondary, cyan accent, plus a full light palette and
diff/markdown/syntax role mappings. Kilo ships 35+ built-in themes (tokyonight,
catppuccin, dracula, gruvbox, nord, …); `secondbrain` appears alongside them in
`/themes` (`<leader>t`).

## Commands

- `/secondbrain` — `kilo.jsonc` `command.secondbrain`, template
  `C:/Users/muaro/.kilo/commands/secondbrain.md` (agent `build`; vault
  context/draft/link/tidy subcommands, ask-before-write and link-once guards).
- Ponytail slash commands (injected by the plugin): `/ponytail`,
  `/ponytail-audit`, `/ponytail-debt`, `/ponytail-gain`, `/ponytail-help`,
  `/ponytail-review`.

## Permissions, instructions, and models

- `permission`: `bash: allow`; `external_directory` allows
  `C:\Users\muaro\Roaming\npm\*`.
- `instructions`: `C:\Users\muaro\.kilo\AGENT.md` (universal template with
  kilocode placeholders — vault `C:\SecondBrain` is source of truth).
  Repository guidance comes from `AGENTS.md` at the repo root.
- `indexing`: provider `kilo`, model `mistralai/mistral-embed-2312`.
- `compaction.auto: false`.
- No `model` / `provider` block in the global config; model selection happens
  per session via `/models` (`<leader>m`). The SecondBrain instruction globs
  used by OpenCode (`C:\SecondBrain\**\*.md`) are not configured here — vault
  reach is gated through the `/secondbrain` command and `AGENT.md` instead.

## Scan boundaries

Read: `kilo.jsonc`, `tui.jsonc`, `themes/secondbrain.json`, installed package
metadata, resolved config output, MCP list output, skill catalog output, and
the repository instruction file.

Not exported: API keys and tokens, `auth.json` and credential stores, chat
history, SQLite/database state under `~/.local/share/kilo/`, and configuration
backups beyond their filenames. `CARBONE_API_KEY` is present in the config by
design (mirrored from OpenCode) but is deliberately not recorded in this file.
Configured is not the same as functionally exercised in a session.

## Effective configuration caveats

- 2026-10-14 update brought Kilo Code from 0 to 10 MCP servers and added the
  ponytail plugin, matching the OpenCode configuration set. Restart Kilo
  sessions (or run `/reload`) to load new server definitions.
- Kilo arrays merge additively with bundled defaults: the user `plugin` entry
  did not replace `@kilocode/kilo-indexing` / `@kilocode/plugin-atomic-chat`.
- The theme and TUI settings live in `tui.jsonc`, not `kilo.jsonc`; editing
  `kilo.jsonc` alone will never change the theme.
- Kilo CLI is a fork of OpenCode and reads the same config schema; legacy
  `opencode.json`/`opencode.jsonc` filenames in the same directories are also
  loaded, so avoid leaving stale copies there.
