export type Dir = "U" | "L" | "D" | "R";
// Теоретична складність алгоритму (найгірший випадок, якщо не вказано інше).
// kind — клас складності простою мовою, depends — з чого саме рахується,
// rank — «наскільки просто за теорією»: 1 = найпростіша. Спочатку клас
// (лінійна → поліномна → експоненційна), далі менша пам'ять і простіша ідея.
export type Complexity = { kind: string; time: string; space: string; depends: string; rank: number };
export type LocalAlgorithm = "bfs" | "astar-moves" | "astar-pushes" | "idastar-pushes" | "greedy-pushes" | "prototype1" | "aco" | "genetic" | "gravity" | "cannibal";
export type Algorithm = LocalAlgorithm | "gemini" | "flybrain";
// Спільні підказки: від чого насправді залежить складність — розмір поля,
// кількість ящиків, довжина розв'язку, а не лише абстрактні символи.
const DEPTH_DEPENDS =
  "Станів близько C(V, k) · V — V клітинок поля, k ящиків: кожне розташування ящиків на полі це окремий стан. Більше поле або більше ящиків → час і пам'ять ростуть вибухово. d — кількість ходів у розв'язку";
const GEN_DEPENDS =
  "Пропорційно поколінням g × популяції n × довжині хромосоми L. L — план на всі ящики одразу, тож більше ящиків → довша хромосома і дорожче кожне покоління";
export const LOCAL_ALGORITHMS: {
  id: LocalAlgorithm;
  label: string;
  metric: string;
  complexity: Complexity;
}[] = [
  { id: "bfs", label: "1 - BFS", metric: "Мінімум ходів", complexity: { kind: "Експоненційна — станів стає вибухово більше з глибиною", time: "O(bᵈ)", space: "O(bᵈ)", depends: DEPTH_DEPENDS, rank: 11 } },
  { id: "astar-moves", label: "2 - A* ходи", metric: "Мінімум ходів", complexity: { kind: "Експоненційна в найгіршому разі, на практиці евристика ріже b", time: "O(bᵈ)", space: "O(bᵈ)", depends: DEPTH_DEPENDS + ". Евристика (скільки залишилось ходів) відсікає частину станів", rank: 9 } },
  { id: "astar-pushes", label: "3 - A* штовхання", metric: "Мінімум штовхань", complexity: { kind: "Експоненційна в найгіршому разі, на практиці евристика ріже b", time: "O(bᵈ)", space: "O(bᵈ)", depends: DEPTH_DEPENDS + ". Евристика (скільки залишилось штовхань) відсікає частину станів", rank: 10 } },
  { id: "idastar-pushes", label: "4 - IDA*", metric: "Мінімум штовхань · мало пам'яті", complexity: { kind: "Експоненційна за часом, пам'ять тримає лише шлях", time: "O(bᵈ)", space: "O(d)", depends: DEPTH_DEPENDS + ". Час той самий, що в BFS, але в пам'яті лише поточний шлях — рахується від глибини d", rank: 7 } },
  { id: "greedy-pushes", label: "5 - Greedy", metric: "Швидко · неоптимально", complexity: { kind: "Експоненційна, але на практиці найшвидша — іде наосліп", time: "O(bᵐ)", space: "O(bᵐ)", depends: DEPTH_DEPENDS + ". Іде просто до цілі без обліку ящиків, тому часто швидша за BFS, але може зайти в глухий кут", rank: 8 } },
  { id: "prototype1", label: "6 - X → Y", metric: "Навчальний · стіни ігноруються", complexity: { kind: "Лінійна — одне обчислення на клітинку", time: "O(V + E)", space: "O(V)", depends: "Лінійно від розміру поля: V — клітинки рівня, E — переходи між ними. Кожен ящик обробляється один раз, поле проходимо одним обходом", rank: 1 } },
  { id: "aco", label: "7 - ACO", metric: "Мурахи · феромонні стежки", complexity: { kind: "Поліномна — росте пропорційно ітераціям і мурахам", time: "O(I · A · d)", space: "O(I · A)", depends: "Ітерації I × мурахи A × довжина шляху d. Шлях росте з кількістю ящиків і розміром поля; пам'ять — феромони на кожній клітинці", rank: 4 } },
  { id: "genetic", label: "8 - GA", metric: "Хромосоми · еволюція", complexity: { kind: "Поліномна — росте пропорційно поколінням і популяції", time: "O(g · n · L)", space: "O(n · L)", depends: GEN_DEPENDS, rank: 5 } },
  { id: "gravity", label: "9 - Drain", metric: "Рельєф · потік і струшування", complexity: { kind: "Лінійна за крок — щоходу оновлюємо все поле", time: "O(k · V)", space: "O(V)", depends: "Кроки k × клітинки V: щоходу перераховуємо все поле, тож велике поле уповільнює кожен крок. Кількість ящиків на час майже не впливає", rank: 2 } },
  { id: "cannibal", label: "10 - Cannibal GA", metric: "Турніри · перенесення ДНК", complexity: { kind: "Поліномна — росте пропорційно поколінням і популяції", time: "O(g · n · L)", space: "O(n · L)", depends: GEN_DEPENDS, rank: 6 } },
];
export const ALGORITHMS: { id: Algorithm; label: string; metric: string; complexity: Complexity }[] = [
  ...LOCAL_ALGORITHMS,
  { id: "flybrain", label: "11 - FlyBrain", metric: "Біо-агент · web only", complexity: { kind: "Евристична — пропорційна кількості нейронів на крок", time: "O(n · d*)", space: "O(n)", depends: "Нейрони n × кроки шляху d*. Ящики впливають лише через довжину плану, а розмір поля — через кількість входів агента", rank: 3 } },
  { id: "gemini", label: "12 - AI", metric: "План перевіряє ядро", complexity: { kind: "Не визначається — план будує зовнішня модель", time: "—", space: "—", depends: "Від розміру поля тут не залежить: час — це відповідь API, пам'ять — промпт із описом рівня", rank: 12 } },
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
  inhibited?: Dir[]; // C++-rejected directions in this exact board state.
  behavior?: "MOVE" | "SEARCH" | "PUSH" | "PAUSE";
  ring?: number[];
  octopamine?: number;
  zones?: [number, number, number, number]; // KC+MBON, CX, FB, PB spikes
  targetDrive?: number[];
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
  memorySessions?: number;
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

export interface FlyBrainMemory {
  version: 1;
  sessions: number;
  weights: number[]; // Quantized KC→MBON weights only; stable topology, <=50 KiB.
}
