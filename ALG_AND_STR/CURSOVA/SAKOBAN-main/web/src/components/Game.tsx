"use client";
import Link from "next/link";
import { useEffect, useMemo, useRef, useState } from "react";
import BoardView from "./BoardView";
import type { BoardCamera } from "./BoardView";
import SearchDebugger from "./SearchDebugger";
import ComparisonReplay from "./ComparisonReplay";
import {
  ALGORITHMS,
  AI_KEY_STORAGE,
  AI_MODEL_STORAGE,
  GROQ_MODELS,
  DEFAULT_GROQ_MODEL,
  DEFAULT_GOOGLE_MODEL,
  GOOGLE_MODELS,
  detectAiProvider,
  normalizeAiKey,
  LOCAL_ALGORITHMS,
  type Algorithm,
  type Dir,
  type Level,
  type SearchResult,
  type Snapshot,
} from "@/lib/types";
import { LEVELS } from "@/lib/levels";

interface Model {
  snapshot: Snapshot | null;
  history: string;
  redo: string[];
}
interface Plan {
  result: SearchResult;
  base: string;
  index: number;
}
interface SavedPlan {
  result: SearchResult;
  base: string;
}
interface Decision {
  result: SearchResult;
  base: string;
}
const arrows: Record<string, string> = { U: "↑", L: "←", D: "↓", R: "→" };
const keys: Record<string, Dir> = {
  ArrowUp: "U",
  ArrowLeft: "L",
  ArrowDown: "D",
  ArrowRight: "R",
  w: "U",
  a: "L",
  s: "D",
  d: "R",
  ц: "U",
  ф: "L",
  і: "D",
  в: "R",
};
const failures = {
  NoSolution: "Із цієї позиції розв’язку немає. Скасуй хід або почни спочатку.",
  LimitReached: "Досягнуто ліміт пошуку. Спробуй A* або іншу позицію.",
  InternalError: "Рішення не пройшло перевірку. Спробуй ще раз.",
};
function fingerprint(value: string) {
  let hash = 2166136261;
  for (let i = 0; i < value.length; i++) {
    hash ^= value.charCodeAt(i);
    hash = Math.imul(hash, 16777619);
  }
  return (hash >>> 0).toString(36);
}
function restoreDecision(value: unknown): Decision | null {
  if (!value || typeof value !== "object") return null;
  const candidate = value as { base?: unknown; result?: Partial<SearchResult> };
  const result = candidate.result;
  const aiSession = result?.aiSession;
  const repairedBy = (result as { repairedBy?: unknown } | undefined)?.repairedBy;
  const numericFields: (keyof SearchResult)[] = [
    "pushes",
    "searchMs",
    "preprocessingMs",
    "validationMs",
    "reconstructionMs",
    "totalMs",
    "explored",
    "generated",
    "deadlocks",
    "frontier",
  ];
  if (
    typeof candidate.base !== "string" ||
    !/^[UDLR]*$/.test(candidate.base) ||
    !result ||
    (result.algorithm === "gemini" && repairedBy !== undefined) ||
    !ALGORITHMS.some((item) => item.id === result.algorithm) ||
    !["Solved", "NoSolution", "LimitReached", "InternalError"].includes(
      String(result.status),
    ) ||
    typeof result.moves !== "string" ||
    !/^[UDLR]*$/.test(result.moves) ||
    typeof result.validated !== "boolean" ||
    (aiSession !== undefined &&
      (!aiSession ||
        typeof aiSession.model !== "string" ||
        typeof aiSession.prompt !== "string" ||
        typeof aiSession.response !== "string" ||
        aiSession.prompt.length > 30000 ||
        aiSession.response.length > 120100)) ||
    !numericFields.every(
      (field) =>
        typeof result[field] === "number" &&
        Number.isFinite(result[field]) &&
        Number(result[field]) >= 0,
    )
  )
    return null;
  const trace = Array.isArray(result.trace)
    ? result.trace.filter(
        (point) =>
          point &&
          typeof point.explored === "number" &&
          typeof point.frontier === "number" &&
          typeof point.elapsedMs === "number" &&
          ["Running", "Solved", "NoSolution", "LimitReached", "InternalError"].includes(
            String(point.status),
          ) &&
          (!point.state ||
            (typeof point.state.player === "number" &&
              Array.isArray(point.state.boxes) &&
              point.state.boxes.every((box) => typeof box === "number"))),
      )
    : [];
  return {
    base: candidate.base,
    result: { ...(result as SearchResult), trace },
  };
}

export default function Game({
  level,
  customXsb,
  guaranteedSolution,
}: {
  level: Level;
  customXsb?: string;
  guaranteedSolution?: { moves: string; pushes: number };
}) {
  const [model, setModel] = useState<Model>({
    snapshot: null,
    history: "",
    redo: [],
  });
  const current = useRef(model);
  const [message, setMessage] = useState("");
  const [busy, setBusy] = useState(false);
  const lock = useRef(false);
  const [algorithm, setAlgorithm] = useState<Algorithm>("astar-pushes");
  const [decisions, setDecisions] = useState<Decision[]>([]);
  const decisionsRef = useRef(decisions);
  const [searching, setSearching] = useState(false);
  const controller = useRef<AbortController | null>(null);
  const version = useRef(0);
  const mounted = useRef(true);
  const [plan, setPlan] = useState<Plan | null>(null);
  const planRef = useRef(plan);
  planRef.current = plan;
  const [playing, setPlaying] = useState(false);
  const [speed, setSpeed] = useState(400);
  const [animated, setAnimated] = useState(false);
  const [elapsed, setElapsed] = useState(0);
  const started = useRef<number | null>(null);
  const keyboard = useRef<(event: KeyboardEvent) => void>(() => {});
  const gameController = useRef<AbortController | null>(null);
  const [mapExpanded, setMapExpanded] = useState(false);
  const closeMap = useRef<HTMLButtonElement | null>(null);
  const boardStage = useRef<HTMLDivElement | null>(null);
  const [camera, setCamera] = useState<BoardCamera>({
    fov: 17,
    follow: true,
    centerX: 0,
    centerY: 0,
  });
  const solutionStorageKey = useMemo(
    () =>
      `sokoban.applied-solution.${level.id}.${customXsb ? fingerprint(customXsb) : "catalog"}`,
    [customXsb, level.id],
  );
  const [savedPlan, setSavedPlan] = useState<SavedPlan | null>(null);
  const decisionStorageKey = useMemo(
    () =>
      `sokoban.algorithm-decisions.${level.id}.${customXsb ? fingerprint(customXsb) : "catalog"}`,
    [customXsb, level.id],
  );
  const [debugAlgorithm, setDebugAlgorithm] = useState<Algorithm | null>(null);
  const [showDebugger, setShowDebugger] = useState(false);
  const [selectedComparisons, setSelectedComparisons] = useState<Algorithm[]>([]);
  const [comparison, setComparison] = useState<Decision[] | null>(null);
  const [aiKey, setAiKey] = useState("");
  const [aiModel, setAiModel] = useState("");

  useEffect(() => {
    try {
      setAiKey(sessionStorage.getItem(AI_KEY_STORAGE) || "");
      setAiModel(sessionStorage.getItem(AI_MODEL_STORAGE) || "");
    } catch {
      setAiKey("");
      setAiModel("");
    }
  }, [algorithm]);
  const aiProvider = detectAiProvider(aiKey);
  const aiModels =
    aiProvider === "google" ? GOOGLE_MODELS : GROQ_MODELS;
  const activeAiModel =
    aiModel && aiModels.includes(aiModel)
      ? aiModel
      : aiProvider === "google"
        ? DEFAULT_GOOGLE_MODEL
        : DEFAULT_GROQ_MODEL;

  function commit(next: Model) {
    current.current = next;
    setModel(next);
  }
  function commitDecisions(next: Decision[]) {
    decisionsRef.current = next;
    setDecisions(next);
    try {
      sessionStorage.setItem(decisionStorageKey, JSON.stringify(next));
    } catch {}
  }
  function saveDecision(decision: Decision) {
    const next = [
      ...decisionsRef.current.filter(
        (item) => item.result.algorithm !== decision.result.algorithm,
      ),
      decision,
    ].sort(
      (a, b) =>
        ALGORITHMS.findIndex((item) => item.id === a.result.algorithm) -
        ALGORITHMS.findIndex((item) => item.id === b.result.algorithm),
    );
    commitDecisions(next);
  }
  async function request<T>(
    history: string,
    mode = "state",
    direction = "",
    algo = algorithm,
    signal?: AbortSignal,
  ): Promise<T> {
    const response = await fetch("/api/game", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        levelId: level.id,
        history,
        mode,
        direction,
        algorithm: algo === "gemini" ? "astar-pushes" : algo,
        customXsb,
      }),
      signal,
    });
    const data = await response.json();
    if (!response.ok)
      throw new Error(data.error || "Не вдалося зв’язатися із сервером.");
    return data;
  }
  async function requestGemini(base: string, signal: AbortSignal) {
    const apiKey = normalizeAiKey(aiKey);
    if (!apiKey)
      throw new Error("Спочатку додай API-ключ у налаштуваннях ШІ.");
    for (let attempt = 0; attempt < 3; attempt++) {
      const response = await fetch("/api/ai", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          apiKey,
          levelId: level.id,
          history: base,
          customXsb,
          model: activeAiModel,
        }),
        signal,
      });
      const data = await response.json();
      if (response.ok) return data as SearchResult;
      const retryable = response.status === 408 || response.status === 429 || response.status >= 500;
      if (!retryable || attempt === 2)
        throw new Error(data.error || "AI не зміг побудувати маршрут.");
      await new Promise((resolve) =>
        setTimeout(resolve, 1000 * 2 ** attempt + Math.random() * 250),
      );
      signal.throwIfAborted();
    }
    throw new Error("AI не зміг побудувати маршрут.");
  }
  function cancelSearch(announce = false) {
    version.current++;
    controller.current?.abort();
    controller.current = null;
    setSearching(false);
    if (announce) setMessage("Пошук зупинено.");
  }
  function invalidate() {
    cancelSearch();
    setPlan(null);
    setPlaying(false);
  }
  async function transition(
    history: string,
    direction: string,
    redo: string[],
    playback = false,
  ) {
    if (lock.current) return false;
    lock.current = true;
    setBusy(true);
    gameController.current = new AbortController();
    try {
      const snapshot = await request<Snapshot>(
        history,
        "state",
        direction,
        algorithm,
        gameController.current.signal,
      );
      if (!mounted.current) return false;
      if (!snapshot.accepted) {
        setMessage("Цей хід заблоковано. Спробуй інший напрямок.");
        return false;
      }
      if (!playback) invalidate();
      setAnimated(playback);
      commit({ snapshot, history: history + direction, redo });
      setMessage("");
      if (direction && started.current === null)
        started.current = performance.now();
      if (snapshot.won && started.current !== null)
        setElapsed(performance.now() - started.current);
      return true;
    } catch (error) {
      if (mounted.current && (error as Error).name !== "AbortError")
        setMessage((error as Error).message);
      return false;
    } finally {
      lock.current = false;
      if (mounted.current) setBusy(false);
    }
  }
  function move(direction: Dir) {
    setPlaying(false);
    if (searching) cancelSearch();
    void transition(current.current.history, direction, []);
  }
  function undo() {
    const m = current.current;
    if (!m.history) return;
    setPlaying(false);
    void transition(m.history.slice(0, -1), "", [
      ...m.redo,
      m.history.slice(-1),
    ]);
  }
  function redo() {
    const m = current.current;
    if (!m.redo.length) return;
    setPlaying(false);
    void transition(m.history, m.redo[m.redo.length - 1], m.redo.slice(0, -1));
  }
  async function restart() {
    setPlaying(false);
    cancelSearch();
    if (await transition("", "", [])) {
      started.current = null;
      setElapsed(0);
    }
  }
  async function solve(
    compare = false,
    requestedAlgorithm = algorithm,
    requestedBase?: string,
  ) {
    if (lock.current || !current.current.snapshot) return;
    cancelSearch();
    setPlaying(false);
    setPlan(null);
    setMessage("");
    const base =
      requestedBase ??
      (current.current.snapshot?.won ? "" : current.current.history);
    const token = version.current;
    const abort = new AbortController();
    controller.current = abort;
    setSearching(true);
    try {
      const outcomes = await Promise.allSettled((compare
        ? LOCAL_ALGORITHMS.map((item) => item.id)
        : [requestedAlgorithm]).map(async (algo) => {
        const result =
          algo === "gemini"
            ? await requestGemini(base, abort.signal)
            : await request<SearchResult>(
                base,
                "solve",
                "",
                algo,
                abort.signal,
              );
        if (token !== version.current || !mounted.current) return;
        saveDecision({ result, base });
      }));
      const failed = outcomes.find(outcome => outcome.status === "rejected");
      if (failed?.status === "rejected") throw failed.reason;
    } catch (error) {
      if (token === version.current && (error as Error).name !== "AbortError")
        setMessage((error as Error).message);
    } finally {
      if (token === version.current && mounted.current) setSearching(false);
    }
  }
  async function replayStep(back = false) {
    const p = planRef.current;
    if (!p || lock.current) return;
    const nextIndex = p.index + (back ? -1 : 1);
    if (nextIndex < 0 || nextIndex > p.result.moves.length) {
      setPlaying(false);
      return;
    }
    const history = p.base + p.result.moves.slice(0, nextIndex);
    if (await transition(history, "", [], true)) {
      setPlan({ ...p, index: nextIndex });
      if (nextIndex === p.result.moves.length) setPlaying(false);
    } else setPlaying(false);
  }
  async function apply(decision: Decision) {
    const { result, base } = decision;
    if (busy || searching || !result.validated || result.status !== "Solved")
      return;
    setPlaying(false);
    if (
      current.current.history !== base &&
      !(await transition(base, "", [], true))
    )
      return;
    if (started.current === null) started.current = performance.now();
    setPlan({ result, base, index: 0 });
    setPlaying(result.moves.length > 0);
    setMessage("");
    const saved = { result, base };
    setSavedPlan(saved);
    try {
      sessionStorage.setItem(solutionStorageKey, JSON.stringify(saved));
    } catch {}
  }
  async function replaySavedSolution() {
    if (!savedPlan || busy || searching) return;
    saveDecision(savedPlan);
    await apply(savedPlan);
  }
  async function applyGuaranteedSolution() {
    if (!guaranteedSolution?.moves || busy || searching) return;
    cancelSearch();
    await apply({
      base: "",
      result: {
        algorithm: "astar-pushes",
        status: "Solved",
        moves: guaranteedSolution.moves,
        pushes: guaranteedSolution.pushes,
        searchMs: 0,
        preprocessingMs: 0,
        validationMs: 0,
        reconstructionMs: 0,
        totalMs: 0,
        explored: 0,
        generated: 0,
        deadlocks: 0,
        frontier: 0,
        validated: true,
        trace: [],
      },
    });
    setMessage("Відтворюємо рішення, перевірене під час генерації.");
  }
  function removeDecision(decision: Decision) {
    commitDecisions(
      decisionsRef.current.filter(
        (item) => item.result.algorithm !== decision.result.algorithm,
      ),
    );
    if (debugAlgorithm === decision.result.algorithm) setDebugAlgorithm(null);
    if (planRef.current?.result.algorithm === decision.result.algorithm) {
      setPlaying(false);
      setPlan(null);
    }
    if (
      savedPlan?.result.algorithm === decision.result.algorithm &&
      savedPlan.base === decision.base
    ) {
      setSavedPlan(null);
      try {
        sessionStorage.removeItem(solutionStorageKey);
      } catch {}
    }
  }
  function recalculateDecision(decision: Decision) {
    void solve(false, decision.result.algorithm, decision.base);
  }
  async function applyFromDebugger(decision: Decision) {
    await apply(decision);
    boardStage.current?.scrollIntoView({ block: "center" });
  }
  useEffect(() => {
    mounted.current = true;
    if (typeof window !== "undefined") {
      const params = new URLSearchParams(window.location.search);
      if (params.get("debug") === "1" || params.get("debug") === "true") {
        setShowDebugger(true);
      }
    }
    const abort = new AbortController();
    request<Snapshot>("", "state", "", "astar-pushes", abort.signal)
      .then((snapshot) => {
        if (!abort.signal.aborted) commit({ snapshot, history: "", redo: [] });
      })
      .catch((error) => {
        if (!abort.signal.aborted) setMessage(error.message);
      });
    const handle = (event: KeyboardEvent) => keyboard.current(event);
    window.addEventListener("keydown", handle);
    return () => {
      mounted.current = false;
      abort.abort();
      controller.current?.abort();
      gameController.current?.abort();
      window.removeEventListener("keydown", handle);
    };
    // Each instance is keyed by level, so initialization runs once per level.
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);
  useEffect(() => {
    if (!playing || !plan) return;
    const timer = setTimeout(() => {
      void replayStep();
    }, speed);
    return () => clearTimeout(timer);
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [playing, plan, speed]);
  useEffect(() => {
    if (model.snapshot?.won) return;
    const timer = setInterval(() => {
      if (started.current !== null)
        setElapsed(performance.now() - started.current);
    }, 250);
    return () => clearInterval(timer);
  }, [model.snapshot?.won]);
  useEffect(() => {
    try {
      const value = sessionStorage.getItem(decisionStorageKey);
      const parsed = value ? (JSON.parse(value) as unknown) : [];
      const restored = Array.isArray(parsed)
        ? parsed
            .map(restoreDecision)
            .filter((item): item is Decision => item !== null)
            .slice(0, ALGORITHMS.length)
        : [];
      decisionsRef.current = restored;
      setDecisions(restored);
    } catch {
      decisionsRef.current = [];
      setDecisions([]);
    }
  }, [decisionStorageKey]);
  useEffect(() => {
    try {
      const value = sessionStorage.getItem(solutionStorageKey);
      const saved = value ? restoreDecision(JSON.parse(value)) : null;
      if (saved?.result.validated && saved.result.status === "Solved")
        setSavedPlan(saved);
    } catch {}
  }, [solutionStorageKey]);
  useEffect(() => {
    document.body.classList.toggle("map-window-open", mapExpanded);
    if (mapExpanded) closeMap.current?.focus();
    return () => document.body.classList.remove("map-window-open");
  }, [mapExpanded]);
  keyboard.current = (event) => {
    if (event.key === "Escape" && mapExpanded) {
      event.preventDefault();
      setMapExpanded(false);
      return;
    }
    const target = event.target as HTMLElement;
    const key = event.key.length === 1 ? event.key.toLowerCase() : event.key;
    if (
      mapExpanded &&
      event.shiftKey &&
      !event.ctrlKey &&
      !event.metaKey &&
      !event.altKey &&
      keys[key]
    ) {
      event.preventDefault();
      const direction = keys[key];
      panCamera(
        direction === "L" ? -3 : direction === "R" ? 3 : 0,
        direction === "U" ? -3 : direction === "D" ? 3 : 0,
      );
      return;
    }
    if (
      target.closest("input, select, textarea") ||
      (target.closest("button, a") && [" ", "Enter"].includes(event.key)) ||
      target.isContentEditable ||
      event.ctrlKey ||
      event.metaKey ||
      event.altKey
    )
      return;
    if (keys[key]) {
      event.preventDefault();
      move(keys[key]);
    } else if (key === "u" || key === "г") {
      event.preventDefault();
      undo();
    } else if (key === "y" || key === "н") {
      event.preventDefault();
      redo();
    } else if (key === "r" || key === "к") {
      event.preventDefault();
      void restart();
    } else if (key === " ") {
      event.preventDefault();
      setPlaying((p) => !p && Boolean(plan));
    } else if (key === "n" || key === "т") {
      event.preventDefault();
      setPlaying(false);
      void replayStep();
    }
  };
  const index = LEVELS.findIndex((l) => l.id === level.id);
  const isCustom = level.id === "custom";
  const snapshot = model.snapshot;
  const largeBoard = Boolean(
    snapshot && (snapshot.board.width - 2 > 22 || snapshot.board.height - 2 > 22),
  );
  const debugDecision =
    showDebugger && debugAlgorithm
      ? decisions.find((item) => item.result.algorithm === debugAlgorithm) ?? null
      : null;
  function openMap() {
    if (snapshot) {
      setCamera((currentCamera) => ({
        ...currentCamera,
        centerX: currentCamera.follow
          ? (snapshot.state.player % snapshot.board.width) - 1
          : currentCamera.centerX,
        centerY: currentCamera.follow
          ? Math.floor(snapshot.state.player / snapshot.board.width) - 1
          : currentCamera.centerY,
      }));
    }
    setMapExpanded(true);
  }
  function panCamera(dx: number, dy: number) {
    setCamera((currentCamera) => ({
      ...currentCamera,
      follow: false,
      centerX:
        (currentCamera.follow && snapshot
          ? (snapshot.state.player % snapshot.board.width) - 1
          : currentCamera.centerX) + dx,
      centerY:
        (currentCamera.follow && snapshot
          ? Math.floor(snapshot.state.player / snapshot.board.width) - 1
          : currentCamera.centerY) + dy,
    }));
  }
  function zoomCamera(delta: number) {
    setCamera((currentCamera) => ({
      ...currentCamera,
      fov: Math.max(7, Math.min(41, currentCamera.fov + delta)),
    }));
  }
  return (
    <main id="main" className="page game-page">
      <div className="game-heading">
        <div>
          <Link className="back" href="/levels">
            ← Усі рівні
          </Link>
          <h1>{level.title}</h1>
          <p>{level.difficulty}</p>
        </div>
        <span className="level-counter">
          {isCustom ? "Свій" : String(index + 1).padStart(2, "0")}
          {isCustom ? null : <span> / 05</span>}
        </span>
      </div>
      <div className="play-layout">
        <section className="play-area" aria-label="Гра">
          <div className="game-stats">
            <div>
              <span>Ходи</span>
              <strong data-testid="moves">{snapshot?.moves ?? 0}</strong>
            </div>
            <div>
              <span>Штовхання</span>
              <strong data-testid="pushes">{snapshot?.pushes ?? 0}</strong>
            </div>
            <div>
              <span>Час гри</span>
              <strong>
                {(elapsed / 1000).toFixed(1)}
                <small> с</small>
              </strong>
            </div>
          </div>
          <div
            ref={boardStage}
            className={`board-stage ${largeBoard ? "large-board-stage" : ""} ${mapExpanded ? "map-expanded" : ""}`}
            role={mapExpanded ? "dialog" : undefined}
            aria-modal={mapExpanded ? true : undefined}
            aria-label={mapExpanded ? "Повноекранна карта рівня" : undefined}
          >
            {mapExpanded ? (
              <button
                ref={closeMap}
                className="button map-close"
                onClick={() => setMapExpanded(false)}
              >
                Закрити карту
              </button>
            ) : null}
            {snapshot ? (
              <BoardView
                board={snapshot.board}
                state={snapshot.state}
                animated={animated}
                camera={camera}
                onCameraPan={largeBoard ? panCamera : undefined}
                onCameraZoom={largeBoard ? zoomCamera : undefined}
              />
            ) : (
              <div className="loading-board" role="status">
                {message ? "Поле недоступне" : "Відкриваємо рівень…"}
              </div>
            )}
            {mapExpanded && largeBoard ? (
              <div className="camera-controls" aria-label="Керування камерою">
                <label className="follow-toggle">
                  <span>Стежити за гравцем</span>
                  <span className="camera-switch">
                    <input
                      type="checkbox"
                      checked={camera.follow}
                      onChange={(event) => {
                        const follow = event.target.checked;
                        setCamera((currentCamera) => ({
                          ...currentCamera,
                          follow,
                          centerX:
                            !follow && snapshot
                              ? (snapshot.state.player % snapshot.board.width) - 1
                              : currentCamera.centerX,
                          centerY:
                            !follow && snapshot
                              ? Math.floor(snapshot.state.player / snapshot.board.width) - 1
                              : currentCamera.centerY,
                        }));
                      }}
                    />
                    <i aria-hidden="true" />
                  </span>
                </label>
                <label className="fov-control">
                  <span className="camera-control-name">Огляд</span>
                  <strong>{camera.fov} × {camera.fov}</strong>
                  <input
                    aria-label="Поле огляду камери"
                    type="range"
                    min="7"
                    max="41"
                    step="2"
                    value={camera.fov}
                    onChange={(event) =>
                      setCamera((currentCamera) => ({
                        ...currentCamera,
                        fov: Number(event.target.value),
                      }))
                    }
                  />
                </label>
                <div className="camera-pad" role="group" aria-label="Переміщення камери">
                  <button aria-label="Камера вліво" onClick={() => panCamera(-5, 0)}>←</button>
                  <button aria-label="Камера вгору" onClick={() => panCamera(0, -5)}>↑</button>
                  <button aria-label="Камера вниз" onClick={() => panCamera(0, 5)}>↓</button>
                  <button aria-label="Камера вправо" onClick={() => panCamera(5, 0)}>→</button>
                </div>
                <p className="camera-hint">Перетягуй поле або натискай Shift + стрілки</p>
              </div>
            ) : null}
          </div>
          <div className="board-toolbar">
            <button
              className="button subtle"
              onClick={undo}
              disabled={busy || !model.history}
              title="U"
            >
              ↶ Скасувати
            </button>
            <button
              className="button subtle"
              onClick={redo}
              disabled={busy || !model.redo.length}
              title="Y"
            >
              ↷ Повторити
            </button>
            <button
              className="button subtle"
              onClick={() => void restart()}
              disabled={busy}
              title="R"
            >
              Спочатку
            </button>
            <button
              className="button subtle"
              onClick={openMap}
              disabled={!snapshot}
            >
              Повний екран
            </button>
          </div>
          <div className="game-bottom">
            <span>
              Рухайся <kbd>W</kbd>
              <kbd>A</kbd>
              <kbd>S</kbd>
              <kbd>D</kbd> або стрілками
            </span>
            <div className="direction-pad">
              {(["L", "U", "D", "R"] as Dir[]).map((d) => (
                <button
                  key={d}
                  aria-label={
                    { U: "Вгору", L: "Вліво", D: "Вниз", R: "Вправо" }[d]
                  }
                  disabled={busy || !snapshot}
                  onClick={() => move(d)}
                >
                  {arrows[d]}
                </button>
              ))}
            </div>
          </div>
        </section>
        <aside className="solver-panel">
          <h2>Пошук рішення</h2>
          <p>Розрахунок розв’язку з поточної позиції.</p>
          <label className="field-label" htmlFor="algorithm">
            Алгоритм
          </label>
          <select
            id="algorithm"
            value={algorithm}
            onChange={(e) => setAlgorithm(e.target.value as Algorithm)}
          >
            {ALGORITHMS.map((a) => (
              <option key={a.id} value={a.id}>
                {a.label} · {a.metric}
              </option>
            ))}
          </select>
          {algorithm === "gemini" ? (
            !aiKey.trim() ? (
              <p className="gemini-help">
                Потрібен власний ключ (Groq або Google).{" "}
                <Link href="/ai">Налаштувати AI →</Link>
              </p>
            ) : (
              <>
                <p className="gemini-help">
                  ✓ Ключ підключено (
                  {aiProvider === "google" ? "Google AI" : "Groq"}).{" "}
                  <Link href="/ai">Змінити →</Link>
                </p>
                <label className="field-label" htmlFor="ai-model">
                  Модель {aiProvider === "google" ? "Google" : "Groq (швидкі, free tier)"}
                </label>
                <select
                  id="ai-model"
                  value={activeAiModel}
                  onChange={(e) => {
                    setAiModel(e.target.value);
                    try {
                      sessionStorage.setItem(AI_MODEL_STORAGE, e.target.value);
                    } catch {}
                  }}
                >
                  {aiModels.map((m) => (
                    <option key={m} value={m}>
                      {m}
                    </option>
                  ))}
                </select>
              </>
            )
          ) : null}
          <button
            className="button primary full"
            disabled={busy || searching || !snapshot}
            onClick={() => void solve()}
          >
            {searching
              ? algorithm === "gemini"
                ? "AI думає…"
                : "Алгоритми працюють…"
              : "Обчислити вибраний"}
          </button>
          {guaranteedSolution?.moves ? (
            <div className="generator-solution">
              <button
                className="button secondary full guaranteed-solution"
                disabled={busy || searching || !snapshot}
                onClick={() => void applyGuaranteedSolution()}
                title="Повертає карту до початкової позиції"
              >
                Шлях генератора
              </button>
              <p>Перевірений під час створення карти.</p>
            </div>
          ) : null}
          {savedPlan ? (
            <div className="saved-solution">
              <span>Останнє застосоване рішення</span>
              <button
                className="button subtle full"
                disabled={busy || searching || !snapshot}
                onClick={() => void replaySavedSolution()}
              >
                Відтворити збережене
              </button>
            </div>
          ) : null}
          <div className="solver-actions">
            <button
              className="button secondary"
              disabled={busy || searching || !snapshot}
              onClick={() => void solve(true)}
            >
              Порівняти всі алгоритми
            </button>
          </div>
          {searching && (
            <button
              className="button subtle full"
              onClick={() => cancelSearch(true)}
            >
              Скасувати пошук
            </button>
          )}
          <div className="message" role="status" aria-live="polite">
            {message}
          </div>
          <span className="sr-only" role="status" data-testid="level-status">{snapshot?.won ? "Рівень завершено" : "Гра триває"}</span>
          {plan && (
            <section className="replay">
              <div className="section-line">
                <h3>Відтворення</h3>
                <span>
                  {plan.index} / {plan.result.moves.length}
                </span>
              </div>
              <p>
                {ALGORITHMS.find((a) => a.id === plan.result.algorithm)?.label}
              </p>
              <div className="replay-controls">
                <button
                  className="button secondary"
                  disabled={busy || plan.index === 0}
                  onClick={() => {
                    setPlaying(false);
                    void replayStep(true);
                  }}
                  aria-label="Попередній крок"
                >
                  ←
                </button>
                <button
                  className="button primary"
                  disabled={busy || plan.index >= plan.result.moves.length}
                  onClick={() => setPlaying((p) => !p)}
                >
                  {playing ? "Пауза" : "Продовжити"}
                </button>
                <button
                  className="button secondary"
                  disabled={busy || plan.index >= plan.result.moves.length}
                  onClick={() => {
                    setPlaying(false);
                    void replayStep();
                  }}
                  aria-label="Наступний крок"
                >
                  →
                </button>
              </div>
              <label className="speed-label">
                Інтервал між ходами <span>{speed} мс</span>
                <input
                  type="range"
                  min="160"
                  max="1000"
                  step="40"
                  value={speed}
                  onChange={(e) => setSpeed(Number(e.target.value))}
                />
              </label>
              <div className="move-strip" aria-label="Послідовність ходів">
                {plan.result.moves.split("").map((d, i) => (
                  <span
                    className={
                      i < plan.index
                        ? "done"
                        : i === plan.index
                          ? "current"
                          : ""
                    }
                    key={i}
                    title={`Хід ${i + 1}`}
                  >
                    {arrows[d]}
                  </span>
                ))}
              </div>
            </section>
          )}
          {!decisions.length && !plan && !snapshot?.won && (
            <div className="solver-empty">
              <span className="empty-route" aria-hidden="true">
                □ <span>···</span> ◎
              </span>
              <p>Оберіть алгоритм для розрахунку розв’язку.</p>
            </div>
          )}
        </aside>
      </div>
      {decisions.length > 0 && (
        <section className="results algorithm-workbench">
          <div className="section-line">
            <div>
              <h2>Рішення алгоритмів</h2>
              <p>Обери рішення для синхронного відтворення.</p>
            </div>
            <Link href="/about">Теорія алгоритмів</Link>
          </div>
          <div className="decision-grid">
            {decisions.map((decision) => {
              const result = decision.result;
              const info = ALGORITHMS.find(
                (item) => item.id === result.algorithm,
              )!;
              const isApplied = Boolean(
                result.status === "Solved" &&
                  result.validated &&
                  savedPlan &&
                  savedPlan.result.algorithm === result.algorithm &&
                  savedPlan.base === decision.base &&
                  savedPlan.result.totalMs === result.totalMs,
              );
              return (
                <article
                  className={`decision-card ${isApplied ? "applied" : ""}`}
                  key={result.algorithm}
                >
                  <div className="decision-card-top">
                    <header>
                      <div className="decision-identity">
                        <input type="checkbox" aria-label={`Порівнювати ${info.label}`} disabled={!result.validated || result.status !== "Solved"} checked={selectedComparisons.includes(result.algorithm)} onChange={e => setSelectedComparisons(previous => e.target.checked ? [...previous, result.algorithm] : previous.filter(id => id !== result.algorithm))} />
                        <div>
                          <h3>{info.label}</h3>
                          <p>{info.metric}</p>
                        </div>
                      </div>
                      <span className={`decision-status ${result.status.toLowerCase()}`}>
                        {isApplied
                          ? "Застосовано"
                          : result.status === "Solved"
                            ? "Розв’язано"
                            : result.algorithm === "gemini"
                              ? "ШІ не впорався"
                              : result.status === "LimitReached"
                                ? "Досягнуто ліміт"
                                : result.status === "NoSolution"
                                  ? "Немає рішення"
                                  : "Помилка"}
                      </span>
                    </header>
                    {result.status === "Solved" ? (
                      <div className="decision-score">
                        <div><strong>{result.moves.length}</strong><span>ходів</span></div>
                        <i aria-hidden="true" />
                        <div><strong>{result.pushes}</strong><span>штовхань</span></div>
                      </div>
                    ) : (
                      <p className="decision-error">
                        {result.status === "LimitReached" && guaranteedSolution?.moves
                          ? "Бюджет пошуку вичерпано. Шлях генератора все ще доступний."
                          : result.algorithm === "gemini"
                            ? "AI не зміг знайти перевірене рішення. Результат не можна застосувати."
                            : failures[result.status]}
                      </p>
                    )}
                  </div>
                  <dl className="decision-metrics">
                    <div>
                      <dt>{result.algorithm === "gemini" ? "Відповідь AI" : "Пошук"}</dt>
                      <dd>{(result.remoteMs ?? result.searchMs).toFixed(3)} мс</dd>
                    </div>
                    <div>
                      <dt>Досліджено</dt>
                      <dd>{result.algorithm === "gemini" ? "—" : result.explored.toLocaleString("uk-UA")}</dd>
                    </div>
                    <div>
                      <dt>Згенеровано</dt>
                      <dd>{result.algorithm === "gemini" ? "—" : result.generated.toLocaleString("uk-UA")}</dd>
                    </div>
                    <div>
                      <dt>Макс. черга</dt>
                      <dd>{result.algorithm === "gemini" ? "—" : result.frontier.toLocaleString("uk-UA")}</dd>
                    </div>
                    <div>
                      <dt>Тупики</dt>
                      <dd>{result.algorithm === "gemini" ? "—" : result.deadlocks.toLocaleString("uk-UA")}</dd>
                    </div>
                    <div>
                      <dt>Перевірка</dt>
                      <dd>{result.validated ? "Пройдено" : "Немає"}</dd>
                    </div>
                  </dl>
                  {result.explanation ? (
                    <p className="decision-explanation">{result.explanation}</p>
                  ) : null}
                  {result.algorithm === "gemini" && result.aiSession ? (
                    <details className="ai-session">
                      <summary>Переглянути сесію AI</summary>
                      <div className="ai-session-grid">
                        <section>
                          <h4>Запит · {result.aiSession.model}</h4>
                          <pre>{result.aiSession.prompt}</pre>
                        </section>
                        <section>
                          <h4>Сира відповідь і кроки інструментів</h4>
                          <pre>{result.aiSession.response}</pre>
                        </section>
                      </div>
                    </details>
                  ) : null}
                  <footer className="decision-footer">
                    <p className="decision-origin">
                      Старт · після {decision.base.length} ходів
                    </p>
                    <div className="decision-actions">
                      {result.status === "Solved" ? (
                        <button
                          className="button primary"
                          disabled={busy || searching}
                          onClick={() => void apply(decision)}
                        >
                          Застосувати
                        </button>
                      ) : null}
                      {showDebugger ? (
                        <button
                          className="button secondary"
                          disabled={searching || result.trace.length === 0}
                          onClick={() => setDebugAlgorithm(result.algorithm)}
                        >
                          Дебаг
                        </button>
                      ) : null}
                    </div>
                    <div className="decision-management">
                      <button
                        className="button subtle"
                        disabled={busy || searching}
                        onClick={() => recalculateDecision(decision)}
                      >
                        ↻ Перерахувати
                      </button>
                      <button
                        className="button subtle danger"
                        disabled={busy || searching}
                        onClick={() => removeDecision(decision)}
                      >
                        Видалити
                      </button>
                    </div>
                  </footer>
                </article>
              );
            })}
          </div>
          <div className="comparison-launch">
            <button className="button secondary" disabled={busy || searching || decisions.filter(d => selectedComparisons.includes(d.result.algorithm)).length < 2} onClick={() => {
              const selected = decisions.filter(d => selectedComparisons.includes(d.result.algorithm) && d.result.validated && d.result.status === "Solved");
              if (new Set(selected.map(d => d.base)).size !== 1) { setMessage("Для порівняння обчисли алгоритми з однакової позиції кнопкою «Порівняти всі алгоритми»."); return; }
              setPlaying(false); setDebugAlgorithm(null); setComparison([...selected]);
            }}>Порівняти вибрані поруч</button>
          </div>
          {comparison && <ComparisonReplay key={comparison.map(d => `${d.result.algorithm}-${d.result.totalMs}-${d.base}`).join("|")} decisions={comparison} load={(d, step, signal) => request<Snapshot>(d.base + d.result.moves.slice(0, step), "state", "", d.result.algorithm, signal)} onClose={() => setComparison(null)} />}
          {showDebugger && debugDecision ? (
            <SearchDebugger
              key={`${debugDecision.result.algorithm}-${debugDecision.result.totalMs}`}
              result={debugDecision.result}
              baseLength={debugDecision.base.length}
              loadSnapshot={(step) =>
                request<Snapshot>(
                  debugDecision.base + debugDecision.result.moves.slice(0, step),
                  "state",
                  "",
                  debugDecision.result.algorithm,
                )
              }
              onApply={() => void applyFromDebugger(debugDecision)}
              onRecalculate={() => recalculateDecision(debugDecision)}
              onClose={() => setDebugAlgorithm(null)}
            />
          ) : null}
        </section>
      )}
    </main>
  );
}
