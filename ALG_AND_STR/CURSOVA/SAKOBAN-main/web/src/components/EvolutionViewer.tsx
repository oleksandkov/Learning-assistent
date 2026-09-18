"use client";

import { useEffect, useMemo, useRef, useState } from "react";
import BoardView, { type BoardMarker } from "./BoardView";
import type { EvolutionIndividual, SearchResult, Snapshot } from "@/lib/types";

const arrows: Record<string, string> = { U: "↑", L: "←", D: "↓", R: "→" };

export default function EvolutionViewer({
  result,
  loadSnapshot,
}: {
  result: SearchResult;
  loadSnapshot: (moves: string) => Promise<Snapshot>;
}) {
  const evolution = result.evolution!;
  const [generation, setGeneration] = useState(0);
  const [individualIndex, setIndividualIndex] = useState(0);
  const [step, setStep] = useState(0);
  const [playing, setPlaying] = useState(false);
  const [speed, setSpeed] = useState(600);
  const [snapshot, setSnapshot] = useState<Snapshot | null>(null);
  const [frames, setFrames] = useState<Record<number, Snapshot>>({});
  const routeRef = useRef<HTMLDivElement>(null);
  const individuals = evolution.generations[generation] ?? [];
  const individual: EvolutionIndividual | undefined = individuals[individualIndex];
  const moves = individual?.moves ?? "";
  const genes = individual?.genes ?? "";
  const isAnt = evolution.kind === "aco";
  const bestCosts = useMemo(
    () => evolution.generations.map(items => Math.min(...items.map(item => item.cost))),
    [evolution.generations],
  );
  const minCost = Math.min(...bestCosts);
  const maxCost = Math.max(...bestCosts);

  useEffect(() => {
    setStep(0);
    setPlaying(false);
    setSnapshot(null);
    setFrames({});
  }, [generation, individualIndex]);

  useEffect(() => {
    let active = true;
    void loadSnapshot(moves.slice(0, step)).then(value => {
      if (!active) return;
      setSnapshot(value);
      setFrames(previous => ({ ...previous, [step]: value }));
    });
    return () => { active = false; };
  }, [generation, individualIndex, loadSnapshot, moves, step]);

  useEffect(() => {
    if (!playing || step >= moves.length) {
      if (step >= moves.length) setPlaying(false);
      return;
    }
    const timer = window.setTimeout(() => setStep(value => value + 1), speed);
    return () => window.clearTimeout(timer);
  }, [moves.length, playing, speed, step]);

  useEffect(() => {
    routeRef.current?.querySelector(".current")?.scrollIntoView({
      behavior: "smooth",
      block: "nearest",
      inline: "center",
    });
  }, [step]);

  const markers = useMemo<BoardMarker[]>(() => {
    const cells = new Map<number, BoardMarker>();
    for (let index = 0; index <= step; ++index) {
      const frame = frames[index];
      if (!frame) continue;
      cells.set(frame.state.player, {
        cell: frame.state.player,
        kind: index === step ? "current" : "route",
        label: index < moves.length ? arrows[moves[index]] : "",
      });
    }
    return Array.from(cells.values());
  }, [frames, moves, step]);

  function selectGeneration(index: number) {
    setGeneration(index);
    setIndividualIndex(0);
  }

  function showWinner() {
    for (let gen = evolution.generations.length - 1; gen >= 0; --gen) {
      const winner = evolution.generations[gen].findIndex(item => item.won);
      if (winner >= 0) {
        setGeneration(gen);
        setIndividualIndex(winner);
        return;
      }
    }
  }

  if (!individual) return null;
  const currentMove = step < moves.length ? arrows[moves[step]] : "✓";
  return (
    <div className={`evolution-viewer ${evolution.kind}`}>
      <header className="evolution-viewer-heading">
        <div>
          <span>{isAnt ? "Лабораторія ACO" : "Лабораторія GA"}</span>
          <h4>{isAnt ? "Маршрут окремої мурахи" : "Життєвий цикл хромосоми"}</h4>
          <p>{isAnt
            ? "Оберіть ітерацію та мураху, потім відтворіть її реальний шлях по полю."
            : "Оберіть покоління та хромосому: поле показує лише допустимі команди, нижче — всі її гени."}</p>
        </div>
        <strong>{generation + 1}<small> / {evolution.generations.length} поколінь</small></strong>
      </header>

      <div className="evolution-toolbar">
        <label>
          {isAnt ? "Ітерація" : "Покоління"}
          <select value={generation} onChange={event => selectGeneration(Number(event.target.value))}>
            {evolution.generations.map((_, index) =>
              <option key={index} value={index}>{index + 1}</option>)}
          </select>
        </label>
        <label>
          {isAnt ? "Мураха" : "Хромосома"}
          <select value={individualIndex} onChange={event => setIndividualIndex(Number(event.target.value))}>
            {individuals.map((item, index) =>
              <option key={item.id} value={index}>
                {item.won ? "✓ " : ""}#{item.id + 1} · вартість {Math.round(item.cost)}
              </option>)}
          </select>
        </label>
        <button type="button" className="button subtle" onClick={showWinner}>Показати переможця</button>
      </div>

      <div className="evolution-workspace">
        <section className="evolution-board-panel">
          <div className="evolution-panel-heading">
            <span>Поле та пройдений слід</span>
            <strong>Крок {step} / {moves.length} · {currentMove}</strong>
          </div>
          <div className="evolution-stage">
            {snapshot ? <BoardView board={snapshot.board} state={snapshot.state} animated markers={markers} /> :
              <div className="evolution-loading">Завантаження поля…</div>}
          </div>
          <div className="route-legend" aria-label="Позначення маршруту">
            <span><i className="visited" /> пройдена клітинка</span>
            <span><i className="active" /> поточна клітинка</span>
          </div>
        </section>

        <aside className="evolution-inspector">
          <div className="evolution-summary">
            <span><b>{individual.won ? "Ціль досягнута" : "Без перемоги"}</b>результат</span>
            <span><b>{Math.round(individual.cost)}</b>вартість, менше — краще</span>
            <span><b>{moves.length}</b>допустимих ходів</span>
            {!isAnt ? <span><b>{individual.parentA == null || individual.parentA < 0 ? "Перше покоління" : `#${individual.parentA + 1} + #${individual.parentB! + 1}`}</b>батьківські хромосоми</span> : null}
          </div>

          <section className="fitness-panel">
            <div className="evolution-panel-heading">
              <span>Найкраща вартість покоління</span>
              <small>натисніть стовпчик</small>
            </div>
            <div className="fitness-chart" aria-label="Найкраща вартість за поколіннями">
              {bestCosts.map((cost, index) => {
                const range = Math.max(1, maxCost - minCost);
                const height = 28 + ((maxCost - cost) / range) * 72;
                return <button key={index} type="button" className={index === generation ? "current" : ""}
                  style={{ height: `${height}%` }} onClick={() => selectGeneration(index)}
                  aria-label={`Покоління ${index + 1}, вартість ${Math.round(cost)}`}
                  title={`Покоління ${index + 1}: ${Math.round(cost)}`} />;
              })}
            </div>
            <div className="fitness-axis"><span>1</span><span>краще ↑</span><span>{bestCosts.length}</span></div>
          </section>
        </aside>
      </div>

      {!isAnt ? (
        <section className="sequence-panel">
          <div className="evolution-panel-heading">
            <span>Повна хромосома · {genes.length} генів</span>
            <small>{individual.mutationIndex == null || individual.mutationIndex < 0 ? "без мутації" : `мутація: ген ${individual.mutationIndex + 1}`}</small>
          </div>
          <div className="gene-strip chromosome-strip" aria-label="Гени вибраної хромосоми">
            {Array.from(genes).map((gene, index) => <span key={index}
              className={individual.mutationIndex === index ? "mutated" : ""}
              title={`Ген ${index + 1}: ${gene}`}>{arrows[gene]}</span>)}
          </div>
          <p>Стіни можуть відхилити частину генів, тому нижній маршрут іноді коротший за хромосому.</p>
        </section>
      ) : null}

      <section className="sequence-panel route-sequence">
        <div className="evolution-panel-heading">
          <span>{isAnt ? "Команди вибраної мурахи" : "Допустимі команди на полі"}</span>
          <small>{moves.length} ходів</small>
        </div>
        <div ref={routeRef} className="gene-strip" aria-label={isAnt ? "Шлях вибраної мурахи" : "Допустимий шлях хромосоми"}>
          {Array.from(moves).map((move, index) => <span key={index}
            className={`${index === step ? "current" : ""} ${index < step ? "done" : ""}`}
            title={`Хід ${index + 1}: ${move}`}>{arrows[move]}</span>)}
        </div>
      </section>

      <div className="evolution-playback">
        <button type="button" className="button secondary" disabled={step === 0}
          onClick={() => { setPlaying(false); setStep(value => value - 1); }}>← Крок</button>
        <button type="button" className="button primary" disabled={!moves.length}
          onClick={() => {
            if (step === moves.length) setStep(0);
            setPlaying(value => !value);
          }}>{playing ? "Пауза" : step === moves.length ? "Ще раз" : "Відтворити"}</button>
        <button type="button" className="button secondary" disabled={step === moves.length}
          onClick={() => { setPlaying(false); setStep(value => value + 1); }}>Крок →</button>
        <label className="evolution-scrubber">Крок
          <input aria-label="Крок маршруту" type="range" min="0" max={moves.length} value={step}
            onChange={event => { setPlaying(false); setStep(Number(event.target.value)); }} />
        </label>
        <span>{step} / {moves.length}</span>
        <label>Швидкість
          <select value={speed} onChange={event => setSpeed(Number(event.target.value))}>
            <option value="1000">Повільно</option><option value="600">Звичайно</option><option value="240">Швидко</option>
          </select>
        </label>
      </div>
    </div>
  );
}
