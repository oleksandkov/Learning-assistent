import type { Dir, FlyBrainConfig, FlyBrainHistory, FlyBrainMemory, FlyBrainSpikeFrame, Snapshot } from "./types";

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

// Fixed anatomical seed: learned synapses keep the same meaning across run seeds.
export const MBON = 1664, CX = 1680, FB = 1696, PB = 1760;
export const MEMORY_KEY = "sokoban.flybrain.memory.v1";
const MEMORY_WEIGHTS = (MBON - CENTRAL) * 4;
export function restoreBrainMemory(value: unknown): FlyBrainMemory | undefined {
  if (!value || typeof value !== "object") return;
  const m = value as FlyBrainMemory;
  if (m.version !== 1 || !Number.isSafeInteger(m.sessions) || m.sessions < 0 || m.sessions > 1000000 ||
      !Array.isArray(m.weights) || m.weights.length !== MEMORY_WEIGHTS ||
      !m.weights.every(w => Number.isInteger(w) && w >= 20 && w <= 1500)) return;
  return { version: 1, sessions: m.sessions, weights: [...m.weights] };
}

export function createFlyBrain(seed: number, memory?: FlyBrainMemory) {
  const random = mulberry32(0xF17B), offsets = new Uint32Array(NEURONS + 1);
  const targets: number[] = [], weights: number[] = [], plastic: number[] = [], sources: number[] = [];
  for (let source = 0; source < NEURONS; source++) {
    offsets[source] = targets.length;
    const connect = (target: number, weight = .2 + random() * .6, learn = false) => {
      if (learn) { plastic.push(targets.length); sources.push(source); }
      targets.push(target); weights.push(weight);
    };
    if (source < CENTRAL) {
      // Sparse expansion into Kenyon cells; fixed projection preserves sensory context.
      for (let j = 0; j < 150; j++) connect(CENTRAL + Math.floor(random() * (MBON - CENTRAL)));
    } else if (source < MBON) {
      for (let d = 0; d < 4; d++) connect(MBON + d * 4 + (source % 4), .2 + random() * .6, true);
    } else if (source < CX) {
      const d = Math.floor((source - MBON) / 4);
      for (let j = 0; j < 21; j++) connect(PB + d * 21 + j);
    } else if (source < FB) {
      // Local excitation + distant inhibition in the compass circuit.
      for (let j = 0; j < 16; j++) if (j !== source - CX) {
        const distance = Math.min((j - source + CX + 16) % 16, (source - CX - j + 16) % 16);
        connect(CX + j, distance <= 2 ? .5 : -.12);
      }
    } else if (source < PB) {
      const d = Math.floor((source - FB) / 16);
      for (let j = 0; j < 21; j++) connect(PB + d * 21 + j);
    } else if (source < MOTOR) {
      const d = Math.floor((source - PB) / 21);
      for (let j = 0; j < 40; j++) connect(MOTOR + d * 40 + j, .55);
      for (let other = 0; other < 4; other++) if (other !== d) connect(PB + other * 21 + source % 21, -.3);
    }
  }
  offsets[NEURONS] = targets.length;
  const sampleIndex = new Int16Array(NEURONS).fill(-1);
  for (let i = 0; i < 40; i++) {
    sampleIndex[Math.floor(i * 64 / 40)] = i;
    sampleIndex[CENTRAL + Math.floor(i * (MOTOR - CENTRAL) / 40)] = 40 + i;
    sampleIndex[MOTOR + i * 4] = 80 + i;
  }
  const saved = restoreBrainMemory(memory);
  if (saved) plastic.forEach((edge, i) => { weights[edge] = saved.weights[i] / 1000; });
  const ring = Float32Array.from({ length: 16 }, (_, i) => Math.exp(5 * (Math.cos(i * Math.PI / 8) - 1)));
  const sum = ring.reduce((a, b) => a + b, 0); ring.forEach((v, i) => { ring[i] = v / sum; });
  return {
    random: mulberry32(seed), offsets, targets: Uint16Array.from(targets), weights: Float32Array.from(weights),
    plastic: Uint32Array.from(plastic), plasticSources: Uint16Array.from(sources),
    voltage: new Float32Array(NEURONS).fill(-70), refractory: new Uint8Array(NEURONS),
    input: new Float32Array(NEURONS), sampleIndex, bias: new Float32Array(4),
    traces: new Float32Array(targets.length), lastSpike: new Float64Array(NEURONS).fill(-Infinity), time: 0,
    ring, heading: 0, lastDirection: "R" as Dir, targetDrive: new Float32Array(4),
    dopamine: 0, octopamine: 0, searchTicks: 0, memorySessions: saved?.sessions ?? 0,
  };
}
export type FlyBrain = ReturnType<typeof createFlyBrain>;

export function saveBrainMemory(brain: FlyBrain): FlyBrainMemory {
  return { version: 1, sessions: Math.min(1000000, brain.memorySessions + 1),
    weights: Array.from(brain.plastic, j => Math.round(brain.weights[j] * 1000)) };
}

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

const dirAngles: Record<Dir, number> = { U: -Math.PI / 2, D: Math.PI / 2, L: Math.PI, R: 0 };
/** A normalized rate ring coupled to the 16 CX LIF cells; rotate only on accepted movement. */
export function updateCXHeading(brain: FlyBrain, chosen: Dir, _targetDir?: Dir): number {
  const shift = Math.round((dirAngles[chosen] - dirAngles[brain.lastDirection]) / (Math.PI / 8));
  const previous = brain.ring.slice();
  for (let i = 0; i < 16; i++) {
    const j = (i - shift + 32) % 16;
    brain.ring[i] = (.8 * previous[j] + .1 * previous[(j + 15) % 16] + .1 * previous[(j + 1) % 16]) ** 1.1;
  }
  const total = brain.ring.reduce((a, b) => a + b, 0);
  brain.ring.forEach((v, i) => { brain.ring[i] = v / total; });
  brain.lastDirection = chosen;
  brain.heading = Math.atan2(brain.ring.reduce((v, a, i) => v + a * Math.sin(i * Math.PI / 8), 0),
    brain.ring.reduce((v, a, i) => v + a * Math.cos(i * Math.PI / 8), 0));
  return brain.heading;
}

/** Pair timing is measured in model milliseconds, before updating this tick's timestamps. */
export function updatePlasticity(brain: FlyBrain, fired: ReadonlySet<number>) {
  for (let k = 0; k < brain.plastic.length; k++) {
    const j = brain.plastic[k], pre = brain.plasticSources[k], post = brain.targets[j];
    let trace = brain.traces[j] * Math.exp(-1 / 200);
    const preAge = brain.time - brain.lastSpike[pre], postAge = brain.time - brain.lastSpike[post];
    if (fired.has(post) && preAge > 0 && preAge < 20) trace += Math.exp(-preAge / 20);
    if (fired.has(pre) && postAge > 0 && postAge < 20) trace -= Math.exp(-postAge / 20);
    brain.traces[j] = Math.max(-2, Math.min(2, trace));
  }
  fired.forEach(id => { brain.lastSpike[id] = brain.time; });
}

export function applyDopamine(brain: FlyBrain, dopamine: number, learningRate = .04) {
  brain.dopamine = dopamine;
  brain.plastic.forEach(j => {
    brain.weights[j] = Math.max(.02, Math.min(1.5, brain.weights[j] + learningRate * dopamine * brain.traces[j]));
  });
}

export function applyOctopamine(brain: FlyBrain, effort: number) {
  brain.octopamine = Math.max(0, Math.min(1, brain.octopamine * .8 + effort));
}

/** Intention reads the recurrent network, never a nearest-box rule in the worker. */
export function readTargetDirection(brain: FlyBrain): Dir {
  return decodeAction(Array.from(brain.targetDrive), brain.random);
}

export function decodeBehavior(brain: FlyBrain, motor: readonly number[], pushing: boolean): NonNullable<FlyBrainSpikeFrame["behavior"]> {
  if (brain.octopamine > .65 && brain.searchTicks >= 3) { brain.searchTicks = 0; return "PAUSE"; }
  if (motor.every(n => n === 0) || brain.octopamine > .5) { brain.searchTicks++; return "SEARCH"; }
  brain.searchTicks = 0;
  return pushing ? "PUSH" : "MOVE";
}

export function encodeState(snapshot: Snapshot, _history = "", rays = computeRayCasting(snapshot)): Float32Array {
  const { board, state } = snapshot;
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
  for (let i = 0; i < 64; i++) currents[i] /= counts[i];
  // Reserve eight genuine retinal channels, plus population-coded box and goal bearings.
  for (let i = 0; i < 8; i++) currents[48 + i] = Math.max(0, Math.min(1.5, (rays[i] ?? 0) * 1.5));
  currents.fill(0, 56);
  const bearing = (cell: number, start: number, scale: number) => {
    const dx = cell % board.width - px, dy = Math.floor(cell / board.width) - py;
    const distance = Math.abs(dx) + Math.abs(dy);
    if (!distance) return;
    const values = [Math.max(0, -dy), Math.max(0, dy), Math.max(0, -dx), Math.max(0, dx)];
    values.forEach((v, d) => { currents[start + d] = Math.min(1.5, currents[start + d] + scale * v / distance / Math.sqrt(distance)); });
  };
  state.boxes.forEach(cell => { if (!board.goals[cell]) bearing(cell, 56, 1.5); });
  board.goals.forEach((goal, cell) => { if (goal && !boxes.has(cell)) bearing(cell, 60, 1.0); });
  return currents;
}

export function stepBrain(brain: FlyBrain, currents: Float32Array, config = DEFAULT_BRAIN_CONFIG) {
  const spikes = { sensory: 0, central: 0, motor: [0, 0, 0, 0] as FlyBrainSpikeFrame["motor"], samples: [] as number[], active: [] as number[], zones: [0, 0, 0, 0] as [number, number, number, number] };
  const sensoryActive = currents.some(n => n > 0);
  const sampled = new Set<number>();
  const active = new Set<number>();
  for (let ms = 0; ms < config.windowMs; ms++) {
    brain.time++;
    const fired: number[] = [];
    for (let i = 0; i < NEURONS; i++) {
      const synaptic = brain.input[i];
      // Synaptic current persists (~5 ms) instead of disappearing after one tick.
      brain.input[i] *= .82;
      if (brain.refractory[i]) { brain.refractory[i]--; continue; }
      const noise = Math.sqrt(-2 * Math.log(Math.max(Number.EPSILON, brain.random()))) * Math.cos(2 * Math.PI * brain.random()) * .5;
      const motor = Math.floor((i - MOTOR) / 40);
      let drive = i < CENTRAL ? currents[i] : i >= MOTOR ? brain.bias[motor] : 0;
      if (sensoryActive && i >= CX && i < FB) drive += brain.ring[i - CX] * 12;
      if (sensoryActive && i >= FB && i < PB) {
        const d = Math.floor((i - FB) / 16);
        // Parallel goal/box population channels compete through PB inhibition.
        drive += (currents[56 + d] * 1.5 + currents[60 + d] * .6) * (1 + brain.octopamine * .35);
        drive += Math.max(0, Math.cos(dirAngles[DIRECTIONS[d]] - brain.heading)) * .25;
      }
      if (sensoryActive && i >= MOTOR) drive += brain.targetDrive[motor] * .3;
      const current = synaptic + drive;
      brain.voltage[i] += -(brain.voltage[i] + 70) / 20 + current + noise;
      if (brain.voltage[i] < -50) continue;
      brain.voltage[i] = -65; brain.refractory[i] = 2; fired.push(i);
      active.add(i);
      if (i < CENTRAL) spikes.sensory++;
      else if (i < MOTOR) {
        spikes.central++;
        spikes.zones[i < CX ? 0 : i < FB ? 1 : i < PB ? 2 : 3]++;
      }
      else spikes.motor[motor]++;
      if (brain.sampleIndex[i] >= 0) sampled.add(brain.sampleIndex[i]);
    }
    for (const source of fired) {
      for (let j = brain.offsets[source]; j < brain.offsets[source + 1]; j++) {
        const gain = source < CENTRAL ? config.sensoryGain * .5 : source < MBON ? 1 : .5;
        brain.input[brain.targets[j]] += brain.weights[j] * gain;

      }
    }
    updatePlasticity(brain, new Set(fired));
    for (let d = 0; d < 4; d++) {
      let drive = 0;
      for (let i = FB + d * 16; i < FB + (d + 1) * 16; i++) drive += Math.max(0, (brain.voltage[i] + 70) / 20);
      brain.targetDrive[d] = .9 * brain.targetDrive[d] + .1 * drive / 16;
    }
    for (let i = 0; i < 4; i++) brain.bias[i] *= .97;
  }
  spikes.samples = Array.from(sampled).sort((a, b) => a - b);
  spikes.active = Array.from(active).sort((a, b) => a - b);
  return { spikes };
}

/** temperature > 0 → softmax over pool spikes (explore when close); 0 → argmax with seeded tie-break. */
export function decodeAction(motor: readonly number[], random = mulberry32(0), allowed: Dir[] = DIRECTIONS, temperature = 0): Dir {
  const scores = allowed.map(d => motor[DIRECTIONS.indexOf(d)] ?? 0);
  const max = Math.max(0, ...scores);
  if (max === 0) return allowed[Math.floor(random() * allowed.length)] ?? "U";
  if (!(temperature > 0)) {
    const candidates = allowed.filter((_, i) => scores[i] === max);
    return candidates[Math.floor(random() * candidates.length)] ?? "U";
  }
  // Numerically stable softmax; subtract max so only competitive pools keep mass.
  let sum = 0;
  const weights = scores.map(s => { const w = Math.exp((s - max) / temperature); sum += w; return w; });
  let roll = random() * sum;
  for (let i = 0; i < allowed.length; i++) {
    roll -= weights[i];
    if (roll <= 0) return allowed[i];
  }
  return allowed[allowed.length - 1] ?? "U";
}

// A short-lived reflex; it never damages all synapses for a direction.
export function weakenMotor(brain: FlyBrain, dir: Dir) {
  brain.bias[DIRECTIONS.indexOf(dir)] = -1.5;
}

// Treat session storage as untrusted; reconstruct only bounded, numeric history.
export function restoreFlyBrain(value: unknown): FlyBrainHistory | undefined {
  if (!value || typeof value !== "object") return;
  const h = value as FlyBrainHistory;
  const finite = (n: unknown, max = Number.MAX_SAFE_INTEGER): n is number => typeof n === "number" && Number.isFinite(n) && n >= 0 && n <= max;
  if (h.kind !== "flybrain" || h.neurons !== NEURONS || !finite(h.steps, FRAME_CAP) ||
      !finite(h.loops, FRAME_CAP) || !finite(h.seed, 0xffffffff) || !finite(h.spikeRate, 100000) || !Array.isArray(h.frames)) return;
  const vector = (v: unknown, length: number, max: number) => Array.isArray(v) && v.length === length && v.every(n => finite(n, max)) ? [...v] as number[] : undefined;
  const frames = h.frames.slice(0, FRAME_CAP).filter(f => f && finite(f.t, FRAME_CAP) &&
    Number.isInteger(f.t) && DIRECTIONS.includes(f.dir) && typeof f.accepted === "boolean" && typeof f.won === "boolean" &&
    finite(f.sensory, 6400) && finite(f.central, 180000) && Array.isArray(f.motor) && f.motor.length === 4 &&
    f.motor.every(n => finite(n, 4000)) && Array.isArray(f.samples) && f.samples.length <= 120 &&
    (f.active === undefined || Array.isArray(f.active) && f.active.length <= NEURONS && f.active.every(n => finite(n, NEURONS - 1) && Number.isInteger(n))) &&
    f.samples.every(n => finite(n, 119) && Number.isInteger(n)) && (f.goals === undefined || finite(f.goals, 1)))
    .map(f => ({
      t: f.t, dir: f.dir, accepted: f.accepted, won: f.won, sensory: f.sensory,
      central: f.central, motor: [...f.motor] as FlyBrainSpikeFrame["motor"], samples: [...f.samples], active: f.active ? [...f.active] : undefined, goals: f.goals,
      inhibited: Array.isArray(f.inhibited) && f.inhibited.length <= 4 && f.inhibited.every(d => DIRECTIONS.includes(d)) ? [...f.inhibited] : undefined,
      behavior: ["MOVE", "SEARCH", "PUSH", "PAUSE"].includes(f.behavior ?? "") ? f.behavior : undefined,
      ring: vector(f.ring, 16, 1), octopamine: finite(f.octopamine, 1) ? f.octopamine : undefined,
      zones: vector(f.zones, 4, 180000) as FlyBrainSpikeFrame["zones"], targetDrive: vector(f.targetDrive, 4, 10),
      dopamine: typeof f.dopamine === "number" && Number.isFinite(f.dopamine) && Math.abs(f.dopamine) <= 3 ? f.dopamine : undefined,
      heading: typeof f.heading === "number" && Number.isFinite(f.heading) ? f.heading : undefined,
      compassDir: typeof f.compassDir === "string" && DIRECTIONS.includes(f.compassDir as Dir) ? (f.compassDir as Dir) : undefined,
      deadlockAverted: typeof f.deadlockAverted === "boolean" ? f.deadlockAverted : undefined,
      rays: vector(f.rays, 8, 1),
      ghostTrace: typeof f.ghostTrace === "string" && /^[UDLR]{0,8}$/.test(f.ghostTrace) ? f.ghostTrace : undefined,
      macroIntent: typeof f.macroIntent === "string" ? f.macroIntent.slice(0, 160) : undefined,
    }));
  return { kind: "flybrain", neurons: NEURONS, steps: h.steps, loops: h.loops, seed: h.seed, spikeRate: h.spikeRate,
    config: h.config ? brainConfig(h.config) : undefined, attempt: finite(h.attempt) ? h.attempt : 1,
    memorySessions: finite(h.memorySessions, 1000000) ? h.memorySessions : 0,
    frames: frames.length === h.frames.length && frames.every((f, i) => f.t === i + 1) ? frames : [] };
}

export function compactFlyBrain(history: FlyBrainHistory): FlyBrainHistory {
  return new TextEncoder().encode(JSON.stringify(history)).length > 50 * 1024 ? { ...history, frames: [] } : history;
}
