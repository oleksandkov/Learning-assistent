import type { Dir, FlyBrainConfig, FlyBrainHistory, FlyBrainSpikeFrame, Snapshot } from "./types";

export const DIRECTIONS: Dir[] = ["U", "D", "L", "R"];
export const NEURONS = 2004;
export const MAX_STEPS = 300;
export const MAX_WALL_MS = 20000;
export const FRAME_CAP = 2000;
export const DEFAULT_BRAIN_CONFIG: FlyBrainConfig = { maxSteps: MAX_STEPS, windowMs: 8, maxWallMs: MAX_WALL_MS, sensoryGain: 8 };
export function brainConfig(value: Partial<FlyBrainConfig> = {}): FlyBrainConfig {
  const bounded = (n: unknown, fallback: number, min: number, max: number) =>
    typeof n === "number" && Number.isFinite(n) ? Math.max(min, Math.min(max, Math.round(n))) : fallback;
  const cfg: FlyBrainConfig = {
    maxSteps: bounded(value.maxSteps, MAX_STEPS, 1, FRAME_CAP),
    windowMs: bounded(value.windowMs, 8, 1, 100),
    maxWallMs: bounded(value.maxWallMs, MAX_WALL_MS, 1000, 120000),
    sensoryGain: bounded(value.sensoryGain, 8, 1, 20),
  };
  if (value.enablePlasticity !== undefined) cfg.enablePlasticity = Boolean(value.enablePlasticity);
  if (value.enableNociception !== undefined) cfg.enableNociception = Boolean(value.enableNociception);
  if (value.enableRays !== undefined) cfg.enableRays = Boolean(value.enableRays);
  return cfg;
}
export const CENTRAL = 64, MOTOR = 1844; // 1844 + 4*40 = 2004 symmetric neurons

export function mulberry32(seed: number) {
  return () => {
    let t = seed += 0x6D2B79F5;
    t = Math.imul(t ^ t >>> 15, t | 1);
    t ^= t + Math.imul(t ^ t >>> 7, t | 61);
    return ((t ^ t >>> 14) >>> 0) / 4294967296;
  };
}

export function createFlyBrain(seed: number) {
  const random = mulberry32(seed);
  const offsets = new Uint32Array(NEURONS + 1);
  const targets: number[] = [], weights: number[] = [];
  const centralCount = MOTOR - CENTRAL; // 1780
  for (let source = 0; source < NEURONS; source++) {
    offsets[source] = targets.length;
    const connect = (target: number) => {
      targets.push(target);
      weights.push((.2 + random() * .6) * (random() < .2 ? -1 : 1));
    };
    if (source < CENTRAL) {
      // Local targets in central neuropil / mushroom body; adjacent sensors overlap
      for (let j = 0; j < 54; j++) connect(CENTRAL + (Math.floor(source * centralCount / 64) + j) % centralCount);
    } else if (source < MOTOR) {
      for (let target = CENTRAL; target < NEURONS; target++) {
        if (target !== source && random() < .03) connect(target);
      }
    }
  }
  offsets[NEURONS] = targets.length;
  const sampleIndex = new Int16Array(NEURONS).fill(-1);
  for (let i = 0; i < 40; i++) {
    sampleIndex[Math.floor(i * 64 / 40)] = i;
    sampleIndex[CENTRAL + Math.floor(i * centralCount / 40)] = 40 + i;
    sampleIndex[MOTOR + i * 4] = 80 + i;
  }
  return {
    random, offsets, targets: Uint16Array.from(targets), weights: Float32Array.from(weights),
    voltage: new Float32Array(NEURONS).fill(-70), refractory: new Uint8Array(NEURONS),
    input: new Float32Array(NEURONS), sampleIndex, bias: new Float32Array(4),
    weakened: new Set<Dir>(),
    traces: new Float32Array(targets.length), // Mushroom Body eligibility traces for 3-factor STDP
    heading: 0, // Central Complex (CX) internal compass angle (-PI..PI)
    dopamine: 0, // Current neuromodulator level
  };
}
export type FlyBrain = ReturnType<typeof createFlyBrain>;

/** 8-ray ommatidia vision: cast rays in N, NE, E, SE, S, SW, W, NW from player */
export function computeRayCasting({ board, state }: Snapshot): number[] {
  const px = state.player % board.width, py = Math.floor(state.player / board.width);
  const boxes = new Set(state.boxes);
  const rayDirs: [number, number][] = [
    [0, -1], [1, -1], [1, 0], [1, 1],
    [0, 1], [-1, 1], [-1, 0], [-1, -1],
  ];
  const maxDim = Math.max(board.width, board.height);
  return rayDirs.map(([dx, dy]) => {
    let step = 1;
    while (step < maxDim) {
      const x = px + dx * step, y = py + dy * step;
      if (x < 0 || y < 0 || x >= board.width || y >= board.height) break;
      const cell = y * board.width + x;
      if (board.walls[cell] || !board.floor[cell] || boxes.has(cell)) break;
      step++;
    }
    return Math.min(1, step / maxDim);
  });
}

/** Check if pushing a box in direction `dir` creates an unrecoverable static deadlock */
export function isDeadlockPush({ board, state }: Snapshot, dir: Dir): boolean {
  const px = state.player % board.width, py = Math.floor(state.player / board.width);
  const dx = dir === "L" ? -1 : dir === "R" ? 1 : 0;
  const dy = dir === "U" ? -1 : dir === "D" ? 1 : 0;
  const boxX = px + dx, boxY = py + dy, boxCell = boxY * board.width + boxX;
  const boxes = new Set(state.boxes);
  if (!boxes.has(boxCell)) return false; // Not a push move
  const targetX = boxX + dx, targetY = boxY + dy, targetCell = targetY * board.width + targetX;
  if (targetX < 0 || targetY < 0 || targetX >= board.width || targetY >= board.height) return true;
  if (board.walls[targetCell] || !board.floor[targetCell] || boxes.has(targetCell)) return false; // Blocked, not deadlock
  if (board.goals[targetCell]) return false; // Placed on goal!

  const isWall = (x: number, y: number) =>
    x < 0 || y < 0 || x >= board.width || y >= board.height || board.walls[y * board.width + x] || !board.floor[y * board.width + x];

  // Corner deadlock check: wall in vertical and horizontal orthogonal sides
  const upWall = isWall(targetX, targetY - 1), downWall = isWall(targetX, targetY + 1);
  const leftWall = isWall(targetX - 1, targetY), rightWall = isWall(targetX + 1, targetY);
  if ((upWall && leftWall) || (upWall && rightWall) || (downWall && leftWall) || (downWall && rightWall)) {
    return true; // Corner deadlock
  }

  // 2x2 deadlock check: box pushed into 2x2 cluster of walls or non-goal boxes
  const offsets2x2 = [
    [0, 0, 1, 0, 0, 1, 1, 1],
    [-1, 0, 0, 0, -1, 1, 0, 1],
    [0, -1, 1, -1, 0, 0, 1, 0],
    [-1, -1, 0, -1, -1, 0, 0, 0],
  ];
  for (const [x1, y1, x2, y2, x3, y3, x4, y4] of offsets2x2) {
    const coords = [[targetX + x1, targetY + y1], [targetX + x2, targetY + y2], [targetX + x3, targetY + y3], [targetX + x4, targetY + y4]];
    let allImpassable = true;
    let anyGoalInCluster = false;
    for (const [cx, cy] of coords) {
      if (cx < 0 || cy < 0 || cx >= board.width || cy >= board.height) { continue; }
      const c = cy * board.width + cx;
      const wall = board.walls[c] || !board.floor[c];
      const box = (c === targetCell) || (c !== boxCell && boxes.has(c));
      if (!wall && !box) { allImpassable = false; break; }
      if (box && board.goals[c]) anyGoalInCluster = true;
    }
    if (allImpassable && !anyGoalInCluster) return true;
  }
  return false;
}

/** Update internal Central Complex (CX) compass orientation */
export function updateCXHeading(brain: FlyBrain, chosen: Dir, targetDir: Dir): number {
  const dirAngles: Record<Dir, number> = { U: -Math.PI / 2, D: Math.PI / 2, L: Math.PI, R: 0 };
  const targetAngle = dirAngles[targetDir];
  let diff = targetAngle - brain.heading;
  while (diff > Math.PI) diff -= 2 * Math.PI;
  while (diff < -Math.PI) diff += 2 * Math.PI;
  brain.heading += diff * 0.35;
  return brain.heading;
}

/** Three-Factor STDP: modulates synapses via eligibility traces and dopamine (DA) */
export function applyDopamine(brain: FlyBrain, dopamine: number, learningRate = 0.04) {
  brain.dopamine = dopamine;
  if (!dopamine) return;
  for (let j = 0; j < brain.weights.length; j++) {
    const trace = brain.traces[j];
    if (trace > 0.02) {
      const delta = learningRate * dopamine * trace;
      brain.weights[j] = Math.max(-1.5, Math.min(1.5, brain.weights[j] + delta));
    }
  }
}

export function encodeState({ board, state }: Snapshot, _history = ""): Float32Array {
  const features = new Float32Array(125), currents = new Float32Array(64);
  const counts = new Uint8Array(64), boxes = new Set(state.boxes);
  const px = state.player % board.width, py = Math.floor(state.player / board.width);
  let offset = 0;
  for (let dy = -2; dy <= 2; dy++) for (let dx = -2; dx <= 2; dx++) {
    const x = px + dx, y = py + dy, cell = y * board.width + x;
    const wall = x < 0 || y < 0 || x >= board.width || y >= board.height || board.walls[cell] || !board.floor[cell];
    features[offset++] = Number(wall);
    features[offset++] = Number(!wall && !boxes.has(cell));
    features[offset++] = Number(!wall && boxes.has(cell));
    features[offset++] = Number(!wall && board.goals[cell]);
    features[offset++] = Number(cell === state.player && board.goals[cell]);
  }
  for (let i = 0; i < features.length; i++) {
    const bin = Math.floor(i * 64 / 125);
    currents[bin] += features[i]; counts[bin]++;
  }
  let nearest = -1, distance = Infinity;
  board.goals.forEach((goal, cell) => {
    const d = Math.abs(cell % board.width - px) + Math.abs(Math.floor(cell / board.width) - py);
    if (goal && d < distance) { nearest = cell; distance = d; }
  });
  const biases = nearest < 0 ? [0, 0, 0, 0] : [
    Number(Math.floor(nearest / board.width) < py), Number(Math.floor(nearest / board.width) > py),
    Number(nearest % board.width < px), Number(nearest % board.width > px),
  ];
  for (let i = 0; i < 64; i++) currents[i] = Math.min(1.5, currents[i] / counts[i] + biases[i % 4] * .5);
  return currents;
}

export function stepBrain(brain: FlyBrain, currents: Float32Array, config = DEFAULT_BRAIN_CONFIG) {
  const spikes = { sensory: 0, central: 0, motor: [0, 0, 0, 0] as FlyBrainSpikeFrame["motor"], samples: [] as number[], active: [] as number[] };
  const sampled = new Set<number>();
  const active = new Set<number>();
  for (let ms = 0; ms < config.windowMs; ms++) {
    const fired: number[] = [];
    for (let i = 0; i < NEURONS; i++) {
      const synaptic = brain.input[i];
      // Synaptic current persists (~5 ms) instead of disappearing after one tick.
      brain.input[i] *= .82;
      if (brain.refractory[i]) { brain.refractory[i]--; continue; }
      const noise = Math.sqrt(-2 * Math.log(Math.max(Number.EPSILON, brain.random()))) * Math.cos(2 * Math.PI * brain.random()) * .5;
      const motor = Math.floor((i - MOTOR) / 40);
      const current = synaptic + (i < CENTRAL ? currents[i] : i >= MOTOR ? brain.bias[motor] : 0);
      brain.voltage[i] += -(brain.voltage[i] + 70) / 20 + current + noise;
      if (brain.voltage[i] < -50) continue;
      brain.voltage[i] = -65; brain.refractory[i] = 2; fired.push(i);
      active.add(i);
      if (i < CENTRAL) spikes.sensory++;
      else if (i < MOTOR) spikes.central++;
      else spikes.motor[motor]++;
      if (brain.sampleIndex[i] >= 0) sampled.add(brain.sampleIndex[i]);
    }
    for (const source of fired) {
      for (let j = brain.offsets[source]; j < brain.offsets[source + 1]; j++) {
        const gain = source < CENTRAL ? config.sensoryGain : brain.targets[j] < MOTOR ? .08 : .5;
        brain.input[brain.targets[j]] += brain.weights[j] * gain;
        // Mushroom Body STDP: register synaptic activity in eligibility trace
        brain.traces[j] = Math.min(1.0, brain.traces[j] + 0.4);
      }
    }
    // Trace decay
    for (let j = 0; j < brain.traces.length; j += 7) brain.traces[j] *= .95;
    for (let i = 0; i < 4; i++) brain.bias[i] *= .97;
  }
  spikes.samples = Array.from(sampled).sort((a, b) => a - b);
  spikes.active = Array.from(active).sort((a, b) => a - b);
  return { spikes };
}

export function decodeAction(motor: readonly number[], random = mulberry32(0), allowed: Dir[] = DIRECTIONS): Dir {
  const max = Math.max(...motor);
  const candidates = max === 0 ? allowed : DIRECTIONS.filter((_, i) => motor[i] === max);
  return candidates[Math.floor(random() * candidates.length)] ?? "U";
}

export function weakenMotor(brain: FlyBrain, dir: Dir) {
  if (brain.weakened.has(dir)) return;
  brain.weakened.add(dir);
  const start = MOTOR + DIRECTIONS.indexOf(dir) * 40;
  for (let source = CENTRAL; source < MOTOR; source++) {
    for (let j = brain.offsets[source]; j < brain.offsets[source + 1]; j++) {
      if (brain.targets[j] >= start && brain.targets[j] < start + 40) brain.weights[j] *= .95;
    }
  }
}

// Treat session storage as untrusted; reconstruct only bounded, numeric history.
export function restoreFlyBrain(value: unknown): FlyBrainHistory | undefined {
  if (!value || typeof value !== "object") return;
  const h = value as FlyBrainHistory;
  const finite = (n: unknown, max = Number.MAX_SAFE_INTEGER): n is number => typeof n === "number" && Number.isFinite(n) && n >= 0 && n <= max;
  if (h.kind !== "flybrain" || h.neurons !== NEURONS || !finite(h.steps, FRAME_CAP) ||
      !finite(h.loops, FRAME_CAP) || !finite(h.seed, 0xffffffff) || !finite(h.spikeRate, 100000) || !Array.isArray(h.frames)) return;
  const frames = h.frames.slice(0, FRAME_CAP).filter(f => f && finite(f.t, FRAME_CAP) &&
    Number.isInteger(f.t) && DIRECTIONS.includes(f.dir) && typeof f.accepted === "boolean" && typeof f.won === "boolean" &&
    finite(f.sensory, 6400) && finite(f.central, 180000) && Array.isArray(f.motor) && f.motor.length === 4 &&
    f.motor.every(n => finite(n, 4000)) && Array.isArray(f.samples) && f.samples.length <= 120 &&
    (f.active === undefined || Array.isArray(f.active) && f.active.length <= NEURONS && f.active.every(n => finite(n, NEURONS - 1) && Number.isInteger(n))) &&
    f.samples.every(n => finite(n, 119) && Number.isInteger(n)) && (f.goals === undefined || finite(f.goals, 1)))
    .map(f => ({
      t: f.t, dir: f.dir, accepted: f.accepted, won: f.won, sensory: f.sensory,
      central: f.central, motor: [...f.motor] as FlyBrainSpikeFrame["motor"], samples: [...f.samples], active: f.active ? [...f.active] : undefined, goals: f.goals,
      dopamine: typeof f.dopamine === "number" && Number.isFinite(f.dopamine) ? f.dopamine : undefined,
      heading: typeof f.heading === "number" && Number.isFinite(f.heading) ? f.heading : undefined,
      compassDir: typeof f.compassDir === "string" && DIRECTIONS.includes(f.compassDir as Dir) ? (f.compassDir as Dir) : undefined,
      deadlockAverted: typeof f.deadlockAverted === "boolean" ? f.deadlockAverted : undefined,
      rays: Array.isArray(f.rays) && f.rays.length === 8 && f.rays.every(r => typeof r === "number" && Number.isFinite(r)) ? f.rays : undefined,
      ghostTrace: typeof f.ghostTrace === "string" ? f.ghostTrace : undefined,
      macroIntent: typeof f.macroIntent === "string" ? f.macroIntent : undefined,
    }));
  return { kind: "flybrain", neurons: NEURONS, steps: h.steps, loops: h.loops, seed: h.seed, spikeRate: h.spikeRate,
    config: h.config ? brainConfig(h.config) : undefined, attempt: finite(h.attempt) ? h.attempt : 1,
    frames: frames.length === h.frames.length && frames.every((f, i) => f.t === i + 1) ? frames : [] };
}

export function compactFlyBrain(history: FlyBrainHistory): FlyBrainHistory {
  return new TextEncoder().encode(JSON.stringify(history)).length > 50 * 1024 ? { ...history, frames: [] } : history;
}
