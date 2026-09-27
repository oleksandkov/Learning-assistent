import {
  applyDopamine, applyOctopamine, decodeBehavior, readTargetDirection, saveBrainMemory, brainConfig, computeRayCasting, createFlyBrain, decodeAction, DIRECTIONS,
  encodeState, isDeadlockPush, mulberry32, NEURONS, stepBrain, updateCXHeading, weakenMotor
} from "../lib/flybrain";
import type { Dir, FlyBrainConfig, FlyBrainHistory, FlyBrainMemory, SearchResult, Snapshot } from "../lib/types";

export interface FlyBrainRequest { levelId: string; customXsb?: string; base: string; seed: number; config?: Partial<FlyBrainConfig>; attempt?: number; memory?: FlyBrainMemory }
export type FlyBrainEvent = { type: "progress" | "done"; result: SearchResult; memory?: FlyBrainMemory } | { type: "error"; message: string };

const arrows: Record<Dir, string> = { U: "↑", D: "↓", L: "←", R: "→" };

function totalGoalDistance(snapshot: Snapshot): number {
  const { board, state } = snapshot;
  let total = 0;
  for (const box of state.boxes) {
    const bx = box % board.width, by = Math.floor(box / board.width);
    let minD = Infinity;
    board.goals.forEach((goal, cell) => {
      if (!goal) return;
      const gx = cell % board.width, gy = Math.floor(cell / board.width);
      const d = Math.abs(bx - gx) + Math.abs(by - gy);
      if (d < minD) minD = d;
    });
    total += minD === Infinity ? 0 : minD;
  }
  return total;
}

export async function runSession(input: FlyBrainRequest, signal: AbortSignal, post: (event: FlyBrainEvent) => void) {
  const started = performance.now();
  const config = brainConfig(input.config);
  const deadline = new AbortController();
  const cancel = () => deadline.abort();
  signal.addEventListener("abort", cancel, { once: true });
  const timer = setTimeout(cancel, config.maxWallMs);
  const brain = createFlyBrain(input.seed, input.memory);
  const preprocessingMs = performance.now() - started;
  const history: FlyBrainHistory = { kind: "flybrain", neurons: NEURONS, steps: 0, frames: [], spikeRate: 0, loops: 0, seed: input.seed, config, attempt: input.attempt ?? 1, memorySessions: brain.memorySessions };
  let moves = "", snapshot: Snapshot | undefined, initialPushes = 0, blocked = 0, generated = 0, searchMs = 0, spikeTotal = 0;
  const attempt = input.attempt ?? 1;
  // Visit counts drive escalating loop penalties (2nd visit worse than 1st, …).
  const visits = new Map<string, number>();
  // Reflex memory comes only from C++ feedback, scoped to the full board state.
  const rejected = new Map<string, Set<Dir>>();
  const key = (s: Snapshot) => `${s.state.player}:${[...s.state.boxes].sort((a, b) => a - b).join(",")}`;
  const getState = async (route: string, direction: string, requestSignal: AbortSignal) => {
    signal.throwIfAborted();
    const response = await fetch("/api/game", {
      method: "POST", headers: { "Content-Type": "application/json" }, signal: requestSignal,
      body: JSON.stringify({ levelId: input.levelId, customXsb: input.customXsb, history: input.base + route,
        direction, mode: "state", algorithm: "astar-pushes" }),
    });
    const data = await response.json();
    if (!response.ok) throw new Error(data.error || "Не вдалося перевірити хід FlyBrain.");
    return data as Snapshot;
  };
  const result = (validated = false, validationMs = 0): SearchResult => ({
    algorithm: "flybrain", status: validated && snapshot?.won ? "Solved" : "LimitReached",
    moves, pushes: Math.max(0, (snapshot?.pushes ?? initialPushes) - initialPushes),
    searchMs, preprocessingMs, validationMs, reconstructionMs: 0, totalMs: performance.now() - started,
    explored: history.steps, generated, deadlocks: blocked, frontier: 0, validated, trace: [],
    flybrain: { ...history, frames: [...history.frames] },
  });
  post({ type: "progress", result: result() });
  try {
    snapshot = await getState("", "", deadline.signal);
    if (!snapshot.accepted) throw new Error("Початкова позиція FlyBrain недійсна.");
    initialPushes = snapshot.pushes;
    visits.set(key(snapshot), 1);
    let prevGoalDist = totalGoalDistance(snapshot);
    let prevBoxesOnGoals = snapshot.state.boxes.filter(c => snapshot!.board.goals[c]).length;

    for (let t = 1; t <= config.maxSteps && !snapshot.won; t++) {
      if (deadline.signal.aborted) break;
      const stateKeyBefore = key(snapshot);
      const blockedHere = rejected.get(stateKeyBefore) ?? new Set<Dir>();
      const inhibited = Array.from(blockedHere);
      const allowed = DIRECTIONS.filter(d => !blockedHere.has(d));
      if (!allowed.length) break; // C++ has rejected every exit from this exact state.
      const tick = performance.now();
      const rays = config.enableRays !== false ? computeRayCasting(snapshot) : Array(8).fill(0);
      const currents = encodeState(snapshot, input.base + moves, rays);
      const { spikes } = stepBrain(brain, currents, config);
      const random = mulberry32(input.seed + t);
      // τ anneals over steps and attempts: explore early, exploit after memory builds.
      const temperature = Math.max(0.8, 2.5 * Math.exp(-t / 120) + 0.5 / Math.sqrt(attempt));
      const epsilon = Math.max(0.05, 0.2 / attempt);
      const dir = random() < epsilon
        ? allowed[Math.floor(random() * allowed.length)]
        : decodeAction(spikes.motor, random, allowed, temperature);
      searchMs += performance.now() - tick;

      let chosen: Dir = dir;
      let deadlockAverted = false;

      // Nociception Reflex: if chosen direction would push a box into an irreversible deadlock
      if (config.enableNociception !== false && isDeadlockPush(snapshot, chosen)) {
        deadlockAverted = true;
        const nonDeadlockAlts = allowed.filter(d => d !== chosen && !isDeadlockPush(snapshot!, d));
        if (!nonDeadlockAlts.length) break; // No untried exit survives the enabled safety reflex.
        if (nonDeadlockAlts.length) {
          chosen = nonDeadlockAlts[Math.floor(random() * nonDeadlockAlts.length)];
        }
      }

      const targetDir = readTargetDirection(brain);
      const oldPushes = snapshot.pushes;
      const adjacent = snapshot.state.player + (chosen === "U" ? -snapshot.board.width : chosen === "D" ? snapshot.board.width : chosen === "L" ? -1 : 1);
      let behavior = decodeBehavior(brain, spikes.motor, snapshot.state.boxes.includes(adjacent));
      if (behavior === "PAUSE") {
        applyOctopamine(brain, 0);
        history.steps = t;
        history.frames.push({ t, dir: chosen, accepted: false, won: false, ...spikes, behavior, inhibited,
          dopamine: 0, octopamine: brain.octopamine, heading: brain.heading, ring: Array.from(brain.ring),
          compassDir: targetDir, targetDrive: Array.from(brain.targetDrive), rays, macroIntent: "Огляд · інтеграція сенсорів без руху" });
        spikeTotal += spikes.sensory + spikes.central + spikes.motor.reduce((a, b) => a + b, 0);
        history.spikeRate = spikeTotal / t / NEURONS * 1000;
        if (t % 5 === 0) post({ type: "progress", result: result() });
        continue;
      }
      generated++;
      let next = await getState(moves, chosen, deadline.signal);
      const rememberRejection = () => {
        blockedHere.add(chosen);
        rejected.set(stateKeyBefore, blockedHere);
        weakenMotor(brain, chosen);
      };

      // Silent pools explore legal moves, with C++ deciding legality even for fallback probes.
      const alternatives = allowed.filter(d => d !== chosen && (config.enableNociception === false || !isDeadlockPush(snapshot!, d)));
      while (!next.accepted && spikes.motor.every(n => n === 0) && alternatives.length) {
        blocked++; rememberRejection();
        chosen = alternatives.splice(Math.floor(random() * alternatives.length), 1)[0];
        generated++;
        next = await getState(moves, chosen, deadline.signal);
      }

      let dopamine = 0;
      let effort = deadlockAverted ? .25 : 0;

      if (next.accepted) {
        moves += chosen;
        updateCXHeading(brain, chosen);
        const newGoalDist = totalGoalDistance(next);
        const newBoxesOnGoals = next.state.boxes.filter(c => next.board.goals[c]).length;
        const stateKey = key(next);
        const seenCount = visits.get(stateKey) ?? 0;

        // Potential-based shaping: Φ = −(box→goal distance). Positive = closer to a win.
        if (newBoxesOnGoals > prevBoxesOnGoals) {
          dopamine = 2.5; // Big dopamine burst for placing a box on goal!
        } else if (newGoalDist < prevGoalDist) {
          const gain = prevGoalDist - newGoalDist;
          dopamine = Math.min(2.4, 0.35 + 0.55 * gain);
        } else if (newGoalDist > prevGoalDist) {
          dopamine = Math.max(-2.5, -0.35 * (newGoalDist - prevGoalDist));
        }

        if (seenCount > 0) {
          history.loops++;
          effort += .35;
          brain.bias[Math.floor(random() * 4)] += 1.5;
          // Escalate: repeat visits hurt more (capped so DA stays within |DA| ≤ 3).
          dopamine = Math.min(dopamine, Math.max(-3, -1.0 * seenCount));
        } else if (dopamine === 0) {
          dopamine = 0.2; // Novel state, no progress yet — keep exploring
        }
        visits.set(stateKey, seenCount + 1);
        // Tiny per-step cost → prefers shorter solutions when several paths look equal.
        dopamine = Math.max(-3, Math.min(3, dopamine - 0.01));

        prevGoalDist = newGoalDist;
        prevBoxesOnGoals = newBoxesOnGoals;
        snapshot = next;
      } else {
        blocked++;
        rememberRejection();
        effort += .45;
        dopamine = Math.max(-3, Math.min(3, -0.8 - 0.01)); // Wall bump + step cost
      }

      applyOctopamine(brain, effort);
      if (config.enablePlasticity !== false) {
        applyDopamine(brain, dopamine);
      }

      const isPush = next.accepted && next.pushes > oldPushes;
      if (isPush) behavior = "PUSH";
      else if (behavior === "PUSH") behavior = "MOVE";
      const macroIntent = next.won ? "Рівень розв’язано: усі ящики на цілях"
        : deadlockAverted ? `Ноцицепція: відвернуто ризик ${arrows[chosen]}`
        : isPush ? `Штовхання ящика ${arrows[chosen]}`
        : behavior === "SEARCH" ? `Пошук виходу ${arrows[chosen]}` : `Нейронний намір ${arrows[targetDir]}`;

      history.steps = t;
      history.frames.push({
        t, dir: chosen, accepted: next.accepted, won: next.accepted && next.won,
        ...spikes,
        dopamine: Math.round(dopamine * 10) / 10,
        heading: brain.heading, ring: Array.from(brain.ring), behavior, inhibited,
        octopamine: brain.octopamine, targetDrive: Array.from(brain.targetDrive),
        compassDir: targetDir,
        deadlockAverted: deadlockAverted || undefined,
        rays,
        macroIntent,
        ...(t % 10 === 0 || next.won ? {
          goals: snapshot.state.boxes.filter(cell => snapshot!.board.goals[cell]).length / Math.max(1, snapshot.state.boxes.length),
        } : {}),
      });

      spikeTotal += spikes.sensory + spikes.central + spikes.motor.reduce((a, b) => a + b, 0);
      history.spikeRate = spikeTotal / t / NEURONS * 1000;
      if (t % 5 === 0 || snapshot.won) post({ type: "progress", result: result() });
    }
  } catch (error) {
    if (!deadline.signal.aborted || signal.aborted) throw error;
  } finally {
    clearTimeout(timer);
    signal.removeEventListener("abort", cancel);
  }
  signal.throwIfAborted();
  const validationStarted = performance.now();
  // The simulation budget excludes this final, separately bounded validation request.
  const validation = new AbortController();
  const stopValidation = () => validation.abort();
  signal.addEventListener("abort", stopValidation, { once: true });
  const validationTimer = setTimeout(stopValidation, 5000);
  let final: SearchResult;
  try {
    if (!/^[UDLR]*$/.test(moves) || !snapshot) throw new Error("Недійсний маршрут.");
    snapshot = await getState(moves, "", validation.signal);
    if (!snapshot.accepted) throw new Error("Маршрут не пройшов перевірку.");
    final = result(true, performance.now() - validationStarted);
  } catch {
    signal.throwIfAborted();
    final = { ...result(false, performance.now() - validationStarted), status: "InternalError" };
  } finally {
    clearTimeout(validationTimer);
    signal.removeEventListener("abort", stopValidation);
  }
  post({ type: "done", result: final, ...(final.validated ? { memory: saveBrainMemory(brain) } : {}) });
  return final;
}

// Guard keeps the session runnable in Node tests without a browser worker global.
if (typeof self !== "undefined" && typeof document === "undefined") {
  let controller: AbortController | undefined;
  self.onmessage = (event: MessageEvent<FlyBrainRequest | { type: "abort" }>) => {
    controller?.abort();
    if ("type" in event.data) return;
    controller = new AbortController();
    void runSession(event.data, controller.signal, message => self.postMessage(message)).catch(error => {
      if (error.name !== "AbortError") self.postMessage({ type: "error", message: error.message });
    });
  };
}
