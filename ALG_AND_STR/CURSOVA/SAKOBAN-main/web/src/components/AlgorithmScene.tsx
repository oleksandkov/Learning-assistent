import type { CSSProperties } from "react";

const SIZE = 7;
const toCell = (x: number, y: number) => y * SIZE + x;
const borderWalls = new Set(
  Array.from({ length: SIZE * SIZE }, (_, index) => index).filter((index) => {
    const x = index % SIZE;
    const y = Math.floor(index / SIZE);
    return x === 0 || y === 0 || x === SIZE - 1 || y === SIZE - 1;
  }),
);
const normalSearch = [
  toCell(2, 3),
  toCell(2, 2),
  toCell(2, 4),
  toCell(1, 3),
  toCell(3, 2),
  toCell(3, 4),
  toCell(3, 3),
  toCell(4, 3),
  toCell(5, 3),
];
const reverseSearch = [...normalSearch].reverse();
const route = new Set([toCell(2, 3), toCell(3, 3), toCell(4, 3), toCell(5, 3)]);

function actorPosition(x: number, y: number): CSSProperties {
  return {
    width: `${100 / SIZE}%`,
    height: `${100 / SIZE}%`,
    transform: `translate(${x * 100}%, ${y * 100}%)`,
  };
}

export default function AlgorithmScene({
  lessonId,
  step,
  total,
}: {
  lessonId: string;
  step: number;
  total: number;
}) {
  const ratio = total <= 1 ? 1 : step / (total - 1);
  const deadlock = lessonId === "deadlocks";
  const generator = lessonId === "generator";
  const parser = lessonId === "parser";
  const reverse = lessonId === "reverse";
  const searchedOrder = reverse ? reverseSearch : normalSearch;
  const searchedCount = Math.max(1, Math.ceil(ratio * searchedOrder.length));
  const searched = new Set(searchedOrder.slice(0, searchedCount));
  const frontier = searchedOrder[Math.min(searchedCount, searchedOrder.length - 1)];
  const showRoute = ratio >= 0.67 && !deadlock && !generator && !parser;
  const generatedWalls = new Set([toCell(4, 1), toCell(1, 4), toCell(4, 4)]);
  const revealedRows = parser ? Math.max(1, Math.ceil(ratio * SIZE)) : SIZE;
  const goal = deadlock ? toCell(5, 3) : toCell(5, 3);
  const movement = ratio >= 0.84 ? 2 : ratio >= 0.68 ? 1 : 0;
  const boxX = deadlock ? (ratio >= 0.65 ? 1 : 3) : 3 + movement;
  const boxY = deadlock ? (ratio >= 0.65 ? 1 : 3) : 3;
  const playerX = deadlock ? (ratio >= 0.65 ? 2 : 2) : 2 + movement;
  const playerY = deadlock ? (ratio >= 0.65 ? 1 : 3) : 3;
  const phase = deadlock
    ? ratio < 0.6
      ? "Перевіряємо доступність"
      : "Тупик підтверджено"
    : generator
      ? ratio < 0.45
        ? "Будуємо геометрію"
        : ratio < 0.8
          ? "Відтягуємо ящики від цілей"
          : "Перевіряємо рішення"
      : parser
        ? ratio < 0.8
          ? "Читаємо поле рядок за рядком"
          : "Поле готове до пошуку"
        : ratio < 0.58
          ? "Досліджуємо можливі стани"
          : ratio < 0.84
            ? "Відновлюємо знайдений шлях"
            : "Відтворюємо ходи";

  return (
    <div className="algorithm-scene">
      <div className="algorithm-phase">{phase}</div>
      <div className="algorithm-field" role="img" aria-label={`Поле алгоритму. ${phase}`}>
        {Array.from({ length: SIZE * SIZE }, (_, index) => {
          const y = Math.floor(index / SIZE);
          const isWall = borderWalls.has(index) || (generator && generatedWalls.has(index) && ratio > 0.2);
          const isRevealed = y < revealedRows;
          return (
            <span
              key={index}
              className={[
                "algorithm-cell",
                isWall ? "wall" : "floor",
                !isRevealed ? "unread" : "",
                searched.has(index) && !isWall ? "searched" : "",
                index === frontier && !isWall ? "frontier" : "",
                showRoute && route.has(index) ? "route" : "",
                index === goal ? "goal" : "",
                deadlock && index === toCell(1, 1) && ratio >= 0.65 ? "danger" : "",
              ].filter(Boolean).join(" ")}
            />
          );
        })}
        {!parser || ratio > 0.45 ? (
          <>
            <span className={`algorithm-actor box ${boxX === 5 && boxY === 3 ? "on-goal" : ""}`} style={actorPosition(boxX, boxY)}>
              <i />
            </span>
            <span className="algorithm-actor player" style={actorPosition(playerX, playerY)}>
              <i />
            </span>
          </>
        ) : null}
      </div>
      <div className="algorithm-legend" aria-hidden="true">
        <span><i className="searched" /> перевірено</span>
        <span><i className="frontier" /> наступне</span>
        <span><i className="route" /> шлях</span>
      </div>
    </div>
  );
}
