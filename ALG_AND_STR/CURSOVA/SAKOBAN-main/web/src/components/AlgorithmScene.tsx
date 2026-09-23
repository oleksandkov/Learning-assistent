import type { CSSProperties } from "react";
import FlyBrainScene from "./FlyBrainScene";

const ROOM = ["#######", "#     #", "#     #", "#     #", "#######"];
const REACH = ["#######", "#  #  #", "# #   #", "#     #", "#######"];
const CORRIDOR = ["#######", "#######", "#     #", "#######", "#######"];
const PARSER = ["       ", " ##### ", " #   # ", " #   # ", " ##### ", "       "];
const PROTOTYPE_ROOM = ["#######", "#     #", "#     #", "#     #", "#     #", "#######"];
const ACO_ROOM = ["#######", "#     #", "#  #  #", "#     #", "#######"];
const cell = (x: number, y: number) => y * 7 + x;

// Small, deterministic examples use the same neighbor/support rules as the C++
// routines. These distances describe the teaching board, not a native solver run.
function distances(rows: string[], start: number, blocked: number[] = [], reverse = false) {
  const width = rows[0].length;
  const result = Array(rows.length * width).fill(-1) as number[];
  const floor = (x: number, y: number) => x >= 0 && x < width && y >= 0 && y < rows.length &&
    rows[y][x] !== "#" && !blocked.includes(y * width + x);
  const queue = [start];
  result[start] = 0;
  for (let i = 0; i < queue.length; i++) {
    const current = queue[i], x = current % width, y = Math.floor(current / width);
    for (const [dx, dy] of [[0, -1], [-1, 0], [0, 1], [1, 0]]) {
      const nx = x + dx, ny = y + dy, next = ny * width + nx;
      if (!floor(nx, ny) || (reverse && !floor(nx + dx, ny + dy)) || result[next] >= 0) continue;
      result[next] = result[current] + 1;
      queue.push(next);
    }
  }
  return result;
}
const reachDistances = distances(REACH, cell(1, 3), [cell(4, 2)]);
const reverseDistances = distances(CORRIDOR, cell(5, 2), [], true);
const exteriorDistances = distances(PARSER, 0);

function Board({ rows = ROOM, player, boxes = [], goals = [], labels = {}, marked = [], route = [], danger = [], caption, text = false }:
  { rows?: string[]; player?: number; boxes?: number[]; goals?: number[]; labels?: Record<number, string>;
    marked?: number[]; route?: number[]; danger?: number[]; caption: string; text?: boolean }) {
  const width = rows[0].length;
  const position = (index: number): CSSProperties => ({
    width: `${100 / width}%`, height: `${100 / rows.length}%`,
    transform: `translate(${index % width * 100}%, ${Math.floor(index / width) * 100}%)`,
  });
  return <div className="lesson-board" style={{ gridTemplateColumns: `repeat(${width}, 1fr)`, aspectRatio: `${width} / ${rows.length}` }}
    role="img" aria-label={caption}>
    {rows.flatMap((row, y) => Array.from(row).map((value, x) => {
      const index = y * width + x;
      return <span key={index} className={["lesson-cell", value === "#" ? "wall" : "floor",
        goals.includes(index) ? "goal" : "", marked.includes(index) ? "marked" : "", route.includes(index) ? "route" : "", danger.includes(index) ? "danger" : ""].join(" ")}>
        <span className="cell-wash" style={{ opacity: labels[index] !== undefined ? 1 : 0,
          transitionDelay: `${Math.min(Number(labels[index]) || 0, 8) * 35}ms` }} />
        <span className="cell-label">{text ? value === "#" ? "#" : player === index ? "@" : boxes.includes(index) ? "$" : goals.includes(index) ? "." : "" : labels[index]}</span>
      </span>;
    }))}
    {!text ? boxes.map((index, i) => <span key={i} className={`lesson-actor box ${goals.includes(index) ? "on-goal" : ""}`} style={position(index)}><i>□</i></span>) : null}
    {!text && player !== undefined ? <span className="lesson-actor player" style={position(player)}><i /></span> : null}
  </div>;
}
function BoardKey({ extra }: { extra?: string }) {
  return <div className="scene-key"><span><i className="key-player" />гравець</span><span><i className="key-box" />ящик</span><span><i className="key-goal" />ціль</span>{extra ? <span>{extra}</span> : null}</div>;
}
function Wave({ kind, step }: { kind: "reach" | "reverse"; step: number }) {
  const reverse = kind === "reverse";
  const ds = reverse ? reverseDistances : reachDistances;
  const limit = (reverse ? [0, 1, 2, 3, 99] : [0, 1, 3, 5, 99])[step];
  const labels = Object.fromEntries(ds.flatMap((d, i) => d >= 0 && d <= limit ? [[i, String(d)]] : []));
  if (reverse && step >= 3) labels[cell(1, 2)] = "×";
  return <>
    <div className="scene-rule">{reverse ? "Від цілі назад ←" : "Від гравця в усі доступні боки"}</div>
    <Board rows={reverse ? CORRIDOR : REACH} player={reverse ? undefined : cell(1, 3)}
      boxes={reverse ? [] : [cell(4, 2)]} goals={[cell(5, 2)]} labels={labels}
      marked={!reverse && step >= 3 ? [cell(3, 2)] : []}
      route={!reverse && step === 4 ? [cell(1, 3), cell(2, 3), cell(3, 3), cell(3, 2)] : []}
      danger={reverse && step >= 3 ? [cell(1, 2)] : []}
      caption={reverse ? `Зворотні відстані до цілі. ${step >= 3 ? "Клітинка біля лівої стіни недосяжна для штовхання." : "Хвиля до відстані " + limit}` : `Досяжність гравця: показано відстані до ${Math.min(limit, 8)} кроків. Ящик не рухається.`} />
    <BoardKey extra={reverse ? "числа — штовхання до цілі" : "числа — кроки від гравця"} />
    <div className="scene-result">{reverse ? step >= 3 ? "× Немає місця, щоб стати позаду ящика" : "Кожна нова клітинка додає 1 штовхання" : step === 4 ? "Виділений підхід: → → ↑ · 3 кроки до ящика" : step === 3 ? "Обведене місце: звідси можна штовхнути →" : "Хвиля проходить підлогою, але не крізь ящик"}</div>
  </>;
}
const graphNodes = [
  { id: "A", x: 42, y: 100 }, { id: "B", x: 145, y: 48 }, { id: "C", x: 145, y: 152 },
  { id: "D", x: 250, y: 48 }, { id: "E", x: 250, y: 152 }, { id: "F", x: 355, y: 48 },
];
function Breadth({ step }: { step: number }) {
  const queue = [["A"], [], ["B", "C"], ["C", "D"], ["D", "E"], ["E", "F"], []][step];
  const seen = [["A"], ["A"], ["A", "B", "C"], ["A", "B", "C", "D"], ["A", "B", "C", "D", "E"], ["A", "B", "C", "D", "E", "F"], ["A", "B", "C", "D", "E", "F"]][step];
  const current = ["", "A", "A", "B", "C", "D", "F"][step];
  return <>
    <div className="scene-rule">Одна літера — ціла позиція гри</div>
    <svg className="search-graph" viewBox="0 0 400 205" role="img" aria-label={`BFS. Поточна позиція ${current || "ще не вибрана"}. Черга: ${queue.join(", ") || "порожня"}`}>
      {[[0, 1], [0, 2], [1, 3], [2, 3], [2, 4], [3, 5]].map(([a, b], i) => <path key={`${a}-${b}`}
        className={`graph-edge ${step >= [2, 2, 3, 4, 4, 5][i] ? "revealed" : ""} ${step === 6 && [[0, 1], [1, 3], [3, 5]].some(([x, y]) => x === a && y === b) ? "solution" : ""}`}
        d={`M${graphNodes[a].x},${graphNodes[a].y} L${graphNodes[b].x},${graphNodes[b].y}`} />)}
      {graphNodes.map(n => <g key={n.id} className={`graph-node ${seen.includes(n.id) ? "seen" : ""} ${current === n.id ? "current" : ""} ${step === 6 && "ABDF".includes(n.id) ? "solution" : ""}`}>
        <circle cx={n.x} cy={n.y} r="22" /><text x={n.x} y={n.y + 5}>{n.id}</text>
        {n.id === "F" ? <text className="graph-caption" x={n.x} y={n.y + 42}>фініш</text> : null}
      </g>)}
      {[42, 145, 250, 355].map((x, i) => <text className="graph-caption" key={x} x={x} y="201">{i} {i === 1 ? "хід" : i === 0 ? "ходів" : "ходи"}</text>)}
    </svg>
    <div className="queue-caption"><span>Беремо звідси</span><span>Додаємо сюди</span></div>
    <div className="queue-track" aria-label={`Черга: ${queue.join(", ") || "порожня"}`}>
      {graphNodes.map(n => <span key={n.id} className="queue-token" aria-hidden={!queue.includes(n.id)}
        style={{ transform: `translateX(${Math.max(0, queue.indexOf(n.id)) * 54}px)`, opacity: queue.includes(n.id) ? 1 : 0 }}>{n.id}</span>)}
      {!queue.length ? <span className="queue-empty">{step === 6 ? "✓ Рішення знайдено" : "A зараз перевіряється"}</span> : null}
    </div>
  </>;
}
function Ranking({ greedy, step }: { greedy: boolean; step: number }) {
  const updated = !greedy && step >= 3;
  const order = updated ? ["B", "A", "C"] : greedy && step >= 1 ? ["B", "A", "C"] : ["A", "B", "C"];
  return <>
    <div className="scene-rule">{greedy ? "Вибір за h: тільки залишок" : "Вибір за f = g + h: повна оцінка"}</div>
    <div className="cost-key"><span>g — уже витрачено</span><span>h — ще залишилося</span></div>
    <div className={`candidate-list ${greedy ? "greedy" : ""}`}>
      {[["A", 2, 4], ["B", updated ? 3 : 5, 2], ["C", 1, 6]].map(([id, g, h]) => {
        const chosen = step >= (greedy ? 1 : 2) && id === (greedy || updated ? "B" : "A");
        return <div key={id} className={`candidate ${chosen ? "chosen" : ""}`} style={{ transform: `translateY(${order.indexOf(String(id)) * 80}px)` }}>
          <b className="candidate-name">{id}</b>
          <div className="candidate-cost"><div className="cost-equation"><span className="cost-g">g = {g}</span><span>h = {h}</span><strong>{greedy ? `h = ${h}` : `f = ${Number(g) + Number(h)}`}</strong></div>
            <div className="cost-bar"><span className="spent" style={{ transform: `scaleX(${Number(g) / 8})` }} /><span className="remaining" style={{ transform: `translateX(${greedy ? 0 : Number(g) / 8 * 100}%) scaleX(${Number(h) / 8})` }} /></div>
          </div><span className="candidate-check">{chosen ? "← вибір" : "чекає"}</span>
        </div>;
      })}
    </div>
    {!greedy ? <div className={`stale-entry ${updated ? "revealed" : ""} ${step === 4 ? "discarded" : ""}`}>
      <span>Старий запис B: g = 5, h = 2, f = 7</span><strong>{step === 4 ? "× Пропускаємо" : "Ще є в черзі"}</strong>
    </div> : null}
    <div className="scene-result">{greedy ? step >= 2 ? "Коротший шлях до B не оновлюється" : "Витрачене g не впливає на порядок" : step === 4 ? "B з g = 5 застарів: уже відомий шлях із g = 3" : updated ? "Краща дорога підняла B на початок" : "Менша сума — вищий пріоритет"}</div>
  </>;
}
function Assignment({ step }: { step: number }) {
  const swapped = step >= 4;
  return <>
    <div className="assignment-layout">
      <table className="cost-matrix"><caption>Оцінка штовхань</caption><thead><tr><th>Ящик</th><th>Ціль 1</th><th>Ціль 2</th></tr></thead>
        <tbody>{[[2, 3], [1, 8]].map((row, i) => <tr key={i}><th>{i === 0 ? "A" : "B"}</th>{row.map((v, j) => <td key={j} className={(swapped ? j === 1 - i : step >= 1 && j === 0 && (i === 0 || step >= 2)) ? step === 2 && j === 0 ? "conflict" : "selected" : ""}>{v}</td>)}</tr>)}</tbody></table>
      <div className="assignment-links" role="img" aria-label={swapped ? "Пари A до цілі 2, B до цілі 1" : step >= 2 ? "Два ящики претендують на ціль 1" : "Ящик A до цілі 1"}>
        <svg viewBox="0 0 260 120"><path className={step >= 1 ? "visible" : ""} d={swapped ? "M35 28 L225 92" : "M35 28 L225 28"} /><path className={step >= 2 ? "visible" : ""} d="M35 92 L225 28" /></svg>
        <span className="match-box a">A</span><span className="match-box b">B</span><span className="match-goal one">1</span><span className="match-goal two">2</span>
        <i className="match-traveler" style={{ opacity: step >= 1 ? 1 : 0, transform: `translate(0, ${swapped ? 64 : 0}px)` }} />
      </div>
    </div>
    <div className="assignment-totals"><span className={step >= 3 ? "discarded" : ""}>A → 1, B → 2 <b>2 + 8 = 10</b></span><span className={swapped ? "selected" : ""}>A → 2, B → 1 <b>3 + 1 = 4</b></span></div>
    <div className="scene-result">{step >= 5 ? "✓ Різні цілі · найменша спільна сума: 4" : step === 2 ? "Одну ціль не можна зайняти двічі" : "Пари можна змінювати, якщо це вигідніше"}</div>
  </>;
}
function Deadlocks({ step }: { step: number }) {
  const block = step === 3, safe = step === 4;
  const box = step === 0 ? cell(2, 1) : cell(1, 1);
  return <>
    <div className={`scene-rule ${step === 2 || block ? "danger-text" : ""}`}>{block ? "Окремий приклад: нерухомий блок" : safe ? "Окремий приклад: ціль у куті" : "Перевіряємо поштовх ліворуч"}</div>
    <Board key={block ? "block" : safe ? "safe" : "corner"} player={block ? cell(1, 3) : step === 0 ? cell(3, 1) : cell(2, 1)}
      boxes={block ? [cell(2, 1), cell(3, 1), cell(2, 2), cell(3, 2)] : [box]}
      goals={block ? [cell(4, 1), cell(5, 1), cell(4, 3), cell(5, 3)] : [safe ? cell(1, 1) : cell(5, 1)]}
      danger={block ? [cell(2, 1), cell(3, 1), cell(2, 2), cell(3, 2)] : step === 2 ? [box] : []}
      caption={block ? "Чотири ящики заблоковані у квадраті 2 на 2" : safe ? "Ящик на цілі в куті: це не кутовий тупик" : `Ящик ${step === 0 ? "ще може рухатись" : "застряг у верхньому лівому куті"}`} />
    <BoardKey /><div className="scene-result">{safe ? "✓ Тут ящик уже виконав свою задачу" : step === 0 ? "Можна штовхати вздовж стіни" : block ? "× Ящики блокують один одного" : "× Не можна стати зліва або зверху"}</div>
  </>;
}
function Hashing({ step }: { step: number }) {
  return <>
    <div className="scene-rule">{step === 4 ? "Zobrist: інший спосіб змішування" : "StateHasher: від позиції до запису"}</div>
    <div className="hash-input"><span>Гравець <b>10</b></span><span>Ящики <b>{step === 0 ? "[12, 8] → [8, 12]" : "[8, 12]"}</b></span></div>
    <div className="hash-flow"><span className={step >= 1 ? "lit" : ""}>↓</span><div className={`hash-machine ${step >= 1 ? "lit" : ""}`}>{step === 4 ? "Числа об’єктів ⊕ XOR" : "Змішати 10, 8, 12"}</div><span className={step >= 2 ? "lit" : ""}>↓</span></div>
    <div className="hash-buckets">{["Інші записи", "Кандидат на повтор", "Інші записи"].map((t, i) => <div key={i} className={step >= 2 && i === 1 ? "selected" : ""}>{t}{i === 1 ? <strong>10 · [8, 12]</strong> : <strong>…</strong>}</div>)}</div>
    <div className={`hash-equality ${step >= 3 ? "revealed" : ""}`}>Гравець збігається ✓<br />Усі ящики збігаються ✓<strong>Тільки тепер: це повтор</strong></div>
  </>;
}
function Replay({ step, generator = false }: { step: number; generator?: boolean }) {
  const boxX = generator ? [5, 4, 3, 3, 4, 5][step] : [3, 3, 4, 5, 5][step];
  const stages = generator ? ["На цілях", "Відтягуємо", "Кандидат", "A*", "Перевірка", "Готово"] : ["Збираємо", "Розвертаємо", "Хід 1", "Хід 2", "Перевірено"];
  return <>
    <div className="scene-stages">{stages.map((s, i) => <span key={s} className={i === step ? "current" : i < step ? "done" : ""}>{s}</span>)}</div>
    {!generator ? <div className="replay-chain"><span style={{ transform: `translateX(${step === 0 ? 200 : 0}%)` }}>A</span><span>B</span><span style={{ transform: `translateX(${step === 0 ? -200 : 0}%)` }}>F</span><i>→</i></div> : <div className="scene-rule">{step < 3 ? "Побудова: тягнемо ←" : "Перевірка: штовхаємо →"}</div>}
    <Board rows={CORRIDOR} boxes={[cell(boxX, 2)]} player={cell(boxX - 1, 2)} goals={[cell(5, 2)]}
      caption={`${generator ? "Генерація" : "Перевірка шляху"}: ящик у стовпці ${boxX + 1}, ціль у стовпці 6.`} />
    <BoardKey /><div className="scene-result">{generator ? step === 5 ? "✓ Зберігаємо кандидата та перевірений шлях" : step < 3 ? "Тягнути дозволено тільки генератору" : "Звичайні правила гри: тільки штовхання" : step === 4 ? "✓ Ціль і лічильники перевірено" : `Виконано штовхань: ${Math.max(0, Math.min(step - 1, 2))} / 2`}</div>
  </>;
}
function Parser({ step }: { step: number }) {
  const labels = Object.fromEntries(exteriorDistances.flatMap((d, i) => d >= 0 && step >= 2 && (step >= 3 || d <= 3) ? [[i, "·"]] : []));
  return <>
    <div className="scene-stages">{["Символи", "Рамка", "Хвиля", "Межі", "Кількості", "Поле"].map((s, i) => <span key={s} className={i === step ? "current" : i < step ? "done" : ""}>{s}</span>)}</div>
    <Board rows={PARSER} player={cell(2, 3)} boxes={[cell(3, 3)]} goals={[cell(4, 3)]}
      labels={labels} text={step === 0} marked={step === 1 ? [0, 1, 2, 3, 4, 5, 6, 35, 36, 37, 38, 39, 40, 41] : []}
      caption={step === 0 ? "XSB: замкнена кімната, один гравець, один ящик і одна ціль" : "Хвиля зовні замкненої кімнати; усередину вона не потрапляє"} />
    <BoardKey extra="блакитне — зовнішній простір" />
    <div className="scene-result">{step >= 4 ? "✓ 1 гравець · 1 ящик · 1 ціль" : step >= 2 ? "Стіна # зупиняє зовнішню хвилю" : "# → стіна   @ → гравець   $ → ящик   . → ціль"}</div>
  </>;
}
function Iterative({ step }: { step: number }) {
  const limit = step >= 4 ? 6 : 4;
  const current = [0, 1, 2, 0, 2, 3][step];
  const nodes = [{ name: "A", x: 40, y: 42, f: 4 }, { name: "B", x: 135, y: 42, f: 4 },
    { name: "C", x: 230, y: 42, f: 6 }, { name: "Ціль", x: 340, y: 42, f: 6 }];
  return <>
    <div className="ida-budget"><span>Дозволена оцінка</span><strong>f ≤ {limit}</strong><div><i style={{ transform: `scaleX(${limit / 8})` }} /></div></div>
    <svg className="ida-graph" viewBox="0 0 390 180" role="img" aria-label={`IDA*: межа ${limit}. Зараз ${nodes[current].name}. ${step === 2 ? "C відсічено: 6 більше 4." : ""}`}>
      <path className="graph-edge revealed" d="M40 42 H340 M40 42 L135 140" />
      {nodes.map((n, i) => <g key={n.name} className={`graph-node ${i === current ? "current" : ""} ${step === 2 && i === 2 ? "cut" : ""} ${step === 5 ? "solution" : ""}`}>
        <circle cx={n.x} cy={n.y} r="23" /><text x={n.x} y={n.y + 5}>{n.name}</text><text className="graph-caption" x={n.x} y={n.y + 45}>f = {n.f}</text>
      </g>)}
      <g className={`graph-node ${step === 3 ? "cut" : ""}`}><circle cx="135" cy="140" r="23" /><text x="135" y="145">D</text><text className="graph-caption" x="200" y="145">f = 7</text></g>
    </svg>
    <div className="ida-stack"><span>Поточний шлях</span>{nodes.map((n, i) => <b key={n.name} style={{ opacity: i <= current ? 1 : 0.2, transform: `translateY(${i <= current ? 0 : 8}px)` }}>{n.name}</b>)}</div>
    <div className="scene-result">{step === 2 ? "× C: 6 > 4 · повертаємося" : step === 3 ? "C: 6, D: 7 → наступна межа 6" : step === 5 ? "✓ g = 6 · h = 0" : "Схема скорочена: між C і ціллю ще 4 штовхання"}</div>
  </>;
}
function Prototype1({ step }: { step: number }) {
  const boxes = [
    [cell(2, 2), cell(2, 4)],
    [cell(2, 2), cell(2, 4)],
    [cell(2, 2), cell(2, 4)],
    [cell(4, 2), cell(2, 4)],
    [cell(4, 3), cell(2, 4)],
    [cell(4, 3), cell(5, 4)],
  ][step];
  const players = [cell(1, 1), cell(1, 1), cell(1, 2), cell(3, 2), cell(4, 2), cell(4, 4)];
  const routes = [[], [], [cell(1, 1), cell(1, 2)], [cell(2, 2), cell(3, 2), cell(4, 2)],
    [cell(4, 1), cell(4, 2), cell(4, 3)], [cell(2, 4), cell(3, 4), cell(4, 4), cell(5, 4)]][step];
  const results = [
    "Board + GameState: повне повторне сканування не потрібне",
    "Ящик A → ціль 1 · ящик B → ціль 2",
    "Гравець став позаду ящика A",
    "X збігся: 4 = 4",
    "Y збігся: 3 = 3 · ціль 1 зайнята",
    "✓ Обидва маршрути пройшли перевірку правил",
  ];
  return <>
    <div className="prototype-phases" aria-label="Фази прототипу">
      <span className={step <= 2 ? "active" : step > 2 ? "done" : ""}>Вибір</span>
      <span className={step === 3 ? "active" : step > 3 ? "done" : ""}>Вісь X</span>
      <span className={step === 4 ? "active" : step > 4 ? "done" : ""}>Вісь Y</span>
      <span className={step === 5 ? "active" : ""}>Перевірка</span>
    </div>
    <div className="prototype-stage">
      <Board rows={PROTOTYPE_ROOM} player={players[step]} boxes={boxes}
        goals={[cell(4, 3), cell(5, 4)]} route={routes}
        caption={`Прототип 1, крок ${step + 1}: ${results[step]}`} />
      <div className="prototype-pairs" aria-label="Призначення ящиків цілям">
        <span className={step >= 1 ? "selected" : ""}><b>A</b><i data-distance="3" /><strong>1</strong></span>
        <span className={step >= 5 ? "selected" : ""}><b>B</b><i data-distance="3" /><strong>2</strong></span>
      </div>
    </div>
    <BoardKey extra="підсвічене — поточний прямий маршрут" />
    <div className="scene-result">{results[step]}</div>
  </>;
}
function AntColony({ step }: { step: number }) {
  const clouds = [
    [[1, 3], [1, 3], [1, 3], [1, 3], [1, 3], [1, 3], [1, 3], [1, 3]],
    [[2, 3], [1, 2], [2, 2], [2, 3], [1, 3], [2, 3], [1, 2], [2, 3]],
    [[3, 3], [2, 1], [2, 2], [3, 3], [1, 1], [2, 3], [2, 1], [3, 3]],
    [[4, 3], [3, 1], [2, 2], [4, 3], [1, 1], [3, 3], [3, 1], [4, 3]],
    [[5, 3], [4, 1], [3, 3], [5, 3], [2, 3], [4, 3], [4, 1], [5, 3]],
    [[5, 2], [5, 2], [4, 3], [5, 2], [5, 3], [5, 2], [5, 1], [5, 2]],
  ][step];
  const route = step >= 4 ? [cell(1, 3), cell(2, 3), cell(3, 3), cell(4, 3), cell(5, 3), cell(5, 2)] : [];
  return <>
    <div className="swarm-meter"><span>Покоління {step + 1}</span><div><i style={{ transform: `scaleX(${(step + 1) / 6})` }} /></div><strong>{step < 4 ? "розвідка" : "збіжність"}</strong></div>
    <div className="swarm-board">
      <Board rows={ACO_ROOM} boxes={[cell(4, 2)]} goals={[cell(5, 2)]} route={route}
        caption={`ACO, покоління ${step + 1}: ${step < 4 ? "мурахи досліджують різні маршрути" : "рій сходиться до правого коридору"}`} />
      {clouds.map(([x, y], index) => <i key={index} className={`teaching-ant ${index === 0 ? "leader" : ""} ${step === 3 && index === 4 ? "stopped" : ""}`}
        style={{ width: `${100 / 7}%`, height: `${100 / 5}%`, transform: `translate(${x * 100}%, ${y * 100}%)`, transitionDelay: `${index * 35}ms` }} />)}
    </div>
    <div className="pheromone-legend"><span>слабкий слід</span><i style={{ transform: `scaleX(${.2 + step * .16})` }} /><strong>підсилений маршрут</strong></div>
    <div className="scene-result">{step === 0 ? "Однаковий феромон · усі напрями рівні" : step === 3 ? "Тупикові стежки отримують штраф" : step === 5 ? "✓ Переможець проходить ReplayValidator" : "Кращі треки стають імовірнішими"}</div>
  </>;
}
function Genetic({ step }: { step: number }) {
  const parentA = "RRUURR", parentB = "ULDDRR", child = step >= 4 ? "RRUDRR" : "RRDDRR";
  const phases = ["Популяція", "Оцінка", "Відбір", "Схрещення", "Мутація", "Перевірка"];
  const strip = (genes: string, role: string) => <div className={`teaching-genes ${role}`}>
    {Array.from(genes).map((gene, index) => {
      const fromA = role === "child" && index < 3;
      const fromB = role === "child" && index >= 3;
      const mutated = step >= 4 && role === "child" && index === 2;
      return <span key={index} className={`${fromA ? "from-a" : ""} ${fromB ? "from-b" : ""} ${mutated ? "mutated" : ""}`}
        title={`Ген ${index + 1}: ${gene}`}>
        <em>{index + 1}</em><b>{arrowsForGene(gene)}</b>
      </span>;
    })}
  </div>;
  return <>
    <div className="genetic-phases" aria-label="Етапи генетичного алгоритму">
      {phases.map((phase, index) => <span key={phase} className={index === step ? "current" : index < step ? "done" : ""}>
        <b>{index + 1}</b>{phase}
      </span>)}
    </div>
    <div className="genetic-workbench">
      <div className="teaching-population">
        <div className={`genetic-card parent-card ${step >= 2 ? "selected" : ""}`}>
          <div><small>{step < 2 ? "Кандидат A" : "Обраний батько A"}</small><strong>fitness 18</strong></div>
          {strip(parentA, "parent-a")}
          {step >= 2 ? <i className="selection-badge">✓ турнір</i> : null}
        </div>
        <div className={`genetic-card parent-card ${step >= 2 ? "selected" : ""}`}>
          <div><small>{step < 2 ? "Кандидат B" : "Обраний батько B"}</small><strong>fitness 23</strong></div>
          {strip(parentB, "parent-b")}
          {step >= 2 ? <i className="selection-badge">✓ турнір</i> : null}
        </div>
        <div className={`crossover-formula ${step >= 3 ? "visible" : ""}`} aria-hidden={step < 3}>
          <span className="source-a">A: 1–3</span><b>+</b><span className="source-b">B: 4–6</span><b>→</b><span>дитина</span>
        </div>
        <div className={`child-row genetic-card ${step >= 3 ? "visible" : ""}`}>
          <div><small>Нова дитина</small><strong>fitness {step >= 4 ? 7 : 14}</strong></div>
          {strip(child, "child")}
        </div>
      </div>

      <aside className="genetic-insight" aria-live="polite">
        {step === 0 ? <><strong>36</strong><span>хромосом у стартовій популяції</span><p>Одна клітинка — одна команда руху.</p></> : null}
        {step === 1 ? <><strong>18 &lt; 23</strong><span>менший fitness — кращий маршрут</span><p>GameRules приймає хід або додає штраф за недопустимий ген.</p></> : null}
        {step === 2 ? <><strong>6</strong><span>еліт переходять без втрати</span><p>Решту батьків обирає турнір: перемагає менший fitness.</p></> : null}
        {step === 3 ? <><div className="gene-origin"><i className="source-a" />A · гени 1–3<i className="source-b" />B · гени 4–6</div><p>Точка розрізу визначає, яку частину дає кожен батько.</p></> : null}
        {step === 4 ? <><div className="mutation-change"><span>3: ↓</span><b>→</b><span>3: ↑</span></div><span>змінився тільки один ген</span><p>Червона рамка показує мутацію.</p></> : null}
        {step === 5 ? <><Board rows={CORRIDOR} player={cell(4, 2)} boxes={[cell(5, 2)]} goals={[cell(5, 2)]}
          caption="Виграшна хромосома поставила ящик на ціль" /><div className="validation-chain"><span>гени</span><b>→</b><span>GameRules</span><b>→</b><span>ReplayValidator ✓</span></div></> : null}
      </aside>
    </div>
    <div className="scene-result">{step === 0 ? "Ген = напрямок, хромосома = можливий маршрут" : step === 1 ? "Кожна хромосома реально програється на полі" : step === 2 ? "Еліта зберігається, турнір обирає батьків" : step === 3 ? "Дитина: перші 3 гени від A, останні 3 — від B" : step === 4 ? "Мутація: третій ген ↓ замінено на ↑" : "✓ Лише допустимий виграшний префікс стає рішенням"}</div>
  </>;
}
function GravityDrain({ step }: { step: number }) {
  const phases = ["Рельєф", "Потоки", "Сила", "Правила", "Струс", "Перевірка"];
  const boxes = [cell(2, 2), cell(2, 2), cell(3, 2), cell(3, 2), cell(4, 2), cell(5, 2)];
  const players = [cell(1, 2), cell(1, 2), cell(2, 2), cell(2, 2), cell(3, 2), cell(4, 2)];
  const results = [
    "Висота спадає до цілі: h3 → h2 → h1 → h0",
    "28 потоків читають той самий рельєф незалежно",
    "Корисне штовхання отримує найбільшу силу",
    "× Вектор у стіну відхилено до оцінювання",
    "Струс додає шум, але не дозволяє заборонений хід",
    "✓ Маршрут повторно пройшов ReplayValidator",
  ];
  return <>
    <div className="drain-phases" aria-label="Етапи гравітаційного зливу">
      {phases.map((phase, index) => <span key={phase} className={index === step ? "current" : index < step ? "done" : ""}><b>{index + 1}</b>{phase}</span>)}
    </div>
    <div className={`gravity-stage ${step === 4 ? "shaking" : ""}`} data-drain-step={step}>
      <section className="terrain-laboratory" aria-label="Поле висот і напрямок потоку">
        <header><span>Поле потенціалу</span><strong>нижче = краще</strong></header>
        <div className="terrain-profile">
          <div className="terrain-scale" aria-hidden="true">
            <span><b>h3</b><small>високо</small></span>
            <span><b>h2</b><small>схил</small></span>
            <span><b>h1</b><small>низина</small></span>
            <span><b>h0</b><small>ціль</small></span>
          </div>
          <div className="drain-liquid" aria-hidden="true">
            {Array.from({ length: 5 }, (_, index) => <i key={index} style={{ transitionDelay: `${index * 55}ms` }} />)}
          </div>
          <div className="drain-mass" aria-hidden="true"><i>□</i><span>ящик</span></div>
          <div className={`drain-vectors ${step >= 1 ? "visible" : ""}`} aria-hidden="true">
            <i className="vector-up blocked"><b>↑</b><small>стіна</small></i>
            <i className="vector-back"><b>←</b><small>−3</small></i>
            <i className="vector-forward"><b>→</b><small>+11</small></i>
          </div>
        </div>
        <div className="drain-causality" aria-label="Послідовність вибору ходу">
          <span className={step <= 2 ? "active" : "done"}><b>1</b>Поле дає силу</span>
          <i>→</i>
          <span className={step === 3 ? "active" : step > 3 ? "done" : ""}><b>2</b>GameRules фільтрує</span>
          <i>→</i>
          <span className={step >= 4 ? "active" : ""}><b>3</b>Виконуємо хід</span>
        </div>
      </section>

      <aside className="drain-console" aria-live="polite">
        <header><span>Сила →</span><strong>{step === 4 ? "+6 шум" : "11"}</strong></header>
        <div className="force-readout">
          <span className={step >= 2 ? "active" : ""}>нахил <b>+8</b></span>
          <span className={step >= 1 ? "active" : ""}>тиск <b>+2</b></span>
          <span className={step >= 1 ? "active" : ""}>інерція <b>+1</b></span>
          <strong><small>разом</small>= 11 →</strong>
        </div>
        <div className={`rules-gate ${step >= 3 ? "checked" : ""}`}>
          <span>GameRules</span>
          <b>{step < 3 ? "очікує" : "→ дозволено"}</b>
          <small>{step >= 3 ? "↑ відхилено: стіна" : "перевіряє реальний Sokoban"}</small>
        </div>
        <div className={`shake-meter ${step === 4 ? "active" : ""}`}><span>Струс</span><i><b /></i><small>{step === 4 ? "вихід із западини" : "не потрібен"}</small></div>
      </aside>

      <div className="drain-reality">
        <div><span>Чесний дискретний replay</span><small>декорація потоку не рухає ящик сама</small></div>
        <Board rows={CORRIDOR} player={players[step]} boxes={[boxes[step]]} goals={[cell(5, 2)]}
          route={step >= 2 ? [cell(2, 2), cell(3, 2), cell(4, 2), cell(5, 2)] : []}
          caption={`Гравітаційний злив, крок ${step + 1}: ${results[step]}`} />
        <div className={`validation-pulse ${step === 5 ? "visible" : ""}`}><b>✓</b><span>ReplayValidator</span></div>
      </div>
    </div>
    <div className="scene-result">{results[step]}</div>
  </>;
}
function CannibalGenetic({ step }: { step: number }) {
  const phases = ["Популяція", "Помилка", "Турнір", "Ролі", "Перенесення", "Перевірка"];
  const genes = (value: string, role: "predator" | "victim" | "offspring") => <div className={`cannibal-genes ${role}`}>
    {Array.from(value).map((gene, index) => <span key={index} style={{ transitionDelay: `${index * 55}ms` }} className={role === "victim" && index === 4 ? "fatal" : role === "offspring" ? index < 4 ? "from-victim" : index === 4 ? "escape" : "from-predator" : ""}>
      <small>{index + 1}</small><b>{arrowsForGene(gene)}</b>{role === "victim" && index === 4 ? <em>×</em> : null}
    </span>)}
  </div>;
  const results = [
    "64 боти стартують з різними послідовностями ходів",
    "Жертва: корисні гени 1–4 · ген 5 веде в тупик",
    "Турнір чотирьох: найменший fitness перемагає",
    "Хижак дає хвіст, жертва — лише безпечний префікс",
    "Дитина = префікс жертви + новий ген + хвіст хижака",
    "✓ Еліта збереглась, переможець пройшов ReplayValidator",
  ];
  return <>
    <div className="cannibal-phases" aria-label="Етапи генетичного канібалізму">
      {phases.map((phase, index) => <span key={phase} className={index === step ? "current" : index < step ? "done" : ""}><b>{index + 1}</b>{phase}</span>)}
    </div>
    <div className="cannibal-arena" data-cannibal-step={step}>
      <section className="tournament-ring" aria-label="Турнір чотирьох ботів">
        <header><span>Локальний турнір</span><strong>4 → 2 ролі</strong></header>
        <div className="combatants">
          <span className="bot predator-bot"><i>#12</i><b>18</b><small>хижак</small></span>
          <span className="bot victim-bot"><i>#31</i><b>1540</b><small>тупик</small></span>
          <span className="bot eliminated"><i>#07</i><b>690</b><small>вибуває</small></span>
          <span className="bot eliminated"><i>#44</i><b>820</b><small>вибуває</small></span>
          <div className="tournament-core"><span>fitness</span><b>менше<br />краще</b></div>
        </div>
        <p>{step < 2 ? "Чотири випадкові боти входять у турнір" : "#12 — найсильніший, #31 має корисний початок до тупика"}</p>
      </section>

      <section className="dna-harvest" aria-label="Перенесення ДНК до нової дитини">
        <div className={`cannibal-card victim ${step >= 2 ? "selected" : ""} ${step >= 3 ? "consumed" : ""}`}>
          <header><span>Жертва #31</span><b>префікс + помилка</b></header>{genes("RRUULR", "victim")}
          <small className="source-caption">Беремо 1–4 · ген 5 відкидаємо</small>
        </div>
        <div className={`cannibal-card predator ${step >= 2 ? "selected" : ""}`}>
          <header><span>Хижак #12</span><b>fitness 18</b></header>{genes("RURDRR", "predator")}
          <small className="source-caption">Зберігаємо хвіст після з’єднання</small>
        </div>

        <div className={`dna-transfer ${step >= 3 ? "visible" : ""}`} aria-hidden="true">
          <i className="victim-beam"><span>1–4</span></i>
          <i className="predator-beam"><span>6</span></i>
          <b>перенесення</b>
        </div>

        <div className={`cannibal-card offspring ${step >= 4 ? "visible" : ""}`}>
          <header><span>Дитина #65</span><b>нова ДНК</b></header>
          {genes("RRUURR", "offspring")}
          <div className="splice-explanation"><span>1–4 від жертви</span><b>+</b><span>5 новий</span><b>+</b><span>6 від хижака</span></div>
        </div>
        <div className={`cannibal-validation ${step === 5 ? "visible" : ""}`}><b>✓</b><span>GameRules</span><i>→</i><span>ReplayValidator</span></div>
      </section>

      <aside className="cannibal-legend"><span><i className="victim-source" />від жертви</span><span><i className="escape-source" />альтернатива</span><span><i className="predator-source" />від хижака</span></aside>
    </div>
    <div className="scene-result">{results[step]}</div>
  </>;
}
function arrowsForGene(gene: string) { return ({ U: "↑", L: "←", D: "↓", R: "→" } as Record<string, string>)[gene]; }
export default function AlgorithmScene({ lessonId, step }: { lessonId: string; step: number }) {
  let scene;
  switch (lessonId) {
    case "flybrain": scene = <FlyBrainScene step={step} />; break;
    case "bfs": scene = <Breadth step={step} />; break;
    case "astar": scene = <Ranking step={step} greedy={false} />; break;
    case "greedy": scene = <Ranking step={step} greedy />; break;
    case "reach": case "reverse": scene = <Wave kind={lessonId} step={step} />; break;
    case "hungarian": scene = <Assignment step={step} />; break;
    case "deadlocks": scene = <Deadlocks step={step} />; break;
    case "hash": scene = <Hashing step={step} />; break;
    case "replay": scene = <Replay step={step} />; break;
    case "generator": scene = <Replay step={step} generator />; break;
    case "parser": scene = <Parser step={step} />; break;
    case "idastar": scene = <Iterative step={step} />; break;
    case "prototype1": scene = <Prototype1 step={step} />; break;
    case "aco": scene = <AntColony step={step} />; break;
    case "genetic": scene = <Genetic step={step} />; break;
    case "gravity": scene = <GravityDrain step={step} />; break;
    case "cannibal": scene = <CannibalGenetic step={step} />; break;
    default: return null;
  }
  return <div className={`algorithm-scene scene-${lessonId}`} data-scene={lessonId} data-step={step}>{scene}</div>;
}
