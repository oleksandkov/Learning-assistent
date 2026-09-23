# Codex on this device

Inventory date: 2026-09-21. Sources: the local files cited below and the tool/skill
catalog exposed to this session. This documents configuration, not private chats
or credentials. It is an inventory, not a portable restore package.

## Settings and installation

- User configuration: `C:/Users/muaro/.codex/config.toml`.
- CLI launcher: `C:/Users/muaro/AppData/Roaming/npm/codex.ps1`.
- Installed npm package: `@openai/codex` **0.155.1**, from
  `C:/Users/muaro/AppData/Roaming/npm/node_modules/@openai/codex/package.json`.
- Default model: **gpt-6-astra**; reasoning effort: **medium**.
- Approval reviewer: `user`; Windows sandbox setting: `elevated`.
  These are config values, not a grant of unrestricted filesystem access.
- Desktop: follow-up queue `steer`, conversation detail `STEPS_PROSE`,
  sans font size 14, code font size 13, ambient suggestions enabled.
- Notification command invokes the installed computer-use executable with `turn-ended`.
- Seven MCP servers, 12 explicitly enabled plugins, 14 cached plugin manifests,
  57 skill files found, 26 skills exposed in this session.
- Runtime config reports app build `26.908.40834`; this is the configured runtime
  value, not an independent check of the running desktop application version.

See [MCP.txt](MCP.txt), [plugins.txt](plugins.txt), [skills.txt](skills.txt),
[MY PULL.txt](MY%20PULL.txt), and [OPENCODE.md](OPENCODE.md) for the full
inventories (OpenCode rescanned 2026-09-22; Codex remains 2026-09-21).

## Document MCP update

MarkItDown, Docling, and Excel MCP are installed and enabled. All passed MCP
initialization and functional file tests. See [MCP-VERIFICATION.md](MCP-VERIFICATION.md)
for versions, commands, Windows cache configuration, and exact test coverage.
The original config was backed up; restart Codex to load the added servers.

## Configuration versus this session

The active session uses PowerShell in
`C:/Github/Learning-assistent/ALG_AND_STR/CURSOVA/SAKOBAN-main`, with workspace-write
filesystem restrictions and restricted network access. The requested documentation
folder is outside that writable project; writing there requires escalation.
Configured trusted-project entries are preserved in the snapshot below.

Personal defaults and project overrides are separate configuration layers; runtime
overrides and managed constraints can affect the resulting permissions.
Reference: [official configuration documentation](https://learn.chatgpt.com/docs/config-file/config-basic.md),
checked using the downloaded official Codex manual on 2026-09-21.

## Instructions, hooks, and automation

- Repository guidance: `C:/Github/Learning-assistent/AGENTS.md`. It describes
  project boundaries, build/test commands, coding style, UTF-8 preservation,
  commit/PR conventions, and credential handling. Its actual text is appended below.
- No `AGENTS.md` at `C:/` or `C:/Github/`, and no target-folder `AGENTS.md`, was found.
- No user `~/.codex/AGENTS.md` or `AGENTS.override.md` was found.
- No `.codex/config.toml` at the repository root or current Sokoban project was found.
- No user `~/.codex/requirements.toml`, `~/.codex/rules`, or
  `~/.codex/automations` directory was found. Host-managed policy and scheduled
  task database contents were not audited, so this does not establish their absence.
- Ponytail 4.10.0 is enabled. Its persisted mode file
  `C:/Users/muaro/.codex/plugins/data/ponytail-ponytail/.ponytail-active` contains `full`.
  Its plugin manifest references `hooks/claude-codex-hooks.json`, declaring
  `SessionStart`, `SubagentStart`, and `UserPromptSubmit` command hooks using
  `ponytail-activate.js`, `ponytail-subagent.js`, and `ponytail-mode-tracker.js`.
  These are declarations; this scan did not execute or independently test the hooks.
- Shared `C:/Users/muaro/.agents/hooks/hooks.json` declares a `PostToolUse`
  telemetry hook and platform-specific scripts under `.agents/hooks/scripts`.
  Presence alone does not establish that Codex loads this shared hook.

## Sanitized user configuration

All configuration entries are retained below. MCP environment variable values
are omitted, including transient pipe identifiers. This block is documentation;
do not use its placeholder values as a working configuration.

```toml
approvals_reviewer = "user"
model = "gpt-6-astra"
model_reasoning_effort = "medium"
notify = [ "C:\\Users\\muaro\\AppData\\Local\\OpenAI\\Codex\\runtimes\\cua_node\\a708e72b10c27b59\\bin\\node_modules\\@oai\\sky\\bin\\windows\\codex-computer-use.exe", "turn-ended" ]
[projects.'c:\\github\\learning-assistent']
trust_level = "trusted"

[projects.'c:\\']
trust_level = "trusted"

[projects.'c:\']
trust_level = "trusted"

[windows]
sandbox = "elevated"

[mcp_servers.node_repl]
args = []
command = 'C:\Users\muaro\AppData\Local\OpenAI\Codex\runtimes\cua_node\a708e72b10c27b59\bin\node_repl.exe'
startup_timeout_sec = 120

[mcp_servers.node_repl.env]
NODE_REPL_NATIVE_PIPE_CONNECT_TIMEOUT_MS = "[VALUE OMITTED]"
NODE_REPL_NODE_MODULE_DIRS = "[VALUE OMITTED]"
NODE_REPL_NODE_PATH = "[VALUE OMITTED]"
NODE_REPL_TRUSTED_CODE_PATHS = "[VALUE OMITTED]"
CODEX_HOME = "[VALUE OMITTED]"
BROWSER_USE_AVAILABLE_BACKENDS = "[VALUE OMITTED]"
BROWSER_USE_TINYSKY_ENABLED = "[VALUE OMITTED]"
NODE_REPL_INSTRUCTIONS_USE_CASE_BROWSER = "[VALUE OMITTED]"
NODE_REPL_INSTRUCTIONS_USE_CASE_CHROME = "[VALUE OMITTED]"
BROWSER_USE_CODEX_APP_BUILD_FLAVOR = "[VALUE OMITTED]"
BROWSER_USE_CODEX_APP_VERSION = "[VALUE OMITTED]"
NODE_REPL_TRUSTED_SERVICES = "[VALUE OMITTED]"
SKY_CUA_NATIVE_PIPE = "[VALUE OMITTED]"
SKY_CUA_NATIVE_PIPE_DIRECTORY = "[VALUE OMITTED]"
CODEX_CLI_PATH = "[VALUE OMITTED]"

[desktop]
followUpQueueMode = "steer"
conversationDetailMode = "STEPS_PROSE"
sansFontSize = 14
codeFontSize = 13
ambient-suggestions-enabled = true

[marketplaces.openai-bundled]
source_type = "local"
source = '\\?\C:\Users\muaro\.codex\.tmp\bundled-marketplaces\openai-bundled'


[marketplaces.openai-primary-runtime]
source_type = "local"
source = '\\?\C:\Users\muaro\.cache\codex-runtimes\codex-primary-runtime\plugins\openai-primary-runtime'

[plugins."codex-app-tools@openai-bundled"]
enabled = true

[plugins."sites@openai-bundled"]
enabled = true

[plugins."browser@openai-bundled"]
enabled = true

[plugins."unified-computer-use@openai-bundled"]
enabled = true

[plugins."visualize@openai-bundled"]
enabled = true

[plugins."computer-use@openai-bundled"]
enabled = true


[plugins."documents@openai-primary-runtime"]
enabled = true

[plugins."pdf@openai-primary-runtime"]
enabled = true

[plugins."spreadsheets@openai-primary-runtime"]
enabled = true

[plugins."presentations@openai-primary-runtime"]
enabled = true

[plugins."template-creator@openai-primary-runtime"]
enabled = true

[tui.model_availability_nux]
gpt-6-astra = 4

[projects."C:\\Github\\Agent-For-TOM"]
trust_level = "trusted"

[projects."B:\\PROJECTS\\MINI-PROJECTS\\SOKOBAN"]
trust_level = "trusted"

[projects."C:\\Github\\SAKOBAN"]
trust_level = "trusted"

[projects."C:\\Github\\Learning-assistent"]
trust_level = "trusted"

[marketplaces.ponytail]
source_type = "git"
source = "https://github.com/DietrichGebert/ponytail.git"

[plugins."ponytail@ponytail"]
enabled = true

[mcp_servers.playwright]
command = 'npx.cmd'
args = ['-y', '@playwright/mcp@latest']
startup_timeout_sec = 60

[mcp_servers.chrome-devtools]
command = 'npx.cmd'
args = ['-y', 'chrome-devtools-mcp@latest']
startup_timeout_sec = 60

[mcp_servers.context7]
command = 'npx.cmd'
args = ['-y', '@upstash/context7-mcp@latest']
startup_timeout_sec = 60

# Document MCPs installed and functionally verified on 2026-09-21.
[mcp_servers.markitdown]
command = 'C:/Users/muaro/.local/bin/markitdown-mcp.exe'
args = []
enabled = true
startup_timeout_sec = 120
tool_timeout_sec = 300

[mcp_servers.docling]
command = 'C:/Users/muaro/.local/bin/docling-mcp-server.exe'
args = ['--transport', 'stdio', 'conversion', 'generation', 'manipulation']
enabled = true
startup_timeout_sec = 120
tool_timeout_sec = 600

[mcp_servers.docling.env]
DOCLING_MCP_CONVERSION_MODE = "[VALUE OMITTED]"
PYTHONIOENCODING = "[VALUE OMITTED]"
# Ordinary file copies avoid Windows model-cache symlink privilege failures.
HF_HUB_DISABLE_SYMLINKS = "[VALUE OMITTED]"
CACHE_DIR = "[VALUE OMITTED]"

[mcp_servers.excel]
command = 'C:/Users/muaro/.codex/mcps/excel/node_modules/@negokaz/excel-mcp-server/dist/excel-mcp-server_windows_amd64_v1/excel-mcp-server.exe'
args = []
enabled = true
startup_timeout_sec = 60
tool_timeout_sec = 120

[mcp_servers.excel.env]
EXCEL_MCP_PAGING_CELLS_LIMIT = "[VALUE OMITTED]"
```

## Repository instructions on disk

Source: `C:/Github/Learning-assistent/AGENTS.md`.

```markdown
# Repository Guidelines

## Project Structure & Module Organization

This repository collects independent university exercises and larger projects. Top-level folders such as `ALG_AND_STR/`, `ARCHIT/`, `LOGIC/`, `MATH/`, `OOP/`, `OP/`, and `PEOPLE-MACHINE/` group work by subject. Keep changes inside the relevant course and lab directory; do not introduce shared root dependencies for a single assignment.

Larger projects document their own layout. For example, `ALG_AND_STR/CURSOVA/SAKOBAN-main/` contains C++ sources in `src/`, unit tests in `tests/`, levels in `levels/`, documentation in `docs/`, and a separate web client in `web/`. Reusable repository scripts live in `scripts/`.

## Build, Test, and Development Commands

There is no repository-wide build. Read the nearest `README.md` and run commands from that project directory.

- Sokoban C++: `cmake -S . -B build -G Ninja`, then `cmake --build build`.
- Sokoban tests: `ctest --test-dir build --output-on-failure`.
- Sokoban web client: from `SAKOBAN-main/web`, run `npm install`, then `npm run dev` or `npm run build`.
- LABA4 web app: from `PEOPLE-MACHINE/LABA4/webapp`, run `npm install`, `npm run dev`, or `npm run build`.
- Standalone Python checks: run the relevant file directly, for example `python ARCHIT/LAB1/test_converter.py`.

Do not commit generated build directories, dependency folders, or compiled binaries unless an assignment explicitly requires them.

## Coding Style & Naming Conventions

Follow the conventions already used in the project being edited. C++ code targets C++20, uses four-space indentation, `PascalCase` types, and descriptive `.hpp`/`.cpp` pairs. Java classes use `PascalCase`; methods and variables use `camelCase`. Python follows PEP 8 with four spaces and `snake_case`. Preserve UTF-8 text, especially Ukrainian documentation. Avoid drive-specific absolute paths in source code.

## Testing Guidelines

Add tests beside the affected project. Name Python tests `test_*.py`; keep Sokoban C++ tests in `tests/test_*.cpp`; JavaScript tests use `*.test.mjs`. Cover the changed behavior and at least one failure or edge case. Run the narrow test first, followed by the project’s complete available suite.

## Commit & Pull Request Guidelines

Recent commits use short, lowercase, topic-focused messages such as `docs update` and `lab 1 algo`. Prefer a clearer imperative form, for example `add parser edge-case test`. Keep each commit limited to one assignment or concern. Pull requests should identify the course/project, summarize the change, list verification commands, link relevant tasks, and include screenshots for visible UI changes.

## Security & Configuration

Never commit API keys, tokens, `.env` files, or personal credentials. Provide sanitized examples and document required environment variables in the local README.
```

## Scan boundaries

Read: user config, installed package metadata, system/shared/repository skill
metadata, installed plugin manifests, MCP declarations, hook declarations,
Ponytail mode state, and the repository instruction file.

Not exported: `auth.json`, `.sandbox-secrets`, authentication material, personal
conversation/history content, `.codex-global-state.json` application history,
SQLite contents, logs, recordings, environment values, and configuration backups.
Those runtime/state files are not a portable configuration bundle. Temporary
marketplace source downloads are not treated as installed plugins.

MCP startup and functional operations were tested for the three added document
servers. Authentication, browser sessions, external account connectivity, other
Windows profiles, WSL installations, and other agents were not tested.
The existing other-agent records remain explicitly dated 2026-09-18.

## Effective runtime configuration

The current CLI uses CODEX_HOME=C:/Users/muaro/AppData/Roaming/orca/codex-runtime-home/home.
Its config.toml also contains the three verified document MCP definitions;
the ordinary ~/.codex/config.toml remains updated for regular Codex use.
The host automatically synchronized the new entries into its runtime config.
No manual runtime edit was necessary; all three definitions match the user config.
The user config backup is ~/.codex/config.toml.bak-document-mcp-20260921.
Runtime-managed settings and hooks were not edited.
