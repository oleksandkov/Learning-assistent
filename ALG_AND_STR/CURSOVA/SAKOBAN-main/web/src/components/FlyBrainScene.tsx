import "./flybrain.css";

const stages = ["Побачити", "Накопичити", "Передати", "Обрати", "Перевірити", "Повторити"];
// Deliberately illustrative: this lesson diagram is not a recorded simulation.
const nodes = Array.from({ length: 44 }, (_, i) => {
  const angle = i * 2.399963;
  const radius = 17 + Math.sqrt(i % 22) * 12;
  return { x: (i < 22 ? 215 : 355) + Math.cos(angle) * radius, y: 140 + Math.sin(angle) * radius * .85 };
});

export default function FlyBrainScene({ step }: { step: number }) {
  const advanced = step === 5;
  return <div className="flybrain-lesson" data-phase={step}>
    <div className="flybrain-phases">{stages.map((stage, i) => <span key={stage} className={i === step ? "active" : ""}><b>{i + 1}</b>{stage}</span>)}</div>
    <div className="flybrain-anatomy">
      <svg viewBox="0 0 570 300" role="img" aria-label={`Схема FlyBrain, етап ${step + 1}: ${stages[step]}. Сенсори передають сигнал центральній мережі, потім чотирьом моторним пулам.`}>
        <path className="fb-lobe" d="M276 80C245 25 153 36 139 107C117 173 160 220 224 220C260 219 279 192 285 166C292 192 310 220 346 220C410 220 454 173 432 107C418 36 325 25 294 80Z" />
        <path className="fb-midline" d="M285 77V209" />
        <g className={`fb-connections ${step >= 2 ? "lit" : ""}`}>
          {nodes.map((n, i) => <path key={i} d={`M${n.x} ${n.y}L${nodes[(i + 7) % nodes.length].x} ${nodes[(i + 7) % nodes.length].y}`} />)}
        </g>
        <g className={`fb-sensory-wires ${step >= 1 ? "lit" : ""}`}>
          {[0, 1, 2, 3].map(i => <path key={i} d={`M64 ${96 + i * 30}C105 ${96 + i * 30} 105 140 ${nodes[i].x} ${nodes[i].y}`} />)}
        </g>
        <g className={`fb-motor-wires ${step >= 3 ? "lit" : ""}`}>
          {[0, 1, 2, 3].map(i => <path key={i} d={`M${nodes[22 + i].x} ${nodes[22 + i].y}C440 140 440 ${96 + i * 30} 504 ${96 + i * 30}`} />)}
        </g>
        {nodes.map((n, i) => <circle key={i} className={`fb-neuron ${step >= 2 && i % 3 !== 0 ? "firing" : ""}`} cx={n.x} cy={n.y} r={i % 7 === 0 ? 4 : 2.8} />)}
        {[0, 1, 2, 3].map(i => <g key={i}>
          <circle className="fb-sensor" cx={55} cy={96 + i * 30} r={7} />
          <circle className={`fb-motor ${step >= 3 && i === 3 ? "selected" : ""}`} cx={514} cy={96 + i * 30} r={11} />
          <text className="fb-direction-label" x={514} y={100 + i * 30}>{["↑", "↓", "←", "→"][i]}</text>
        </g>)}
        <circle className="fb-signal" r="6" style={{ transform: `translate(${[55, 155, 285, 420, 514, 55][step]}px, ${[126, 126, 125, 160, 186, 126][step]}px)` }} />
        <text className="fb-caption" x="55" y="252">64 сенсори</text>
        <text className="fb-caption" x="285" y="252">1 800 центральних нейронів</text>
        <text className="fb-caption" x="514" y="252">4 × 40</text>
        <path className={`fb-feedback ${advanced ? "lit" : ""}`} d="M514 213V281H55V212" />
        <text className="fb-feedback-label" x="285" y="296">{advanced ? "новий стан повертається до сенсорів" : "замкнений сенсомоторний контур"}</text>
      </svg>
      <p>Схема зв’язків · синтетична мережа, не анатомічна копія</p>
    </div>
    <div className="flybrain-lesson-readouts">
      <section className={step < 2 ? "focused" : ""}>
        <h4>Потенціал нейрона</h4>
        <svg viewBox="0 0 240 108" role="img" aria-label={step >= 1 ? "Умовний потенціал досягає порога мінус 50 мілівольт, спайк і скидання" : "Потенціал спокою мінус 70 мілівольт"}>
          <path d="M8 28H232" stroke="currentColor" strokeDasharray="3 5" opacity=".4" />
          <text x="8" y="19">−50 мВ · поріг</text>
          <path className="fb-voltage-rest" d="M8 86H232" style={{ opacity: step ? 0 : 1 }} />
          <path className="fb-voltage-spike" d="M8 86L30 79L48 83L70 66L91 70L112 51L132 54L153 27L158 7L164 77L186 81L208 83L232 85" style={{ opacity: step >= 1 ? 1 : 0 }} />
          <text x="8" y="104">−70 мВ · спокій</text>
        </svg>
      </section>
      <section className={step === 3 ? "focused" : ""}>
        <h4>Спайки за 8 мс</h4>
        <div className="fb-votes">{[3, 1, 5, 11].map((n, i) => <div key={i} className={step >= 3 && i === 3 ? "winner" : ""}>
          <span>{["↑", "↓", "←", "→"][i]}</span><i><b style={{ transform: `scaleX(${step >= 3 ? n / 11 : 0})` }} /></i><strong>{step >= 3 ? n : "—"}</strong>
        </div>)}</div>
      </section>
      <section className={step >= 4 ? "focused" : ""}>
        <h4>{advanced ? "Повертаємо нову позицію" : "Перевірка напрямку R"}</h4>
        <div className="fb-teaching-board" role="img" aria-label={advanced ? "Прийнято: гравець і ящик зрушили праворуч, ящик на цілі" : "Гравець ліворуч від ящика; праворуч вільна ціль"}>
          {Array.from({ length: 15 }, (_, i) => <span key={i} className={i < 5 || i > 9 || i % 5 === 0 || i % 5 === 4 ? "wall" : "floor"} />)}
          <span className="fb-goal">◎</span>
          <span className="fb-teaching-player" style={{ transform: `translateX(${advanced ? 100 : 0}%)` }}>●</span>
          <span className={`fb-teaching-box ${advanced ? "on-goal" : ""}`} style={{ transform: `translateX(${advanced ? 100 : 0}%)` }}>□</span>
        </div>
        <p className="fb-verdict">{step >= 4 ? "✓ C++: accepted" : "Лише намір — поле ще не змінене"}</p>
      </section>
    </div>
  </div>;
}
