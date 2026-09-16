# Web adapter over the shared C++ core

`native.cpp` provides the running Next.js app with structured JSON. It does not change core headers or implement independent rules/search algorithms.

## Build

From `web/`, run `npm run native` (also called by `predev` and `prebuild`). The compiler script builds the existing `src/core/*.cpp`, `src/solvers/*.cpp` and this adapter into `bindings/web/build/sokoban_web(.exe)` with C++20 and `-O2`. Windows builds link the runtime statically. Generated binaries are ignored.

## Protocol

For gameplay, stdin is XSB level text. Arguments are `state|solve`, `bfs|astar-moves|astar-pushes`, history as UDLR, and an optional direction for `state`.

The adapter parses XSB with `LevelParser`, reconstructs a `GameSession` by applying the valid history, then either applies a move or searches from that exact snapshot. Invalid history returns an error. `state` returns `{ accepted, won, moves, pushes, board: { width, height, walls, goals, floor }, state: { player, boxes } }`.

`solve` creates the shared solver, enables its optional debug-state snapshot, advances one node at a time while the bounded trace is being collected, validates the solution with `ReplayValidator`, and returns status, UDLR, counts, timings, statistics, validation, and up to 160 trace points. Each captured point contains the real state selected from the frontier plus progress, BFS depth or A* `g`/`h`, and transition metadata. After the trace cap, solving continues in larger batches. BFS uses Moves. A* uses the explicitly selected metric. A solved starting state yields a validated empty path.

Generation uses `generate <width> <height> <boxes> <internal-walls> [seed]` with empty stdin. The adapter selects a reproducible candidate seed whose first `LevelGenerator` wall draw matches the requested count, then delegates construction, solving and replay validation to the shared generator. Dimensions are 5-100, boxes are capped at 50, and distinct internal walls are capped at the core generator's limit of 20. The response includes XSB, the effective seed, attempts, and the complete validated UDLR solution.

Next.js validates bounded request parameters and executes the adapter without a shell. Each request owns an isolated process; timeout/abort kills it. No user-controlled filesystem paths are accepted.

## Future WASM

The current product uses the native Node server adapter, not WASM or a TypeScript fallback. The offline Emscripten/Worker integration remains a future PLAN.md milestone; no duplicate game engine is shipped.
