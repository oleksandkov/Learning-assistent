"use client";

import { useEffect, useMemo, useRef, useState } from "react";
import BoardView, { type BoardMarker } from "./BoardView";
import {
  ALGORITHMS,
  type Algorithm,
  type SearchResult,
  type SearchTracePoint,
  type Snapshot,
} from "@/lib/types";

type DebugPhase = "search" | "route" | "move";

const arrows: Record<string, string> = { U: "↑", L: "←", D: "↓", R: "→" };
const directions: Record<string, string> = {
  U: "вгору",
  L: "вліво",
  D: "вниз",
  R: "вправо",
};
const rules: Record<
  Algorithm,
  { structure: string; choice: string; cost: string; explanation: string }
> = {
  bfs: {
    structure: "Черга FIFO",
    choice: "Стан із початку черги",
    cost: "Один хід = один рівень",
    explanation:
      "BFS перевіряє повні стани поля шар за шаром і знаходить маршрут із мінімальною кількістю ходів.",
  },
  "astar-moves": {
    structure: "Пріоритетна черга",
    choice: "Найменше g + h",
    cost: "g рахує ходи",
    explanation:
      "A* спочатку розгортає перспективні стани: виконані ходи g плюс оцінка відстані ящиків до цілей h.",
  },
  "astar-pushes": {
    structure: "Черга штовхань",
    choice: "Найменше g + h",
    cost: "g рахує штовхання",
    explanation:
      "A* переходить між можливими штовханнями, а маршрут гравця до ящика зберігає всередині макрокроку.",
  },
  "idastar-pushes": {
    structure: "Стек шляху + поріг f",
    choice: "DFS в межах порога bound",
    cost: "g рахує штовхання",
    explanation:
      "IDA* поглиблює поріг f = g + h і шукає вглиб: оптимально як A*, але пам'ять тільки під поточний шлях.",
  },
  "greedy-pushes": {
    structure: "Пріоритетна черга",
    choice: "Найменше h (без g)",
    cost: "g ігнорується",
    explanation:
      "Greedy завжди йде туди, де менше h, і не перевідкриває стани: швидко, але рішення довші й неоптимальні.",
  },
  prototype1: {
    structure: "Послідовний план X → Y",
    choice: "Найближча вільна ціль",
    cost: "Мангеттенська відстань",
    explanation:
      "Прототип бере ящики по черзі, призначає найближчу ціль і спочатку штовхає по X, потім по Y. Стіни бачить лише фінальна перевірка правил.",
  },
  aco: {
    structure: "Рій випадкових маршрутів",
    choice: "Феромон × близькість цілі",
    cost: "Шлях + штраф тупика",
    explanation:
      "Мурахи пробують допустимі ходи, а найкращі маршрути підсилюють феромон для наступного покоління.",
  },
  genetic: {
    structure: "Популяція хромосом",
    choice: "Відбір за fitness",
    cost: "Ходи + відстань + штрафи",
    explanation:
      "GA схрещує послідовності напрямків, мутує окремі гени та залишає кращі маршрути для нового покоління.",
  },
  gravity: {
    structure: "Популяція потоків",
    choice: "Найсильніший дозволений нахил",
    cost: "Висота + шлях + струшування",
    explanation:
      "Поле потенціалу тягне ящики до цілей, а контрольоване струшування допомагає залишити локальну западину.",
  },
  cannibal: {
    structure: "Турніри хромосом",
    choice: "Хижак + префікс жертви",
    cost: "Fitness + штраф тупика",
    explanation:
      "Сильний бот зберігає хвіст власної ДНК, забирає безпечний префікс невдалого бота й змінює критичний ген.",
  },
  gemini: {
    structure: "Зовнішня модель",
    choice: "Згенерований план",
    cost: "Без гарантії оптимуму",
    explanation:
      "AI пропонує маршрут, після чого локальне ядро перевіряє кожен його хід.",
  },
  flybrain: {
    structure: "LIF-мережа у Web Worker", choice: "Моторний пул із найбільшою активністю",
    cost: "Без гарантії рішення", explanation: "Спайки й допустимий маршрут показує окремий спостерігач FlyBrain.",
  },
};

export default function SearchDebugger({
  result,
  baseLength,
  loadSnapshot,
  onApply,
  onRecalculate,
  onClose,
}: {
  result: SearchResult;
  baseLength: number;
  loadSnapshot: (step: number) => Promise<Snapshot>;
  onApply: () => void;
  onRecalculate: () => void;
  onClose: () => void;
}) {
  const trace = useMemo<SearchTracePoint[]>(() => {
    if (result.trace.length > 1) return result.trace;
    return [
      { explored: 0, frontier: 1, elapsedMs: 0, status: "Running" as const },
      {
        explored: result.explored,
        frontier: 0,
        elapsedMs: result.searchMs,
        status: result.status,
      },
    ];
  }, [result]);
  const loaderRef = useRef(loadSnapshot);
  loaderRef.current = loadSnapshot;
  const framesRef = useRef<Record<number, Snapshot>>({});
  const [frames, setFrames] = useState<Record<number, Snapshot>>({});
  const [phase, setPhase] = useState<DebugPhase>("search");
  const [searchIndex, setSearchIndex] = useState(0);
  const [routeIndex, setRouteIndex] = useState(0);
  const [playing, setPlaying] = useState(false);
  const [speed, setSpeed] = useState(560);
  const [loadingFrame, setLoadingFrame] = useState(false);
  const [frameError, setFrameError] = useState("");

  async function ensureFrame(index: number) {
    if (framesRef.current[index]) return framesRef.current[index];
    setLoadingFrame(true);
    try {
      const snapshot = await loaderRef.current(index);
      framesRef.current = { ...framesRef.current, [index]: snapshot };
      setFrames(framesRef.current);
      setFrameError("");
      return snapshot;
    } catch {
      setFrameError("Не вдалося завантажити цей крок.");
      setPlaying(false);
      return null;
    } finally {
      setLoadingFrame(false);
    }
  }

  useEffect(() => {
    framesRef.current = {};
    setFrames({});
    setPhase("search");
    setSearchIndex(0);
    setRouteIndex(0);
    setPlaying(
      !window.matchMedia("(prefers-reduced-motion: reduce)").matches &&
        trace.length > 1,
    );
    void ensureFrame(0);
    // The parent keys this component by the completed solver run.
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [result.algorithm, result.totalMs, trace.length]);

  useEffect(() => {
    if (!playing || phase !== "search" || searchIndex >= trace.length - 1)
      return;
    const timer = window.setTimeout(
      () => setSearchIndex((value) => value + 1),
      speed,
    );
    return () => window.clearTimeout(timer);
  }, [phase, playing, searchIndex, speed, trace.length]);

  useEffect(() => {
    if (phase === "search" && searchIndex >= trace.length - 1)
      setPlaying(false);
  }, [phase, searchIndex, trace.length]);

  useEffect(() => {
    if (
      !playing ||
      phase === "search" ||
      routeIndex >= result.moves.length
    )
      return;
    let cancelled = false;
    const timer = window.setTimeout(async () => {
      const next = routeIndex + 1;
      const snapshot = await ensureFrame(next);
      if (!cancelled && snapshot) setRouteIndex(next);
    }, speed);
    return () => {
      cancelled = true;
      window.clearTimeout(timer);
    };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [phase, playing, routeIndex, result.moves.length, speed]);

  useEffect(() => {
    if (phase !== "search" && routeIndex >= result.moves.length)
      setPlaying(false);
  }, [phase, routeIndex, result.moves.length]);

  const point = trace[searchIndex];
  const previous = trace[Math.max(0, searchIndex - 1)];
  const rule = rules[result.algorithm];
  const algorithm = ALGORITHMS.find((item) => item.id === result.algorithm)!;
  const delta = Math.max(0, point.explored - previous.explored);
  const baseSnapshot = frames[0];
  const activeSnapshot = frames[routeIndex] ?? baseSnapshot;
  const searchState = point.state ?? baseSnapshot?.state;
  const statusText =
    point.status === "Solved"
      ? "Рішення знайдено й передано на перевірку"
      : point.status === "LimitReached"
        ? "Пошук зупинено лімітом"
        : point.status === "NoSolution"
          ? "Черга вичерпана"
          : searchIndex === 0
            ? "Початковий стан додано до черги"
            : `Перевірено ще ${delta.toLocaleString("uk-UA")} станів`;
  const routeMarkers = useMemo(() => {
    const byCell = new Map<number, BoardMarker>();
    for (let index = 0; index <= routeIndex; index++) {
      const snapshot = frames[index];
      if (!snapshot) continue;
      byCell.set(snapshot.state.player, {
        cell: snapshot.state.player,
        kind: index === routeIndex ? "current" : "route",
        label: index < result.moves.length ? arrows[result.moves[index]] : "",
      });
      if (index > 0 && frames[index - 1]) {
        const oldBoxes = new Set(frames[index - 1].state.boxes);
        const pushedTo = snapshot.state.boxes.find((box) => !oldBoxes.has(box));
        if (pushedTo !== undefined)
          byCell.set(pushedTo, { cell: pushedTo, kind: "push", label: "•" });
      }
    }
    return Array.from(byCell.values());
  }, [frames, result.moves, routeIndex]);
  const searchMarkers: BoardMarker[] =
    typeof point.boxTo === "number"
      ? [{ cell: point.boxTo, kind: "push", label: "•" }]
      : [];
  const isAStar = !["bfs", "prototype1", "aco", "genetic", "gravity", "cannibal"].includes(result.algorithm);
  const g = point.g ?? 0;
  const h = point.h ?? 0;
  const selectionText =
    searchIndex === 0
      ? "Початковий стан"
      : result.algorithm === "bfs"
        ? `FIFO: рівень ${point.depth ?? g}`
        : result.algorithm === "prototype1"
          ? `Крок плану ${g} · залишилось ${h}`
        : result.algorithm === "aco"
          ? "Найкраща феромонна стежка"
        : result.algorithm === "genetic"
          ? "Найкраща хромосома"
        : result.algorithm === "gravity"
          ? "Найсильніший дозволений потік"
        : result.algorithm === "cannibal"
          ? "Найкращий канібальний бот"
        : `Найменше f = ${g + h}`;

  function switchPhase(next: DebugPhase) {
    setPlaying(false);
    setPhase(next);
    if (next === "search") setSearchIndex(0);
    else {
      setRouteIndex(0);
      void ensureFrame(0);
    }
  }

  async function stepRoute(direction: -1 | 1) {
    setPlaying(false);
    const next = Math.max(
      0,
      Math.min(result.moves.length, routeIndex + direction),
    );
    if (next > routeIndex && !(await ensureFrame(next))) return;
    setRouteIndex(next);
  }

  const atPhaseEnd =
    phase === "search"
      ? searchIndex >= trace.length - 1
      : routeIndex >= result.moves.length;
  const phaseProgress =
    phase === "search"
      ? `${searchIndex + 1} / ${trace.length}`
      : `${routeIndex} / ${result.moves.length}`;

  return (
    <section className="search-debugger" aria-label={`Дебаг ${algorithm.label}`}>
      <div className="debug-heading">
        <div>
          <span>Візуальний дебаг</span>
          <h2>{algorithm.label}</h2>
          <p>{rule.explanation}</p>
        </div>
        <div className="debug-heading-actions">
          <button className="button subtle" onClick={onRecalculate}>
            Перерахувати
          </button>
          <button className="button subtle" onClick={onClose}>
            Закрити
          </button>
        </div>
      </div>

      <nav className="debug-phases" aria-label="Етап дебагу">
        {(
          [
            ["search", "01", "Пошук"],
            ["route", "02", "Маршрут"],
            ["move", "03", "Рух"],
          ] as const
        ).map(([id, number, label]) => (
          <button
            className={phase === id ? "active" : ""}
            onClick={() => switchPhase(id)}
            key={id}
          >
            <span>{number}</span>
            {label}
          </button>
        ))}
      </nav>

      <div className="debug-game-layout">
        <div className="debug-board-shell">
          {baseSnapshot && activeSnapshot && searchState ? (
            <BoardView
              board={baseSnapshot.board}
              state={
                phase === "search"
                  ? searchState
                  : phase === "move"
                    ? activeSnapshot.state
                    : baseSnapshot.state
              }
              animated={phase === "search" || phase === "move"}
              markers={
                phase === "search"
                  ? searchMarkers
                  : phase === "move"
                    ? routeMarkers.filter((marker) => marker.kind !== "current")
                    : routeMarkers
              }
              className="debug-game-board"
            />
          ) : (
            <div className="debug-board-loading">Завантажуємо поле…</div>
          )}
          <span className="debug-board-phase">
            {phase === "search"
              ? `Стан ${searchIndex + 1} · ${selectionText}`
              : phase === "route"
                ? "Побудова маршруту"
                : "Відтворення"}
          </span>
        </div>

        <aside className="debug-console">
          <div className="debug-console-top">
            <span>{phase === "search" ? "Запис пошуку" : "Крок маршруту"}</span>
            <strong>{phaseProgress}</strong>
          </div>
          {phase === "search" ? (
            <>
              <div className="debug-selected-state">
                <span>Алгоритм вибрав</span>
                <strong>{selectionText}</strong>
                <p>
                  {point.hasTransition
                    ? point.pushed
                      ? `Штовхання ${arrows[point.direction ?? "U"]}`
                      : `Хід ${arrows[point.direction ?? "U"]}`
                    : "Стартова позиція"}
                </p>
              </div>
              <div className="debug-live-metrics" aria-live="polite">
                <div>
                  <span>{isAStar ? "g · уже витрачено" : "Глибина"}</span>
                  <strong>{isAStar ? g : point.depth ?? g}</strong>
                </div>
                <div>
                  <span>{isAStar ? "h · оцінка до цілі" : "Черга"}</span>
                  <strong>{isAStar ? h : point.frontier.toLocaleString("uk-UA")}</strong>
                </div>
                <div>
                  <span>{isAStar ? "f · пріоритет" : "Досліджено"}</span>
                  <strong>{isAStar ? g + h : point.explored.toLocaleString("uk-UA")}</strong>
                </div>
              </div>
              <p className="debug-status">{statusText}</p>
              <div className="debug-rule-list">
                <div><span>Структура</span><strong>{rule.structure}</strong></div>
                <div><span>Вибір</span><strong>{rule.choice}</strong></div>
                <div><span>Ціна</span><strong>{rule.cost}</strong></div>
              </div>
              <p className="debug-honesty">
                Поле показує реальний стан, який solver зараз дістає з черги.
                Наступний кадр може належати іншій гілці пошуку.
              </p>
            </>
          ) : (
            <>
              <div className="debug-step-focus">
                <span>{phase === "route" ? "Будуємо шлях" : "Виконуємо хід"}</span>
                <strong>
                  {routeIndex < result.moves.length
                    ? arrows[result.moves[routeIndex]]
                    : "✓"}
                </strong>
                <p>
                  {routeIndex === result.moves.length
                    ? "Маршрут завершено"
                    : `Хід ${routeIndex + 1}: ${
                        directions[result.moves[routeIndex]]
                      }`}
                </p>
              </div>
              <dl className="debug-route-summary">
                <div><dt>Усього ходів</dt><dd>{result.moves.length}</dd></div>
                <div><dt>Штовхань</dt><dd>{result.pushes}</dd></div>
                <div><dt>Старт</dt><dd>після {baseLength} ходів</dd></div>
              </dl>
              <p className="debug-honesty">
                {phase === "route"
                  ? "Стрілки — реальний знайдений шлях; крапка позначає штовхання."
                  : "Кожен кадр повторно обчислює спільний GameSession."}
              </p>
            </>
          )}
        </aside>
      </div>

      <div className="debug-controls">
        <button
          className="button secondary"
          aria-label="Попередній крок дебагу"
          disabled={phase === "search" ? searchIndex === 0 : routeIndex === 0}
          onClick={() => {
            if (phase === "search") {
              setPlaying(false);
              setSearchIndex((value) => Math.max(0, value - 1));
            } else void stepRoute(-1);
          }}
        >
          ←
        </button>
        <button
          className="button primary"
          disabled={loadingFrame || (phase !== "search" && !result.moves.length)}
          onClick={() => {
            if (atPhaseEnd) {
              if (phase === "search") setSearchIndex(0);
              else setRouteIndex(0);
            }
            setPlaying((value) => !value);
          }}
        >
          {playing ? "Пауза" : atPhaseEnd ? "Спочатку" : "Продовжити"}
        </button>
        <button
          className="button secondary"
          aria-label="Наступний крок дебагу"
          disabled={atPhaseEnd || loadingFrame}
          onClick={() => {
            if (phase === "search") {
              setPlaying(false);
              setSearchIndex((value) => Math.min(trace.length - 1, value + 1));
            } else void stepRoute(1);
          }}
        >
          →
        </button>
        <label>
          Темп <span>{speed} мс</span>
          <input
            type="range"
            min="240"
            max="1200"
            step="80"
            value={speed}
            onChange={(event) => setSpeed(Number(event.target.value))}
          />
        </label>
        {phase === "search" && atPhaseEnd && result.status === "Solved" ? (
          <button
            className="button secondary debug-next-phase"
            onClick={() => switchPhase("route")}
          >
            Показати маршрут →
          </button>
        ) : phase === "route" && atPhaseEnd ? (
          <button
            className="button secondary debug-next-phase"
            onClick={() => switchPhase("move")}
          >
            Запустити рух →
          </button>
        ) : (
          <button className="button primary debug-next-phase" onClick={onApply}>
            Застосувати до рівня
          </button>
        )}
      </div>
      {frameError ? <p className="debug-frame-error">{frameError}</p> : null}
    </section>
  );
}
