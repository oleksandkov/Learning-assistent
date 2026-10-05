import Link from "next/link";
import { readFile } from "node:fs/promises";
import path from "node:path";
import MiniBoard from "@/components/MiniBoard";
export const metadata = { title: "Про гру" };
const elements = [
  { kind: "player", title: "Гравець", symbol: "@", text: "Ходить у чотирьох напрямках." },
  { kind: "box", title: "Ящик", symbol: "$", text: "Перемістити на ціль." },
  { kind: "goal", title: "Ціль", symbol: ".", text: "Клітинка для ящика." },
  { kind: "wall", title: "Стіна", symbol: "#", text: "Непрохідна." },
  { kind: "box on-goal", title: "Ящик на цілі", symbol: "*", text: "Мета виконана частково." },
  { kind: "floor", title: "Підлога", symbol: "пробіл", text: "Вільна клітинка." },
];
export default async function GameInfo() {
  const xsb = await readFile(
    path.resolve(process.cwd(), "../levels/01_simple.xsb"),
    "utf8",
  );
  return (
    <main id="main" className="page">
      <div className="page-heading">
        <Link className="back" href="/">
          ← Головне меню
        </Link>
        <h1>Про гру</h1>
      </div>
      <div className="info-layout">
        <section>
          <h2>Історія</h2>
          <p>
            Сокобан створив Хіроюкі Імабаясі, у 1982 році гру випустила
            Thinking Rabbit. Мета — розмістити всі ящики на цілях.
          </p>
          <p>Ящики можна лише штовхати вперед, не тягнути й не по два.</p>
          <p className="source-note">
            Історія:{" "}
            <a
              href="https://www.sokoban.jp/history.html"
              target="_blank"
              rel="noreferrer"
            >
              офіційний сайт Sokoban
            </a>
            .
          </p>
        </section>
        <MiniBoard xsb={xsb} />
      </div>
      <section>
        <h2>Елементи поля</h2>
        <div className="legend-grid">
          {elements.map((e) => (
            <div className="legend-item" key={e.title}>
              <span className={`mini-cell ${e.kind}`} aria-hidden="true">
                <i />
              </span>
              <div>
                <h3>{e.title}</h3>
                <p>
                  {e.text} <code>{e.symbol}</code> у XSB.
                </p>
              </div>
            </div>
          ))}
        </div>
      </section>
      <div className="info-rules">
        <section>
          <h2>Правила гри</h2>
          <ol>
            <li>Керування стрілками або WASD.</li>
            <li>
              Штовхання одного ящика у вільну клітинку (тягнути та штовхати декілька разом заборонено).
            </li>
            <li>Розміщення всіх ящиків на цілях.</li>
          </ol>
          <p>Доступне скасування та повторення ходів.</p>
        </section>
      </div>
      <div className="info-links">
        <Link href="/levels" className="button primary">
          Обрати рівень
        </Link>
        <Link href="/about" className="button secondary">
          Дослідити алгоритми
        </Link>
      </div>
    </main>
  );
}
