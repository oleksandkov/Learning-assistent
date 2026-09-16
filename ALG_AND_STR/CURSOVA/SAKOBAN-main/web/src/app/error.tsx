"use client";
export default function ErrorPage({ reset }: { reset: () => void }) {
  return (
    <main id="main" className="page quiet-page">
      <h1>Не вдалося відкрити сторінку.</h1>
      <p>Перевір, чи запущено сервер, і спробуй знову.</p>
      <button className="button primary" onClick={reset}>
        Спробувати знову
      </button>
    </main>
  );
}
