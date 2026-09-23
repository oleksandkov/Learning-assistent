import { test } from "node:test";
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import ts from "typescript";

// Use the installed compiler so these checks work on the project's Node 18+ baseline.
const compile = source => `data:text/javascript;base64,${Buffer.from(ts.transpileModule(source, {
  compilerOptions: { target: ts.ScriptTarget.ES2022, module: ts.ModuleKind.ESNext },
}).outputText).toString("base64")}`;
const brainUrl = compile(readFileSync("src/lib/flybrain.ts", "utf8"));
const { encodeState, decodeAction, createFlyBrain, stepBrain, restoreFlyBrain, compactFlyBrain, mulberry32, brainConfig, applyDopamine, computeRayCasting, isDeadlockPush, updateCXHeading } = await import(brainUrl);
const { runSession } = await import(compile(readFileSync("src/workers/flybrain.worker.ts", "utf8").replace('"../lib/flybrain"', JSON.stringify(brainUrl))));
const snapshot = {
  board: { width: 5, height: 5, walls: Array(25).fill(false), floor: Array(25).fill(true), goals: Array.from({ length: 25 }, (_, i) => i === 13) },
  state: { player: 12, boxes: [11] }, moves: 0, pushes: 0, accepted: true, won: false,
};

test("encoder is deterministic, bounded, handles board edges and detects a moved box", () => {
  const currents = encodeState(snapshot);
  assert.deepEqual(currents, encodeState(snapshot));
  assert.equal(currents.length, 64);
  assert.ok([...currents].every(n => Number.isFinite(n) && n >= 0 && n <= 1.5));
  assert.notDeepEqual(currents, encodeState({ ...snapshot, state: { player: 12, boxes: [13] } }));
  const edge = encodeState({ ...snapshot, state: { player: 0, boxes: [] } });
  assert.ok([...edge].every(n => Number.isFinite(n) && n >= 0 && n <= 1.5));
});

test("decoder chooses majority, resolves ties reproducibly and respects silent fallback candidates", () => {
  assert.equal(decodeAction([1, 4, 2, 0]), "D");
  assert.equal(decodeAction([0, 0, 0, 0], mulberry32(8), ["R"]), "R");
  assert.equal(decodeAction([4, 4, 4, 4], mulberry32(8)), decodeAction([4, 4, 4, 4], mulberry32(8)));
});

test("LIF rests without current and records actual sampled spikes with seeded reproducibility", () => {
  const a = createFlyBrain(8), b = createFlyBrain(8);
  assert.deepEqual(a.weights, b.weights);
  for (let t = 0; t < 20; t++) {
    const { spikes } = stepBrain(a, new Float32Array(64));
    assert.equal(spikes.sensory + spikes.central + spikes.motor.reduce((x, y) => x + y, 0), 0);
    assert.deepEqual(spikes.samples, []);
  }
  b.voltage[0] = -49;
  assert.ok(stepBrain(b, new Float32Array(64)).spikes.samples.includes(0));
});

test("closed loop uses only C++ state requests, publishes live frames and revalidates full route", async t => {
  const requests = [], messages = [];
  t.mock.method(globalThis, "fetch", async (_url, options) => {
    const body = JSON.parse(options.body);
    requests.push(body);
    assert.equal(body.mode, "state"); assert.equal(body.algorithm, "astar-pushes");
    const accepted = body.direction !== "U";
    const moves = body.history.length + Number(accepted && !!body.direction);
    return Response.json({ ...snapshot, accepted, moves, pushes: Math.floor(moves / 3), won: moves >= 12 });
  });
  const result = await runSession({ levelId: "01-simple", base: "R", seed: 8 }, new AbortController().signal, m => messages.push(m));
  assert.equal(result.status, "Solved"); assert.equal(result.validated, true);
  assert.equal(requests.at(-1).direction, "");
  assert.equal(requests.at(-1).history, "R" + result.moves);
  assert.equal(result.moves.includes("U"), false);
  assert.ok(result.deadlocks > 0);
  assert.ok(messages.some(m => m.type === "progress" && m.result.flybrain.steps === 10));
  assert.equal(result.pushes, 4);
  assert.ok(result.flybrain.frames.filter(f => f.goals !== undefined).every(f => f.t % 10 === 0 || f.won));
});

test("rejected final validation never produces an applicable result", async t => {
  let calls = 0;
  t.mock.method(globalThis, "fetch", async (_url, options) => {
    const body = JSON.parse(options.body); calls++;
    return Response.json({ ...snapshot, accepted: calls < 3, won: !!body.direction });
  });
  const result = await runSession({ levelId: "01-simple", base: "", seed: 8 }, new AbortController().signal, () => {});
  assert.equal(result.status, "InternalError"); assert.equal(result.validated, false);
});

test("unfinished runs stop at 300 frames and retain only a validated partial route", async t => {
  t.mock.method(globalThis, "fetch", async () => Response.json(snapshot));
  const result = await runSession({ levelId: "01-simple", base: "", seed: 8 }, new AbortController().signal, () => {});
  assert.equal(result.status, "LimitReached");
  assert.equal(result.validated, true);
  assert.equal(result.moves.length, 300);
  assert.equal(result.flybrain.frames.length, 300);
  assert.equal(result.flybrain.loops, 300);
});

test("abort stops in-flight requests and never publishes a completed decision", async t => {
  const abort = new AbortController(), events = [];
  t.mock.method(globalThis, "fetch", async (_url, options) => new Promise((_, reject) => {
    options.signal.addEventListener("abort", () => reject(new DOMException("Aborted", "AbortError")), { once: true });
    queueMicrotask(() => abort.abort());
  }));
  await assert.rejects(runSession({ levelId: "01-simple", base: "", seed: 8 }, abort.signal, e => events.push(e)), { name: "AbortError" });
  assert.ok(events.every(e => e.type === "progress"));
});

test("sensory drive propagates into central and motor neurons without saturating the network", () => {
  const brain = createFlyBrain(8), total = [0, 0, 0];
  for (let i = 0; i < 100; i++) {
    const { spikes } = stepBrain(brain, new Float32Array(64).fill(1.2));
    total[0] += spikes.sensory; total[1] += spikes.central;
    total[2] += spikes.motor.reduce((a, b) => a + b, 0);
    assert.ok(spikes.active.every(id => Number.isInteger(id) && id >= 0 && id < 2004));
    assert.equal(spikes.active.length, new Set(spikes.active).size);
  }
  assert.ok(total.every(n => n > 0));
  assert.ok(total[1] < 1800 * 800 / 10, "central neurons must not settle at their refractory ceiling");
});

test("configuration bounds, custom duration, and step cap are honored", async t => {
  assert.deepEqual(brainConfig({ maxSteps: Infinity, windowMs: 0, maxWallMs: 999999, sensoryGain: -2 }),
    { maxSteps: 300, windowMs: 1, maxWallMs: 120000, sensoryGain: 1 });
  const a = createFlyBrain(8), b = createFlyBrain(8), currents = new Float32Array(64).fill(1.2);
  stepBrain(a, currents, brainConfig({ windowMs: 24 }));
  for (let i = 0; i < 3; i++) stepBrain(b, currents);
  assert.deepEqual(a.voltage, b.voltage);
  t.mock.method(globalThis, "fetch", async () => Response.json(snapshot));
  const result = await runSession({ levelId: "01-simple", base: "", seed: 8, config: { maxSteps: 7, windowMs: 24 } }, new AbortController().signal, () => {});
  assert.equal(result.flybrain.steps, 7); assert.equal(result.flybrain.config.windowMs, 24);
  assert.equal(result.status, "LimitReached"); assert.equal(result.validated, true);
  assert.equal(restoreFlyBrain(result.flybrain).config.windowMs, 24);
  assert.deepEqual(restoreFlyBrain(result.flybrain).frames[0].active, result.flybrain.frames[0].active);
});

test("storage sanitizes non-finite and malformed frames and drops recordings over 50 KiB", () => {
  const h = { kind: "flybrain", neurons: 2004, steps: 300, loops: 0, spikeRate: 1, seed: 8,
    frames: Array.from({ length: 300 }, (_, i) => ({ t: i + 1, dir: "R", accepted: true, won: false, sensory: 1, central: 1, motor: [1, 1, 1, 1], samples: Array.from({ length: 120 }, (_, n) => n) })) };
  assert.equal(restoreFlyBrain(h).frames.length, 300);
  assert.equal(compactFlyBrain(h).frames.length, 0);
  assert.equal(restoreFlyBrain({ ...h, loops: Infinity }), undefined);
  assert.equal(restoreFlyBrain({ ...h, frames: [{ ...h.frames[0], motor: [NaN, 0, 0, 0] }] }).frames.length, 0);
  assert.equal(restoreFlyBrain({ ...h, frames: [{ ...h.frames[0], samples: ["bad"] }] }).frames.length, 0);
});

test("biological enhancements: three-factor STDP, ray-casting, deadlock nociception, and CX heading", () => {
  const brain = createFlyBrain(8);
  brain.traces[0] = 0.8;
  const initialWeight = brain.weights[0];
  applyDopamine(brain, 1.0);
  assert.ok(brain.weights[0] > initialWeight, "positive dopamine must strengthen active synapses (LTP)");

  applyDopamine(brain, -2.0);
  assert.ok(brain.weights[0] < initialWeight + 0.05, "negative dopamine must depress active synapses (LTD)");

  const rays = computeRayCasting(snapshot);
  assert.equal(rays.length, 8);
  assert.ok(rays.every(r => Number.isFinite(r) && r >= 0 && r <= 1));

  const heading1 = updateCXHeading(brain, "R", "U");
  assert.ok(Number.isFinite(heading1));

  // Corner deadlock check on snapshot with player at (1, 3)=16 and box at (1, 2)=11:
  // Moving Up ('U') pushes box into open floor corner at (1, 1)=6 surrounded by walls at (0, 1) and (1, 0):
  const wallSnapshot = {
    ...snapshot,
    state: { player: 16, boxes: [11] },
    board: {
      ...snapshot.board,
      walls: Array.from({ length: 25 }, (_, i) => i % 5 === 0 || Math.floor(i / 5) === 0),
      goals: Array(25).fill(false),
    },
  };
  assert.equal(isDeadlockPush(wallSnapshot, "U"), true, "pushing box into walled corner must be detected as deadlock");
});
