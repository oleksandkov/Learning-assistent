export type Dir = "U" | "L" | "D" | "R";
export type LocalAlgorithm = "bfs" | "astar-moves" | "astar-pushes" | "idastar-pushes" | "greedy-pushes" | "prototype1" | "aco" | "genetic" | "gravity" | "cannibal";
export type Algorithm = LocalAlgorithm | "gemini" | "flybrain";
export const LOCAL_ALGORITHMS: {
  id: LocalAlgorithm;
  label: string;
  metric: string;
}[] = [
  { id: "bfs", label: "1 - BFS", metric: "Мінімум ходів" },
  { id: "astar-moves", label: "2 - A* ходи", metric: "Мінімум ходів" },
  { id: "astar-pushes", label: "3 - A* штовхання", metric: "Мінімум штовхань" },
  { id: "idastar-pushes", label: "4 - IDA*", metric: "Мінімум штовхань · мало пам'яті" },
  { id: "greedy-pushes", label: "5 - Greedy", metric: "Швидко · неоптимально" },
  { id: "prototype1", label: "6 - X → Y", metric: "Навчальний · стіни ігноруються" },
  { id: "aco", label: "7 - ACO", metric: "Мурахи · феромонні стежки" },
  { id: "genetic", label: "8 - GA", metric: "Хромосоми · еволюція" },
  { id: "gravity", label: "9 - Drain", metric: "Рельєф · потік і струшування" },
  { id: "cannibal", label: "10 - Cannibal GA", metric: "Турніри · перенесення ДНК" },
];
export const ALGORITHMS: { id: Algorithm; label: string; metric: string }[] = [
  ...LOCAL_ALGORITHMS,
  { id: "flybrain", label: "11 - FlyBrain", metric: "Біо-агент · web only" },
  { id: "gemini", label: "12 - AI", metric: "План перевіряє ядро" },
];
export const AI_KEY_STORAGE = "sokoban.ai-key";
export const AI_MODEL_STORAGE = "sokoban.ai-model";
// Швидкі моделі Groq (https://api.groq.com, free tier без картки).
export const GROQ_MODELS = [
  "openai/gpt-oss-120b",
  "llama-3.3-70b-versatile",
  "moonshotai/kimi-k2-instruct",
  "qwen/qwen3-32b",
  "openai/gpt-oss-20b",
  "llama-3.1-8b-instant",
];
export const GOOGLE_MODELS = [
  "gemini-3.8-flash",
  "gemini-3.7-flash",
  "gemini-3.6-flash",
  "gemini-3.5-flash",
  "gemini-3.5-flash-lite",
  "gemini-3.1-pro",
  "gemini-3-flash",
];
export const DEFAULT_GROQ_MODEL = "openai/gpt-oss-120b";
export const DEFAULT_GOOGLE_MODEL = "gemini-3.8-flash";
export function detectAiProvider(key: string): "google" | "groq" {
  const k = normalizeAiKey(key);
  // Google: legacy AIza... + нові Auth keys AQ. (AI Studio видає лише AQ).
  return k.startsWith("AIza") || k.startsWith("AQ.") ? "google" : "groq";
}
// Копіювання з лапками ("key", 'key') або пробілами — чистимо перед аналізом.
export function normalizeAiKey(key: string): string {
  let s = (key || "").trim();
  if (
    s.length >= 2 &&
    ((s.startsWith('"') && s.endsWith('"')) ||
      (s.startsWith("'") && s.endsWith("'")))
  ) {
    s = s.slice(1, -1).trim();
  }
  return s;
}
export interface Board {
  width: number;
  height: number;
  walls: boolean[];
  goals: boolean[];
  floor: boolean[];
}
export interface State {
  player: number;
  boxes: number[];
}
export interface Snapshot {
  board: Board;
  state: State;
  moves: number;
  pushes: number;
  won: boolean;
  accepted: boolean;
}
export interface SearchTracePoint {
  explored: number;
  frontier: number;
  elapsedMs: number;
  status: "Running" | "Solved" | "NoSolution" | "LimitReached" | "InternalError";
  state?: State;
  g?: number;
  h?: number;
  depth?: number;
  hasTransition?: boolean;
  pushed?: boolean;
  direction?: Dir;
  boxFrom?: number | null;
  boxTo?: number | null;
}
export interface SearchResult {
  algorithm: Algorithm;
  status: "Solved" | "NoSolution" | "LimitReached" | "InternalError";
  moves: string;
  pushes: number;
  searchMs: number;
  preprocessingMs: number;
  validationMs: number;
  reconstructionMs: number;
  totalMs: number;
  explored: number;
  generated: number;
  deadlocks: number;
  frontier: number;
  validated: boolean;
  trace: SearchTracePoint[];
  evolution?: EvolutionHistory;
  flybrain?: FlyBrainHistory;
  remoteMs?: number;
  explanation?: string;
  aiSession?: {
    model: string;
    prompt: string;
    response: string;
  };
}
export interface FlyBrainSpikeFrame {
  t: number;
  dir: Dir;
  accepted: boolean;
  sensory: number;
  central: number;
  motor: [number, number, number, number];
  samples: number[]; // Actual spikes from 40 evenly spaced neurons in each group.
  active?: number[]; // Actual neuron IDs that fired in this window (for the 3D view).
  won: boolean;
  goals?: number; // Fraction on goals, sampled every ten steps by the C++ snapshot.
  dopamine?: number;
  heading?: number;
  compassDir?: Dir;
  deadlockAverted?: boolean;
  rays?: number[];
  ghostTrace?: string;
  macroIntent?: string;
}
export interface FlyBrainHistory {
  kind: "flybrain";
  neurons: number;
  steps: number;
  frames: FlyBrainSpikeFrame[];
  spikeRate: number;
  loops: number;
  seed: number;
  config?: FlyBrainConfig;
  attempt?: number;
}
export interface FlyBrainConfig {
  maxSteps: number;
  windowMs: number;
  maxWallMs: number;
  sensoryGain: number;
  enablePlasticity?: boolean;
  enableNociception?: boolean;
  enableRays?: boolean;
}
export interface EvolutionIndividual {
  id: number;
  moves: string;
  genes?: string;
  cost: number;
  pushes?: number;
  won: boolean;
  parentA?: number;
  parentB?: number;
  mutationIndex?: number;
  shakes?: number;
  consumedId?: number;
  fatalGene?: number;
  validPrefix?: number;
}
export interface EvolutionHistory {
  kind: "aco" | "genetic" | "gravity" | "cannibal";
  generations: EvolutionIndividual[][];
}
export interface GeneratedLevel {
  success: boolean;
  xsb: string;
  width: number;
  height: number;
  boxes: number;
  walls: number;
  attempts: number;
  seed: string;
  moves: number;
  pushes: number;
  solution: string;
  error?: string;
}
export interface Level {
  id: string;
  title: string;
  difficulty: string;
  file: string;
  note: string;
}
