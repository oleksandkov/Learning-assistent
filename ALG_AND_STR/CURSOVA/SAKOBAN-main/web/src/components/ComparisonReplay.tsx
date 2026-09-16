"use client";
import { useEffect, useRef, useState } from "react";
import BoardView from "./BoardView";
import { ALGORITHMS, type SearchResult, type Snapshot } from "@/lib/types";

export interface ComparisonDecision { result: SearchResult; base: string }

export default function ComparisonReplay({ decisions, load, onClose }: {
  decisions: ComparisonDecision[];
  load: (decision: ComparisonDecision, step: number, signal: AbortSignal) => Promise<Snapshot>;
  onClose: () => void;
}) {
  const [step, setStep] = useState(0);
  const [frames, setFrames] = useState<Snapshot[]>([]);
  const [playing, setPlaying] = useState(false);
  const [pending, setPending] = useState(true);
  const [error, setError] = useState("");
  const [speed, setSpeed] = useState(500);
  const loader = useRef(load);
  loader.current = load;
  const maximum = Math.max(0, ...decisions.map(d => d.result.moves.length));

  useEffect(() => {
    setStep(0);
    setPlaying(false);
  }, [decisions]);

  useEffect(() => {
    const abort = new AbortController();
    setPending(true);
    setError("");
    Promise.all(decisions.map(d => loader.current(d, Math.min(step, d.result.moves.length), abort.signal)))
      .then(next => { if (!abort.signal.aborted) setFrames(next); })
      .catch(() => { if (!abort.signal.aborted) { setError("Крок не завантажено. Спробуй ще раз."); setPlaying(false); } })
      .finally(() => { if (!abort.signal.aborted) setPending(false); });
    return () => abort.abort();
  }, [step, decisions]);

  useEffect(() => {
    if (!playing || pending || error || step >= maximum) return;
    const timer = setTimeout(() => setStep(n => n + 1), speed);
    return () => clearTimeout(timer);
  }, [playing, pending, error, step, maximum, speed]);

  useEffect(() => {
    if (step >= maximum && playing) {
      setPlaying(false);
    }
  }, [step, maximum, playing]);

  return <section className="comparison-replay" aria-label="Синхронне порівняння">
    <div className="section-line"><h3>Маршрути поруч</h3><button className="button subtle" onClick={onClose}>Закрити порівняння</button></div>
    <p>Один такт — один хід. Усі поля починають з однакової позиції.</p>
    <div className="comparison-boards">
      {decisions.map((d, i) => <article key={d.result.algorithm}>
        <header>
          <h3>{ALGORITHMS.find(a => a.id === d.result.algorithm)?.label}</h3>
          <span>{frames[i]?.won || (d.result.moves.length > 0 && step >= d.result.moves.length) ? "Завершено" : `${Math.min(step, d.result.moves.length)} / ${d.result.moves.length}`}</span>
        </header>
        <div className="comparison-board">{frames[i] ? <BoardView board={frames[i].board} state={frames[i].state} animated /> : <span>Завантаження…</span>}</div>
        <p>Ходи {frames[i]?.moves ?? "—"} · Штовхання {frames[i]?.pushes ?? "—"}</p>
        <p>{d.result.algorithm === "gemini" ? "Відповідь AI" : "Пошук"} {(d.result.remoteMs ?? d.result.searchMs).toFixed(3)} мс · Досліджено {d.result.algorithm === "gemini" ? "—" : d.result.explored}</p>
      </article>)}
    </div>
    <div className="comparison-controls">
      <button className="button secondary" disabled={pending || step === 0} onClick={() => { setPlaying(false); setStep(n => Math.max(0, n - 1)); }} aria-label="Порівняння: попередній крок">←</button>
      <button className="button primary" disabled={pending || !!error || maximum === 0} onClick={() => {
        if (step >= maximum) {
          setStep(0);
          setPlaying(true);
        } else {
          setPlaying(p => !p);
        }
      }}>{playing && step < maximum ? "Пауза разом" : "Запустити разом"}</button>
      <button className="button secondary" disabled={pending || step >= maximum || maximum === 0} onClick={() => { setPlaying(false); setStep(n => Math.min(maximum, n + 1)); }} aria-label="Порівняння: наступний крок">→</button>
      <button className="button subtle" disabled={pending || step === 0} onClick={() => { setPlaying(false); setStep(0); }}>На початок</button>
      <label>Темп <input aria-label="Темп порівняння" type="range" min="200" max="1200" step="100" value={speed} onChange={e => setSpeed(Number(e.target.value))} /></label>
      <span aria-live="polite">Крок {step} / {maximum}</span>
    </div>
    {maximum === 0 && (
      <p className="comparison-note" role="note">
        Вибрані рішення розраховані для вже завершеної позиції (0 нових ходів).
      </p>
    )}
    {error && <p role="alert">{error}</p>}
  </section>;
}
