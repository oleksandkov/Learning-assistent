# Repository Guidelines

## Project Structure & Module Organization

This repository collects independent university exercises and larger projects. Top-level folders such as `ALG_AND_STR/`, `ARCHIT/`, `LOGIC/`, `MATH/`, `OOP/`, `OP/`, and `PEOPLE-MACHINE/` group work by subject. Keep changes inside the relevant course and lab directory; do not introduce shared root dependencies for a single assignment.

Larger projects document their own layout. For example, `ALG_AND_STR/CURSOVA/SAKOBAN-main/` contains C++ sources in `src/`, unit tests in `tests/`, levels in `levels/`, documentation in `docs/`, and a separate web client in `web/`. Reusable repository scripts live in `scripts/`.

## Document and PDF Tooling

Never use the `carbone` MCP server unless the user explicitly asks for it. For document and PDF work use other available MCP servers or local tooling instead:

- `docling` — read, convert and analyse existing documents (DOCX, PDF, HTML, images).
- `docsmith` — read, inspect and write documents (Excel, Word, PowerPoint, PDF, text).
- `markitdown` — convert documents to Markdown for inspection.
- Local tooling — `python-docx` for DOCX editing; Microsoft Word COM automation (`ExportAsFixedFormat`) for DOCX → PDF on Windows; `pypdf` for PDF inspection.

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

## Cleanup & Temporary Files

Agents must maintain repository cleanliness and clean up after themselves:
- **Remove disposable scripts and helper files**: Any temporary scripts, one-off migration utilities, scratch files, or test harnesses created during a task must be removed immediately after their application and verification, unless they are explicitly intended to be preserved as reusable assets or requested by the user (видалення допоміжних файлів і скриптів після їх використання, якщо вони точно не знадобляться в майбутньому).
- **No dangling artifacts**: Do not leave disposable data files, temporary outputs, or scratch logs in project directories. Keep the working tree clean.
