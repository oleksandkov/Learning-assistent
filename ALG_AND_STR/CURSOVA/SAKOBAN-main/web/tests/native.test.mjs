import { test } from "node:test";
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import { spawnSync } from "node:child_process";
import path from "node:path";
const root = path.resolve("..");
const bin = path.join(
  root,
  "bindings/web/build",
  process.platform === "win32" ? "sokoban_web.exe" : "sokoban_web",
);
const files = [
  "01_simple.xsb",
  "02_microban.xsb",
  "03_two_boxes.xsb",
  "04_warehouse.xsb",
  "05_triple.xsb",
];
function run(file, mode = "state", algo = "bfs", history = "", dir = "") {
  return runXsb(
    readFileSync(path.join(root, "levels", file)),
    mode,
    algo,
    history,
    dir,
  );
}
function runXsb(xsb, mode = "state", algo = "bfs", history = "", dir = "") {
  const result = spawnSync(bin, [mode, algo, history, dir], {
    input: xsb,
    encoding: "utf8",
    timeout: 12000,
    windowsHide: true,
  });
  assert.equal(result.status, 0, result.stderr || String(result.error));
  return JSON.parse(result.stdout);
}
function generate(width, height, boxes, walls, seed = "") {
  const result = spawnSync(
    bin,
    ["generate", String(width), String(height), String(boxes), String(walls), seed],
    { encoding: "utf8", timeout: 30000, windowsHide: true },
  );
  assert.equal(result.status, 0, result.stderr || String(result.error));
  return JSON.parse(result.stdout);
}
for (const file of files) {
  test(`${file}: all algorithms solve and replay through GameSession`, () => {
    const results = ["bfs", "astar-moves", "astar-pushes"].map((algo) => {
      const result = run(file, "solve", algo);
      assert.equal(result.status, "Solved");
      assert.equal(result.validated, true);
      assert.ok(result.searchMs >= 0 && result.generated >= 0);
      assert.ok(Array.isArray(result.trace) && result.trace.length >= 2);
      assert.equal(result.trace[0].explored, 0);
      assert.equal(result.trace.at(-1).status, "Solved");
      assert.equal(result.trace.at(-1).explored, result.explored);
      assert.ok(result.trace.every((point) =>
        point.state &&
        Number.isInteger(point.state.player) &&
        Array.isArray(point.state.boxes) &&
        Number.isFinite(point.g) &&
        Number.isFinite(point.h) &&
        Number.isFinite(point.depth)
      ));
      const initialDebugState = JSON.stringify(result.trace[0].state);
      assert.ok(
        result.trace.some((point) => JSON.stringify(point.state) !== initialDebugState),
        "debug trace must contain an expanded state distinct from the root",
      );
      for (let i = 1; i < result.trace.length; ++i) {
        assert.ok(result.trace[i].explored >= result.trace[i - 1].explored);
        assert.ok(result.trace[i].elapsedMs >= result.trace[i - 1].elapsedMs);
      }
      const replay = run(file, "state", algo, result.moves);
      assert.equal(replay.won, true);
      assert.equal(replay.moves, result.moves.length);
      assert.equal(replay.pushes, result.pushes);
      console.log(
        `${file} ${algo}: ${result.moves.length} moves, ${result.pushes} pushes, replay valid`,
      );
      return result;
    });
    assert.equal(
      results[0].moves.length,
      results[1].moves.length,
      "BFS and A* Moves must agree",
    );
    assert.ok(results[2].pushes <= results[0].pushes);
  });
}
test("blocked move, undo/redo histories, current-state solving, solved state and invalid history", () => {
  const file = files[0],
    initial = run(file);
  const blocked = run(file, "state", "bfs", "", "D");
  assert.equal(blocked.accepted, false);
  assert.deepEqual(blocked.state, initial.state);
  assert.equal(blocked.moves, 0);
  const after = run(file, "state", "bfs", "", "L");
  assert.equal(after.moves, 1);
  const solved = run(file, "solve", "astar-moves", "L");
  assert.equal(solved.validated, true);
  assert.equal(run(file, "state", "bfs", "L" + solved.moves).won, true);
  assert.deepEqual(run(file).state, initial.state);
  assert.deepEqual(run(file, "state", "bfs", "L").state, after.state);
  assert.equal(run(file, "state", "bfs", "D").error, "InvalidHistory");
  const goal = run(file, "solve", "bfs", "U");
  assert.equal(goal.status, "Solved");
  assert.equal(goal.moves, "");
  assert.equal(goal.validated, true);
});
test("the old third map is unsolvable and reports NoSolution", () => {
  for (const algo of ["bfs", "astar-moves", "astar-pushes", "idastar-pushes", "greedy-pushes"]) {
    const result = run("03_microban_3.xsb", "solve", algo);
    assert.equal(result.status, "NoSolution");
    assert.equal(result.validated, false);
    assert.equal(result.moves, "");
  }
});
test("new algorithms: IDA* optimal pushes, Greedy valid but not optimal-claimed", () => {
  const astar = run("01_simple.xsb", "solve", "astar-pushes");
  assert.equal(astar.status, "Solved");
  const ida = run("01_simple.xsb", "solve", "idastar-pushes");
  assert.equal(ida.status, "Solved");
  assert.equal(ida.validated, true);
  assert.equal(ida.pushes, astar.pushes);
  assert.ok(Array.isArray(ida.trace) && ida.trace.length >= 2);
  const greedy = run("01_simple.xsb", "solve", "greedy-pushes");
  assert.equal(greedy.status, "Solved");
  assert.equal(greedy.validated, true);
  assert.ok(greedy.pushes >= ida.pushes);
  const replay = run("01_simple.xsb", "state", "greedy-pushes", greedy.moves);
  assert.equal(replay.won, true);
});
test("custom generator honors dimensions, boxes, exact walls and seed", () => {
  const generated = generate(8, 8, 2, 4, "12345");
  assert.equal(generated.success, true);
  assert.equal(generated.width, 8);
  assert.equal(generated.height, 8);
  assert.equal(generated.boxes, 2);
  assert.equal(generated.walls, 4);
  assert.equal(typeof generated.seed, "string");
  const rows = generated.xsb.trimEnd().split("\n");
  assert.equal(rows.length, 8);
  assert.ok(rows.every((row) => row.length === 8));
  const totalWalls = [...generated.xsb].filter((char) => char === "#").length;
  assert.equal(totalWalls - (2 * 8 + 2 * 6), 4);
  assert.equal([...generated.xsb].filter((char) => "$*".includes(char)).length, 2);
  assert.equal([...generated.xsb].filter((char) => ".*+".includes(char)).length, 2);
  assert.equal(generated.solution.length, generated.moves);
  assert.equal(runXsb(generated.xsb, "state", "bfs", generated.solution).won, true);
  const solved = runXsb(generated.xsb, "solve", "astar-pushes");
  assert.equal(solved.validated, true);
  assert.equal(runXsb(generated.xsb, "state", "bfs", solved.moves).won, true);
  assert.equal(generate(8, 8, 2, 4, generated.seed).xsb, generated.xsb);
});
test("custom generator rejects impossible wall count", () => {
  const generated = generate(5, 5, 1, 1, "1");
  assert.equal(generated.success, false);
  assert.equal(generated.error, "InvalidGeneratorOptions");
});
test("custom generator supports the 100 by 100 boundary", () => {
  const generated = generate(100, 100, 3, 20, "42");
  assert.equal(generated.success, true);
  const rows = generated.xsb.trimEnd().split("\n");
  assert.equal(rows.length, 100);
  assert.ok(rows.every((row) => row.length === 100));
  assert.equal(generated.walls, 20);
  const snapshot = runXsb(generated.xsb);
  assert.equal(snapshot.board.width, 102);
  assert.equal(snapshot.board.height, 102);
});
