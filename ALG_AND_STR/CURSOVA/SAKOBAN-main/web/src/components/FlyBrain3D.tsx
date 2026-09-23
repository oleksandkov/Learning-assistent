"use client";
import { useEffect, useMemo, useRef, useState } from "react";
import type { FlyBrainSpikeFrame } from "@/lib/types";
import { NEURONS } from "@/lib/flybrain";

const colors = ["#56c6e7", "#bc9cfa", "#f4c26b"];
const names = ["Sensory", "Central", "Motor"];
// A schematic bilateral volume, not imported anatomical/connectome coordinates.
const neurons = Array.from({ length: NEURONS }, (_, id) => {
  const group = id < 64 ? 0 : id < 1864 ? 1 : 2;
  const index = group === 0 ? id : group === 1 ? id - 64 : id - 1864;
  const side = index % 2 ? 1 : -1;
  const angle = index * 2.399963;
  const y = 1 - 2 * ((index * 37 % 997) / 997);
  const radius = Math.sqrt(1 - y * y);
  const depth = .35 + .65 * ((index * 71 % 101) / 101) ** (1 / 3);
  const center = group === 0 ? side * 1.55 : group === 1 ? side * .58 : (Math.floor(index / 40) - 1.5) * .35;
  return { id, group, x: center + Math.cos(angle) * radius * depth * (group === 1 ? .82 : .25),
    y: (group === 2 ? -1.08 : .05) + y * depth * (group === 1 ? .84 : .38),
    z: Math.sin(angle) * radius * depth * (group === 1 ? .62 : .3) };
});

export default function FlyBrain3D({ frame }: { frame?: FlyBrainSpikeFrame }) {
  const canvas = useRef<HTMLCanvasElement>(null);
  const [view, setView] = useState({ yaw: -.3, pitch: .2, zoom: 1 });
  const [selected, setSelected] = useState<number | null>(null);
  const [groups, setGroups] = useState([true, true, true]);
  const drag = useRef<{ x: number; y: number; moved: boolean } | null>(null);
  const points = useRef<{ id: number; x: number; y: number }[]>([]);
  const active = useMemo(() => new Set(frame?.active ?? (frame?.samples ?? []).map(sample =>
    sample < 40 ? Math.floor(sample * 64 / 40) : sample < 80 ? 64 + Math.floor((sample - 40) * 1800 / 40) : 1864 + (sample - 80) * 4)), [frame]);

  useEffect(() => {
    const element = canvas.current, ctx = element?.getContext("2d");
    if (!element || !ctx) return;
    const draw = () => {
      const width = element.clientWidth, height = element.clientHeight;
      if (!width || !height) return;
      const dpr = Math.min(window.devicePixelRatio || 1, 2);
      element.width = width * dpr; element.height = height * dpr; ctx.scale(dpr, dpr);
      ctx.fillStyle = "#101e35"; ctx.fillRect(0, 0, width, height);
      const project = (p: { x: number; y: number; z: number }) => {
        const x = p.x * Math.cos(view.yaw) + p.z * Math.sin(view.yaw);
        const z = -p.x * Math.sin(view.yaw) + p.z * Math.cos(view.yaw);
        const y = p.y * Math.cos(view.pitch) - z * Math.sin(view.pitch);
        const depth = p.y * Math.sin(view.pitch) + z * Math.cos(view.pitch);
        const scale = Math.min(width / 4.7, height / 3.3) * view.zoom * 5 / (5 - depth);
        return { x: width / 2 + x * scale, y: height / 2 - y * scale, z: depth, scale };
      };
      // Wireframe outlines make rotation and depth legible even during a silent window.
      ctx.strokeStyle = "#879cbd30"; ctx.lineWidth = 1;
      for (const side of [-1, 1]) for (let ring = 0; ring < 8; ring++) {
        ctx.beginPath();
        for (let i = 0; i <= 64; i++) {
          const a = i / 64 * Math.PI * 2, b = ring / 8 * Math.PI;
          const p = project({ x: side * .58 + .86 * Math.cos(a) * Math.cos(b), y: .05 + .88 * Math.sin(a), z: .65 * Math.cos(a) * Math.sin(b) });
          if (i) ctx.lineTo(p.x, p.y); else ctx.moveTo(p.x, p.y);
        }
        ctx.stroke();
      }
      const projected = neurons.filter(n => groups[n.group]).map(n => ({ ...n, ...project(n) })).sort((a, b) => a.z - b.z);
      points.current = projected;
      for (const n of projected) {
        const firing = active.has(n.id), radius = (firing ? 2.4 : 1.1) * Math.max(.7, n.scale / 150);
        ctx.fillStyle = colors[n.group];
        if (firing) { ctx.globalAlpha = .13; ctx.beginPath(); ctx.arc(n.x, n.y, radius * 3.5, 0, Math.PI * 2); ctx.fill(); }
        ctx.globalAlpha = firing ? 1 : .2 + (n.z + 1) * .16;
        ctx.beginPath(); ctx.arc(n.x, n.y, radius, 0, Math.PI * 2); ctx.fill();
        if (n.id === selected) {
          ctx.globalAlpha = 1; ctx.strokeStyle = "white"; ctx.beginPath(); ctx.arc(n.x, n.y, radius + 7, 0, Math.PI * 2); ctx.stroke();
        }
      }
      ctx.globalAlpha = 1;
    };
    draw(); const resize = new ResizeObserver(draw); resize.observe(element);
    return () => resize.disconnect();
  }, [view, active, groups, selected]);

  return <section className="flybrain-3d" aria-label="Об’ємна модель нейронної мережі">
    <div className="flybrain-3d-legend">{names.map((name, i) => <label key={name} style={{ color: colors[i] }}>
      <input type="checkbox" checked={groups[i]} onChange={e => setGroups(g => g.map((v, j) => j === i ? e.target.checked : v))} />{name}
    </label>)}<span>{active.size} активних / {NEURONS}</span></div>
    <canvas ref={canvas} tabIndex={0} role="img" aria-label={`3D-мозок: крок ${frame?.t ?? 0}, ${active.size} нейронів спрацювали. Перетягніть або використайте стрілки для обертання.`}
      onKeyDown={e => {
        if (!["ArrowLeft", "ArrowRight", "ArrowUp", "ArrowDown", "+", "-"].includes(e.key)) return;
        e.preventDefault(); e.stopPropagation();
        setView(v => ({ ...v, yaw: v.yaw + (e.key === "ArrowLeft" ? -.15 : e.key === "ArrowRight" ? .15 : 0),
          pitch: Math.max(-1.3, Math.min(1.3, v.pitch + (e.key === "ArrowUp" ? .15 : e.key === "ArrowDown" ? -.15 : 0))),
          zoom: Math.max(.6, Math.min(2, v.zoom + (e.key === "+" ? .1 : e.key === "-" ? -.1 : 0))) }));
      }}
      onPointerDown={e => { if (e.button !== 0) return; drag.current = { x: e.clientX, y: e.clientY, moved: false }; e.currentTarget.setPointerCapture(e.pointerId); }}
      onPointerMove={e => {
        const d = drag.current; if (!d) return;
        const dx = e.clientX - d.x, dy = e.clientY - d.y;
        if (Math.abs(dx) + Math.abs(dy) < 2) return;
        drag.current = { x: e.clientX, y: e.clientY, moved: true };
        setView(v => ({ ...v, yaw: v.yaw + dx * .008, pitch: Math.max(-1.3, Math.min(1.3, v.pitch + dy * .008)) }));
      }}
      onPointerUp={e => {
        if (drag.current && !drag.current.moved) {
          const bounds = e.currentTarget.getBoundingClientRect(), x = e.clientX - bounds.left, y = e.clientY - bounds.top;
          const closest = [...points.current].reverse().find(p => Math.hypot(p.x - x, p.y - y) < 9);
          setSelected(closest?.id ?? null);
        }
        drag.current = null; if (e.currentTarget.hasPointerCapture(e.pointerId)) e.currentTarget.releasePointerCapture(e.pointerId);
      }} onPointerCancel={() => { drag.current = null; }} />
    <div className="flybrain-3d-tools">
      <label>Масштаб<input aria-label="Масштаб 3D" type="range" min={.6} max={2} step={.05} value={view.zoom} onChange={e => setView(v => ({ ...v, zoom: Number(e.target.value) }))} /></label>
      <button type="button" className="button secondary" onClick={() => setView({ yaw: -.3, pitch: .2, zoom: 1 })}>Скинути ракурс</button>
      <span>{selected === null ? "Перетягніть для обертання · натисніть нейрон" : `Нейрон #${selected} · ${names[neurons[selected].group]} · ${active.has(selected) ? "спрацював" : "без спайку"}`}</span>
    </div>
    <p>Умовна 3D-геометрія мережі, не анатомічний коннектом. Світяться лише нейрони, що справді спрацювали у вибраному вікні.{frame && !frame.active ? " Старий запис: доступні лише 120 семплованих нейронів." : ""}</p>
  </section>;
}
