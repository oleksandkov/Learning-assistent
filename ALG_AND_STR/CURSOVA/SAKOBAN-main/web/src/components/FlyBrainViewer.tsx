"use client";
import { useEffect, useMemo, useRef, useState } from "react";
import type { SearchResult, Snapshot } from "@/lib/types";
import { DIRECTIONS, MAX_STEPS } from "@/lib/flybrain";
import FlyBrain3D from "./FlyBrain3D";
import BoardView, { type BoardMarker } from "./BoardView";
import { replayState } from "./EvolutionViewer";
import "./flybrain.css";

const arrows = { U: "↑", D: "↓", L: "←", R: "→" };
const colors = ["#56c6e7", "#bc9cfa", "#f4c26b"];

function CXCompassRadar({ heading = 0 }: { heading?: number }) {
  const deg = (heading * 180) / Math.PI;
  return (
    <div className="flybrain-cx-dial" title={`Центральний комплекс (CX): азимут ${Math.round(deg)}°`}>
      <svg viewBox="0 0 100 100" className="cx-dial-svg" aria-label={`Компас CX: ${Math.round(deg)} градусів`}>
        <circle cx="50" cy="50" r="44" className="cx-ring-outer" />
        <circle cx="50" cy="50" r="32" className="cx-ring-inner" />
        <line x1="50" y1="8" x2="50" y2="92" className="cx-axis" />
        <line x1="8" y1="50" x2="92" y2="50" className="cx-axis" />
        <text x="50" y="16" className="cx-cardinal">Пн</text>
        <text x="88" y="54" className="cx-cardinal">Сх</text>
        <text x="50" y="90" className="cx-cardinal">Пд</text>
        <text x="12" y="54" className="cx-cardinal">Зх</text>
        <g className="cx-needle-group" style={{ transform: `rotate(${deg}deg)`, transformOrigin: "50px 50px" }}>
          <polygon points="50,14 46,50 54,50" className="cx-needle-north" />
          <polygon points="50,86 46,50 54,50" className="cx-needle-south" />
          <circle cx="50" cy="50" r="3.5" className="cx-pivot" />
        </g>
      </svg>
    </div>
  );
}

function RayCastingRadar({ rays }: { rays?: number[] }) {
  if (!rays || rays.length !== 8) return null;
  const labels = ["Пн", "Пн-Сх", "Сх", "Пд-Сх", "Пд", "Пд-Зх", "Зх", "Пн-Зх"];
  return (
    <div className="flybrain-rays-panel">
      <div className="flybrain-rays-header">
        <span>Фасетковий зір</span>
        <span>360° сканування</span>
      </div>
      <div className="flybrain-rays-grid">
        {rays.map((dist, i) => (
          <div key={labels[i]} className="ray-item" title={`${labels[i]}: огляд ${Math.round(dist * 100)}%`}>
            <span className="ray-label">{labels[i]}</span>
            <div className="ray-bar-bg">
              <div className="ray-bar-fill" style={{ transform: `scaleX(${Math.max(0.06, dist)})` }} />
            </div>
            <span className="ray-val">{Math.round(dist * 100)}%</span>
          </div>
        ))}
      </div>
    </div>
  );
}

export default function FlyBrainViewer({ result, loadSnapshot, running = false, baseline, compare, recalculate, cancel, retryUntilSolved = false, onRetryChange }: {
  result: SearchResult;
  loadSnapshot: (moves: string, signal?: AbortSignal) => Promise<Snapshot>;
  running?: boolean;
  baseline?: SearchResult;
  compare?: () => void;
  recalculate?: () => void;
  cancel?: () => void;
  retryUntilSolved?: boolean;
  onRetryChange?: (value: boolean) => void;
}) {
  const history = result.flybrain!;
  const frames = history.frames;
  const windowMs = history.config?.windowMs ?? 8;
  const [open3d, setOpen3d] = useState(false);
  const dialog = useRef<HTMLDialogElement>(null);
  const [step, setStep] = useState(running ? 0 : frames.length);
  const [playing, setPlaying] = useState(false);
  const [following, setFollowing] = useState(false);
  const [speed, setSpeed] = useState(240);
  const [trails, setTrails] = useState(true);
  const [origin, setOrigin] = useState<Snapshot | null>(null);
  const [error, setError] = useState("");
  const loader = useRef(loadSnapshot);
  loader.current = loadSnapshot;
  const canvas = useRef<HTMLCanvasElement>(null);
  const count = Math.min(step, frames.length);
  const frame = frames[count - 1];

  useEffect(() => {
    const preference = window.matchMedia("(prefers-reduced-motion: reduce)");
    setFollowing(!preference.matches);
    setStep(running ? 0 : frames.length);
    setPlaying(false);
    const changed = () => { if (preference.matches) { setFollowing(false); setPlaying(false); } };
    preference.addEventListener("change", changed);
    const abort = new AbortController();
    void loader.current("", abort.signal).then(value => {
      if (!value.accepted) throw new Error("Початкова позиція недійсна.");
      setOrigin(value);
    }).catch(e => { if (!abort.signal.aborted) setError(e.message); });
    return () => { abort.abort(); preference.removeEventListener("change", changed); };
  }, [history.seed]);

  useEffect(() => {
    if (following) setStep(frames.length);
  }, [frames.length, following, running]);

  useEffect(() => {
    if (!running && result.status === "Solved" && result.validated) {
      const won = frames.findIndex(f => f.won);
      setStep(won >= 0 ? won + 1 : frames.length);
      setPlaying(false);
    }
  }, [running, result.status, result.validated, history.seed]);

  useEffect(() => {
    if (!playing || count >= frames.length) { setPlaying(false); return; }
    let handle = 0, last = performance.now();
    const tick = (now: number) => {
      if (now - last >= speed) { setStep(value => Math.min(value + 1, frames.length)); last = now; }
      handle = requestAnimationFrame(tick);
    };
    handle = requestAnimationFrame(tick);
    return () => cancelAnimationFrame(handle);
  }, [playing, speed, frames.length, count]);

  useEffect(() => {
    const element = canvas.current;
    const ctx = element?.getContext("2d");
    if (!element || !ctx) return;
    const draw = () => {
      const width = Math.max(240, element.clientWidth), height = 300;
      element.width = width * 2; element.height = height * 2;
      ctx.scale(2, 2); ctx.fillStyle = "#172a47"; ctx.fillRect(0, 0, width, height);
      const left = 65, right = width - 20, top = 22, band = 76;
      const start = Math.max(0, count - 150), span = Math.max(30, Math.min(150, count));
      ctx.font = "11px 'Segoe UI', sans-serif";
      for (let group = 0; group < 3; group++) {
        ctx.fillStyle = colors[group]; ctx.fillText(["Sensory", "Central", "Motor"][group], 12, top + group * band + 37);
        ctx.fillStyle = "#ffffff06"; ctx.fillRect(left, top + group * band, right - left, band - 7);
      }
      ctx.strokeStyle = "#ffffff18"; ctx.fillStyle = "#b7c9e2";
      for (let i = 0; i <= 5; i++) {
        const x = left + (right - left) * i / 5;
        ctx.beginPath(); ctx.moveTo(x, top); ctx.lineTo(x, top + band * 3); ctx.stroke();
        ctx.fillText(String(start + Math.round(span * i / 5)), x - 4, height - 20);
      }
      for (const f of frames.slice(start, count)) for (const sample of f.samples) {
        const group = Math.floor(sample / 40);
        ctx.fillStyle = colors[group];
        ctx.fillRect(left + (f.t - start) / span * (right - left) - 2,
          top + group * band + sample % 40 / 40 * (band - 10), 2.5, 2.5);
      }
    };
    draw();
    const resize = new ResizeObserver(draw);
    resize.observe(element);
    return () => resize.disconnect();
  }, [frames, count]);

  useEffect(() => {
    if (open3d) dialog.current?.showModal(); else dialog.current?.close();
  }, [open3d]);

  const acceptedMoves = useMemo(() => frames.slice(0, count).filter(f => f.accepted).map(f => f.dir).join(""), [frames, count]);
  const state = useMemo(() => origin ? replayState(origin, acceptedMoves, acceptedMoves.length) : null, [origin, acceptedMoves]);
  const markers = useMemo<BoardMarker[]>(() => {
    if (!origin || !trails) return [];
    const cells = new Map<number, BoardMarker>();
    let cell = origin.state.player;
    // Presentation only: every move in this trail has already been accepted by C++.
    for (let i = 0; i < acceptedMoves.length; i++) {
      const dir = acceptedMoves[i] as keyof typeof arrows;
      if (i >= acceptedMoves.length - 150) cells.set(cell, { cell, kind: "route", label: arrows[dir] });
      cell += dir === "U" ? -origin.board.width : dir === "D" ? origin.board.width : dir === "L" ? -1 : 1;
    }
    cells.set(cell, { cell, kind: "current" });

    // Ghost traces for mental replay (active inference projection):
    if (frame?.ghostTrace && origin) {
      let ghostCell = cell;
      for (let i = 0; i < frame.ghostTrace.length; i++) {
        const gdir = frame.ghostTrace[i] as keyof typeof arrows;
        ghostCell += gdir === "U" ? -origin.board.width : gdir === "D" ? origin.board.width : gdir === "L" ? -1 : 1;
        if (!cells.has(ghostCell) && ghostCell >= 0 && ghostCell < origin.board.walls.length && !origin.board.walls[ghostCell]) {
          cells.set(ghostCell, { cell: ghostCell, kind: "push", label: "·" });
        }
      }
    }
    return Array.from(cells.values());
  }, [acceptedMoves, origin, trails, frame?.ghostTrace]);

  const directionCounts = DIRECTIONS.map(d => frames.filter(f => f.dir === d).length);
  const entropy = directionCounts.reduce((total, n) => n ? total - n / frames.length * Math.log2(n / frames.length) : total, 0);
  const activity = frame ? [frame.sensory, frame.central, ...frame.motor] : [0, 0, 0, 0, 0, 0];
  const progress = frames.filter(f => f.goals !== undefined);
  const winner = frames.findIndex(f => f.won);
  const large = origin && (origin.board.width - 2 > 22 || origin.board.height - 2 > 22);
  const selectStep = (value: number) => { setFollowing(false); setPlaying(false); setStep(value); };

  const playbackControls = (
    <div className="flybrain-controls">
      <button type="button" aria-label="FlyBrain: попередній крок" disabled={!count} onClick={() => selectStep(count - 1)}>←</button>
      <button type="button" className="flybrain-play" disabled={!frames.length} onClick={() => {
        if (playing || (following && running)) { setFollowing(false); setPlaying(false); }
        else if (running && count === frames.length) setFollowing(true);
        else { if (count === frames.length) setStep(0); setPlaying(true); }
      }}>{playing || (following && running) ? "Пауза" : "Відтворити"}</button>
      <button type="button" aria-label="FlyBrain: наступний крок" disabled={count >= frames.length} onClick={() => selectStep(count + 1)}>→</button>
      <label className="flybrain-scrubber">Крок <input type="range" aria-label="Крок FlyBrain" min={0} max={frames.length} value={count} onChange={e => selectStep(Number(e.target.value))} /></label>
      <output>{count} / {frames.length}</output>
      <label>Темп <select aria-label="Швидкість FlyBrain" value={speed} onChange={e => setSpeed(Number(e.target.value))}>
        <option value={1000}>Повільно</option><option value={600}>Звичайно</option><option value={240}>Швидко</option>
      </select></label>
      {running && <button type="button" onClick={() => { setPlaying(false); setFollowing(true); }}>До live</button>}
    </div>
  );

  const runControls = (
    <div className="flybrain-run-controls">
      <label><input type="checkbox" checked={retryUntilSolved} onChange={e => onRetryChange?.(e.target.checked)} />Повторювати до розв’язання</label>
      <p>Новий seed після кожної невдалої спроби. Зупиниться, коли C++ підтвердить усі ящики на цілях.</p>
      {running ? <button type="button" className="button secondary" onClick={cancel}>Скасувати пошук</button>
        : <button type="button" className="button primary" onClick={recalculate}>↻ Перерахувати тут</button>}
    </div>
  );

  return (
    <div className="flybrain-viewer" data-seed={history.seed} data-attempt={history.attempt ?? 1} data-step={count} data-frames={frames.length}>
      <header className="flybrain-heading">
        <div><h3>Від імпульсу до руху</h3><p>Біо-агент · не гарантує рішення · ліміт {history.config?.maxSteps ?? MAX_STEPS} кроків</p></div>
        <span className={`flybrain-session ${running ? "recording" : ""}`}><i />{running ? `Спроба ${history.attempt ?? 1} · запис триває` : "Запис завершено"}</span>
      </header>
      <div className="flybrain-circuit" aria-label="Будова симуляції">
        <span><b>64</b> сенсорні (фасеткові)</span><i>→</i><span><b>1 780</b> центральні (MB)</span><i>→</i><span><b>4 × 40</b> моторні (симетричні)</span><i>→</i><span>хід перевіряє C++</span>
      </div>
      <div className="flybrain-model-launch">
        <button type="button" className="button secondary" onClick={() => setOpen3d(true)}>Відкрити 3D-мозок</button>
        <span>Обертайте мережу та відтворюйте справжні спайки</span>
      </div>
      {runControls}
      {!running && result.status === "Solved" && result.validated && (
        <p className="flybrain-solved" role="status">✓ Розв’язано · усі ящики на цілях. Повторні спроби зупинено.</p>
      )}
      <div className="flybrain-workspace">
        <section className="flybrain-recording" aria-label="Нейронна активність">
          <div className="flybrain-panel-title"><h4>Спайки нейронів</h4><span>120 із {history.neurons} · вікно {windowMs} мс</span></div>
          <canvas ref={canvas} role="img" aria-label={`Спайки: крок ${count}, дія ${frame ? arrows[frame.dir] : "ще не обрана"}. Sensory, Central, Motor.`} />
          <div className="flybrain-raster-caption"><span>Крапка = спайк · порожня смуга = тиша</span><span>Кроки {Math.max(0, count - 150)}–{count}</span></div>
          <div className="flybrain-output">
            <div className="flybrain-directions" aria-label="Моторна команда">
              {(["U", "L", "D", "R"] as const).map(dir => (
                <span key={dir} className={frame?.dir === dir ? (frame.accepted ? "chosen" : "blocked") : ""}>
                  {arrows[dir]}
                </span>
              ))}
            </div>
            <div className="flybrain-output-info">
              <div className="flybrain-output-primary">
                <strong>{frame ? (frame.accepted ? "Хід прийнято" : "Хід заблоковано") : "Очікування імпульсу"}</strong>
                {frame?.macroIntent && <span className="flybrain-intent-pill">{frame.macroIntent}</span>}
              </div>
              <p>
                {frame
                  ? frame.deadlockAverted
                    ? "Рефлекс болю: відвернуто кутовий дедлок, обрано безпечну альтернативу."
                    : frame.motor.every(n => n === 0)
                    ? "Моторні пули мовчать: випадкова стохастична спроба."
                    : "Напрямок обрано за сумарною кількістю спайків моторного пулу."
                  : "Поле → струми → спайки → напрямок."}
              </p>
            </div>
            {frame?.ghostTrace && (
              <span className="flybrain-projection-tag" title="Проекція уявного моделювання кроку">
                Проекція: {frame.ghostTrace.split("").map(d => arrows[d as keyof typeof arrows] ?? d).join(" ")}
              </span>
            )}
          </div>
        </section>
        <section className="flybrain-board-panel" aria-label="Поле FlyBrain">
          <div className="flybrain-panel-title"><h4>Слід на полі</h4><span>{acceptedMoves.length} ходів</span></div>
          <div className="flybrain-board">
            {origin && state ? <BoardView board={origin.board} state={state} markers={markers} fullBoard /> : <p>{error || "Завантаження поля…"}</p>}
          </div>
          <label className="flybrain-trails">
            <input type="checkbox" checked={trails} onChange={e => setTrails(e.target.checked)} />Показати слід
          </label>
        </section>
      </div>
      {!frames.length && !running && <p className="flybrain-notice">{history.steps ? "Історія спайків не збережена. Маршрут і підсумки доступні." : "Сесію завершено без кроків."}</p>}
      {large && <p className="flybrain-notice">На такому полі муха тут страждатиме. Спробуйте 01_simple або власний рівень з одним ящиком.</p>}
      {playbackControls}
      <div className="flybrain-analysis">
        <section>
          <h4>Активність на кроці {count}</h4>
          <div className="flybrain-bars">
            {["Sensory", "Central", "↑ U", "↓ D", "← L", "→ R"].map((label, i) => (
              <div key={label}>
                <span>{label}</span><meter min={0} max={Math.max(1, ...activity)} value={activity[i]} aria-label={`${label}: спайки`} /><b>{activity[i]}</b>
              </div>
            ))}
          </div>
        </section>
        <section>
          <h4>Що відбулося</h4>
          <dl className="flybrain-stats">
            <div><dt>Спайків / крок / 1 000 нейронів</dt><dd>{history.spikeRate.toFixed(2)}</dd></div>
            <div><dt>Ходи / штовхання</dt><dd>{result.moves.length} / {result.pushes}</dd></div>
            <div><dt>Повторні стани / заблоковані спроби</dt><dd>{history.loops} / {result.deadlocks}</dd></div>
            <div><dt>Ентропія напрямків (0–2 біти)</dt><dd>{entropy.toFixed(2)}</dd></div>
            <div>
              <dt>Дофамін STDP (крок {count})</dt>
              <dd className={frame?.dopamine && frame.dopamine > 0 ? "fb-val-reward" : frame?.dopamine && frame.dopamine < 0 ? "fb-val-penalty" : ""}>
                {frame?.dopamine !== undefined
                  ? `${frame.dopamine > 0 ? "+" : ""}${frame.dopamine.toFixed(1)} DA · ${frame.dopamine > 0 ? "LTP зміцнення" : frame.dopamine < 0 ? "LTD пригнічення" : "спокій"}`
                  : "0.0 DA"}
              </dd>
            </div>
          </dl>
          <p className="flybrain-distribution">{DIRECTIONS.map((d, i) => <span key={d}>{arrows[d]} {directionCounts[i]}</span>)}</p>
        </section>
        <section className="flybrain-nav-section">
          <h4>Орієнтація та зір (CX)</h4>
          <div className="flybrain-nav-instrument">
            <CXCompassRadar heading={frame?.heading} />
            <div className="flybrain-nav-meta">
              <div className="nav-row">
                <span className="nav-row-label">Азимут CX</span>
                <b className="nav-row-value">{frame ? `${Math.round(((((frame.heading ?? 0) * 180 / Math.PI) + 360) % 360))}°` : "—"}</b>
              </div>
              <div className="nav-row">
                <span className="nav-row-label">Вектор цілі</span>
                <b className="nav-row-value">{frame?.compassDir ? (arrows[frame.compassDir as keyof typeof arrows] ?? frame.compassDir) : "—"}</b>
              </div>
              <p className="nav-caption">Кільцевий атрактор CX утримує внутрішній вектор напрямку.</p>
            </div>
          </div>
          <RayCastingRadar rays={frame?.rays} />
        </section>
        <section>
          <h4>Ящики на цілях</h4>
          <svg className="flybrain-goals" viewBox="0 0 300 95" role="img" aria-label={`Ящики на цілях: ${Math.round((progress.at(-1)?.goals ?? 0) * 100)} відсотків; вибірка кожні 10 кроків`}>
            <path d="M0 12H300 M0 78H300" fill="none" stroke="currentColor" opacity=".2" />
            <polyline points={progress.map(f => `${f.t / Math.max(1, history.steps) * 300},${78 - f.goals! * 66}`).join(" ")} fill="none" stroke="currentColor" strokeWidth="3" />
            {progress.map(f => <circle key={f.t} cx={f.t / Math.max(1, history.steps) * 300} cy={78 - f.goals! * 66} r="3" fill="currentColor" />)}
          </svg>
          <p>{progress.length ? `${Math.round(progress[progress.length - 1].goals! * 100)}% · вибірка кожні 10 кроків` : "Перша вибірка після 10 кроків"}</p>
          <button type="button" className="flybrain-winner" disabled={winner < 0} onClick={() => selectStep(winner + 1)}>Показати переможний префікс</button>
        </section>
      </div>
      <footer className="flybrain-footnote">
        <div>
          <strong>A* {baseline ? (baseline.status === "Solved" ? baseline.pushes : "ліміт / без рішення") : "—"} vs FlyBrain {result.pushes} штовхань</strong>
          <p>{result.status === "Solved" ? "FlyBrain завершив рівень; оптимальність не гарантована." : "Маршрут FlyBrain ще не розв’язав рівень."}</p>
          {!baseline && compare && <button type="button" className="button subtle" disabled={running} onClick={compare}>Обчислити A* з тієї ж позиції</button>}
        </div>
        <p>Спрощена LIF-мережа, не повний коннектом і не жива муха.<br />Seed {history.seed} · до {(history.config?.maxWallMs ?? 20000) / 1000} с + фінальна перевірка.</p>
      </footer>
      <dialog ref={dialog} className="flybrain-dialog" aria-label="3D-мозок FlyBrain" onCancel={() => setOpen3d(false)} onClose={() => setOpen3d(false)}>
        {open3d && (
          <>
            <header>
              <div><h3>Мозок у просторі</h3><p>Спроба {history.attempt ?? 1} · seed {history.seed} · крок {count} · вікно {windowMs} мс</p></div>
              <button type="button" className="button secondary" aria-label="Закрити 3D-мозок" onClick={() => setOpen3d(false)}>Закрити ×</button>
            </header>
            <FlyBrain3D frame={frame} />
            {playbackControls}
            <div className="flybrain-dialog-actions">
              <span role="status">{running ? "Симуляція триває в цьому вікні" : result.status === "Solved" && result.validated ? "✓ Розв’язано · усі ящики на цілях. Повторні спроби зупинено." : "Спробу завершено без рішення"}</span>
            </div>
            {runControls}
          </>
        )}
      </dialog>
    </div>
  );
}
