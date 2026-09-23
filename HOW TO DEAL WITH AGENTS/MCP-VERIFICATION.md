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
