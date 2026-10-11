"""UML 2.5 class diagram generator, academic monochrome style.

Locked style: sans-serif (Helvetica/Arial), 1px black strokes, white fills,
three-compartment class boxes, italic names for interfaces and abstract
classes, guillemet stereotypes, dashed realization, hollow-triangle
generalization, open-arrow navigability.

Boxes are placed explicitly (x, y) - class layout is a hand job, not a
solver. The generator only does the tedious part: box sizing, compartment
rules, and correctly shaped relationship ends.

    d = Diagram()
    a = d.klass("Main", attrs=["-rows: List<ConnectionRow>"],
                methods=["+calculateRoute()"], x=320, y=40)
    b = d.klass("PathResult", attrs=["-path: List<String>"], x=560, y=520)
    d.depends(a, b, "використовує", label=True)
    open("out.svg", "w", encoding="utf-8").write(d.render())
"""

from __future__ import annotations

import html
from dataclasses import dataclass, field

FONT = "Helvetica, Arial, sans-serif"


def esc(s: str) -> str:
    return html.escape(str(s), quote=False)


def _w(s: str, fs: float) -> float:
    total = 0.0
    for ch in str(s):
        if ch == " ":
            total += 0.28
        elif ch in "iltfjI.,:;'|!()[]-":
            total += 0.30
        elif ch in "mwMW@":
            total += 0.85
        elif ch.isupper():
            total += 0.70
        elif ch.isdigit():
            total += 0.55
        else:
            total += 0.53
    return total * fs


@dataclass
class Klass:
    id: str
    name: str
    attrs: list
    methods: list
    stereotype: str | None
    abstract: bool
    x: float
    y: float
    w: float
    h: float
    fs: float = 13.0
    lh: float = 17.0
    pad: float = 12.0

    @property
    def cx(self) -> float:
        return self.x + self.w / 2

    @property
    def cy(self) -> float:
        return self.y + self.h / 2

    @property
    def top(self) -> float:
        return self.y

    @property
    def bottom(self) -> float:
        return self.y + self.h

    @property
    def left(self) -> float:
        return self.x

    @property
    def right(self) -> float:
        return self.x + self.w

    def header_bottom(self) -> float:
        rows = 2 if self.stereotype else 1
        return self.y + self.pad * 0.7 + rows * (self.lh + 3)

    def attrs_bottom(self) -> float:
        if not self.attrs:
            return self.header_bottom()
        return self.header_bottom() + 6 + len(self.attrs) * self.lh + 6


class Diagram:
    def __init__(self, fs: float = 13.0, lh: float = 17.0, margin: float = 30):
        self.fs = fs
        self.lh = lh
        self.margin = margin
        self.boxes: list[Klass] = []
        self.by_id: dict[str, Klass] = {}
        self.edges: list[dict] = []
        self.labels: list[tuple] = []

    def klass(
        self,
        name: str,
        attrs: list | None = None,
        methods: list | None = None,
        stereotype: str | None = None,
        abstract: bool = False,
        x: float = 0,
        y: float = 0,
        id: str | None = None,
        min_w: float = 130,
    ) -> Klass:
        attrs = list(attrs or [])
        methods = list(methods or [])
        rows = ([name] + ([stereotype] if stereotype else []) + attrs + methods)
        w = max(_w(r, self.fs) for r in rows) + self.lh * 1.6
        w = max(w, min_w)
        head = self.lh + 3 + (self.lh + 3 if stereotype else 0) + 10
        if attrs:
            h = head + 6 + len(attrs) * self.lh + 6
        else:
            h = head
        if methods:
            h += len(methods) * self.lh + 8
        k = Klass(
            id or name,
            name,
            attrs,
            methods,
            stereotype,
            abstract,
            x,
            y,
            w,
            h,
            self.fs,
            self.lh,
        )
        self.boxes.append(k)
        self.by_id[k.id] = k
        return k

    def _ref(self, r) -> Klass:
        return self.by_id[r] if isinstance(r, str) else r

    # --------------------------------------------------------- relationships

    def _edge(
        self,
        a,
        b,
        kind: str,
        label: str | None = None,
        label_at: tuple | None = None,
        source_anchor: str = "auto",
        target_anchor: str = "auto",
        routing: str = "ortho",
        multiplicity: tuple[str, str] | None = None,
        bends: list | None = None,
    ) -> None:
        self.edges.append(
            {
                "a": self._ref(a),
                "b": self._ref(b),
                "kind": kind,
                "label": label,
                "label_at": label_at,
                "sa": source_anchor,
                "ta": target_anchor,
                "routing": routing,
                "mult": multiplicity,
                "bends": bends or [],
            }
        )

    def inherits(self, sub, sup, label=None, **kw) -> None:
        self._edge(sub, sup, "generalization", label, **kw)

    def implements(self, cls, iface, label=None, **kw) -> None:
        self._edge(cls, iface, "realization", label, **kw)

    def depends(self, a, b, label=None, **kw) -> None:
        self._edge(a, b, "dependency", label, **kw)

    def uses(self, a, b, label=None, **kw) -> None:
        self._edge(a, b, "association", label, **kw)

    def aggregates(self, owner, part, label=None, filled=False, **kw) -> None:
        self._edge(owner, part, "composition" if filled else "aggregation", label, **kw)

    def note(self, x: float, y: float, text: str, anchor: str = "middle") -> None:
        self.labels.append((x, y, text, anchor))

    # ------------------------------------------------------------- anchors

    def _anchor(self, k: Klass, mode: str, toward: tuple[float, float]):
        tx, ty = toward
        if mode != "auto":
            return {
                "top": (k.cx, k.top),
                "bottom": (k.cx, k.bottom),
                "left": (k.left, k.cy),
                "right": (k.right, k.cy),
            }[mode]
        cands = {
            "top": (k.cx, k.top),
            "bottom": (k.cx, k.bottom),
            "left": (k.left, k.cy),
            "right": (k.right, k.cy),
        }
        return max(cands.items(), key=lambda kv: -((kv[1][0] - tx) ** 2 + (kv[1][1] - ty) ** 2))[1]

    def _route(self, e: dict) -> list[tuple[float, float]]:
        a, b = e["a"], e["b"]
        p1 = self._anchor(a, e["sa"], (b.cx, b.cy))
        p2 = self._anchor(b, e["ta"], p1)
        if e["bends"]:
            return [p1] + [tuple(x) for x in e["bends"]] + [p2]
        if e["routing"] == "straight":
            return [p1, p2]
        if abs(p1[0] - p2[0]) < 1 or abs(p1[1] - p2[1]) < 1:
            return [p1, p2]
        if abs(p1[0] - p2[0]) > abs(p1[1] - p2[1]):
            mx = (p1[0] + p2[0]) / 2
            return [p1, (mx, p1[1]), (mx, p2[1]), p2]
        my = (p1[1] + p2[1]) / 2
        return [p1, (p1[0], my), (p2[0], my), p2]

    # -------------------------------------------------------------- drawing

    def _end(self, kind: str, at: tuple[float, float], toward: tuple[float, float], s: float = 9.0):
        dx, dy = toward[0] - at[0], toward[1] - at[1]
        n = (dx * dx + dy * dy) ** 0.5 or 1.0
        ux, uy = dx / n, dy / n
        px, py = -uy, ux
        if kind in ("generalization", "realization"):
            tip = (at[0] - ux * s, at[1] - uy * s)
            return (
                f'<polygon points="{at[0]:.1f},{at[1]:.1f} {tip[0] + px * s * 0.45:.1f},'
                f"{tip[1] + py * s * 0.45:.1f} {tip[0] - px * s * 0.45:.1f},"
                f'{tip[1] - py * s * 0.45:.1f}" class="b"/>'
            )
        if kind in ("aggregation", "composition"):
            bx = at[0] - ux * s
            by = at[1] - uy * s
            m1 = (bx - ux * s * 0.5 + px * s * 0.35, by - uy * s * 0.5 + py * s * 0.35)
            m2 = (bx - ux * s * 0.5 - px * s * 0.35, by - uy * s * 0.5 - py * s * 0.35)
            cls = "b" if kind == "aggregation" else "bf"
            return (
                f'<polygon points="{at[0]:.1f},{at[1]:.1f} {m1[0]:.1f},{m1[1]:.1f} '
                f'{bx:.1f},{by:.1f} {m2[0]:.1f},{m2[1]:.1f}" class="{cls}"/>'
            )
        # open arrow (dependency / navigability)
        bx, by = at[0] - ux * s, at[1] - uy * s
        return (
            f'<polyline points="{at[0]:.1f},{at[1]:.1f} {bx + px * s * 0.38:.1f},'
            f'{by + py * s * 0.38:.1f} {bx - px * s * 0.38:.1f},{by - py * s * 0.38:.1f}" class="l"/>'
        )

    def _box(self, k: Klass) -> str:
        parts = [
            f'<rect x="{k.x:.1f}" y="{k.y:.1f}" width="{k.w:.1f}" '
            f'height="{k.h:.1f}" class="b"/>'
        ]
        hb = k.header_bottom()
        parts.append(
            f'<line x1="{k.left:.1f}" y1="{hb:.1f}" x2="{k.right:.1f}" '
            f'y2="{hb:.1f}" class="l"/>'
        )
        if k.attrs:
            ab = k.attrs_bottom()
            parts.append(
                f'<line x1="{k.left:.1f}" y1="{ab:.1f}" x2="{k.right:.1f}" '
                f'y2="{ab:.1f}" class="l"/>'
            )
        rows: list[tuple[float, str, str]] = []
        y = k.y + self.lh * 0.9
        if k.stereotype:
            rows.append((y, f"«{k.stereotype}»", ' class="ts"'))
            y += self.lh + 3
        rows.append((y, k.name, ' class="ti"' if (k.abstract or k.stereotype) else ""))
        y = hb + self.lh * 0.9
        for a in k.attrs:
            rows.append((y, a, ""))
            y += self.lh
        if k.attrs:
            y = k.attrs_bottom() + self.lh * 0.9
        for m in k.methods:
            rows.append((y, m, ""))
            y += self.lh
        x0 = k.x + self.lh * 0.55
        for ry, txt, cls in rows:
            if 'class="ts"' in cls or cls.startswith('class="ti'):
                parts.append(
                    f'<text x="{k.cx:.1f}" y="{ry:.1f}" text-anchor="middle" '
                    f'dominant-baseline="central"{cls}>{esc(txt)}</text>'
                )
            else:
                parts.append(
                    f'<text x="{x0:.1f}" y="{ry:.1f}" dominant-baseline="central"'
                    f'>{esc(txt)}</text>'
                )
        return "".join(parts)

    def render(self) -> str:
        edges: list[str] = []
        ends: list[str] = []
        labels = list(self.labels)
        for e in self.edges:
            pts = self._route(e)
            d = " ".join(
                ("M" if i == 0 else "L") + f" {x:.1f} {y:.1f}"
                for i, (x, y) in enumerate(pts)
            )
            dash = ' stroke-dasharray="5,4"' if e["kind"] in ("dependency", "realization") else ""
            edges.append(f'<path d="{d}" class="l"{dash}/>')
            ends.append(self._end(e["kind"], pts[-1], pts[-2]))
            if e["kind"] in ("aggregation", "composition"):
                ends.append(self._end(e["kind"], pts[0], pts[1]))
            if e["label"]:
                if e["label_at"]:
                    lx, ly = e["label_at"]
                else:
                    mid = pts[len(pts) // 2]
                    lx, ly = mid[0], mid[1] - 8
                labels.append((lx, ly, e["label"], "middle"))
            if e["mult"]:
                ma, mb = e["mult"]
                if ma:
                    labels.append((pts[0][0], pts[0][1] - 14, ma, "middle"))
                if mb:
                    labels.append((pts[-1][0], pts[-1][1] - 14, mb, "middle"))

        boxes = [self._box(k) for k in self.boxes]
        texts = [
            f'<text x="{x:.1f}" y="{y:.1f}" text-anchor="{anc}" '
            f'dominant-baseline="central" class="lbl">{esc(t)}</text>'
            for x, y, t, anc in labels
        ]

        xs: list[float] = []
        ys: list[float] = []
        for k in self.boxes:
            xs += [k.left, k.right]
            ys += [k.top, k.bottom]
        for e in self.edges:
            for x, y in self._route(e):
                xs.append(x)
                ys.append(y)
        for x, y, _, _ in self.labels:
            xs.append(x)
            ys.append(y)
        m = self.margin
        x0, x1 = min(xs) - m, max(xs) + m
        y0, y1 = min(ys) - m, max(ys) + m

        return f"""<svg xmlns="http://www.w3.org/2000/svg" viewBox="{x0:.1f} {y0:.1f} {x1 - x0:.1f} {y1 - y0:.1f}" style="background:transparent;font-family:{FONT};">
<defs>
<style>
.b{{stroke:#000;stroke-width:1;fill:#fff}}
.bf{{stroke:#000;stroke-width:1;fill:#000}}
.l{{stroke:#000;stroke-width:1;fill:none}}
text{{font-size:{self.fs}px;fill:#000}}
.ti{{font-style:italic}}
.ts{{font-style:italic;font-size:{self.fs - 1}px}}
.lbl{{font-size:{self.fs - 1}px}}
</style>
</defs>
<g>{''.join(edges)}</g>
<g>{''.join(boxes)}</g>
<g>{''.join(ends)}</g>
<g>{''.join(texts)}</g>
</svg>
"""


if __name__ == "__main__":
    d = Diagram()
    a = d.klass("Main", attrs=["-rows: List<Row>"], methods=["+run()"], x=300, y=40)
    b = d.klass("PathResult", attrs=["-cost: double"], x=300, y=240)
    d.inherits(b, a)
    print(d.render())