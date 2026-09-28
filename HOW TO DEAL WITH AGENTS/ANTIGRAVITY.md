# Antigravity CLI on this device

Inventory date: 2026-09-28. Sources: local config files under
`C:/Users/muaro/.gemini/`, this repository's `.agents/`, and the skill
catalog exposed to this session. This documents configuration, not private
chats or credentials. It is an inventory, not a portable restore package.

## Settings and installation

- CLI launcher: `C:/Users/muaro/AppData/Local/agy/bin/agy.exe`.
- Global config directory: `C:/Users/muaro/.gemini/config/`.
- CLI settings: `C:/Users/muaro/.gemini/antigravity-cli/settings.json`.
- Color scheme: `tokyo night`.
- Trusted workspaces: `C:\Users\muaro`, `C:\Users\muaro\Documents\GitHub\SAKOBAN`,
  `C:\Users\muaro\Documents\GitHub\Learning-assistent`,
  `C:\Users\muaro\Documents\GitHub\Agent-For-TOM`.
- Hooks: `C:/Users/muaro/.gemini/config/hooks.json` — orca-status hooks
  (PreInvocation, PostInvocation, Stop, PreToolUse, PostToolUse) pointing to
  `C:\Users\muaro\.orca\agent-hooks\antigravity-*.cmd`.
- Built-in skills (5): agy-customizations, antigravity-guide, generative_ui,
  migrate-workflows, permissioned-github.
- Plugin skills (1): android-cli (historical; plugin directory now empty).

See [MCP.txt](MCP.txt), [plugins.txt](plugins.txt), [skills.txt](skills.txt),
and [MY PULL.txt](MY%20PULL.txt) for cross-agent inventories.

## Provider and model

- Antigravity CLI uses Google Gemini models via the `agy` CLI.
- No explicit model configuration in user settings; model selection is
  handled by the CLI/IDE integration.
- No custom provider endpoints configured.

## Instructions, permissions, and commands

- Repository guidance: `C:/Github/Learning-assistent/AGENTS.md`. It describes
  project boundaries, build/test commands, coding style, UTF-8 preservation,
  commit/PR conventions, and credential handling.
- No user `~/.gemini/AGENTS.md` or `AGENTS.override.md` was found.
- No project-level Antigravity config at the repository root.
- Trusted workspaces are configured in `antigravity-cli/settings.json`.
- Hooks are configured in `config/hooks.json` (orca-status integration).

## Plugins

- Plugin directory: `C:/Users/muaro/.gemini/config/plugins/` — ponytail installed
  2026-09-28 via `agy plugin install` (6 skills + 6 commands).
- Historical plugins (2026-09-18): android-cli-plugin, chrome-devtools-plugin,
  firebase, google-antigravity-sdk — all explicitly enabled at that time.

## MCP

- `mcp_config.json`: `mcpServers` map with 9 servers (mirrored from Codex/OpenCode
  2026-09-28):
  - `playwright` — `npx.cmd -y @playwright/mcp@latest`
  - `chrome-devtools` — `npx.cmd -y chrome-devtools-mcp@latest`
  - `context7` — `npx.cmd -y @upstash/context7-mcp@latest`
  - `markitdown` — `C:/Users/muaro/.local/bin/markitdown-mcp.exe`
  - `docling` — `C:/Users/muaro/.local/bin/docling-mcp-server.exe --transport stdio conversion generation manipulation`
  - `excel` — `C:/Users/muaro/.codex/mcps/excel/node_modules/@negokaz/excel-mcp-server/dist/excel-mcp-server_windows_amd64_v1/excel-mcp-server.exe`
  - `carbone` — `npx.cmd -y carbone-mcp` (needs CARBONE_API_KEY)
  - `office` — `npx.cmd -y office-mcp` (needs .NET 9)
  - `docsmith` — `npx.cmd -y docsmith-mcp`
- All commands and environment variables match Codex `config.toml` and
  OpenCode `opencode.jsonc` (same binaries, same args/env).
- `node_repl` skipped: Codex-runtime internal, not portable.
- Verification 2026-09-28: `agy mcp list` → all 9 enabled.

## Skills

- Built-in skills (5 files):
  - `agy-customizations` — C:/Users/muaro/.gemini/antigravity-cli/builtin/skills/agy-customizations/SKILL.md
  - `antigravity-guide` — C:/Users/muaro/.gemini/antigravity-cli/builtin/skills/antigravity-guide/SKILL.md
  - `generative_ui` — C:/Users/muaro/.gemini/antigravity-cli/builtin/skills/generative_ui/SKILL.md
  - `migrate-workflows` — C:/Users/muaro/.gemini/antigravity-cli/builtin/skills/migrate-workflows/SKILL.md
  - `permissioned-github` — C:/Users/muaro/.gemini/antigravity-cli/builtin/skills/permissioned-github/SKILL.md
- Plugin skills (6 files, ponytail installed 2026-09-28):
  - `ponytail`, `ponytail-audit`, `ponytail-debt`, `ponytail-gain`,
    `ponytail-help`, `ponytail-review`
- No user skill root configured for Antigravity CLI.
- No repository skills configured for Antigravity CLI.
- Shared Microsoft Foundry skills (C:/Users/muaro/.agents/skills/) are not
  automatically available to Antigravity CLI.

## Repository context

- `AGENTS.md` at repo root: course layout, build/test commands, style, commit
  and credential rules (same file other agents read).
- `AI_TOOLS.md`: Ukrainian catalog of recommended skills/MCPs/plugins for this
  repo (2026-09-20); recommendations only, not an installer.
- No project-level Antigravity JSON config; behavior comes from global config +
  `AGENTS.md` + trusted workspaces.

## Scan boundaries

Read: Antigravity CLI settings, MCP config, hooks config, built-in skills,
plugin directory, and the repository instruction file.

Not exported: conversation history, brain/annotation files, crash logs,
plugin data, and other runtime/state files. Presence of a plugin source tree
is not a load test. Provider reachability was not probed.

## Effective configuration caveats

- MCP servers mirrored from Codex/OpenCode 2026-09-28; restart Antigravity
  CLI to load them.
- Plugin system differs from Codex/OpenCode; ponytail and other marketplace
  plugins are not available.
- Built-in skills are Antigravity-specific; no shared user or repository
  skills are configured.
- Antigravity CLI rescan date: 2026-09-28. Codex rescan date: 2026-09-21.
  OpenCode rescan date: 2026-09-22.
