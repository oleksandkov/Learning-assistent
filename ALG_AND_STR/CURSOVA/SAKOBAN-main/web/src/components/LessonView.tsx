"use client";
import { useEffect, useState } from "react";
import Link from "next/link";
import type { Lesson } from "@/lib/lessons";
import AlgorithmScene from "./AlgorithmScene";
export default function LessonView({
  lesson,
  topics,
}: {
  lesson: Lesson;
  topics: { id: string; nav: string }[];
}) {
  const [step, setStep] = useState(0);
  const [playing, setPlaying] = useState(false);
  const [pace, setPace] = useState(3000);
  useEffect(() => {
    if (!playing) return;
    if (step === lesson.frames.length - 1) {
      setPlaying(false);
      return;
    }
    const timer = setTimeout(() => setStep((s) => s + 1), pace);
    return () => clearTimeout(timer);
  }, [pace, playing, step, lesson.frames.length]);
  const frame = lesson.frames[step];
  const isMatrix = lesson.id === "hungarian";
  return (
    <main id="main" className="page">
      <div className="page-heading">
        <Link href="/" className="back">
          ← Головне меню
        </Link>
        <h1>Алгоритми пошуку</h1>
      </div>
      <div className="learning-layout">
        <nav className="topic-nav" aria-label="Теми алгоритмів">
          {topics.map((l) => (
            <Link
              className={l.id === lesson.id ? "active-topic" : ""}
              key={l.id}
              href={l.id === "bfs" ? "/about" : `/about/${l.id}`}
              aria-current={l.id === lesson.id ? "page" : undefined}
            >
              {l.nav}
            </Link>
          ))}
        </nav>
        <article className="lesson">
          <h2>{lesson.title}</h2>
          <p className="lesson-lead">{lesson.lead}</p>
          <div className="lesson-meta">
            <span>{lesson.complexity}</span>
            <span>{lesson.frames.length} кроків пояснення</span>
          </div>
          <section className="demo" aria-label="Покрокова демонстрація">
            <div className="demo-top">
              <span>Крок {step + 1} із {lesson.frames.length}</span>
              <span>Псевдокод</span>
            </div>
            <div className="demo-progress" aria-label="Перейти до кроку">
              {lesson.frames.map((_, index) => (
                <button
                  key={index}
                  type="button"
                  className={index === step ? "current" : index < step ? "complete" : ""}
                  aria-label={`Крок ${index + 1}`}
                  aria-current={index === step ? "step" : undefined}
                  onClick={() => {
                    setPlaying(false);
                    setStep(index);
                  }}
                >
                  <span />
                </button>
              ))}
            </div>
            <div className="demo-content">
              <div className="demo-visual">
                <div className="frame-enter">
                  {isMatrix ? (
                    <div
                      className="trace-tiles matrix"
                      style={{ gridTemplateColumns: "repeat(3, 1fr)" }}
                    >
                      {frame.values.map((value, i) => (
                        <span
                          key={`${lesson.id}-${i}`}
                          className={frame.active.includes(i) ? "active" : ""}
                        >
                          {value}
                        </span>
                      ))}
                    </div>
                  ) : (
                    <AlgorithmScene lessonId={lesson.id} step={step} total={lesson.frames.length} />
                  )}
                  <div className="trace-value" aria-live="off">
                    <small>Стан структури</small>
                    <strong>{frame.note}</strong>
                  </div>
                </div>
              </div>
              <div
                className="code-panel"
                aria-label="Псевдокод з активним рядком"
              >
                {lesson.code.map((line, i) => (
                  <div
                    key={i}
                    className={`code-line ${frame.line === i ? "active" : ""}`}
                    aria-current={frame.line === i ? "step" : undefined}
                  >
                    <em>{i + 1}</em>
                    <span className="code-text">{line}</span>
                    {frame.line === i ? <strong className="code-running">зараз</strong> : null}
                  </div>
                ))}
              </div>
            </div>
            <div className="demo-bottom">
              <div className="step-copy">
                <span>Що відбувається</span>
                <p key={`${lesson.id}-copy-${step}`} className="frame-enter" aria-live={playing ? "off" : "polite"}>
                  {frame.text}
                </p>
              </div>
              <div className="demo-buttons">
                <button
                  className="button secondary"
                  disabled={step === 0}
                  onClick={() => {
                    setPlaying(false);
                    setStep((s) => s - 1);
                  }}
                  aria-label="Попередній крок пояснення"
                >
                  ← Назад
                </button>
                <button
                  className="button primary"
                  onClick={() => {
                    if (step === lesson.frames.length - 1) setStep(0);
                    setPlaying((p) => !p);
                  }}
                >
                  {playing
                    ? "Пауза"
                    : step === lesson.frames.length - 1
                      ? "Ще раз"
                      : "Відтворити"}
                </button>
                <button
                  className="button secondary"
                  disabled={step === lesson.frames.length - 1}
                  onClick={() => {
                    setPlaying(false);
                    setStep((s) => s + 1);
                  }}
                  aria-label="Наступний крок пояснення"
                >
                  Далі →
                </button>
                <button
                  className="button subtle"
                  onClick={() => {
                    setPlaying(false);
                    setStep(0);
                  }}
                >
                  Спочатку
                </button>
                <span className="step-count">
                  {step + 1} / {lesson.frames.length}
                </span>
              </div>
              <label className="lesson-pace">
                Темп
                <select value={pace} onChange={(event) => setPace(Number(event.target.value))}>
                  <option value="4200">Повільно</option>
                  <option value="3000">Звичайно</option>
                  <option value="1800">Швидко</option>
                </select>
              </label>
            </div>
          </section>
          <div className="lesson-reading">
            {lesson.sections.map((section, i) => (
              <details key={section.title} open={i === 0 ? true : undefined}>
                <summary>{section.title}</summary>
                {section.paragraphs.map((p) => (
                  <p key={p}>{p}</p>
                ))}
              </details>
            ))}
          </div>
          <div className="lesson-callout">
            <p>{lesson.takeaway}</p>
          </div>
          <p className="source-note">
            Реалізація в проєкті: <code>{lesson.source}</code>. Приклад вище
            ілюструє принцип, а не виміряний запуск гри.
          </p>
          <Link
            className="button secondary"
            style={{ marginTop: 24 }}
            href="/levels"
          >
            Спробувати на рівні
          </Link>
        </article>
      </div>
    </main>
  );
}
