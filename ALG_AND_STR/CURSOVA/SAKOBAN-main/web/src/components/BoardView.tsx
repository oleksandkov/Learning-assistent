import { useRef, type CSSProperties, type PointerEvent as ReactPointerEvent } from "react";
import type { Board, State } from "@/lib/types";
export interface BoardCamera {
  fov: number;
  follow: boolean;
  centerX: number;
  centerY: number;
}
export interface BoardMarker {
  id?: string;
  cell: number;
  kind: "route" | "push" | "current" | "swarm" | "leader" | "stopped";
  label?: string;
}
export interface BoardCellHint {
  label: string;
  intensity: number;
}
export default function BoardView({
  board,
  state,
  animated = false,
  camera,
  onCameraPan,
  onCameraZoom,
  markers = [],
  cellHints = {},
  hidePlayer = false,
  className = "",
  fullBoard = false,
}: {
  board: Board;
  state: State;
  animated?: boolean;
  camera?: BoardCamera;
  onCameraPan?: (dx: number, dy: number) => void;
  onCameraZoom?: (delta: number) => void;
  markers?: BoardMarker[];
  cellHints?: Record<number, BoardCellHint>;
  hidePlayer?: boolean;
  className?: string;
  fullBoard?: boolean;
}) {
  // Core parser pads the original XSB by one cell; omit that exterior frame for display.
  const fullWidth = board.width - 2,
    fullHeight = board.height - 2;
  const cameraMode = !fullBoard && (fullWidth > 22 || fullHeight > 22);
  const fieldOfView = Math.max(7, Math.min(41, camera?.fov ?? 17));
  const w = cameraMode ? Math.min(fieldOfView, fullWidth) : fullWidth;
  const h = cameraMode ? Math.min(fieldOfView, fullHeight) : fullHeight;
  const playerX = (state.player % board.width) - 1;
  const playerY = Math.floor(state.player / board.width) - 1;
  const focusX = camera?.follow === false ? camera.centerX : playerX;
  const focusY = camera?.follow === false ? camera.centerY : playerY;
  const startX = cameraMode
    ? Math.max(0, Math.min(fullWidth - w, focusX - Math.floor(w / 2)))
    : 0;
  const startY = cameraMode
    ? Math.max(0, Math.min(fullHeight - h, focusY - Math.floor(h / 2)))
    : 0;
  const visible = (cell: number) => {
    const x = (cell % board.width) - 1;
    const y = Math.floor(cell / board.width) - 1;
    return x >= startX && x < startX + w && y >= startY && y < startY + h;
  };
  const drag = useRef<{ x: number; y: number } | null>(null);
  function startPan(event: ReactPointerEvent<HTMLDivElement>) {
    if (!cameraMode || !onCameraPan || event.button !== 0) return;
    drag.current = { x: event.clientX, y: event.clientY };
    event.currentTarget.setPointerCapture(event.pointerId);
    event.currentTarget.classList.add("dragging");
  }
  function movePan(event: ReactPointerEvent<HTMLDivElement>) {
    if (!drag.current || !onCameraPan) return;
    const cellSize = event.currentTarget.clientWidth / w;
    const dx = Math.trunc((drag.current.x - event.clientX) / cellSize);
    const dy = Math.trunc((drag.current.y - event.clientY) / cellSize);
    if (dx === 0 && dy === 0) return;
    onCameraPan(dx, dy);
    drag.current = {
      x: drag.current.x - dx * cellSize,
      y: drag.current.y - dy * cellSize,
    };
  }
  function endPan(event: ReactPointerEvent<HTMLDivElement>) {
    drag.current = null;
    event.currentTarget.classList.remove("dragging");
    if (event.currentTarget.hasPointerCapture(event.pointerId))
      event.currentTarget.releasePointerCapture(event.pointerId);
  }
  const position = (cell: number): CSSProperties => ({
    width: `${100 / w}%`,
    height: `${100 / h}%`,
    transform: `translate(${((cell % board.width) - 1 - startX) * 100}%, ${(Math.floor(cell / board.width) - 1 - startY) * 100}%)`,
  });
  return (
    <div
      className={`game-board ${animated ? "animated-board" : ""} ${cameraMode ? "camera-board" : ""} ${className}`}
      role="img"
      aria-label={`Поле: ${state.boxes.length} ящиків. Гравець: стовпець ${state.player % board.width}, рядок ${Math.floor(state.player / board.width)}.`}
      style={{ aspectRatio: `${w}/${h}` }}
      onPointerDown={startPan}
      onPointerMove={movePan}
      onPointerUp={endPan}
      onPointerCancel={endPan}
      onWheel={(event) => {
        if (!cameraMode || !onCameraZoom) return;
        event.preventDefault();
        onCameraZoom(event.deltaY > 0 ? 2 : -2);
      }}
    >
      <div
        className="board-base"
        style={{ gridTemplateColumns: `repeat(${w}, 1fr)` }}
      >
        {Array.from({ length: w * h }, (_, i) => {
          const cell = (Math.floor(i / w) + 1 + startY) * board.width + (i % w) + 1 + startX;
          return (
            <div
              key={cell}
              className={`board-cell ${board.walls[cell] ? "wall" : board.floor[cell] ? "floor" : "void"} ${board.goals[cell] ? "goal" : ""}`}
            >
              <i />
              {cellHints[cell] ? <span className="board-cell-hint" style={{ opacity: cellHints[cell].intensity }}>{cellHints[cell].label}</span> : null}
            </div>
          );
        })}
      </div>
      {state.boxes.map((box, index) =>
        visible(box) ? (
          <div
            className={`actor box ${board.goals[box] ? "on-goal" : ""}`}
            style={position(box)}
            key={`box-${index}`}
          >
            <i>{board.goals[box] ? "✓" : ""}</i>
          </div>
        ) : null,
      )}
      {!hidePlayer && visible(state.player) ? (
        <div className="actor player" style={position(state.player)}>
          <i />
        </div>
      ) : null}
      {markers.filter((marker) => visible(marker.cell)).map((marker, index) => (
        <span
          className={`board-marker ${marker.kind}`}
          style={position(marker.cell)}
          key={marker.id ?? `${marker.kind}-${marker.cell}-${index}`}
          aria-hidden="true"
        >
          <i>{marker.label}</i>
        </span>
      ))}
      {cameraMode ? (
        <span className="camera-label" aria-hidden="true">
          Камера {w} × {h} · {startX + 1}:{startY + 1}
        </span>
      ) : null}
    </div>
  );
}
