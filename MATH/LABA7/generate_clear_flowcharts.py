import os
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, FancyArrowPatch

OUT_DIR = r"c:\Users\muaro\Documents\GitHub\Learning-assistent\MATH\LABA7"


class Node:
    def __init__(self, node_id, text, x, y, w, h):
        self.id = node_id
        self.text = text
        self.x = x
        self.y = y
        self.w = w
        self.h = h

    @property
    def left(self):
        return self.x - self.w / 2

    @property
    def right(self):
        return self.x + self.w / 2

    @property
    def top(self):
        return self.y + self.h / 2

    @property
    def bottom(self):
        return self.y - self.h / 2


def add_node(ax, n: Node):
    rect = Rectangle(
        (n.left, n.bottom),
        n.w,
        n.h,
        linewidth=1.8,
        edgecolor="#222222",
        facecolor="#F7FAFF",
    )
    ax.add_patch(rect)
    ax.text(n.x, n.y, n.text, ha="center", va="center", fontsize=10)


def draw_path_arrow(ax, points, label=None, label_pos=0.5):
    # Polyline segments
    for i in range(len(points) - 2):
        x1, y1 = points[i]
        x2, y2 = points[i + 1]
        ax.plot([x1, x2], [y1, y2], color="#222222", lw=1.5)

    # Final segment with arrow head
    x1, y1 = points[-2]
    x2, y2 = points[-1]
    arr = FancyArrowPatch(
        (x1, y1),
        (x2, y2),
        arrowstyle="->",
        mutation_scale=13,
        linewidth=1.6,
        color="#222222",
    )
    ax.add_patch(arr)

    if label:
        total = len(points) - 1
        idx = max(0, min(total - 1, int(label_pos * total)))
        lx = (points[idx][0] + points[idx + 1][0]) / 2
        ly = (points[idx][1] + points[idx + 1][1]) / 2
        ax.text(
            lx,
            ly + 0.015,
            label,
            fontsize=9,
            ha="center",
            va="center",
            bbox=dict(boxstyle="round,pad=0.15", fc="white", ec="none"),
        )


def make_dp_chart(path):
    fig, ax = plt.subplots(figsize=(14, 12), dpi=180)
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.axis("off")
    ax.set_title("Блок-схема: Найкоротший шлях у DAG (метод ДП)", fontsize=16, fontweight="bold", pad=16)

    n = {}
    n["start"] = Node("start", "Початок", 0.5, 0.95, 0.20, 0.05)
    n["input"] = Node("input", "Ввід матриці A, вершин s і t", 0.5, 0.875, 0.36, 0.06)
    n["valid"] = Node("valid", "Матриця коректна і квадратна?", 0.5, 0.795, 0.36, 0.06)
    n["err_in"] = Node("err_in", "Помилка вводу", 0.18, 0.715, 0.20, 0.055)

    n["topo"] = Node("topo", "order = topo_sort(A)", 0.5, 0.715, 0.30, 0.06)
    n["cycle"] = Node("cycle", "order == None?", 0.5, 0.635, 0.26, 0.06)
    n["err_cycle"] = Node("err_cycle", "Граф має цикл,\nметод ДП не застосовується", 0.17, 0.555, 0.30, 0.07)

    n["init"] = Node("init", "Ініціалізація: λ[i]=∞, λ[s]=0", 0.5, 0.555, 0.38, 0.06)
    n["loop"] = Node("loop", "Для кожної j у order", 0.5, 0.475, 0.28, 0.055)
    n["relax"] = Node("relax", "Релаксація вхідних дуг:\nλ[j] = min(λ[j], λ[i] + A[i][j])", 0.5, 0.39, 0.46, 0.075)
    n["reach"] = Node("reach", "λ[t] == ∞ ?", 0.5, 0.305, 0.22, 0.055)

    n["no_path"] = Node("no_path", "Шлях не існує", 0.25, 0.225, 0.22, 0.055)
    n["restore"] = Node("restore", "Відновити шлях від t до s", 0.5, 0.225, 0.28, 0.055)
    n["out"] = Node("out", "Вивести order, λ, path, λ[t]", 0.5, 0.13, 0.36, 0.06)

    for node in n.values():
        add_node(ax, node)

    # Main vertical flow
    draw_path_arrow(ax, [(n["start"].x, n["start"].bottom), (n["input"].x, n["input"].top)])
    draw_path_arrow(ax, [(n["input"].x, n["input"].bottom), (n["valid"].x, n["valid"].top)])
    draw_path_arrow(ax, [(n["valid"].x, n["valid"].bottom), (n["topo"].x, n["topo"].top)], label="Так")
    draw_path_arrow(ax, [(n["topo"].x, n["topo"].bottom), (n["cycle"].x, n["cycle"].top)])
    draw_path_arrow(ax, [(n["cycle"].x, n["cycle"].bottom), (n["init"].x, n["init"].top)], label="Ні")
    draw_path_arrow(ax, [(n["init"].x, n["init"].bottom), (n["loop"].x, n["loop"].top)])
    draw_path_arrow(ax, [(n["loop"].x, n["loop"].bottom), (n["relax"].x, n["relax"].top)])
    draw_path_arrow(ax, [(n["relax"].x, n["relax"].bottom), (n["reach"].x, n["reach"].top)])
    draw_path_arrow(ax, [(n["reach"].x, n["reach"].bottom), (n["restore"].x, n["restore"].top)], label="Ні")

    # Side branches routed around boxes
    draw_path_arrow(
        ax,
        [
            (n["valid"].left, n["valid"].y),
            (0.33, n["valid"].y),
            (0.33, n["err_in"].y),
            (n["err_in"].right, n["err_in"].y),
        ],
        label="Ні",
        label_pos=0.2,
    )

    draw_path_arrow(
        ax,
        [
            (n["cycle"].left, n["cycle"].y),
            (0.31, n["cycle"].y),
            (0.31, n["err_cycle"].y),
            (n["err_cycle"].right, n["err_cycle"].y),
        ],
        label="Так",
        label_pos=0.2,
    )

    draw_path_arrow(
        ax,
        [
            (n["reach"].left, n["reach"].y),
            (0.31, n["reach"].y),
            (0.31, n["no_path"].top + 0.02),
            (n["no_path"].x, n["no_path"].top),
        ],
        label="Так",
        label_pos=0.2,
    )

    # Both outcomes to output block without overlap
    draw_path_arrow(ax, [(n["restore"].x, n["restore"].bottom), (n["out"].x, n["out"].top)])
    draw_path_arrow(
        ax,
        [
            (n["no_path"].x, n["no_path"].bottom),
            (n["no_path"].x, 0.17),
            (n["out"].left - 0.03, 0.17),
            (n["out"].left - 0.03, n["out"].y),
            (n["out"].left, n["out"].y),
        ],
    )

    fig.savefig(path, bbox_inches="tight")
    plt.close(fig)


def make_topo_chart(path):
    fig, ax = plt.subplots(figsize=(14, 12), dpi=180)
    ax.set_xlim(0, 1)
    ax.set_ylim(0, 1)
    ax.axis("off")
    ax.set_title("Блок-схема: Топологічне сортування + випадкова нумерація", fontsize=16, fontweight="bold", pad=16)

    n = {}
    n["start"] = Node("start", "Початок", 0.5, 0.95, 0.20, 0.05)
    n["input"] = Node("input", "Ввід/отримання матриці A", 0.5, 0.875, 0.34, 0.06)
    n["ren_q"] = Node("ren_q", "Потрібна випадкова перенумерація?", 0.5, 0.795, 0.38, 0.06)

    n["perm"] = Node("perm", "perm = shuffle(0..n-1),\nпобудувати A'", 0.78, 0.72, 0.28, 0.07)
    n["prep"] = Node("prep", "out_deg, remaining, order_desc", 0.5, 0.72, 0.34, 0.06)

    n["empty_q"] = Node("empty_q", "remaining порожня?", 0.5, 0.64, 0.26, 0.06)
    n["done"] = Node("done", "order = reverse(order_desc),\nвивести порядок", 0.80, 0.56, 0.30, 0.07)
    n["end"] = Node("end", "Кінець", 0.80, 0.47, 0.20, 0.055)

    n["sinks"] = Node("sinks", "sinks = вершини з out_deg = 0", 0.5, 0.56, 0.34, 0.06)
    n["sinks_q"] = Node("sinks_q", "sinks порожній?", 0.5, 0.48, 0.24, 0.06)
    n["cycle"] = Node("cycle", "Цикл у графі (стоп)", 0.22, 0.40, 0.24, 0.06)
    n["pick"] = Node("pick", "Вибрати випадково xk з sinks,\nвидалити, оновити out_deg", 0.5, 0.40, 0.42, 0.07)

    for node in n.values():
        add_node(ax, node)

    # Main flow
    draw_path_arrow(ax, [(n["start"].x, n["start"].bottom), (n["input"].x, n["input"].top)])
    draw_path_arrow(ax, [(n["input"].x, n["input"].bottom), (n["ren_q"].x, n["ren_q"].top)])
    draw_path_arrow(ax, [(n["ren_q"].x, n["ren_q"].bottom), (n["prep"].x, n["prep"].top)], label="Ні")
    draw_path_arrow(ax, [(n["prep"].x, n["prep"].bottom), (n["empty_q"].x, n["empty_q"].top)])
    draw_path_arrow(ax, [(n["empty_q"].x, n["empty_q"].bottom), (n["sinks"].x, n["sinks"].top)], label="Ні")
    draw_path_arrow(ax, [(n["sinks"].x, n["sinks"].bottom), (n["sinks_q"].x, n["sinks_q"].top)])
    draw_path_arrow(ax, [(n["sinks_q"].x, n["sinks_q"].bottom), (n["pick"].x, n["pick"].top)], label="Ні")

    # Loop back from pick to empty_q (left corridor)
    draw_path_arrow(
        ax,
        [
            (n["pick"].left, n["pick"].y),
            (0.30, n["pick"].y),
            (0.30, n["empty_q"].y),
            (n["empty_q"].left, n["empty_q"].y),
        ],
    )

    # Renumber YES branch to perm then to prep (right corridor)
    draw_path_arrow(
        ax,
        [
            (n["ren_q"].right, n["ren_q"].y),
            (0.64, n["ren_q"].y),
            (0.64, n["perm"].y),
            (n["perm"].left, n["perm"].y),
        ],
        label="Так",
        label_pos=0.25,
    )
    draw_path_arrow(ax, [(n["perm"].left, n["perm"].y - 0.02), (n["prep"].right, n["prep"].y - 0.02)])

    # remaining empty YES branch to done and end
    draw_path_arrow(
        ax,
        [
            (n["empty_q"].right, n["empty_q"].y),
            (0.64, n["empty_q"].y),
            (0.64, n["done"].y),
            (n["done"].left, n["done"].y),
        ],
        label="Так",
        label_pos=0.2,
    )
    draw_path_arrow(ax, [(n["done"].x, n["done"].bottom), (n["end"].x, n["end"].top)])

    # sinks empty YES branch to cycle
    draw_path_arrow(
        ax,
        [
            (n["sinks_q"].left, n["sinks_q"].y),
            (0.33, n["sinks_q"].y),
            (0.33, n["cycle"].y),
            (n["cycle"].right, n["cycle"].y),
        ],
        label="Так",
        label_pos=0.22,
    )

    fig.savefig(path, bbox_inches="tight")
    plt.close(fig)


def main():
    os.makedirs(OUT_DIR, exist_ok=True)
    dp_path = os.path.join(OUT_DIR, "flowchart_shortest_path_dp.png")
    topo_path = os.path.join(OUT_DIR, "flowchart_toposort_renumber.png")

    make_dp_chart(dp_path)
    make_topo_chart(topo_path)

    print(dp_path)
    print(topo_path)


if __name__ == "__main__":
    main()
