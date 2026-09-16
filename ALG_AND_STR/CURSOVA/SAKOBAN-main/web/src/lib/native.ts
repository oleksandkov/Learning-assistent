import { spawn } from "node:child_process";
import { readFile } from "node:fs/promises";
import path from "node:path";
import { LEVELS } from "./levels";
import type {
  GeneratedLevel,
  LocalAlgorithm,
  SearchResult,
  Snapshot,
} from "./types";

const root = path.resolve(process.cwd(), "..");
const bin = path.join(
  root,
  "bindings/web/build",
  process.platform === "win32" ? "sokoban_web.exe" : "sokoban_web",
);

function runNative<T>(
  args: string[],
  input: string,
  signal?: AbortSignal,
  timeoutMs = 12000,
): Promise<T> {
  return new Promise((resolve, reject) => {
    if (signal?.aborted) return reject(new Error("Пошук скасовано."));
    const child = spawn(bin, args, { cwd: root, windowsHide: true });
    let output = "";
    let errorOutput = "";
    const stop = () => child.kill();
    const timeout = setTimeout(stop, timeoutMs);
    const cleanup = () => {
      clearTimeout(timeout);
      signal?.removeEventListener("abort", stop);
    };
    signal?.addEventListener("abort", stop, { once: true });
    child.stdout.on("data", (chunk) => {
      output += chunk;
    });
    child.stderr.on("data", (chunk) => {
      errorOutput += chunk;
    });
    child.stdin.on("error", () => {});
    child.on("error", () => {
      cleanup();
      reject(
        new Error(
          "Не вдалося запустити ядро. Виконай npm run native у папці web.",
        ),
      );
    });
    child.on("close", (code) => {
      cleanup();
      if (code !== 0)
        return reject(
          new Error(
            signal?.aborted
              ? "Пошук скасовано."
              : errorOutput || "Ядро не завершило запит. Спробуй ще раз.",
          ),
        );
      try {
        resolve(JSON.parse(output));
      } catch (error) {
        reject(error);
      }
    });
    child.stdin.end(input);
  });
}

export async function nativeRequest(
  levelId: string,
  history = "",
  mode: "state" | "solve" = "state",
  algorithm: LocalAlgorithm = "astar-pushes",
  direction = "",
  signal?: AbortSignal,
  customXsb?: string,
): Promise<Snapshot | SearchResult> {
  const level = LEVELS.find((l) => l.id === levelId);
  if (!level && levelId !== "custom") throw new Error("Рівень не знайдено.");
  const xsb = level
    ? await readFile(path.join(root, "levels", level.file), "utf8")
    : customXsb;
  if (!xsb) throw new Error("Власний рівень не знайдено.");
  const data = await runNative<Snapshot | SearchResult | { error: string }>(
    [mode, algorithm, history, direction],
    xsb,
    signal,
    mode === "solve" && levelId === "custom" ? 40000 : 12000,
  );
  if ("error" in data)
    throw new Error("Історія ходів недійсна. Перезапусти рівень.");
  return mode === "solve" ? { ...data, algorithm } : data;
}

export async function generateNativeLevel(
  width: number,
  height: number,
  boxes: number,
  walls: number,
  seed: string,
  signal?: AbortSignal,
): Promise<GeneratedLevel> {
  return runNative<GeneratedLevel>(
    ["generate", String(width), String(height), String(boxes), String(walls), seed],
    "",
    signal,
    30000,
  );
}
