import Link from "next/link";
export default function NotFound() {
  return (
    <main id="main" className="page quiet-page">
      <span className="small-label">404</span>
      <h1>Сторінку не знайдено</h1>
      <p>Запитуваної сторінки або рівня не існує.</p>
      <Link className="button primary" href="/levels">
        Обрати рівень
      </Link>
    </main>
  );
}
