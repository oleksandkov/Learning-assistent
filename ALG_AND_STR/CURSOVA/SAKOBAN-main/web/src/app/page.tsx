import Link from "next/link";
import { readFile } from "node:fs/promises";
import path from "node:path";
import HeroReplay from "@/components/HeroReplay";
import Entrance from "@/components/Entrance";
export default async function Home() {
  const xsb = await readFile(
    path.resolve(process.cwd(), "../levels/04_warehouse.xsb"),
    "utf8",
  );
  return (
    <main id="main" className="home">
      <Entrance />
      <div className="home-copy">
        <h1>Сокобан</h1>
        <nav className="home-menu" aria-label="Головне меню">
          <Link href="/levels" className="main-choice">
            <span>Грати</span>
          </Link>
          <Link href="/about">
            <span>Алгоритми</span>
          </Link>
          <Link href="/ai">
            <span>Налаштувати ШІ</span>
          </Link>
          <Link href="/game-info">
            <span>Про гру</span>
          </Link>
        </nav>
      </div>
      <div className="home-art">
        <div className="board-plinth">
          <div className="hero-float">
            <HeroReplay fallbackXsb={xsb} />
          </div>
        </div>
        <div className="art-caption">
          <span className="mini-key">
            <i className="key-player" />
            Ти
          </span>
          <span className="mini-key">
            <i className="key-box" />
            Ящик
          </span>
          <span className="mini-key">
            <i className="key-goal" />
            Ціль
          </span>
        </div>
      </div>
    </main>
  );
}
