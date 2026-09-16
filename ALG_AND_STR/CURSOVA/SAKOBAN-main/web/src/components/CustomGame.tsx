"use client";

import Link from "next/link";
import { useEffect, useState } from "react";
import Game from "./Game";
import { CUSTOM_LEVEL_KEY } from "./LevelCreator";
import type { GeneratedLevel, Level } from "@/lib/types";

export default function CustomGame() {
  const [stored, setStored] = useState<GeneratedLevel | null | undefined>();
  useEffect(() => {
    try {
      const value = sessionStorage.getItem(CUSTOM_LEVEL_KEY);
      const parsed = value ? (JSON.parse(value) as GeneratedLevel) : null;
      setStored(parsed?.success && typeof parsed.xsb === "string" ? parsed : null);
    } catch {
      setStored(null);
    }
  }, []);

  if (stored === undefined)
    return <main className="page custom-loading" role="status">Відкриваємо власний рівень…</main>;
  if (!stored)
    return (
      <main id="main" className="page custom-missing">
        <Link className="back" href="/levels">← Каталог рівнів</Link>
        <h1>Рівень не створено</h1>
        <p>Спочатку налаштуйте параметри по��я та згенеруйте рівень.</p>
        <Link className="button primary" href="/levels/create">Створити рівень</Link>
      </main>
    );

  const level: Level = {
    id: "custom",
    title: "Власний рівень",
    difficulty: `${stored.width} × ${stored.height}, ${stored.boxes} ящиків`,
    file: "",
    note: "",
  };
  return (
    <Game
      level={level}
      customXsb={stored.xsb}
      guaranteedSolution={{ moves: stored.solution, pushes: stored.pushes }}
    />
  );
}
