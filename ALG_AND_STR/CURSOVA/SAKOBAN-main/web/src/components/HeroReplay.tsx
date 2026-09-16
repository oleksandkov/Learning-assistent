"use client";
// Ambient homepage demo: plays all five levels on a loop, each solved by a
// different local algorithm (BFS, A* moves, A* pushes, IDA*, Greedy).
// Plans come from the C++ core via /api/game (validated there); the client
// only replays the validated move strings frame by frame. Any level that
// fails to load or replays illegally is skipped, falling back to the static
// board. Honors prefers-reduced-motion by rendering the static board.
import { useEffect, useState } from "react";
import BoardView from "./BoardView";
import MiniBoard from "./MiniBoard";
import { LEVELS } from "@/lib/levels";
import {
  ALGORITHMS,
  type Board,
  type LocalAlgorithm,
  type SearchResult,
  type Snapshot,
  type State,
} from "@/lib/types";

interface HeroPlan {
  levelId: string;
  algorithm: LocalAlgorithm;
}
// One algorithm per level so the loop showcases the whole local roster,
// including the IDA* (good) and Greedy (bad) coursework pair.
const PLAN: HeroPlan[] = [
  { levelId: "01-simple", algorithm: "bfs" },
  { levelId: "02-microban", algorithm: "astar-moves" },
  { levelId: "03-microban-3", algorithm: "astar-pushes" },
  { levelId: "04-small", algorithm: "idastar-pushes" },
  { levelId: "05-corridor", algorithm: "greedy-pushes" },
];
const FALLBACK_ALGORITHM: LocalAlgorithm = "astar-pushes";
const MOVE_MS = 320;
const PUSH_MS = 560;
const START_MS = 1000;
const DONE_MS = 2200;

interface HeroLevel {
  levelId: string;
  title: string;
  algoLabel: string;
  board: Board;
  frames: State[];
  pushes: boolean[];
}

// Replay a core-validated move string into per-frame states. The core padded
// the XSB by one cell, so vertical steps span board.width. Returns null on
// any illegal step or a non-won final (then the level is skipped).
function simulate(
  board: Board,
  start: State,
  moves: string,
): { frames: State[]; pushes: boolean[] } | null {
  const boxes = [...start.boxes];
  const boxSet = new Set(boxes);
  const frames: State[] = [{ player: start.player, boxes: [...boxes] }];
  const pushes: boolean[] = [];
  let player = start.player;
  for (const dir of moves) {
    const d =
      dir === "U"
        ? -board.width
        : dir === "D"
          ? board.width
          : dir === "L"
            ? -1
            : dir === "R"
              ? 1
              : undefined;
    if (d === undefined) return null;
    const target = player + d;
    if (board.walls[target]) return null;
    let pushed = false;
    if (boxSet.has(target)) {
      const beyond = target + d;
      if (board.walls[beyond] || boxSet.has(beyond)) return null;
      boxSet.delete(target);
      boxSet.add(beyond);
      boxes[boxes.indexOf(target)] = beyond;
      pushed = true;
    }
    player = target;
    frames.push({ player, boxes: [...boxes] });
    pushes.push(pushed);
  }
  if (!boxes.every((box) => board.goals[box])) return null;
  return { frames, pushes };
}

async function post<T>(body: object, signal: AbortSignal): Promise<T> {
  const response = await fetch("/api/game", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
    signal,
  });
  const data = await response.json();
  if (!response.ok) throw new Error(data.error || "Запит не вдався.");
  return data as T;
}

async function loadLevel(
  plan: HeroPlan,
  signal: AbortSignal,
): Promise<HeroLevel | null> {
  const level = LEVELS.find((item) => item.id === plan.levelId);
  if (!level) return null;
  const attempts =
    plan.algorithm === FALLBACK_ALGORITHM
      ? [FALLBACK_ALGORITHM]
      : [plan.algorithm, FALLBACK_ALGORITHM];
  for (const algorithm of attempts) {
    try {
      const solved = await post<SearchResult>(
        {
          levelId: plan.levelId,
          history: "",
          mode: "solve",
          direction: "",
          algorithm,
        },
        signal,
      );
      if (solved.status !== "Solved" || !solved.moves) continue;
      const snapshot = await post<Snapshot>(
        {
          levelId: plan.levelId,
          history: "",
          mode: "state",
          direction: "",
          algorithm,
        },
        signal,
      );
      const replay = simulate(snapshot.board, snapshot.state, solved.moves);
      if (!replay) continue;
      return {
        levelId: plan.levelId,
        title: level.title,
        algoLabel:
          ALGORITHMS.find((item) => item.id === algorithm)?.label ?? algorithm,
        board: snapshot.board,
        frames: replay.frames,
        pushes: replay.pushes,
      };
    } catch {
      // Try the fallback algorithm, then skip the level.
    }
  }
  return null;
}

export default function HeroReplay({ fallbackXsb }: { fallbackXsb: string }) {
  const [reduced] = useState(
    () =>
      typeof window !== "undefined" &&
      window.matchMedia("(prefers-reduced-motion: reduce)").matches,
  );
  const [levels, setLevels] = useState<HeroLevel[] | null>(null);
  const [index, setIndex] = useState(0);
  const [step, setStep] = useState(0);
  const [done, setDone] = useState(false);

  useEffect(() => {
    if (reduced) return;
    const abort = new AbortController();
    Promise.all(PLAN.map((plan) => loadLevel(plan, abort.signal)))
      .then((settled) => {
        if (abort.signal.aborted) return;
        const ready = settled.filter(
          (item): item is HeroLevel => item !== null,
        );
        if (ready.length > 0) setLevels(ready);
      })
      .catch(() => {});
    return () => abort.abort();
  }, [reduced]);

  const current = levels ? levels[index % levels.length] : null;
  const total = current ? current.frames.length - 1 : 0;

  useEffect(() => {
    if (!current || !levels) return;
    if (done) {
      const timer = setTimeout(() => {
        setDone(false);
        setStep(0);
        setIndex((i) => (i + 1) % levels.length);
      }, DONE_MS);
      return () => clearTimeout(timer);
    }
    if (step >= total) {
      setDone(true);
      return;
    }
    const pushed = current.pushes[step] ?? false;
    const delay = step === 0 ? START_MS : pushed ? PUSH_MS : MOVE_MS;
    const timer = setTimeout(() => setStep((s) => s + 1), delay);
    return () => clearTimeout(timer);
  }, [current, levels, step, done, total]);

  if (reduced || !current || !levels) {
    return <MiniBoard xsb={fallbackXsb} hero />;
  }
  const shown = Math.min(step, total);
  return (
    <div className="hero-live">
      <BoardView
        board={current.board}
        state={current.frames[shown]}
        animated
        className="hero-board"
      />
      <p className="hero-status" aria-hidden="true">
        <span className="hero-status-row">
          <span className="hero-status-algo">
            {done ? "✓ Завершено" : current.algoLabel}
          </span>
          <span className="hero-status-steps">
            {shown}/{total}
          </span>
        </span>
        <span className="hero-status-row">
          <span className="hero-status-level">{current.title}</span>
        </span>
        <span className="hero-progress">
          <i style={{ width: `${total ? (shown / total) * 100 : 0}%` }} />
        </span>
      </p>
      <span className="sr-only" role="status">
        {`${current.title}, алгоритм ${current.algoLabel}${done ? ", завершено" : ""}`}
      </span>
    </div>
  );
}
