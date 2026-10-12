"""Checks a generated SVG for layout faults.

Flowcharts (ГОСТ): every connector must be axis-aligned, no wire vertex may
land inside a block, no two blocks may overlap.
Class diagrams (UML): boxes must not overlap each other. Relationship lines
are legitimately diagonal and deliberately touch their endpoints, so the
diagonal and vertex rules do not apply there.

    python examples/check_orthogonal.py

Exits non-zero on any finding, so it doubles as the smoke test for a new
diagram. Arrowheads inside <defs> are ignored on purpose.
"""

from __future__ import annotations

import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent.parent
TOL = 0.6
MIN_BOX = 24.0  # smaller filled polygons are arrowheads, not blocks

seg = re.compile(r"([ML])\s*(-?[\d.]+)\s+(-?[\d.]+)")
box = re.compile(r'<rect x="(-?[\d.]+)" y="(-?[\d.]+)" width="([\d.]+)" height="([\d.]+)"')
poly = re.compile(r'<polygon points="([^"]+)"')
circ = re.compile(r'<circle cx="(-?[\d.]+)" cy="(-?[\d.]+)" r="([\d.]+)"')


def strip_defs(svg: str) -> str:
    return re.sub(r"<defs>.*?</defs>", "", svg, flags=re.S)


def shapes(svg: str) -> list[tuple[float, float, float, float]]:
    """All class="b" geometry as (left, top, right, bottom).

    Anything smaller than MIN_BOX is dropped: UML arrowheads are filled
    polygons too, and they legitimately sit on a parent's border.
    """
    out = []
    for x, y, w, h in box.findall(svg):
        x, y, w, h = float(x), float(y), float(w), float(h)
        out.append((x, y, x + w, y + h))
    for pts in poly.findall(svg):
        p = [tuple(map(float, q.split(","))) for q in pts.split()]
        b = (
            min(q[0] for q in p),
            min(q[1] for q in p),
            max(q[0] for q in p),
            max(q[1] for q in p),
        )
        if b[2] - b[0] >= MIN_BOX and b[3] - b[1] >= MIN_BOX:
            out.append(b)
    for cx, cy, r in circ.findall(svg):
        cx, cy, r = float(cx), float(cy), float(r)
        out.append((cx - r, cy - r, cx + r, cy + r))
    return out


def inside(pt, b) -> bool:
    return b[0] + TOL < pt[0] < b[2] - TOL and b[1] + TOL < pt[1] < b[3] - TOL


def overlaps(a, b) -> bool:
    ox = min(a[2], b[2]) - max(a[0], b[0])
    oy = min(a[3], b[3]) - max(a[1], b[1])
    return ox > 2 and oy > 2


def check(path: pathlib.Path) -> int:
    svg = strip_defs(path.read_text(encoding="utf-8"))
    # A flowchart uses Times serif and a 1.5px stroke; the UML class generator
    # uses sans-serif and 1px. That distinguishes the two rule sets.
    is_uml = "Helvetica" in svg
    boxes = shapes(svg)
    bad = 0

    if not is_uml:
        for d in re.findall(r'<path d="([^"]+)"', svg):
            pts = [(float(a), float(b)) for _, a, b in seg.findall(d)]
            for p, q in zip(pts, pts[1:]):
                if abs(p[0] - q[0]) > TOL and abs(p[1] - q[1]) > TOL:
                    print(f"  diagonal segment {p} -> {q}")
                    bad += 1
            for p in pts:
                for b in boxes:
                    if inside(p, b):
                        print(f"  vertex {p} inside block {b}")
                        bad += 1

    for i, a in enumerate(boxes):
        for b in boxes[i + 1 :]:
            if overlaps(a, b):
                print(f"  blocks overlap: {a} and {b}")
                bad += 1

    kind = "uml " if is_uml else "gost"
    print(f"[{kind}] {path.name}: {'OK' if not bad else str(bad) + ' problem(s)'}")
    return bad


if __name__ == "__main__":
    total = sum(check(f) for f in sorted(HERE.glob("*.svg")))
    sys.exit(1 if total else 0)