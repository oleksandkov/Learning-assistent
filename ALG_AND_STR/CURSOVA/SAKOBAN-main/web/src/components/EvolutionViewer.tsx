"use client";

import { useEffect, useMemo, useRef, useState } from "react";
import BoardView, { type BoardCellHint, type BoardMarker } from "./BoardView";
import type { EvolutionIndividual, SearchResult, Snapshot, State } from "@/lib/types";

const arrows: Record<string, string> = { U: "↑", L: "←", D: "↓", R: "→" };
const viewerCopy = {
  aco: { singular: "мураха", plural: "мурах", cycle: "ітерації", cycleLabel: "Ітерація", cycles: "ітерацій", lab: "Лабораторія ACO", title: "Маршрут окремої мурахи", description: "Оберіть ітерацію та мураху, потім відтворіть її реальний шлях по полю.", best: "Найкраща мураха кожної ітерації", commands: "Команди вибраної мурахи", route: "Шлях вибраної мурахи" },
  genetic: { singular: "хромосома", plural: "хромосом", cycle: "покоління", cycleLabel: "Покоління", cycles: "поколінь", lab: "Лабораторія GA", title: "Життєвий цикл хромосоми", description: "Оберіть покоління та хромосому: поле показує лише допустимі команди, нижче — всі її гени.", best: "Найкраща хромосома покоління", commands: "Допустимі команди на полі", route: "Допустимий шлях хромосоми" },
  gravity: { singular: "потік", plural: "потоків", cycle: "циклу", cycleLabel: "Цикл", cycles: "циклів", lab: "Лабораторія зливу", title: "Маршрут окремого потоку", description: "Оберіть цикл і потік: поле відтворить дозволені рухи, штовхання та виходи зі стійкої западини.", best: "Найкращий потік кожного циклу", commands: "Команди вибраного потоку", route: "Шлях вибраного потоку" },
  cannibal: { singular: "бот", plural: "ботів", cycle: "покоління", cycleLabel: "Покоління", cycles: "поколінь", lab: "Лабораторія Cannibal GA", title: "Родовід канібального бота", description: "Оберіть покоління та бота: кольори ДНК показують префікс жертви, змінений ген і хвіст хижака.", best: "Найкращий бот покоління", commands: "Допустимі команди бота", route: "Допустимий шлях бота" },
} as const;

export function replayState(origin: Snapshot, moves: string, count: number): State {
  const state = { player: origin.state.player, boxes: [...origin.state.boxes] };
  for (const move of moves.slice(0, count)) {
    const delta = move === "U" ? -origin.board.width : move === "D" ? origin.board.width : move === "L" ? -1 : 1;
    const next = state.player + delta;
    const box = state.boxes.indexOf(next);
    if (box >= 0) state.boxes[box] += delta;
    state.player = next;
  }
  return state;
}

function gravityHeight(origin: Snapshot, state: State) {
  const goals = origin.board.goals.flatMap((goal, cell) => goal ? [cell] : []);
  return state.boxes.reduce((total, box) => total + Math.min(...goals.map(goal =>
    Math.abs(box % origin.board.width - goal % origin.board.width) +
    Math.abs(Math.floor(box / origin.board.width) - Math.floor(goal / origin.board.width)))), 0);
}

function gravityField(origin: Snapshot): Record<number, BoardCellHint> {
  const { board } = origin;
  if (board.width - 2 > 22 || board.height - 2 > 22) return {};
  const goals = board.goals.flatMap((goal, cell) => goal ? [cell] : []);
  const cells: { cell: number; distance: number; label: string }[] = [];
  for (let y = 1; y < board.height - 1; y++) for (let x = 1; x < board.width - 1; x++) {
    const cell = y * board.width + x;
    if (!board.floor[cell] || board.walls[cell] || goals.length === 0) continue;
    const goal = goals.reduce((best, candidate) => {
      const distance = Math.abs(x - candidate % board.width) + Math.abs(y - Math.floor(candidate / board.width));
      const bestDistance = Math.abs(x - best % board.width) + Math.abs(y - Math.floor(best / board.width));
      return distance < bestDistance ? candidate : best;
    });
    const gx = goal % board.width, gy = Math.floor(goal / board.width);
    const distance = Math.abs(x - gx) + Math.abs(y - gy);
    const label = distance === 0 ? "◎" : Math.abs(gx - x) >= Math.abs(gy - y) ? gx > x ? "→" : "←" : gy > y ? "↓" : "↑";
    cells.push({ cell, distance, label });
  }
  const max = Math.max(1, ...cells.map(item => item.distance));
  return Object.fromEntries(cells.map(item => [item.cell, {
    label: item.label,
    intensity: .12 + (1 - item.distance / max) * .32,
  }]));
}

export default function EvolutionViewer({
  result,
  loadSnapshot,
}: {
  result: SearchResult;
  loadSnapshot: (moves: string) => Promise<Snapshot>;
}) {
  const evolution = result.evolution!;
  const initialGeneration = evolution.kind === "cannibal" && evolution.generations.length > 1 ? 1 : 0;
  const initialIndividual = evolution.kind === "cannibal"
    ? Math.max(0, evolution.generations[initialGeneration]?.findIndex(item => (item.consumedId ?? -1) >= 0) ?? 0)
    : 0;
  const [generation, setGeneration] = useState(initialGeneration);
  const [individualIndex, setIndividualIndex] = useState(initialIndividual);
  const [viewMode, setViewMode] = useState<"individual" | "generation">("individual");
  const [step, setStep] = useState(0);
  const [playing, setPlaying] = useState(false);
  const [speed, setSpeed] = useState(600);
  const [origin, setOrigin] = useState<Snapshot | null>(null);
  const routeRef = useRef<HTMLDivElement>(null);
  const individuals = evolution.generations[generation] ?? [];
  const individual: EvolutionIndividual | undefined = individuals[individualIndex];
  const moves = individual?.moves ?? "";
  const genes = individual?.genes ?? "";
  const copy = viewerCopy[evolution.kind];
  const hasGenes = evolution.kind === "genetic" || evolution.kind === "cannibal";
  const participantName = copy.singular;
  const participantPlural = copy.plural;
  const cycleName = copy.cycle;
  const bestCosts = useMemo(
    () => evolution.generations.map(items => Math.min(...items.map(item => item.cost))),
    [evolution.generations],
  );
  const runningBestCosts = useMemo(() => {
    let best = Number.POSITIVE_INFINITY;
    return bestCosts.map(cost => (best = Math.min(best, cost)));
  }, [bestCosts]);
  const minCost = Math.min(...bestCosts);
  const maxCost = Math.max(...bestCosts);
  const leaderIndex = individuals.reduce(
    (best, item, index) => item.cost < (individuals[best]?.cost ?? Number.POSITIVE_INFINITY) ? index : best,
    0,
  );
  const leader = individuals[leaderIndex];
  const generationLength = Math.max(0, ...individuals.map(item => item.moves.length));
  const playbackLength = viewMode === "generation" ? generationLength : moves.length;
  const winnerLocation = useMemo(() => {
    for (let gen = evolution.generations.length - 1; gen >= 0; --gen) {
      const index = evolution.generations[gen].findIndex(item => item.won);
      if (index >= 0) return { generation: gen, individual: index };
    }
    return null;
  }, [evolution.generations]);
  const cellHints = useMemo(
    () => origin && evolution.kind === "gravity" ? gravityField(origin) : {},
    [evolution.kind, origin],
  );

  useEffect(() => {
    let active = true;
    void loadSnapshot("").then(value => {
      if (active) setOrigin(value);
    });
    return () => { active = false; };
  }, [loadSnapshot]);

  useEffect(() => {
    setStep(0);
    setPlaying(false);
  }, [generation, individualIndex, viewMode]);

  useEffect(() => {
    if (!playing || step >= playbackLength) {
      if (step >= playbackLength) setPlaying(false);
      return;
    }
    const timer = window.setTimeout(() => setStep(value => value + 1), speed);
    return () => window.clearTimeout(timer);
  }, [playbackLength, playing, speed, step]);

  useEffect(() => {
    if (viewMode === "individual") {
      routeRef.current?.querySelector(".current")?.scrollIntoView({
        behavior: "smooth",
        block: "nearest",
        inline: "center",
      });
    }
  }, [step, viewMode]);

  const selectedState = useMemo(
    () => origin ? replayState(origin, moves, step) : null,
    [moves, origin, step],
  );
  const leaderState = useMemo(
    () => origin && leader ? replayState(origin, leader.moves, step) : null,
    [leader, origin, step],
  );
  const routeMarkers = useMemo<BoardMarker[]>(() => {
    if (!origin) return [];
    // ponytail: solver traces are capped at 120 moves, so a tiny local replay is cheaper than 120 API calls.
    const cells = new Map<number, BoardMarker>();
    let state = replayState(origin, "", 0);
    cells.set(state.player, { cell: state.player, kind: step === 0 ? "current" : "route", label: arrows[moves[0]] });
    for (let index = 0; index < step; ++index) {
      state = replayState(origin, moves, index + 1);
      cells.set(state.player, {
        cell: state.player,
        kind: index + 1 === step ? "current" : "route",
        label: arrows[moves[index + 1]] ?? "",
      });
    }
    return Array.from(cells.values());
  }, [moves, origin, step]);
  const generationMarkers = useMemo<BoardMarker[]>(() => {
    if (!origin) return [];
    return individuals.map((item, index) => ({
      id: `participant-${item.id}`,
      cell: replayState(origin, item.moves, step).player,
      kind: index === leaderIndex ? "leader" : evolution.kind === "cannibal" && (item.fatalGene ?? -1) >= 0 ? "stopped" : "swarm",
      label: index === leaderIndex ? "★" : evolution.kind === "cannibal" && (item.fatalGene ?? -1) >= 0 ? "×" : "",
    }));
  }, [evolution.kind, individuals, leaderIndex, origin, step]);

  function selectGeneration(index: number) {
    setGeneration(index);
    const cannibalized = evolution.kind === "cannibal"
      ? evolution.generations[index]?.findIndex(item => (item.consumedId ?? -1) >= 0) ?? -1
      : -1;
    setIndividualIndex(Math.max(0, cannibalized));
  }

  function showWinner() {
    if (!winnerLocation) return;
    setGeneration(winnerLocation.generation);
    setIndividualIndex(winnerLocation.individual);
    setViewMode("individual");
  }

  function selectParticipant(index: number) {
    setIndividualIndex(index);
    setViewMode("individual");
  }

  if (!individual) return null;
  const currentMove = step < moves.length ? arrows[moves[step]] : "✓";
  const winnerCount = individuals.filter(item => item.won).length;
  const activeCount = individuals.filter(item => step < item.moves.length).length;
  const currentBest = bestCosts[generation];
  const recordBest = runningBestCosts[generation];
  const watchedState = viewMode === "generation" ? leaderState : selectedState;
  const watchedMoves = viewMode === "generation" ? leader.moves : moves;
  const nextMove = watchedMoves[step];
  const nextDelta = origin && nextMove ? nextMove === "U" ? -origin.board.width : nextMove === "D" ? origin.board.width : nextMove === "L" ? -1 : 1 : 0;
  const willPush = Boolean(watchedState && nextDelta && watchedState.boxes.includes(watchedState.player + nextDelta));
  const terrainHeight = origin && watchedState ? gravityHeight(origin, watchedState) : 0;
  const mutationIndex = individual.mutationIndex ?? -1;
  const deadlockedBots = individuals.filter(item => (item.fatalGene ?? -1) >= 0).length;
  return (
    <div className={`evolution-viewer ${evolution.kind}`}>
      <header className="evolution-viewer-heading">
        <div>
          <span>{copy.lab}</span>
          <h4>{viewMode === "generation"
            ? `Одночасний запуск ${individuals.length} ${participantPlural}`
            : copy.title}</h4>
          <p>{viewMode === "generation"
            ? `Усі учасники виконують команди синхронно. ★ позначає лідера цього ${cycleName}.`
            : copy.description}</p>
        </div>
        <strong>{generation + 1}<small> / {evolution.generations.length} {copy.cycles}</small></strong>
      </header>

      <div className="evolution-mode" role="group" aria-label="Режим спостереження">
        <button type="button" aria-pressed={viewMode === "individual"} onClick={() => setViewMode("individual")}>
          Окремо: {participantName}
        </button>
        <button type="button" aria-pressed={viewMode === "generation"} onClick={() => setViewMode("generation")}>
          Усі {individuals.length} {participantPlural}
        </button>
      </div>

      <div className="evolution-toolbar">
        <label>
          {copy.cycleLabel}
          <select aria-label={copy.cycleLabel} value={generation}
            onChange={event => selectGeneration(Number(event.target.value))}>
            {evolution.generations.map((_, index) =>
              <option key={index} value={index}>{index + 1}</option>)}
          </select>
        </label>
        <label>
          {participantName[0].toUpperCase() + participantName.slice(1)}
          <select aria-label={participantName[0].toUpperCase() + participantName.slice(1)} value={individualIndex}
            onChange={event => selectParticipant(Number(event.target.value))}>
            {individuals.map((item, index) =>
              <option key={item.id} value={index}>
                {index === leaderIndex ? "★ " : item.won ? "✓ " : ""}#{item.id + 1} · вартість {Math.round(item.cost)}
              </option>)}
          </select>
        </label>
        <button type="button" className="button subtle" disabled={!winnerLocation} onClick={showWinner}>
          {winnerLocation ? "Показати переможця" : "Переможця не знайдено"}
        </button>
      </div>

      <div className="evolution-workspace">
        <section className="evolution-board-panel">
          <div className="evolution-panel-heading">
            <span>{viewMode === "generation" ? "Карта всього покоління" : "Поле та пройдений слід"}</span>
            <strong>{viewMode === "generation"
              ? `Крок ${step} / ${playbackLength} · рухаються ${activeCount}`
              : `Крок ${step} / ${moves.length} · ${currentMove}`}</strong>
          </div>
          <div className="evolution-stage">
            {origin && (viewMode === "generation" ? leaderState : selectedState)
              ? evolution.kind === "gravity" || evolution.kind === "cannibal" ? (
                <div className={`method-stage-shell ${evolution.kind} ${willPush ? "will-push" : ""}`} data-live-step={step}>
                  {evolution.kind === "gravity" ? (
                    <div className="gravity-live-readout">
                      <span className="flow-orb" aria-hidden="true" />
                      <div><b>{willPush ? "Штовхання вниз по схилу" : "Підхід гравця до ящика"}</b><small>наступна команда {arrows[nextMove] ?? "✓"}</small></div>
                      <strong>h = {terrainHeight}</strong>
                    </div>
                  ) : viewMode === "generation" ? (
                    <div className="cannibal-live-readout">
                      <div><b>{individuals.length}</b><small>ботів на арені</small></div>
                      <i>→</i>
                      <div className="fatal"><b>{deadlockedBots}</b><small>зупинено тупиком</small></div>
                      <i>→</i>
                      <div className="leader"><b>#{leader.id + 1}</b><small>лідер покоління</small></div>
                    </div>
                  ) : (
                    <div className="cannibal-live-readout lineage">
                      {mutationIndex < 0 ? <div className="leader"><b>{individual.parentA == null || individual.parentA < 0 ? "Початкова ДНК" : "Еліта"}</b><small>{individual.parentA == null || individual.parentA < 0 ? "ще без канібалізації" : "перенесено без змін"}</small></div> : <>
                        <div className="victim"><b>#{(individual.consumedId ?? -1) + 1}</b><small>гени 1–{mutationIndex}</small></div>
                        <i>+</i>
                        <div className="fatal"><b>ген {mutationIndex + 1}</b><small>нова альтернатива</small></div>
                        <i>+</i>
                        <div className="leader"><b>#{(individual.parentA ?? -1) + 1}</b><small>хвіст хижака</small></div>
                      </>}
                    </div>
                  )}
                  <div className="method-board-frame">
                    <BoardView board={origin.board} state={(viewMode === "generation" ? leaderState : selectedState)!}
                      animated hidePlayer={viewMode === "generation"}
                      markers={viewMode === "generation" ? generationMarkers : routeMarkers}
                      cellHints={evolution.kind === "gravity" ? cellHints : {}} />
                  </div>
                  {evolution.kind === "gravity" ? (
                    <div className="gravity-live-key"><span><i /> поле висот</span><span><i /> напрямок стоку</span><strong>{individual.shakes ?? 0} струшувань у маршруті</strong></div>
                  ) : (
                    <div className={`cannibal-live-dna ${mutationIndex < 0 ? "elite" : ""}`} aria-label="Походження ДНК вибраного бота">
                      {mutationIndex < 0 ? <span className="predator" style={{ flexGrow: Math.max(1, genes.length) }}>{individual.parentA == null || individual.parentA < 0 ? "початкова ДНК" : "елітна ДНК"}</span> : <>
                        <span className="victim" style={{ flexGrow: Math.max(1, mutationIndex) }}>жертва</span>
                        <span className="escape">нова</span>
                        <span className="predator" style={{ flexGrow: Math.max(1, genes.length - mutationIndex - 1) }}>хижак</span>
                      </>}
                    </div>
                  )}
                </div>
              ) : <BoardView board={origin.board} state={(viewMode === "generation" ? leaderState : selectedState)!}
                  animated hidePlayer={viewMode === "generation"}
                  markers={viewMode === "generation" ? generationMarkers : routeMarkers} />
              : <div className="evolution-loading">Завантаження поля…</div>}
          </div>
          {viewMode === "generation" ? null : (
            <div className="route-legend" aria-label="Позначення маршруту">
              <span><i className="visited" /> пройдена клітинка</span>
              <span><i className="active" /> поточна клітинка</span>
            </div>
          )}
        </section>

        <aside className="evolution-inspector">
          <div className="evolution-summary">
            {viewMode === "generation" ? <>
              <span><b>#{leader.id + 1}</b>лідер {cycleName}</span>
              <span><b>{Math.round(leader.cost)}</b>найменша вартість</span>
              <span><b>{individuals.length}</b>учасників</span>
              <span><b>{winnerCount}</b>завершили рівень</span>
            </> : <>
              <span><b>{individual.won ? "Ціль досягнута" : "Без перемоги"}</b>результат</span>
              <span><b>{Math.round(individual.cost)}</b>вартість, менше — краще</span>
              <span><b>{moves.length}</b>допустимих ходів</span>
              {hasGenes ? <span><b>{individual.parentA == null || individual.parentA < 0
                ? "Перше покоління"
                : evolution.kind === "cannibal" && (individual.consumedId == null || individual.consumedId < 0)
                  ? `Еліта від #${individual.parentA + 1}`
                  : `#${individual.parentA + 1} + #${individual.parentB! + 1}`}</b>{evolution.kind === "cannibal" ? "походження ДНК" : "батьківські хромосоми"}</span> : null}
              {evolution.kind === "gravity" ? <span><b>{individual.shakes ?? 0}</b>струшувань рельєфу</span> : null}
              {evolution.kind === "cannibal" && individual.fatalGene != null && individual.fatalGene >= 0 ? <span><b>ген {individual.fatalGene + 1}</b>підтверджений тупик</span> : null}
            </>}
          </div>

          <section className="fitness-panel">
            <div className="evolution-panel-heading">
              <span>{copy.best}</span>
              <small>натисніть стовпчик</small>
            </div>
            <div className="fitness-values">
              <span>Цей цикл <b>{Math.round(currentBest)}</b></span>
              <span>Рекорд дотепер <b>{Math.round(recordBest)}</b></span>
            </div>
            <div className="fitness-chart" aria-label="Найкраща вартість за поколіннями">
              {bestCosts.map((cost, index) => {
                const range = maxCost - minCost;
                const height = range === 0 ? 64 : 28 + ((maxCost - cost) / range) * 72;
                const newRecord = index === 0 || cost < runningBestCosts[index - 1];
                return <button key={index} type="button"
                  className={`${index === generation ? "current" : ""} ${newRecord ? "record" : ""}`}
                  style={{ height: `${height}%` }} onClick={() => selectGeneration(index)}
                  aria-label={`${copy.cycleLabel} ${index + 1}, вартість ${Math.round(cost)}`}
                  title={`${copy.cycleLabel} ${index + 1}: ${Math.round(cost)}`} />;
              })}
            </div>
            <div className="fitness-axis"><span>1</span><span>менше — краще</span><span>{bestCosts.length}</span></div>
          </section>
        </aside>
      </div>

      {viewMode === "generation" ? (
        <section className="generation-roster">
          <div className="evolution-panel-heading">
            <span>Усі {individuals.length} {participantPlural} · ★ лідер</span>
            <small>натисніть учасника для детального маршруту</small>
          </div>
          <div className="generation-roster-list">
            {individuals.map((item, index) => {
              const sequence = hasGenes ? item.genes ?? "" : item.moves;
              const progress = item.moves.length === 0 ? 1 : Math.min(step / item.moves.length, 1);
              return <button type="button" key={item.id} className={index === leaderIndex ? "leader" : ""}
                onClick={() => selectParticipant(index)}>
                <span>{index === leaderIndex ? "★ " : ""}#{item.id + 1}</span>
                <code>{Array.from(sequence.slice(0, 9)).map(gene => arrows[gene]).join("")}{sequence.length > 9 ? "…" : ""}</code>
                <i><b style={{ transform: `scaleX(${progress})` }} /></i>
                <strong>{Math.round(item.cost)}</strong>
              </button>;
            })}
          </div>
        </section>
      ) : (
        <>
          {hasGenes ? (
            <section className="sequence-panel">
              <div className="evolution-panel-heading">
                <span>Повна хромосома · {genes.length} генів</span>
                <small>{individual.mutationIndex == null || individual.mutationIndex < 0 ? "без мутації" : `мутація: ген ${individual.mutationIndex + 1}`}</small>
              </div>
              <div className="gene-strip chromosome-strip" aria-label="Гени вибраної хромосоми">
                {Array.from(genes).map((gene, index) => <span key={index}
                  className={`${individual.mutationIndex === index ? "mutated" : ""} ${evolution.kind === "cannibal" && individual.mutationIndex != null && index < individual.mutationIndex ? "consumed" : ""} ${evolution.kind === "cannibal" && individual.mutationIndex != null && index > individual.mutationIndex ? "predator" : ""}`}
                  title={`Ген ${index + 1}: ${gene}`}>{arrows[gene]}</span>)}
              </div>
              <p>Стіни можуть відхилити частину генів, тому нижній маршрут іноді коротший за хромосому.</p>
            </section>
          ) : null}

          <section className="sequence-panel route-sequence">
            <div className="evolution-panel-heading">
              <span>{copy.commands}</span>
              <small>{moves.length} ходів</small>
            </div>
            <div ref={routeRef} className="gene-strip" aria-label={copy.route}>
              {Array.from(moves).map((move, index) => <span key={index}
                className={`${index === step ? "current" : ""} ${index < step ? "done" : ""}`}
                title={`Хід ${index + 1}: ${move}`}>{arrows[move]}</span>)}
            </div>
          </section>
        </>
      )}

      <div className="evolution-playback">
        <button type="button" className="button secondary" disabled={step === 0}
          onClick={() => { setPlaying(false); setStep(value => value - 1); }}>← Крок</button>
        <button type="button" className="button primary" disabled={!playbackLength}
          onClick={() => {
            if (step === playbackLength) setStep(0);
            setPlaying(value => !value);
          }}>{playing ? "Пауза" : step === playbackLength ? "Ще раз" : viewMode === "generation" ? `Запустити всіх ${participantPlural}` : "Відтворити"}</button>
        <button type="button" className="button secondary" disabled={step === playbackLength}
          onClick={() => { setPlaying(false); setStep(value => value + 1); }}>Крок →</button>
        <label className="evolution-scrubber">Крок
          <input aria-label="Крок маршруту" type="range" min="0" max={playbackLength} value={step}
            onChange={event => { setPlaying(false); setStep(Number(event.target.value)); }} />
        </label>
        <span>{step} / {playbackLength}</span>
        <label>Швидкість
          <select value={speed} onChange={event => setSpeed(Number(event.target.value))}>
            <option value="1000">Повільно</option><option value="600">Звичайно</option><option value="240">Швидко</option>
          </select>
        </label>
      </div>
    </div>
  );
}
