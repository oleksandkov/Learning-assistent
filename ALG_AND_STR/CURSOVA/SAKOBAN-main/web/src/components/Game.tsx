"use client";
import Link from "next/link";
import { useEffect, useMemo, useRef, useState } from "react";
import BoardView from "./BoardView";
import type { BoardCamera } from "./BoardView";
import SearchDebugger from "./SearchDebugger";
import ComparisonReplay from "./ComparisonReplay";
import EvolutionViewer from "./EvolutionViewer";
import FlyBrainViewer from "./FlyBrainViewer";
import { brainConfig, compactFlyBrain, DEFAULT_BRAIN_CONFIG, restoreFlyBrain } from "@/lib/flybrain";
import type { FlyBrainEvent } from "@/workers/flybrain.worker";
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
    result: { ...(result as SearchResult), trace, flybrain: restoreFlyBrain(result.flybrain) },
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
  const [liveBrain, setLiveBrain] = useState<Decision | null>(null);
  const [brainSettings, setBrainSettings] = useState(DEFAULT_BRAIN_CONFIG);
  const [retryBrain, setRetryBrain] = useState(false);
  const retryBrainRef = useRef(false);
  retryBrainRef.current = retryBrain;
  const nextBrainSeed = useRef(8);
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
  const [sortBy, setSortBy] = useState<"default" | "time" | "moves" | "pushes">("default");
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
      sessionStorage.setItem(decisionStorageKey, JSON.stringify(next.map(compactDecision)));
    } catch {}
  }
  function compactDecision(decision: Decision): Decision {
    return decision.result.flybrain ? { ...decision, result: { ...decision.result,
      flybrain: compactFlyBrain(decision.result.flybrain) } } : decision;
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
        algorithm: algo === "gemini" || algo === "flybrain" ? "astar-pushes" : algo,
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
  async function requestFlyBrain(base: string, signal: AbortSignal): Promise<SearchResult> {
    const config = brainConfig(brainSettings);
    const previous = decisionsRef.current.find(d => d.result.algorithm === "flybrain")?.result.flybrain;
    nextBrainSeed.current = Math.max(nextBrainSeed.current, (previous?.seed ?? 7) + 1);
    let attempt = 0;
    while (true) {
    const seed = nextBrainSeed.current++ >>> 0;
    attempt++;
    const result = await new Promise<SearchResult>((resolve, reject) => {
      signal.throwIfAborted();
      const worker = new Worker(new URL("../workers/flybrain.worker.ts", import.meta.url));
      const cleanup = () => { signal.removeEventListener("abort", cancel); worker.terminate(); };
      const cancel = () => { worker.postMessage({ type: "abort" }); cleanup(); reject(new DOMException("Пошук скасовано", "AbortError")); };
      signal.addEventListener("abort", cancel, { once: true });
      worker.onerror = () => { cleanup(); reject(new Error("Не вдалося запустити FlyBrain. Спробуйте ще раз.")); };
      worker.onmessage = (event: MessageEvent<FlyBrainEvent>) => {
        if (signal.aborted) return;
        const message = event.data;
        if (message.type === "error") { cleanup(); reject(new Error(message.message)); }
        else if (message.type === "done") { cleanup(); resolve(message.result); }
        else setLiveBrain({ result: message.result, base });
      };
      worker.postMessage({ levelId: level.id, customXsb, base, seed, config, attempt });
    });
    signal.throwIfAborted();
    saveDecision({ result, base });
    setLiveBrain({ result, base });
    if (!retryBrainRef.current || result.status !== "LimitReached") return result;
    // Retry from the same base with a new seeded network; never continue a deadlocked route.
    }
  }
  function cancelSearch(announce = false) {
    version.current++;
    controller.current?.abort();
    controller.current = null;
    setSearching(false);
    setLiveBrain(null);
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
            : algo === "flybrain"
              ? await requestFlyBrain(base, abort.signal)
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
      if (token === version.current && mounted.current) { setSearching(false); setLiveBrain(null); }
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
    const partialBrain = result.algorithm === "flybrain" && result.status === "LimitReached" && result.moves.length > 0;
    if (lock.current || busy || searching || !result.validated || (result.status !== "Solved" && !partialBrain))
      return;
    setPlaying(false);
    if (result.algorithm === "flybrain") {
      lock.current = true;
      setBusy(true);
      gameController.current = new AbortController();
      try {
        const checked = await request<Snapshot>(base + result.moves, "state", "", "astar-pushes", gameController.current.signal);
        if (!mounted.current) return;
        if (!/^[UDLR]*$/.test(result.moves) || !checked.accepted || result.status === "Solved" && !checked.won)
          throw new Error("Маршрут FlyBrain не пройшов перевірку.");
      } catch (error) {
        if (!mounted.current || (error as Error).name === "AbortError") return;
        saveDecision({ ...decision, result: { ...result, validated: false, status: "InternalError" } });
        setMessage((error as Error).message); return;
      } finally {
        lock.current = false;
        if (mounted.current) setBusy(false);
      }
    }
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
      sessionStorage.setItem(solutionStorageKey, JSON.stringify(compactDecision(saved)));
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
      target.closest("input, select, textarea, .flybrain-viewer") ||
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
  const visibleDecisions = useMemo(() => {
    const merged = liveBrain
      ? decisions.some((d) => d.result.algorithm === "flybrain")
        ? decisions.map((d) => (d.result.algorithm === "flybrain" ? liveBrain : d))
        : [...decisions, liveBrain]
      : decisions;
    if (sortBy === "default") return merged;
    const value = (d: Decision) =>
      sortBy === "time"
        ? d.result.totalMs
        : sortBy === "moves"
          ? d.result.moves.length
          : d.result.pushes;
    return [...merged].sort((a, b) => value(a) - value(b));
  }, [decisions, liveBrain, sortBy]);
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
                {a.label}
              </option>
            ))}
          </select>
          {(algorithm === "flybrain" || liveBrain) && <fieldset className="flybrain-settings">
            <legend>Налаштування FlyBrain</legend>
            <label>Ліміт кроків<input aria-label="Ліміт кроків FlyBrain" type="number" min={1} max={2000} value={brainSettings.maxSteps} disabled={searching}
              onChange={e => setBrainSettings(s => ({ ...s, maxSteps: Number(e.target.value) }))} onBlur={() => setBrainSettings(brainConfig(brainSettings))} /></label>
            <label>Вікно нейронів, мс<input aria-label="Вікно нейронів, мс" type="number" min={1} max={100} value={brainSettings.windowMs} disabled={searching}
              onChange={e => setBrainSettings(s => ({ ...s, windowMs: Number(e.target.value) }))} onBlur={() => setBrainSettings(brainConfig(brainSettings))} /></label>
            <label>Час спроби, с<input aria-label="Час спроби, с" type="number" min={1} max={120} value={brainSettings.maxWallMs / 1000} disabled={searching}
              onChange={e => setBrainSettings(s => ({ ...s, maxWallMs: Number(e.target.value) * 1000 }))} onBlur={() => setBrainSettings(brainConfig(brainSettings))} /></label>
            <label>Сенсорне підсилення<input aria-label="Сенсорне підсилення" type="number" min={1} max={20} value={brainSettings.sensoryGain} disabled={searching}
              onChange={e => setBrainSettings(s => ({ ...s, sensoryGain: Number(e.target.value) }))} onBlur={() => setBrainSettings(brainConfig(brainSettings))} /></label>
            <label className="flybrain-retry"><input type="checkbox" checked={retryBrain} onChange={e => setRetryBrain(e.target.checked)} />Повторювати до розв’язання</label>
            <div className="flybrain-settings-help" aria-label="Пояснення налаштувань FlyBrain">
              <details>
                <summary>Ліміт кроків — що змінює?</summary>
                <p>Максимальна кількість рішень агента за одну спробу. Заблоковані ходи теж враховуються.</p>
                <p><strong>Збільшити:</strong> більше можливостей дослідити поле й знайти довший шлях, але й більше повторних ходів та обчислень.</p>
                <p><strong>Зменшити:</strong> спроба завершиться раніше, проте може обірвати перспективний шлях. Обмеження часу може зупинити її ще до ліміту кроків.</p>
              </details>
              <details>
                <summary>Вікно нейронів — що змінює?</summary>
                <p>Час роботи нейронної симуляції перед вибором кожного напрямку. Це модельні мілісекунди; швидкість відтворення ходів налаштовується окремо.</p>
                <p><strong>Збільшити:</strong> сигнали мають більше часу поширитися, а нейрони — накопичити імпульси. Обчислення кожного кроку триватимуть довше; кращий результат не гарантовано.</p>
                <p><strong>Зменшити:</strong> менше обчислень на крок, але моторні нейрони можуть не встигнути спрацювати. Якщо вони мовчать, агент пробує випадковий напрямок.</p>
              </details>
              <details>
                <summary>Час спроби — що змінює?</summary>
                <p>Ліміт реального часу обчислення однієї спроби в секундах. Перемога або ліміт кроків можуть завершити її раніше. Кожна повторна спроба отримує цей час заново.</p>
                <p><strong>Збільшити:</strong> більше часу для повільних обчислень, але ліміт кроків залишається тим самим.</p>
                <p><strong>Зменшити:</strong> швидше завершення та перехід до нової спроби, якщо повторення ввімкнено; менше часу на пошук у поточній спробі.</p>
              </details>
              <details>
                <summary>Сенсорне підсилення — що змінює?</summary>
                <p>Множник сили сигналів від сенсорних нейронів до центральних. Кількість нейронів залишається незмінною.</p>
                <p><strong>Збільшити:</strong> центральні нейрони легше збуджуються від сигналів поля. Надмірне підсилення може спричинити зайву активність і не покращити вибір ходів.</p>
                <p><strong>Зменшити:</strong> слабший вплив сенсорних сигналів; центральні й моторні нейрони можуть частіше мовчати, а агент — обирати випадкові напрямки.</p>
              </details>
              <details>
                <summary>Повторення — як працює?</summary>
                <p><strong>Увімкнено:</strong> після вичерпання ліміту запускається нова спроба з тієї самої початкової позиції та новим зерном випадковості (seed). Знайдене й перевірене рішення зупиняє повторення та показує завершене поле.</p>
                <p><strong>Вимкнено:</strong> виконується лише поточна спроба. Якщо вимкнути під час роботи, вона завершиться без наступного запуску. Щоб зупинити її одразу, натисніть «Скасувати».</p>
                <p>Повторення не гарантує розв’язання. Помилка також зупиняє пошук.</p>
              </details>
            </div>
            <p>Кожна спроба має новий seed і той самий старт. Працює до перемоги або скасування; рішення не гарантоване. Вікно — час симуляції на хід, темп перегляду налаштовується окремо.</p>
          </fieldset>}
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
        </aside>
      </div>
      {(decisions.length > 0 || liveBrain) && (
        <section className="results algorithm-workbench">
          <div className="section-line">
            <h2>Рішення алгоритмів</h2>
            <label style={{ display: "flex", gap: 8, alignItems: "center", fontSize: 12 }}>
              Сортувати
              <select
                aria-label="Сортувати рішення"
                value={sortBy}
                onChange={(e) => setSortBy(e.target.value as typeof sortBy)}
              >
                <option value="default">За замовчуванням</option>
                <option value="time">За часом</option>
                <option value="moves">За ходами</option>
                <option value="pushes">За штовханнями</option>
              </select>
            </label>
          </div>
          <div className="decision-grid">
            {visibleDecisions.map((decision) => {
              const result = decision.result;
              const brainRunning = result.algorithm === "flybrain" && Boolean(liveBrain) && searching;
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
                  className={`decision-card ${isApplied ? "applied" : ""} ${brainRunning ? "flybrain-live" : ""}`}
                  key={result.algorithm}
                >
                  <div className="decision-card-top">
                    <header>
                      <div className="decision-identity">
                        <input type="checkbox" aria-label={`Порівнювати ${info.label}`} disabled={result.algorithm === "flybrain" || !result.validated || result.status !== "Solved"} checked={selectedComparisons.includes(result.algorithm)} onChange={e => setSelectedComparisons(previous => e.target.checked ? [...previous, result.algorithm] : previous.filter(id => id !== result.algorithm))} />
                        <div>
                          <h3>{info.label}</h3>
                          <p>{info.metric}</p>
                        </div>
                      </div>
                      <span className={`decision-status ${result.status.toLowerCase()}`}>
                        {brainRunning ? `Спроба ${result.flybrain?.attempt ?? 1}` : isApplied
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
                            : brainRunning ? "Симуляція триває в цьому спостерігачі." : failures[result.status]}
                      </p>
                    )}
                  </div>
                  <dl className="decision-metrics">
                    <div>
                      <dt>{result.algorithm === "gemini" ? "Відповідь AI" : result.algorithm === "flybrain" ? "Симуляція" : "Пошук"}</dt>
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
                      <dt>{result.algorithm === "flybrain" ? "Заблоковано" : "Тупики"}</dt>
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
                  {result.evolution ? (
                    <details className="evolution-observer">
                      <summary>{{ aco: "Спостерігати за мурахами", gravity: "Спостерігати за потоками", cannibal: "Спостерігати за турнірами", genetic: "Спостерігати за еволюцією" }[result.evolution.kind]}</summary>
                      <EvolutionViewer result={result} loadSnapshot={(moves) =>
                        request<Snapshot>(decision.base + moves, "state", "", result.algorithm)} />
                    </details>
                  ) : null}
                  {result.flybrain && <details className="flybrain-observer" open>
                    <summary>Спостерігати за мозком</summary>
                    <FlyBrainViewer result={result} running={brainRunning}
                      recalculate={() => recalculateDecision(decision)} cancel={() => cancelSearch(true)}
                      retryUntilSolved={retryBrain} onRetryChange={setRetryBrain}
                      loadSnapshot={(moves, signal) => request<Snapshot>(decision.base + moves, "state", "", "astar-pushes", signal)}
                      baseline={decisions.find(d => d.base === decision.base && d.result.algorithm === "astar-pushes")?.result}
                      compare={searching ? undefined : () => void solve(false, "astar-pushes", decision.base)} />
                  </details>}
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
                    <div className="decision-actions">
                      {result.status === "Solved" || result.algorithm === "flybrain" && result.status === "LimitReached" && result.moves.length > 0 ? (
                        <button
                          className="button primary"
                          disabled={busy || searching || !result.validated}
                          onClick={() => void apply(decision)}
                        >
                          {result.status === "Solved" ? "Застосувати" : "Застосувати частковий маршрут"}
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
                      {result.algorithm !== "flybrain" && <button
                        className="button subtle"
                        disabled={busy || searching}
                        onClick={() => recalculateDecision(decision)}
                      >
                        ↻ Перерахувати
                      </button>}
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
