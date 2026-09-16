"use client";

import Link from "next/link";
import { useEffect, useMemo, useRef, useState } from "react";
import { useRouter } from "next/navigation";
import GeneratedPreview from "./GeneratedPreview";
import type { GeneratedLevel } from "@/lib/types";

export const CUSTOM_LEVEL_KEY = "sokoban.custom-level.v1";

export default function LevelCreator() {
  const router = useRouter();
  const [width, setWidth] = useState(8);
  const [height, setHeight] = useState(8);
  const [boxes, setBoxes] = useState(2);
  const [walls, setWalls] = useState(4);
  const [seed, setSeed] = useState("");
  const [result, setResult] = useState<GeneratedLevel | null>(null);
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState("");
  const controller = useRef<AbortController | null>(null);
  const innerCells = Math.max(0, (width - 2) * (height - 2));
  const maxBoxes = Math.min(50, Math.max(1, Math.floor((innerCells - 3) / 4)));
  const maxWalls = useMemo(
    () =>
      Math.min(
        20,
        Math.max(0, Math.floor((innerCells - (boxes * 3 + 4)) / 6)),
      ),
    [boxes, innerCells],
  );

  useEffect(
    () => () => {
      controller.current?.abort();
    },
    [],
  );

  function resetResult() {
    setResult(null);
    setError("");
  }

  async function generate(event: React.FormEvent<HTMLFormElement>) {
    event.preventDefault();
    controller.current?.abort();
    const abort = new AbortController();
    controller.current = abort;
    setBusy(true);
    setError("");
    setResult(null);
    try {
      const response = await fetch("/api/generate", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ width, height, boxes, walls, seed }),
        signal: abort.signal,
      });
      const data = await response.json();
      if (!response.ok) throw new Error(data.error || "Генерація не вдалася.");
      setResult(data);
    } catch (reason) {
      if ((reason as Error).name !== "AbortError")
        setError((reason as Error).message);
    } finally {
      if (!abort.signal.aborted) setBusy(false);
    }
  }

  function play() {
    if (!result) return;
    try {
      sessionStorage.setItem(CUSTOM_LEVEL_KEY, JSON.stringify(result));
      router.push("/play/custom");
    } catch {
      setError("Браузер не дозволив зберегти рівень у цій вкладці.");
    }
  }

  return (
    <main id="main" className="page generator-page">
      <div className="page-heading generator-heading">
        <Link className="back" href="/levels">
          ← Каталог рівнів
        </Link>
        <h1>Створити рівень</h1>
        <p>C++ ядро згенерує та перевірить розв’язний рівень.</p>
      </div>
      <div className="generator-layout">
        <form className="generator-form" onSubmit={generate}>
          <div className="generator-fields">
            <label>
              <span>Ширина</span>
              <input
                name="width"
                type="number"
                min="5"
                max="100"
                value={width}
                onChange={(event) => {
                  setWidth(Number(event.target.value));
                  resetResult();
                }}
                required
              />
              <small>Від 5 до 100 клітинок</small>
            </label>
            <label>
              <span>Висота</span>
              <input
                name="height"
                type="number"
                min="5"
                max="100"
                value={height}
                onChange={(event) => {
                  setHeight(Number(event.target.value));
                  resetResult();
                }}
                required
              />
              <small>Від 5 до 100 клітинок</small>
            </label>
            <label>
              <span>Ящики й цілі</span>
              <input
                name="boxes"
                type="number"
                min="1"
                max={maxBoxes}
                value={boxes}
                onChange={(event) => {
                  setBoxes(Number(event.target.value));
                  resetResult();
                }}
                required
              />
              <small>Одна ціль для кожного ящика, максимум {maxBoxes}</small>
            </label>
            <label>
              <span>Внутрішні стіни</span>
              <input
                name="walls"
                type="number"
                min="0"
                max={maxWalls}
                value={walls}
                onChange={(event) => {
                  setWalls(Number(event.target.value));
                  resetResult();
                }}
                required
              />
              <small>Розміщуються випадково, максимум {maxWalls}</small>
            </label>
          </div>
          <label className="seed-field">
            <span>Seed, необов’язково</span>
            <input
              name="seed"
              inputMode="numeric"
              pattern="[0-9]{1,20}"
              placeholder="Новий випадковий seed"
              value={seed}
              onChange={(event) => {
                setSeed(event.target.value);
                resetResult();
              }}
            />
            <small>Збережи seed, щоб повторити ту саму карту.</small>
          </label>
          <button className="button primary" disabled={busy} type="submit">
            {busy ? "Будуємо й перевіряємо…" : "Згенерувати рівень"}
          </button>
          {busy ? (
            <div className="generation-progress" role="status">
              <span />
              <p>Генератор розставляє цілі, відтягує ящики й перевіряє рішення A*.</p>
            </div>
          ) : null}
          <p className="generator-error" role="alert" aria-live="polite">
            {error}
          </p>
        </form>
        <section className={`generator-result ${result ? "ready" : ""}`} aria-live="polite">
          {result ? (
            <>
              <div className="generated-map">
                <GeneratedPreview xsb={result.xsb} />
              </div>
              <div className="generated-summary">
                <div>
                  <span>Карта готова</span>
                  <h2>{result.width} × {result.height}</h2>
                </div>
                <dl>
                  <div><dt>Ящики</dt><dd>{result.boxes}</dd></div>
                  <div><dt>Стіни</dt><dd>{result.walls}</dd></div>
                  <div><dt>Перевірка</dt><dd>{result.moves} ходів</dd></div>
                  <div><dt>Seed</dt><dd>{result.seed}</dd></div>
                </dl>
                <button className="button primary" type="button" onClick={play}>
                  Грати на цій карті
                </button>
              </div>
            </>
          ) : (
            <div className="generator-empty">
              <div className="empty-board-mark" aria-hidden="true">
                <span>#</span><span>#</span><span>#</span>
                <span>#</span><span>+</span><span>#</span>
                <span>#</span><span>#</span><span>#</span>
              </div>
              <h2>Тут з’явиться твоя карта.</h2>
              <p>Кожен результат уже має перевірений шлях до перемоги.</p>
            </div>
          )}
        </section>
      </div>
    </main>
  );
}
