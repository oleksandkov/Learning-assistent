// Presentation of XSB glyphs only. Rules and state changes belong to the C++ core.
export default function MiniBoard({
  xsb,
  hero = false,
}: {
  xsb: string;
  hero?: boolean;
}) {
  const rows = xsb
    .replace(/\r/g, "")
    .split("\n")
    .filter((row) => row.includes("#"));
  const width = Math.max(...rows.map((row) => row.length));
  return (
    <div
      className={`mini-board ${hero ? "hero-board" : ""}`}
      style={{ gridTemplateColumns: `repeat(${width}, 1fr)` }}
      aria-hidden="true"
    >
      {rows.flatMap((row, y) =>
        Array.from({ length: width }, (_, x) => {
          const ch = row[x] ?? " ";
          const kind =
            ch === "#"
              ? "wall"
              : ch === "$"
                ? "box"
                : ch === "@"
                  ? "player"
                  : ch === "."
                    ? "goal"
                    : ch === "*"
                      ? "box on-goal"
                      : ch === "+"
                        ? "player"
                        : "floor";
          return (
            <span key={`${x}-${y}`} className={`mini-cell ${kind}`}>
              <i />
            </span>
          );
        }),
      )}
    </div>
  );
}
