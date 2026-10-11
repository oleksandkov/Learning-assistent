"""ГОСТ 19.701-90 (ISO 5807) flowchart generator.

Style is locked to the academic reference set:
Times New Roman serif, 1.5px black strokes, white fills, transparent
background, uniform-width blocks, wide flat diamonds, orthogonal wiring
with 90-degree corners only, arrowheads on horizontal enters.

Two passes: add nodes (positions from `below=` / `row=` / explicit y),
then `layout()` derives column widths and x axes, then render.

    c = Chart()
    s = c.terminator("Початок")
    i = c.io("Введення: a, b", below=s)
    p = c.process("max = a", below=i)
    c.end("Кінець", below=p)
    open("out.svg", "w", encoding="utf-8").write(c.render())
"""

from __future__ import annotations

import html
from dataclasses import dataclass

FONT = "'Times New Roman', Times, serif"

_NARROW = set("iljtfrI.,:;'!|()[]-·")
_WIDE = set("mwMW@%ЖШЩЮЯ")


def text_width(s: str, fs: float) -> float:
    """Approximate Times New Roman advance width, accurate to ~5%."""
    total = 0.0
    for ch in s:
        if ch == " ":
            total += 0.25
        elif ch in _NARROW:
            total += 0.30
        elif ch in _WIDE:
            total += 0.82
        elif ch.isupper():
            total += 0.68
        elif ch.isdigit():
            total += 0.50
        else:
            total += 0.52
    return total * fs


def esc(s: str) -> str:
    return html.escape(s, quote=False)


@dataclass
class Node:
    id: str
    kind: str
    text: str
    col: int
    y: float
    w: float
    h: float
    lines: list
    x: float = 0.0

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


class Chart:
    def __init__(
        self,
        fs: float = 16,
        lh: float = 24,
        padx: float = 22,
        pady: float = 13,
        gap: float = 38,
        col_gap: float = 56,
        margin: float = 26,
        slant: float = 30,
        sub_inset: float = 24,
        wrap_w: float | None = 470,
    ):
        self.fs = fs
        self.lh = lh
        self.padx = padx
        self.pady = pady
        self.gap = gap
        self.col_gap = col_gap
        self.margin = margin
        self.slant = slant
        self.sub_inset = sub_inset
        self.wrap_w = wrap_w

        self.nodes: list[Node] = []
        self.wires: list[tuple] = []
        self.labels: list[tuple] = []
        self.by_id: dict[str, Node] = {}

    # ---------------------------------------------------------------- text

    def _wrap(self, text: str, maxw: float) -> list[str]:
        if not maxw or maxw <= 0:
            return text.split("\n")
        out: list[str] = []
        for para in text.split("\n"):
            words = para.split()
            if not words:
                out.append("")
                continue
            cur = words[0]
            for word in words[1:]:
                cand = cur + " " + word
                if text_width(cand, self.fs) > maxw:
                    out.append(cur)
                    cur = word
                else:
                    cur = cand
            out.append(cur)
        return out

    def _measure(self, kind: str, text: str) -> tuple[list[str], float, float]:
        cap = self.wrap_w or 0
        if kind == "terminator":
            lines = self._wrap(text, cap)
            w = max(text_width(l, self.fs) for l in lines) + self.fs * 2.4
            h = self.lh * 1.15
        elif kind == "decision":
            lines = self._wrap(text, cap * 0.55)
            w = cap + self.fs
            h = self.lh * len(lines) + self.lh + self.fs
        elif kind == "connector":
            lines = [text]
            w = h = max(
                self.fs * 2.2, max(text_width(l, self.fs) for l in lines) + self.fs
            )
        elif kind == "io":
            lines = self._wrap(text, cap)
            w = max(text_width(l, self.fs) for l in lines) + self.padx * 2 + self.slant
            h = self.lh * len(lines) + self.pady * 2
        elif kind == "preparation":
            lines = self._wrap(text, cap)
            w = max(text_width(l, self.fs) for l in lines) + self.padx * 2 + self.slant
            h = self.lh * len(lines) + self.pady * 2
        elif kind == "subroutine":
            cap = cap * 0.84
            lines = self._wrap(text, cap)
            tw = max(text_width(l, self.fs) for l in lines)
            w = max(cap, tw + (self.padx + self.sub_inset) * 2)
            h = self.lh * len(lines) + self.pady * 2
        else:  # process
            lines = self._wrap(text, cap)
            w = max(text_width(l, self.fs) for l in lines) + self.padx * 2
            h = self.lh * len(lines) + self.pady * 2
        return lines, w, h

    # ---------------------------------------------------------------- nodes

    def add(
        self,
        kind: str,
        text: str,
        id: str | None = None,
        below: str | Node | None = None,
        row: str | Node | None = None,
        dy: float = 0,
        col: int = 0,
        y: float | None = None,
    ) -> Node:
        if id is None:
            id = f"n{len(self.nodes) + 1}"
        lines, w, h = self._measure(kind, text)
        if y is None:
            if below is not None:
                ref = self._ref(below)
                y = ref.bottom + self.gap + dy
            elif row is not None:
                ref = self._ref(row)
                y = ref.y + (ref.h - h) / 2 + dy
            else:
                y = self.margin
        node = Node(id, kind, text, col, y, w, h, lines)
        self.nodes.append(node)
        self.by_id[id] = node
        return node

    def _ref(self, ref: str | Node) -> Node:
        self._place()
        return self.by_id[ref] if isinstance(ref, str) else ref

    def _place(self) -> None:
        """Assign x from column axes. Idempotent; safe to call any time."""
        axes = self._axes()
        for n in self.nodes:
            n.x = axes[n.col] - n.w / 2

    def terminator(self, text: str = "Початок", **kw) -> Node:
        return self.add("terminator", text, **kw)

    def io(self, text: str, **kw) -> Node:
        return self.add("io", text, **kw)

    def process(self, text: str, **kw) -> Node:
        return self.add("process", text, **kw)

    def subroutine(self, text: str, **kw) -> Node:
        return self.add("subroutine", text, **kw)

    def decision(self, text: str, **kw) -> Node:
        return self.add("decision", text, **kw)

    def preparation(self, text: str, **kw) -> Node:
        return self.add("preparation", text, **kw)

    def connector(self, text: str = "A", **kw) -> Node:
        return self.add("connector", text, **kw)

    def start(self, **kw) -> Node:
        return self.terminator("Початок", **kw)

    def end(self, **kw) -> Node:
        return self.terminator("Кінець", **kw)

    # --------------------------------------------------------------- wiring

    def wire(self, points, arrow: bool = True) -> None:
        """Free-form polyline. `points` = [(x, y), ...] or [node, side, dx]."""
        pts = []
        for p in points:
            if isinstance(p, Node):
                pts.append((p.cx, p.cy))
            elif isinstance(p, (list, tuple)) and len(p) == 3:
                n, side, dx = p
                pts.append(self.anchor(n, side, dx))
            else:
                pts.append((p[0], p[1]))
        self.wires.append((pts, arrow))

    def anchor(self, node: str | Node, side: str, off: float = 0) -> tuple[float, float]:
        n = self._ref(node)
        return {
            "top": (n.cx, n.top),
            "bottom": (n.cx, n.bottom),
            "left": (n.left, n.cy),
            "right": (n.right, n.cy),
            "left_top": (n.left, n.cy - n.h / 4),
            "right_top": (n.right, n.cy - n.h / 4),
            "left_bot": (n.left, n.cy + n.h / 4),
            "right_bot": (n.right, n.cy + n.h / 4),
            "top_r": (n.cx + n.w / 4, n.top),
            "top_l": (n.cx - n.w / 4, n.top),
        }[side]

    def drop(self, a: str | Node, b: str | Node, arrow: bool = False) -> None:
        """Trunk segment. Straight when the blocks share a column, otherwise
        an orthogonal step. No arrowhead — that is the reference style."""
        s = self._ref(a)
        d = self._ref(b)
        p1 = self.anchor(s, "bottom")
        p2 = self.anchor(d, "top")
        if abs(p1[0] - p2[0]) < 1:
            self.wires.append(([p1, p2], arrow))
            return
        my = (p1[1] + p2[1]) / 2
        self.wires.append(([p1, (p1[0], my), (p2[0], my), p2], arrow))

    def branch(
        self,
        src: str | Node,
        dst: str | Node,
        side: str = "right",
        label: str | None = None,
        out: float = 46,
        at: float | None = None,
        arrow: bool = True,
    ) -> None:
        """Exit a decision sideways, step down/up, enter `dst` from the top.

        `out` is the reach from the exit point; `at` sets an absolute x,
        which is what you want for lanes that must clear the block column.
        """
        s = self._ref(src)
        d = self._ref(dst)
        start = self.anchor(s, side)
        mid_x = at if at is not None else (start[0] + out if side == "right" else start[0] - out)
        # stop the arrow a few px short of the border so the head sits on the
        # block, not on top of it
        arr = 6 if arrow else 0
        pts = [
            start,
            (mid_x, start[1]),
            (mid_x, d.top - arr),
            (d.cx, d.top - arr),
        ]
        self.wires.append((pts, arrow))
        self._branch_label(start, side, label)

    def merge(
        self,
        src: str | Node,
        dst: str | Node,
        side: str = "left",
        lane: float = 46,
        arrow: bool = True,
    ) -> None:
        """Leave `src` sideways, drop, enter `dst` from the top."""
        s = self._ref(src)
        d = self._ref(dst)
        start = self.anchor(s, side)
        lane_x = d.cx - lane if side == "left" else d.cx + lane
        self.wires.append(
            (
                [
                    start,
                    (lane_x, start[1]),
                    (lane_x, d.top),
                    (d.cx, d.top),
                ],
                arrow,
            )
        )

    def loop(
        self,
        src: str | Node,
        dst: str | Node,
        side: str = "left",
        lane: float = 46,
        label: str | None = None,
        at: float | None = None,
    ) -> None:
        """Back edge: down out of `src`, out to the lane, up, into `dst`.

        The return enters the target's side vertex (the horizontal midline of
        a rectangle, the centre point of a diamond) so the arrow lands on the
        outline instead of somewhere inside the shape.
        """
        s = self._ref(src)
        d = self._ref(dst)
        lane_x = at if at is not None else (d.cx - lane if side == "left" else d.cx + lane)
        start = (s.cx, s.bottom)
        pts = [
            start,
            (s.cx, s.bottom + self.gap * 0.45),
            (lane_x, s.bottom + self.gap * 0.45),
            (lane_x, d.cy),
            ((d.left if side == "left" else d.right), d.cy),
        ]
        self.wires.append((pts, True))
        if label:
            self.labels.append((lane_x, d.cy - 12, label, "middle"))

    def rail(self, x: float, y1: float, y2: float) -> None:
        """Shared vertical bus. Pair with `join` for each incoming branch."""
        self.wires.append(([(x, y1), (x, y2)], False))

    def join(self, node: str | Node, side: str, rail_x: float, label=None) -> None:
        """Horizontal arrow into the bus at the node's vertical centre."""
        p = self.anchor(node, side)
        pts = [p, (rail_x, p[1])]
        self.wires.append((pts, True))
        self._branch_label(p, side, label)

    def feed(self, rail_x: float, y: float, dst: str | Node) -> None:
        """Horizontal arrow from the bus into the trunk at height `y`."""
        d = self._ref(dst)
        self.wires.append(([(rail_x, y), (d.cx, y)], True))

    def label(self, x: float, y: float, text: str, anchor: str = "start") -> None:
        self.labels.append((x, y, text, anchor))

    def tag(self, node: str | Node, text: str, side: str = "bottom", dx=0, dy=0):
        """Place a Так/Ні label relative to a node's anchor. Resolved at render.

        Sideways labels (left/right) sit clear of the wire; a top/bottom tag
        is nudged off the trunk so the line never strikes through it.
        """
        self._place()
        n = self._ref(node)
        ax, ay = self.anchor(node, side)
        if side in ("top", "bottom") and not dx:
            dx, anchor = 10, "start"
        else:
            anchor = "start"
        self.labels.append((ax + dx, ay + dy, text, anchor))

    def _branch_label(self, start, side, text) -> None:
        if not text:
            return
        if side == "bottom":
            self.labels.append((start[0] + 8, start[1] + 18, text, "start"))
        elif side == "right":
            self.labels.append((start[0] + 8, start[1] - 10, text, "start"))
        else:
            self.labels.append((start[0] - 8, start[1] - 10, text, "end"))

    # --------------------------------------------------------------- layout

    def _col_width(self, col: int) -> float:
        widths = [n.w for n in self.nodes if n.col == col]
        return max(widths) if widths else 0.0

    def _axis(self, col: int) -> float:
        axes = self._axes()
        return axes[col]

    def _axes(self) -> dict[int, float]:
        cols = sorted({n.col for n in self.nodes})
        axes: dict[int, float] = {}
        x = self.margin
        for col in cols:
            w = self._col_width(col)
            axes[col] = x + w / 2
            x += w + self.col_gap
        return axes

    def rail_x(self, col: int = 0, off: float = 34) -> float:
        return self._axis(col) + self._col_width(col) / 2 + off

    def lane(self, side: str = "left", off: float = 34) -> float:
        """x just outside the widest shape, so lanes never cross blocks."""
        self._place()
        left = min(n.left for n in self.nodes)
        right = max(n.right for n in self.nodes)
        return left - off if side == "left" else right + off

    # --------------------------------------------------------------- render

    def _shape(self, n: Node) -> list[str]:
        k = n.kind
        out = []
        if k == "terminator":
            r = n.h / 2
            out.append(
                f'<rect x="{n.x:.1f}" y="{n.y:.1f}" width="{n.w:.1f}" '
                f'height="{n.h:.1f}" rx="{r:.1f}" ry="{r:.1f}" class="b"/>'
            )
        elif k == "decision":
            pts = (
                f"{n.cx:.1f},{n.top:.1f} {n.right:.1f},{n.cy:.1f} "
                f"{n.cx:.1f},{n.bottom:.1f} {n.left:.1f},{n.cy:.1f}"
            )
            out.append(f'<polygon points="{pts}" class="b"/>')
        elif k == "io":
            s = self.slant
            pts = (
                f"{n.x + s:.1f},{n.top:.1f} {n.right:.1f},{n.top:.1f} "
                f"{n.right - s:.1f},{n.bottom:.1f} {n.left:.1f},{n.bottom:.1f}"
            )
            out.append(f'<polygon points="{pts}" class="b"/>')
        elif k == "preparation":
            s = self.slant * 0.7
            pts = (
                f"{n.x + s:.1f},{n.top:.1f} {n.right - s:.1f},{n.top:.1f} "
                f"{n.right:.1f},{n.cy:.1f} {n.right - s:.1f},{n.bottom:.1f} "
                f"{n.x + s:.1f},{n.bottom:.1f} {n.left:.1f},{n.cy:.1f}"
            )
            out.append(f'<polygon points="{pts}" class="b"/>')
        elif k == "connector":
            r = n.h / 2
            out.append(
                f'<circle cx="{n.cx:.1f}" cy="{n.cy:.1f}" r="{r:.1f}" class="b"/>'
            )
        else:
            out.append(
                f'<rect x="{n.x:.1f}" y="{n.y:.1f}" width="{n.w:.1f}" '
                f'height="{n.h:.1f}" class="b"/>'
            )
            if k == "subroutine":
                for x in (n.left + self.sub_inset, n.right - self.sub_inset):
                    out.append(
                        f'<line x1="{x:.1f}" y1="{n.top:.1f}" x2="{x:.1f}" '
                        f'y2="{n.bottom:.1f}" class="l"/>'
                    )
        return out

    def _text(self, n: Node) -> list[str]:
        out = []
        n_lines = len(n.lines)
        for i, line in enumerate(n.lines):
            y = n.cy + (i - (n_lines - 1) / 2) * self.lh
            out.append(
                f'<text x="{n.cx:.1f}" y="{y:.1f}" class="t">{esc(line)}</text>'
            )
        return out

    def render(self) -> str:
        axes = self._axes()
        for n in self.nodes:
            n.x = axes[n.col] - n.w / 2

        shapes: list[str] = []
        wires: list[str] = []
        for n in self.nodes:
            shapes += self._shape(n) + self._text(n)
        for pts, arrow in self.wires:
            d = " ".join(
                ("M" if i == 0 else "L") + f" {x:.1f} {y:.1f}"
                for i, (x, y) in enumerate(pts)
            )
            m = ' marker-end="url(#a)"' if arrow else ""
            wires.append(f'<path d="{d}" class="l"{m}/>')

        labels = [
            f'<text x="{x:.1f}" y="{y:.1f}" class="t" text-anchor="{anc}">{esc(t)}</text>'
            for x, y, t, anc in self.labels
        ]

        xs: list[float] = []
        ys: list[float] = []
        for n in self.nodes:
            xs += [n.left, n.right]
            ys += [n.top, n.bottom]
        for pts, _ in self.wires:
            for x, y in pts:
                xs.append(x)
                ys.append(y)
        m = self.margin
        x0, x1 = min(xs) - m, max(xs) + m
        y0, y1 = min(ys) - m, max(ys) + m

        return f"""<svg xmlns="http://www.w3.org/2000/svg" viewBox="{x0:.1f} {y0:.1f} {x1 - x0:.1f} {y1 - y0:.1f}" style="background:transparent;font-family:{FONT};">
<defs>
<marker id="a" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse"><path d="M0 1.2 L10 5 L0 8.8 z" fill="#000"/></marker>
<style>
.b{{stroke:#000;stroke-width:1.5;fill:#fff}}
.l{{stroke:#000;stroke-width:1.5;fill:none;stroke-linejoin:miter;stroke-linecap:butt}}
.t{{font-size:{self.fs}px;fill:#000;text-anchor:middle;dominant-baseline:central}}
</style>
</defs>
<g>{''.join(wires)}</g>
<g>{''.join(shapes)}</g>
<g>{''.join(labels)}</g>
</svg>
"""


if __name__ == "__main__":
    c = Chart()
    s = c.start()
    i = c.io("Введення: a, b", below=s)
    d = c.decision("a > b", below=i)
    a = c.process("max = a", below=d)
    b = c.process("max = b", below=d, row=d, dy=60)
    e = c.end(below=a)
    c.drop(d, a)
    c.branch(d, b, "right", "Ні")
    c.merge(b, e, "left")
    print(c.render())