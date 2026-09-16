"use client";

import { useEffect, useMemo, useRef } from "react";

export default function GeneratedPreview({ xsb }: { xsb: string }) {
  const canvas = useRef<HTMLCanvasElement>(null);
  const rows = useMemo(() => xsb.trimEnd().split(/\r?\n/), [xsb]);
  const width = Math.max(...rows.map((row) => row.length));
  const height = rows.length;

  useEffect(() => {
    const element = canvas.current;
    if (!element) return;
    const draw = () => {
      const context = element.getContext("2d");
      if (!context) return;
      const styles = getComputedStyle(document.documentElement);
      const color = (name: string) => styles.getPropertyValue(name).trim();
      const cell = 12;
      element.width = width * cell;
      element.height = height * cell;
      context.clearRect(0, 0, element.width, element.height);
      rows.forEach((row, y) => {
        Array.from({ length: width }, (_, x) => row[x] ?? " ").forEach(
          (symbol, x) => {
            const px = x * cell;
            const py = y * cell;
            context.fillStyle = symbol === "#" ? color("--wall") : color("--floor");
            context.fillRect(px, py, cell, cell);
            if (".*+".includes(symbol)) {
              context.fillStyle = color("--accent");
              context.beginPath();
              context.arc(px + cell / 2, py + cell / 2, cell * 0.16, 0, Math.PI * 2);
              context.fill();
            }
            if ("$*".includes(symbol)) {
              context.fillStyle = color("--box");
              context.fillRect(px + 2, py + 2, cell - 4, cell - 4);
            }
            if ("@+".includes(symbol)) {
              context.fillStyle = color("--player");
              context.beginPath();
              context.arc(px + cell / 2, py + cell / 2, cell * 0.32, 0, Math.PI * 2);
              context.fill();
            }
          },
        );
      });
    };
    draw();
    const media = window.matchMedia("(prefers-color-scheme: dark)");
    media.addEventListener("change", draw);
    return () => media.removeEventListener("change", draw);
  }, [height, rows, width]);

  return (
    <canvas
      ref={canvas}
      className="generated-preview-canvas"
      style={{
        width: `${Math.max(18, Math.round((width / Math.max(width, height)) * 420))}px`,
        height: `${Math.max(18, Math.round((height / Math.max(width, height)) * 420))}px`,
      }}
      aria-label={`Попередній перегляд рівня ${width} на ${height}`}
      role="img"
    />
  );
}
