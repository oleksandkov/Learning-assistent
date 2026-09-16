import type { Metadata } from "next";
import Link from "next/link";
import "./globals.css";
export const metadata: Metadata = {
  title: { default: "Сокобан", template: "%s · Сокобан" },
  description:
    "П’ять головоломок. Грай сам, досліджуй алгоритми й знаходь свій шлях.",
};
export default function Layout({ children }: { children: React.ReactNode }) {
  return (
    <html lang="uk">
      <body>
        <a className="skip-link" href="#main">
          До вмісту
        </a>
        <div className="site-shell">
          <header className="site-header">
            <Link className="brand" href="/" aria-label="Сокобан: головне меню">
              <span className="brand-symbol" aria-hidden="true" />
              <span>сокобан</span>
            </Link>
            <Link className="menu-link" href="/">
              Меню <span aria-hidden="true">☰</span>
            </Link>
          </header>
          {children}
          <footer className="site-footer">
            <Link href="/game-info">Про гру</Link>
          </footer>
        </div>
      </body>
    </html>
  );
}
