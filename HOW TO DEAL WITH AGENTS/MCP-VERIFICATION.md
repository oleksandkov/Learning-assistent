# Document MCP installation and verification

Verified: 2026-09-21 on this Windows device, using real MCP stdio client calls.

| Server | Installed version | Result |
| --- | --- | --- |
| Microsoft MarkItDown MCP | 0.0.1a7 (MarkItDown 0.1.5) | PASS: PDF, DOCX, PPTX, XLSX text extraction; missing-file error |
| Docling MCP | 3.2.0 (docling-slim 2.129.0) | PASS: text PDF conversion, scanned-PDF OCR, Markdown content checks; missing-file error |
| Excel MCP (@negokaz) | npm 0.12.0 | PASS: XLSX reading, text/formula write and read-back, native Excel screenshot; missing-sheet error |

All three passed MCP initialization and tool discovery: respectively 1, 19, and 7
tools. The Excel executable identifies itself as `0.12.0-SNAPSHOT-1ff4340` inside
the published npm 0.12.0 package; both identifiers were observed.

## Installation

- MarkItDown and Docling use separate `uv tool` environments with Python 3.12.
  Environments: `%APPDATA%/uv/tools/markitdown-mcp` and
  `%APPDATA%/uv/tools/docling-mcp`; launchers: `%USERPROFILE%/.local/bin`.
- Excel is installed in `%USERPROFILE%/.codex/mcps/excel`, with its npm version
  recorded exactly in `package.json` and `package-lock.json`. Codex launches the
  installed Windows executable directly; no download is needed at each launch.
- Docling runs locally. OCR, layout, and table models needed by these tests were
  downloaded. Hugging Face model cache: `%USERPROFILE%/.cache/huggingface/hub`.
  Document cache: `%LOCALAPPDATA%/DoclingMCP/cache`.
- `HF_HUB_DISABLE_SYMLINKS=1` fixes a Windows model-cache privilege failure found
  during verification. It uses normal copies without changing Windows policy.
- Codex user config now contains seven MCP servers. The four existing server
  definitions and every unrelated setting were preserved.
- Backup: `%USERPROFILE%/.codex/config.toml.bak-document-mcp-20260921`.

## Use and limits

Restart Codex or start a fresh session so it loads the new server definitions.
This running conversation's tool catalog was created before installation.
Verification launched the configured server commands through an MCP client;
it does not claim the new tools were hot-loaded into this conversation.

MarkItDown extracts content; Docling handles structured extraction and OCR.
Use the existing Documents and Presentations plugins to create/edit DOCX and PPTX.
Excel MCP edits workbooks; native sheet rendering was verified with installed Excel.
Testing used generated disposable documents, not personal files. Formula storage
and read-back were tested; the full Excel formula engine was not exhaustively tested.

## Repeat the checks

The retained test reads `$CODEX_HOME/config.toml` when set, otherwise
`~/.codex/config.toml`, and fails if any of the three server definitions is missing.
Run this in PowerShell; it writes fixtures/results only to the user's temporary folder:

```powershell
$mcpPython = Join-Path $env:APPDATA 'uv/tools/docling-mcp/Scripts/python.exe'
$mcpTest = Join-Path $env:USERPROFILE '.codex/mcps/test_document_mcps.py'
& $mcpPython $mcpTest fixtures
foreach ($server in @('markitdown', 'docling', 'excel')) {
    & $mcpPython $mcpTest $server
    if ($LASTEXITCODE -ne 0) { throw "MCP verification failed: $server" }
}
```

Test outputs: `%TEMP%/codex-document-mcp-test`, including `*.result.json`, tool
schemas, stderr logs, sample documents, and Docling Markdown extraction results.
Retained passing reports: `%USERPROFILE%/.codex/mcps/verification-20260921.json`.

## Sources

- [Codex MCP configuration](https://learn.chatgpt.com/docs/extend/mcp)
- [Microsoft MarkItDown MCP](https://github.com/microsoft/markitdown/tree/main/packages/markitdown-mcp)
- [Docling MCP](https://github.com/docling-project/docling-mcp)
- [Excel MCP](https://github.com/negokaz/excel-mcp-server)

## Effective runtime configuration

The current CLI uses CODEX_HOME=C:/Users/muaro/AppData/Roaming/orca/codex-runtime-home/home.
Its config.toml also contains the three verified document MCP definitions;
the ordinary ~/.codex/config.toml remains updated for regular Codex use.
The host automatically synchronized the new entries into its runtime config.
No manual runtime edit was necessary; all three definitions match the user config.
The user config backup is ~/.codex/config.toml.bak-document-mcp-20260921.
Runtime-managed settings and hooks were not edited.

## OpenCode

Mirrored to OpenCode 2026-09-22: all three servers plus playwright,
chrome-devtools, and context7 are in `opencode.jsonc`'s `mcp` map.
`opencode mcp list` reports all 6 `connected`. Codex verification above
(same binaries, same args/env) applies; plus a 2026-09-22 stdio re-probe:
markitdown 1 tool, excel 7 tools. See [OPENCODE.md](OPENCODE.md).

## New MCPs added 2026-09-28

| Server | Command | Verification |
| --- | --- | --- |
| Carbone | `npx -y carbone-mcp` | Starts; requires CARBONE_API_KEY |
| Office | `npx -y office-mcp` | Starts; .NET 9 stdio server |
| Docsmith | `npx -y docsmith-mcp` | PASS: initialize handshake, serverInfo v0.1.0 |

All three added to Codex config.toml, OpenCode opencode.jsonc, and
Antigravity mcp_config.json. `agy mcp list` reports all 9 enabled.
`opencode mcp list` 2026-09-28: markitdown ✓, excel ✓, docsmith ✓,
playwright ✓, chrome-devtools ✓; context7 ✗, docling ✗, carbone ✗, office ✗
(OpenCode timeout too short for npx cold starts; all verified via stdio).

## Restoration and re-verification — 2026-09-29

Drift found against this inventory and fixed:

| Item | Was | Now |
| --- | --- | --- |
| Codex `~/.codex/config.toml` | 7 servers (carbone/office/docsmith absent) | 10 servers; `codex mcp list` lists all 10 |
| Codex runtime `.../orca/codex-runtime-home/home/config.toml` | 7 servers | 10 servers; 12 plugins, 3 marketplaces unchanged |
| OpenCode `opencode.jsonc` `mcp` map | 6 servers | 9 servers |
| Antigravity `~/.gemini/config/mcp_config.json` | `{"mcpServers": {}}` (file untouched since 2026-09-18) | 9 servers, `agy mcp list` all enabled |

Stdio probes on 2026-09-29 (MCP initialize + tools/list) against the exact
command/args/env in each agent's config: **27/27 passed** — 9 servers x 3
agents (Codex, OpenCode, Antigravity). Tool counts: playwright 25,
chrome-devtools 30, context7 2, markitdown 1, docling 19, excel 7,
carbone 11, office 17, docsmith 4.

Carbone API key applied 2026-09-29 to all three configs
(`carbone.env.CARBONE_API_KEY`; value deliberately not recorded here).
Functional check: `get_api_status` → `Carbone API: online (v5.15.2)`,
`list_templates` → empty list (authenticated, not a 401). Carbone is now
functional, not just configured.


`office-mcp` targets `net9.0` but only .NET 8.0.30 and 10.0.8 runtimes are
installed, so it failed to launch. Fixed with `DOTNET_ROLL_FORWARD=LatestMajor`
in the `office.env` block of all three agent configs — no runtime install.
Verified: office connects in `opencode mcp list`.

`opencode mcp list` 2026-09-29: all 9 connected — playwright, chrome-devtools,
context7, markitdown, docling, excel, carbone, office, docsmith.
`agy mcp list` 2026-09-29: all 9 enabled. `codex mcp list` 2026-09-29:
all 10 enabled (including Codex-internal `node_repl`).

Skills and plugins were already at the documented state: Codex 12 plugins /
14 cached manifests / 3 marketplaces / `.system` skills (6), `.agents` Foundry
skills, repository skills (2), OpenCode `review-agent` + ponytail,
Antigravity ponytail (6 skills + 6 commands) and 5 built-ins.

Backups written: `~/.codex/config.toml.bak-mcp-restore-20260929`,
runtime `config.toml.bak-mcp-restore-20260929`,
`~/.gemini/config/mcp_config.json.bak-mcp-restore-20260929`.
The older `mcp_config.json.bak` (2026-09-06, blender/godot/firebase set) was
left untouched and is not the documented 9-server configuration.

