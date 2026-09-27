import { test } from "node:test";
import assert from "node:assert/strict";
import { readFileSync } from "node:fs";
import ts from "typescript";

// Use the installed compiler so these checks work on the project's Node 18+ baseline.
const compile = source => `data:text/javascript;base64,${Buffer.from(ts.transpileModule(source, {
  compilerOptions: { target: ts.ScriptTarget.ES2022, module: ts.ModuleKind.ESNext },
}).outputText).toString("base64")}`;
const brainUrl = compile(readFileSync("src/lib/flybrain.ts", "utf8"));
const { encodeState, decodeAction, createFlyBrain, stepBrain, restoreFlyBrain, compactFlyBrain, mulberry32, brainConfig, applyDopamine, computeRayCasting, isDeadlockPush, updateCXHeading, updatePlasticity, applyOctopamine, saveBrainMemory, restoreBrainMemory, decodeBehavior, readTargetDirection, CENTRAL, MBON, CX, FB, PB, MOTOR, NEURONS } = await import(brainUrl);
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
  b.voltage[0] = -40; // Safely above threshold even after this tick’s leak and seeded noise.
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
  assert.equal(result.moves.length, result.flybrain.frames.filter(f => f.accepted).length);
  assert.ok(result.flybrain.frames.some(f => f.behavior === "PAUSE"));
  assert.equal(result.flybrain.frames.length, 300);
  assert.equal(result.flybrain.loops, result.moves.length);
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
  const edge = brain.plastic[0];
  brain.traces[edge] = 0.8;
  const initialWeight = brain.weights[edge];
  applyDopamine(brain, 1.0);
  assert.ok(brain.weights[edge] > initialWeight, "positive dopamine must strengthen active synapses (LTP)");

  applyDopamine(brain, -2.0);
  assert.ok(brain.weights[edge] < initialWeight + 0.05, "negative dopamine must depress active synapses (LTD)");

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


test("retinal channels see walls outside the 5×5 patch and disabled rays are silent", () => {
  const open = { ...snapshot, board: { width: 15, height: 15, walls: Array(225).fill(false), floor: Array(225).fill(true), goals: Array(225).fill(false) }, state: { player: 112, boxes: [] } };
  const blocked = { ...open, board: { ...open.board, walls: open.board.walls.map((v, i) => i === 52 || v) } };
  assert.deepEqual(encodeState(open).slice(0, 48), encodeState(blocked).slice(0, 48));
  assert.notEqual(encodeState(open)[48], encodeState(blocked)[48]);
  assert.ok(encodeState(open, "", Array(8).fill(0)).slice(48, 56).every(v => v === 0));
});

test("CX ring rotates four sectors for 90 degrees and remains normalized at wraparound", () => {
  const brain = createFlyBrain(8);
  const peak = () => brain.ring.indexOf(Math.max(...brain.ring));
  assert.equal(peak(), 0);
  updateCXHeading(brain, "D"); assert.equal(peak(), 4);
  updateCXHeading(brain, "L"); assert.equal(peak(), 8);
  updateCXHeading(brain, "U"); assert.equal(peak(), 12);
  updateCXHeading(brain, "R"); assert.equal(peak(), 0);
  assert.ok(Math.abs(brain.ring.reduce((a,b) => a+b, 0) - 1) < 1e-6);
  assert.equal(brain.ring.length, 16);
});

test("pair STDP changes sign with spike order, ignores simultaneous pairs and confines DA to MB", () => {
  for (const causal of [true, false]) {
    const brain = createFlyBrain(8), edge = brain.plastic[0];
    const pre = brain.plasticSources[0], post = brain.targets[edge];
    brain.time = 1; updatePlasticity(brain, new Set([causal ? pre : post]));
    brain.time = 10; updatePlasticity(brain, new Set([causal ? post : pre]));
    const before = brain.weights.slice(); applyDopamine(brain, 1);
    assert.ok(causal ? brain.weights[edge] > before[edge] : brain.weights[edge] < before[edge]);
    const plastic = new Set(brain.plastic);
    before.forEach((w,j) => { if (!plastic.has(j)) assert.equal(brain.weights[j], w); });
  }
  const brain = createFlyBrain(8), edge = brain.plastic[0];
  brain.time = 1; updatePlasticity(brain, new Set([brain.plasticSources[0], brain.targets[edge]]));
  assert.equal(brain.traces[edge], 0);
  brain.time = 30; updatePlasticity(brain, new Set([brain.targets[edge]]));
  assert.equal(brain.traces[edge], 0);
});

test("learned memory survives a new seed and rejects incompatible or corrupted storage", () => {
  const brain = createFlyBrain(8); brain.traces[brain.plastic[0]] = 1; applyDopamine(brain, 1);
  const memory = saveBrainMemory(brain), restored = restoreBrainMemory(JSON.parse(JSON.stringify(memory)));
  const next = createFlyBrain(9, restored);
  brain.plastic.forEach((j,i) => assert.ok(Math.abs(next.weights[j] - memory.weights[i] / 1000) < 1e-6));
  assert.equal(next.memorySessions, 1);
  assert.ok(Buffer.byteLength(JSON.stringify(memory)) < 50 * 1024);
  assert.equal(restoreBrainMemory({ ...memory, version: 2 }), undefined);
  assert.equal(restoreBrainMemory({ ...memory, weights: [NaN] }), undefined);
  assert.equal(restoreBrainMemory({ ...memory, sessions: -1 }), undefined);
  assert.equal(MOTOR, NEURONS - 160); assert.equal(CENTRAL, 64);
});

test("effort, behavior and target direction read independent network state", () => {
  const brain = createFlyBrain(8);
  applyDopamine(brain, 2); applyOctopamine(brain, .8);
  assert.equal(brain.dopamine, 2); assert.equal(brain.octopamine, .8);
  brain.targetDrive.set([0,0,0,1]); assert.equal(readTargetDirection(brain), "R");
  brain.searchTicks = 3; assert.equal(decodeBehavior(brain, [0,0,0,0], false), "PAUSE");
  assert.equal(decodeBehavior(brain, [0,0,0,0], false), "SEARCH");
  brain.octopamine = 0;
  assert.equal(decodeBehavior(brain, [0,0,0,3], true), "PUSH");
  assert.equal(decodeBehavior(brain, [0,0,0,3], false), "MOVE");
});

test("PAUSE performs no game command and learned memory travels through worker completion", async t => {
  const requests = [], events = [];
  t.mock.method(globalThis, "fetch", async (_url, options) => { requests.push(JSON.parse(options.body)); return Response.json(snapshot); });
  const result = await runSession({ levelId: "02-microban", base: "", seed: 8, config: { maxSteps: 30 } }, new AbortController().signal, e => events.push(e));
  const pauses = result.flybrain.frames.filter(f => f.behavior === "PAUSE");
  assert.ok(pauses.length > 0); assert.ok(pauses.every(f => !f.accepted));
  assert.equal(requests.filter(r => r.direction).length, result.generated);
  assert.equal(result.generated, result.flybrain.frames.length - pauses.length);
  const memory = events.at(-1).memory; assert.ok(restoreBrainMemory(memory));
  const next = await runSession({ levelId: "different-level", base: "", seed: 9, memory, config: { maxSteps: 1 } }, new AbortController().signal, () => {});
  assert.equal(next.flybrain.memorySessions, 1);
});


test("natural sensory input drives MB timing and changes reusable memory without synthetic traces", () => {
  const brain = createFlyBrain(8), before = saveBrainMemory(brain);
  let mbon = 0;
  for (let t = 0; t < 120; t++) {
    const { spikes } = stepBrain(brain, encodeState(snapshot));
    mbon += spikes.active.filter(id => id >= MBON && id < CX).length;
    applyDopamine(brain, .2);
  }
  assert.ok(mbon > 0, "sensory experience must actually reach memory output cells");
  assert.notDeepEqual(saveBrainMemory(brain).weights, before.weights);
  for (let t = 0; t < 2000; t++) updateCXHeading(brain, "R");
  assert.ok(Math.max(...brain.ring) > .2, "the compass bump must survive a long run");
});


test("active motor pools cannot repeatedly select an exit already rejected in this state", async t => {
  assert.equal(decodeAction([100, 2, 1, 0], mulberry32(8), ["D", "L", "R"]), "D");
  const requests = [];
  t.mock.method(globalThis, "fetch", async (_url, options) => {
    const body = JSON.parse(options.body); requests.push(body);
    return Response.json({ ...snapshot, accepted: !body.direction });
  });
  const result = await runSession({ levelId: "trapped", base: "", seed: 8, config: { maxSteps: 30, windowMs: 100 } }, new AbortController().signal, () => {});
  const attempted = requests.filter(r => r.direction).map(r => r.direction);
  assert.equal(attempted.length, 4); assert.equal(new Set(attempted).size, 4);
  assert.equal(result.moves, ""); assert.equal(result.validated, true);
  assert.equal(result.deadlocks, 4);
});


test("remembered blocked exits cannot force a corner push past nociception", async t => {
  const floor = [6,7,8,12,13,17,18];
  const trapped = { ...snapshot, board: { width: 5, height: 5, floor: Array.from({length:25},(_,i)=>floor.includes(i)), walls: Array.from({length:25},(_,i)=>!floor.includes(i)), goals: Array.from({length:25},(_,i)=>i===17) }, state: { player: 6, boxes: [7] } };
  assert.equal(isDeadlockPush(trapped, "R"), true);
  const directions = [];
  t.mock.method(globalThis, "fetch", async (_url, options) => {
    const body = JSON.parse(options.body);
    if (body.direction) directions.push(body.direction);
    return Response.json({ ...trapped, accepted: !body.direction || body.direction === "R" });
  });
  const result = await runSession({ levelId: "corner", base: "", seed: 8, config: {maxSteps:30} }, new AbortController().signal, () => {});
  assert.equal(directions.includes("R"), false);
  assert.equal(new Set(directions).size, 3);
  assert.equal(result.moves, ""); assert.equal(result.validated, true);
});
