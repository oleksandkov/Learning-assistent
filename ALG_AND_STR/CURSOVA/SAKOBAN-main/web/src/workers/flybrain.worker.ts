import {
  applyDopamine, brainConfig, computeRayCasting, createFlyBrain, decodeAction, DIRECTIONS,
  encodeState, isDeadlockPush, mulberry32, NEURONS, stepBrain, updateCXHeading, weakenMotor
} from "../lib/flybrain";
import type { Dir, FlyBrainConfig, FlyBrainHistory, SearchResult, Snapshot } from "../lib/types";

export interface FlyBrainRequest { levelId: string; customXsb?: string; base: string; seed: number; config?: Partial<FlyBrainConfig>; attempt?: number }
export type FlyBrainEvent = { type: "progress" | "done"; result: SearchResult } | { type: "error"; message: string };

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

function getBestTargetDir(snapshot: Snapshot): Dir {
  const { board, state } = snapshot;
  const px = state.player % board.width, py = Math.floor(state.player / board.width);
  let targetCell = -1, minD = Infinity;
  for (const box of state.boxes) {
    const bx = box % board.width, by = Math.floor(box / board.width);
    const d = Math.abs(px - bx) + Math.abs(py - by);
    if (d < minD) { minD = d; targetCell = box; }
  }
  if (targetCell < 0) return "R";
  const tx = targetCell % board.width, ty = Math.floor(targetCell / board.width);
  if (Math.abs(tx - px) > Math.abs(ty - py)) {
    return tx > px ? "R" : "L";
  }
  return ty > py ? "D" : "U";
}

export async function runSession(input: FlyBrainRequest, signal: AbortSignal, post: (event: FlyBrainEvent) => void) {
  const started = performance.now();
  const config = brainConfig(input.config);
  const deadline = new AbortController();
  const cancel = () => deadline.abort();
  signal.addEventListener("abort", cancel, { once: true });
  const timer = setTimeout(cancel, config.maxWallMs);
  const brain = createFlyBrain(input.seed);
  const preprocessingMs = performance.now() - started;
  const history: FlyBrainHistory = { kind: "flybrain", neurons: NEURONS, steps: 0, frames: [], spikeRate: 0, loops: 0, seed: input.seed, config, attempt: input.attempt ?? 1 };
  let moves = "", snapshot: Snapshot | undefined, initialPushes = 0, blocked = 0, generated = 0, searchMs = 0, spikeTotal = 0;
  const seen = new Set<string>();
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
    seen.add(key(snapshot));
    let prevGoalDist = totalGoalDistance(snapshot);
    let prevBoxesOnGoals = snapshot.state.boxes.filter(c => snapshot!.board.goals[c]).length;

    for (let t = 1; t <= config.maxSteps && !snapshot.won; t++) {
      if (deadline.signal.aborted) break;
      const tick = performance.now();
      const currents = encodeState(snapshot, input.base + moves);
      const { spikes } = stepBrain(brain, currents, config);
      const random = mulberry32(input.seed + t);
      const dir = decodeAction(spikes.motor, random);
      searchMs += performance.now() - tick;

      let chosen: Dir = dir;
      let deadlockAverted = false;

      // Nociception Reflex: if chosen direction would push a box into an irreversible deadlock
      if (config.enableNociception !== false && isDeadlockPush(snapshot, chosen)) {
        deadlockAverted = true;
        const nonDeadlockAlts = DIRECTIONS.filter(d => d !== chosen && !isDeadlockPush(snapshot!, d));
        if (nonDeadlockAlts.length) {
          chosen = nonDeadlockAlts[Math.floor(random() * nonDeadlockAlts.length)];
        }
      }

      generated++;
      let next = await getState(moves, chosen, deadline.signal);

      // Silent pools explore legal moves, with C++ deciding legality even for fallback probes.
      const alternatives = DIRECTIONS.filter(d => d !== chosen);
      while (!next.accepted && spikes.motor.every(n => n === 0) && alternatives.length) {
        blocked++; weakenMotor(brain, chosen);
        chosen = alternatives.splice(Math.floor(random() * alternatives.length), 1)[0];
        generated++;
        next = await getState(moves, chosen, deadline.signal);
      }

      let dopamine = 0;
      const targetDir = getBestTargetDir(snapshot);
      const heading = updateCXHeading(brain, chosen, targetDir);

      if (next.accepted) {
        moves += chosen;
        const newGoalDist = totalGoalDistance(next);
        const newBoxesOnGoals = next.state.boxes.filter(c => next.board.goals[c]).length;
        const stateKey = key(next);

        if (newBoxesOnGoals > prevBoxesOnGoals) {
          dopamine = 2.5; // Big dopamine burst for placing a box on goal!
        } else if (newGoalDist < prevGoalDist) {
          dopamine = 1.0; // Positive reinforcement for closing in on goals
        } else if (seen.has(stateKey)) {
          history.loops++;
          brain.bias[Math.floor(random() * 4)] += 1.5;
          dopamine = -1.2; // LTD punishment for spinning in loops
        } else {
          dopamine = 0.2; // Small baseline explorer dopamine
        }

        prevGoalDist = newGoalDist;
        prevBoxesOnGoals = newBoxesOnGoals;
        snapshot = next;
        seen.add(stateKey);
      } else {
        blocked++;
        weakenMotor(brain, chosen);
        dopamine = -0.8; // Negative feedback for bumping into walls
      }

      if (config.enablePlasticity !== false) {
        applyDopamine(brain, dopamine);
      }

      const isPush = snapshot.pushes > (t > 1 ? (history.frames[t - 2]?.motor ? snapshot.pushes : initialPushes) : initialPushes);
      const macroIntent = next.won
        ? "Рівень розв'язано: усі ящики на цілях"
        : deadlockAverted
        ? `Ноцицепція: захист від дедлоку (${arrows[chosen]})`
        : isPush
        ? `Штовхання ящика ${arrows[chosen]}`
        : `Навігація до цілі (компас: ${arrows[targetDir]})`;

      const rays = config.enableRays !== false ? computeRayCasting(snapshot) : undefined;
      const ghostTrace = chosen + (targetDir !== chosen ? targetDir : "");

      history.steps = t;
      history.frames.push({
        t, dir: chosen, accepted: next.accepted, won: next.accepted && next.won,
        ...spikes,
        dopamine: Math.round(dopamine * 10) / 10,
        heading: Math.round(heading * 100) / 100,
        compassDir: targetDir,
        deadlockAverted: deadlockAverted || undefined,
        rays,
        ghostTrace,
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
  post({ type: "done", result: final });
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
