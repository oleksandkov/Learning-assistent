import Link from "next/link";
import { readFile } from "node:fs/promises";
import path from "node:path";
import { LEVELS } from "@/lib/levels";
import MiniBoard from "@/components/MiniBoard";
export const metadata = { title: "Рівні" };
export default async function Levels() {
  const maps = await Promise.all(
    LEVELS.map((l) =>
      readFile(path.resolve(process.cwd(), "../levels", l.file), "utf8"),
    ),
  );
  return (
    <main id="main" className="page">
      <div className="page-heading">
        <Link className="back" href="/">
          ← Головне меню
        </Link>
        <h1>Рівні</h1>
      </div>
      <div className="level-catalog">
        {LEVELS.map((level, i) => (
          <Link
            className="level-item"
            href={`/play/${level.id}`}
            key={level.id}
          >
            <div className="level-visual">
              <span className="level-number">
                {String(i + 1).padStart(2, "0")}
              </span>
              <MiniBoard xsb={maps[i]} />
            </div>
            <div className="level-copy">
              <h2>
                {level.title}
                <span aria-hidden="true">↗</span>
              </h2>
            </div>
          </Link>
        ))}
        <Link className="level-item create-level-item" href="/levels/create">
          <div className="level-visual create-level-visual">
            <span className="create-range">5 × 5</span>
            <span className="create-plus" aria-hidden="true">+</span>
            <span className="create-range">100 × 100</span>
          </div>
          <div className="level-copy">
            <h2>Створити рівень<span aria-hidden="true">↗</span></h2>
          </div>
        </Link>
      </div>
    </main>
  );
}
