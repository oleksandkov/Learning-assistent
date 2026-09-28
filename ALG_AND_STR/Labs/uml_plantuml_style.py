"""
Генератор UML-діаграм у стилі PlantUML (білий фон, чорні рамки, стандартні нотації).
Підтримує: діаграму класів, діаграму послідовності, діаграму станів.
Використовується для створення UML-діаграм у звітах лабораторних робіт (v3),
уніфікованих зі стилем курсової роботи «Сокобан».
"""
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import FancyBboxPatch, Circle, Rectangle
import numpy as np

# ---------- загальні налаштування стилю PlantUML ----------
FONT = "DejaVu Sans"
COLOR_BLACK = "#1a1a1a"
COLOR_WHITE = "#ffffff"
COLOR_HEADER = "#f5f5f5"


def _new_fig(w_inch, h_inch):
    fig, ax = plt.subplots(figsize=(w_inch, h_inch), dpi=150)
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.axis("off")
    ax.set_facecolor(COLOR_WHITE)
    fig.patch.set_facecolor(COLOR_WHITE)
    return fig, ax


def _txt(ax, x, y, s, size=8, bold=False, ha="center", va="center", color=COLOR_BLACK,
         style="normal", bbox=False):
    kw = {}
    if bbox:
        kw["bbox"] = dict(boxstyle="round,pad=0.15", facecolor=COLOR_WHITE,
                          edgecolor="none", alpha=0.9)
    ax.text(x, y, s, fontname=FONT, fontsize=size, fontweight="bold" if bold else "normal",
            ha=ha, va=va, color=color, style=style, clip_on=False, zorder=6, **kw)


def _arrow(ax, x1, y1, x2, y2, dashed=False, color=COLOR_BLACK, lw=1.0, arrowstyle="-|>"):
    ls = (0, (4, 3)) if dashed else "solid"
    ax.annotate("", xy=(x2, y2), xytext=(x1, y1),
                arrowprops=dict(arrowstyle=arrowstyle, color=color, lw=lw,
                                linestyle=ls, shrinkA=0, shrinkB=0), zorder=4)


def _hollow_triangle(ax, x, y, w, h, direction="up", color=COLOR_BLACK):
    if direction == "up":
        pts = [(x - w / 2, y - h), (x + w / 2, y - h), (x, y)]
    elif direction == "down":
        pts = [(x - w / 2, y + h), (x + w / 2, y + h), (x, y)]
    elif direction == "left":
        pts = [(x + w, y - h / 2), (x + w, y + h / 2), (x, y)]
    else:
        pts = [(x - w, y - h / 2), (x - w, y + h / 2), (x, y)]
    tri = plt.Polygon(pts, closed=True, facecolor=COLOR_WHITE, edgecolor=color, lw=1.2, zorder=5)
    ax.add_patch(tri)


def _diamond(ax, x, y, size, filled=True, color=COLOR_BLACK):
    pts = [(x, y + size), (x + size, y), (x, y - size), (x - size, y)]
    d = plt.Polygon(pts, closed=True,
                    facecolor=COLOR_BLACK if filled else COLOR_WHITE,
                    edgecolor=color, lw=1.2, zorder=5)
    ax.add_patch(d)


def _wrap(s, width_units, fontsize=7):
    """Перенесення тексту щоб вміщувався у ширину блока (у координатах осі)."""
    # середня ширина символу ≈ 0.53 одиниці при fontsize=7 (для осі 0..100)
    char_w = 0.53 * (fontsize / 7.0)
    max_chars = max(8, int(width_units / char_w))
    if len(s) <= max_chars:
        return [s]
    words = s.split()
    lines, cur = [], ""
    for w in words:
        if cur and len(cur) + 1 + len(w) > max_chars:
            lines.append(cur)
            cur = w
        else:
            cur = (cur + " " + w).strip()
    if cur:
        lines.append(cur)
    return lines


def _line_rect_exit(x1, y1, x2, y2, rx, ry, rw, rh):
    """Точка виходу прямої (x1,y1)->(x2,y2) з прямокутника [rx,rw]x[ry,rh]."""
    dx, dy = x2 - x1, y2 - y1
    if dx == 0 and dy == 0:
        return x1, y1
    ts = []
    if dx > 0:
        ts.append(((rx + rw - x1) / dx, rx + rw, None))
    elif dx < 0:
        ts.append(((rx - x1) / dx, rx, None))
    if dy > 0:
        ts.append(((ry + rh - y1) / dy, None, ry + rh))
    elif dy < 0:
        ts.append(((ry - y1) / dy, None, ry))
    best = None
    for t, hx, hy in ts:
        if 0 <= t <= 1:
            px, py = x1 + t * dx, y1 + t * dy
            if hx is not None and abs(py - (ry + rh / 2)) <= rh / 2 + 0.01:
                if best is None or t < best[0]:
                    best = (t, px, py)
            if hy is not None and abs(px - (rx + rw / 2)) <= rw / 2 + 0.01:
                if best is None or t < best[0]:
                    best = (t, px, py)
    if best is None:
        return x1, y1
    return best[1], best[2]


def _line_rect_enter(x1, y1, x2, y2, rx, ry, rw, rh):
    """Точка входу прямої (x1,y1)->(x2,y2) у прямокутник [rx,rw]x[ry,rh]."""
    dx, dy = x2 - x1, y2 - y1
    if dx == 0 and dy == 0:
        return x2, y2
    ts = []
    if dx > 0:
        ts.append(((rx - x1) / dx, rx, None))
    elif dx < 0:
        ts.append(((rx + rw - x1) / dx, rx + rw, None))
    if dy > 0:
        ts.append(((ry - y1) / dy, None, ry))
    elif dy < 0:
        ts.append(((ry + rh - y1) / dy, None, ry + rh))
    best = None
    for t, hx, hy in ts:
        if 0 <= t <= 1:
            px, py = x1 + t * dx, y1 + t * dy
            if hx is not None and ry - 0.01 <= py <= ry + rh + 0.01:
                if best is None or t < best[0]:
                    best = (t, px, py)
            if hy is not None and rx - 0.01 <= px <= rx + rw + 0.01:
                if best is None or t < best[0]:
                    best = (t, px, py)
    if best is None:
        return x2, y2
    return best[1], best[2]


# ============================================================
#  ДІАГРАМА КЛАСІВ
# ============================================================
def draw_class_diagram(path, classes, relations, title=None, w_inch=11, h_inch=8):
    """
    classes: list of dicts:
        name, stereotype (optional), attributes [str], methods [str],
        x, y (bottom-left, опційно), w (ширина, опційно)
    relations: list of dicts:
        type: "association" | "dependency" | "generalization" | "aggregation" | "composition"
        from, to: class names; label (optional); self (bool)
    """
    fig, ax = _new_fig(w_inch, h_inch)

    def class_h(c):
        w = c.get("w", 20)
        n_attr_lines = sum(len(_wrap(a, w - 2.4)) for a in c.get("attributes", []))
        n_met_lines = sum(len(_wrap(m, w - 2.4)) for m in c.get("methods", []))
        return 6 + 3.0 * n_attr_lines + 3.0 * n_met_lines + 2

    # --- розкладка: сітка, якщо немає координат ---
    manual = all("x" in c for c in classes)
    if not manual:
        n = len(classes)
        cols = max(1, min(4, int(np.ceil(np.sqrt(n * 1.4)))))
        col_w = 92 / cols
        col_heights = [0.0] * cols
        for i, c in enumerate(classes):
            col = i % cols
            c["x"] = 4 + col_w * col + 1.5
            c["w"] = col_w - 3
            c["y"] = 92 - col_heights[col] - class_h(c)
            col_heights[col] += class_h(c) + 6

    pos = {c["name"]: (c["x"], c["y"], c.get("w", 20)) for c in classes}
    hmap = {c["name"]: class_h(c) for c in classes}

    # --- відношення ---
    for r in relations:
        x1, y1, w1 = pos[r["from"]]
        x2, y2, w2 = pos[r["to"]]
        h1, h2 = hmap[r["from"]], hmap[r["to"]]
        rtype = r.get("type", "association")
        dashed = rtype == "dependency"

        if r.get("self") or r["from"] == r["to"]:
            # петля у верхньому правому куті блока
            bx, by, bw = x1, y1, w1
            sx, sy = bx + bw - 1, by + h1 - 1
            mx, my = sx + 5, sy + 3.5
            ax.plot([sx, mx, mx], [sy, sy, my], color=COLOR_BLACK, lw=1.1,
                    linestyle="solid" if not dashed else (0, (4, 3)), zorder=3)
            _arrow(ax, mx, my, sx + 1, my)
            if r.get("label"):
                _txt(ax, mx + 0.5, my + 1.2, r["label"], size=6.5, bbox=True)
            continue

        cx1, cy1 = x1 + w1 / 2, y1 + h1 / 2
        cx2, cy2 = x2 + w2 / 2, y2 + h2 / 2
        sx, sy = _line_rect_exit(cx1, cy1, cx2, cy2, x1, y1, w1, h1)
        ex, ey = _line_rect_enter(cx1, cy1, cx2, cy2, x2, y2, w2, h2)

        if rtype == "generalization":
            _arrow(ax, sx, sy, ex, ey, arrowstyle="-")
            dx, dy = ex - sx, ey - sy
            dist = max(1e-6, np.hypot(dx, dy))
            ux, uy = dx / dist, dy / dist
            if abs(ux) > abs(uy):
                direction = "right" if ux > 0 else "left"
            else:
                direction = "down" if uy > 0 else "up"
            _hollow_triangle(ax, ex, ey, 5, 2.5, direction=direction)
        elif rtype in ("aggregation", "composition"):
            _arrow(ax, sx, sy, ex, ey, arrowstyle="-")
            filled = rtype == "composition"
            dx, dy = ex - sx, ey - sy
            dist = max(1e-6, np.hypot(dx, dy))
            ux, uy = dx / dist, dy / dist
            if abs(ux) > abs(uy):
                direction = "right" if ux > 0 else "left"
            else:
                direction = "down" if uy > 0 else "up"
            if direction == "right":
                _diamond(ax, sx + 2.2, sy, 2.2, filled=filled)
            elif direction == "left":
                _diamond(ax, sx - 2.2, sy, 2.2, filled=filled)
            elif direction == "down":
                _diamond(ax, sx, sy + 2.2, 2.2, filled=filled)
            else:
                _diamond(ax, sx, sy - 2.2, 2.2, filled=filled)
        else:
            _arrow(ax, sx, sy, ex, ey, dashed=dashed)
        if r.get("label"):
            lp = r.get("label_pos", 0.5)
            mx, my = sx + (ex - sx) * lp, sy + (ey - sy) * lp
            _txt(ax, mx, my + 1.8, r["label"], size=6.5, bbox=True)

    # --- класи ---
    for c in classes:
        x, y, w = c["x"], c["y"], c.get("w", 20)
        h = class_h(c)
        n_attr = len(c.get("attributes", []))
        n_met = len(c.get("methods", []))
        header_h = 5 if not c.get("stereotype") else 7
        ax.add_patch(FancyBboxPatch((x, y), w, h, boxstyle="round,pad=0,rounding_size=0.4",
                                    facecolor=COLOR_WHITE, edgecolor=COLOR_BLACK, lw=1.2, zorder=2))
        ax.add_patch(Rectangle((x, y + h - header_h), w, header_h,
                               facecolor=COLOR_HEADER, edgecolor=COLOR_BLACK, lw=1.0, zorder=3))
        cy = y + h - header_h / 2 - 0.4
        name_disp = f"«{c['stereotype']}» {c['name']}" if c.get("stereotype") else c["name"]
        _txt(ax, x + w / 2, cy, name_disp, size=8.5, bold=True)
        ay = y + h - header_h - 1.2
        for a in c.get("attributes", []):
            for line in _wrap(a, w - 2.4):
                _txt(ax, x + 1.2, ay, line, size=7, ha="left")
                ay -= 3.0
        if n_attr and n_met:
            ax.plot([x, x + w], [ay + 1.5, ay + 1.5], color=COLOR_BLACK, lw=0.8, zorder=3)
        my = ay - 1.2 if n_attr else y + h - header_h - 1.2
        for m in c.get("methods", []):
            for line in _wrap(m, w - 2.4):
                _txt(ax, x + 1.2, my, line, size=7, ha="left")
                my -= 3.0

    if title:
        _txt(ax, 50, 97.5, title, size=10, bold=True)

    plt.savefig(path, dpi=150, bbox_inches="tight", facecolor=COLOR_WHITE, pad_inches=0.15)
    plt.close(fig)


# ============================================================
#  ДІАГРАМА ПОСЛІДОВНОСТІ
# ============================================================
def draw_sequence_diagram(path, participants, messages, title=None, w_inch=11, h_inch=7):
    """
    participants: list of str
    messages: list of dicts: from, to, label, kind: "call"|"return"|"self"
    """
    fig, ax = _new_fig(w_inch, h_inch)
    n = len(participants)
    margin = 8
    span = 100 - 2 * margin
    step = span / max(1, n - 1) if n > 1 else span
    px = {p: margin + i * step for i, p in enumerate(participants)}

    top = 88
    box_w = min(16, step * 0.8)
    box_h = 5
    for p in participants:
        x = px[p] - box_w / 2
        ax.add_patch(FancyBboxPatch((x, top - box_h), box_w, box_h,
                                    boxstyle="round,pad=0,rounding_size=0.5",
                                    facecolor=COLOR_WHITE, edgecolor=COLOR_BLACK, lw=1.2, zorder=3))
        _txt(ax, px[p], top - box_h / 2, p, size=7.5, bold=True)
    for p in participants:
        ax.plot([px[p], px[p]], [top - box_h, 8], color=COLOR_BLACK, lw=1.0, ls=(0, (5, 4)), zorder=1)

    y = top - box_h - 6
    for m in messages:
        kind = m.get("kind", "call")
        dashed = kind == "return"
        if kind == "self":
            _txt(ax, px[m["from"]] + 3, y, m["label"], size=7, ha="left")
            _arrow(ax, px[m["from"]], y - 1.5, px[m["from"]], y - 4, dashed=False)
            y -= 5
            continue
        x1, x2 = px[m["from"]], px[m["to"]]
        # смужка активації на лінії життя викликаючого
        ax.add_patch(Rectangle((x1 - 0.5, y - 3.2), 1.0, 2.6,
                               facecolor="#e0e0e0", edgecolor="none", zorder=2))
        _arrow(ax, x1, y - 1, x2, y - 1, dashed=dashed, lw=1.1)
        _txt(ax, (x1 + x2) / 2, y + 0.9, m["label"], size=7,
             ha="center" if abs(x2 - x1) > 12 else ("left" if x2 > x1 else "right"),
             bbox=abs(x2 - x1) <= 12)
        y -= m.get("gap", 4.5)
        if m.get("note"):
            _txt(ax, (x1 + x2) / 2, y + 0.5, m["note"], size=6.5, style="italic", color="#555555")
            y -= 3.5

    if title:
        _txt(ax, 50, 97.5, title, size=10, bold=True)

    plt.savefig(path, dpi=150, bbox_inches="tight", facecolor=COLOR_WHITE, pad_inches=0.15)
    plt.close(fig)


# ============================================================
#  ДІАГРАМА СТАНІВ
# ============================================================
def draw_state_diagram(path, states, transitions, title=None, w_inch=11, h_inch=8,
                       initial=None, final=None):
    """
    states: list of dicts: name, x, y (центр), w, h
    transitions: list of dicts: from, to, label
    """
    fig, ax = _new_fig(w_inch, h_inch)
    pos = {s["name"]: (s["x"], s["y"]) for s in states}
    sizes = {s["name"]: (s.get("w", 18), s.get("h", 7)) for s in states}

    for t in transitions:
        x1, y1 = pos[t["from"]]
        x2, y2 = pos[t["to"]]
        w1, h1 = sizes[t["from"]]
        w2, h2 = sizes[t["to"]]
        sx, sy = _line_rect_exit(x1, y1, x2, y2, x1 - w1 / 2, y1 - h1 / 2, w1, h1)
        ex, ey = _line_rect_enter(x1, y1, x2, y2, x2 - w2 / 2, y2 - h2 / 2, w2, h2)
        _arrow(ax, sx, sy, ex, ey)
        # підпис збоку від стрілки
        dx, dy = ex - sx, ey - sy
        dist = max(1e-6, np.hypot(dx, dy))
        ux, uy = dx / dist, dy / dist
        mx, my = (sx + ex) / 2, (sy + ey) / 2
        _txt(ax, mx + uy * 3.2, my - ux * 3.2 + 0.8, t["label"], size=6.5, bbox=True)

    if initial and initial in pos:
        x, y = pos[initial]
        w, h = sizes[initial]
        ax.add_patch(Circle((x - w / 2 - 5, y), 1.6, facecolor=COLOR_BLACK, edgecolor=COLOR_BLACK, zorder=6))
        _arrow(ax, x - w / 2 - 3.4, y, x - w / 2 - 0.6, y)

    if final and final in pos:
        x, y = pos[final]
        ax.add_patch(Circle((x, y), 2.2, facecolor=COLOR_WHITE, edgecolor=COLOR_BLACK, lw=1.4, zorder=6))
        ax.add_patch(Circle((x, y), 1.0, facecolor=COLOR_BLACK, edgecolor=COLOR_BLACK, zorder=7))

    for s in states:
        x, y = s["x"], s["y"]
        w, h = s.get("w", 18), s.get("h", 7)
        ax.add_patch(FancyBboxPatch((x - w / 2, y - h / 2), w, h,
                                    boxstyle="round,pad=0,rounding_size=1.5",
                                    facecolor=COLOR_WHITE, edgecolor=COLOR_BLACK, lw=1.3, zorder=2))
        _txt(ax, x, y, s["name"], size=8, bold=True)

    if title:
        _txt(ax, 50, 97.5, title, size=10, bold=True)

    plt.savefig(path, dpi=150, bbox_inches="tight", facecolor=COLOR_WHITE, pad_inches=0.15)
    plt.close(fig)
