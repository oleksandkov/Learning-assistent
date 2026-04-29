from typing import List, Optional, Tuple
import tkinter as tk
import math

INF = float("inf")


class DirectedGraph:
    def __init__(self, matrix: List[List[float]], is_weighted: bool = True):
        if not is_weighted:
            self.weights = adjacency_to_weight_simple(matrix)  
        else:
            self.weights = matrix 
        self.n = len(self.weights)

    @classmethod
    def from_adjacency(cls, adjacency: List[List[int]]) -> "DirectedGraph":
        return cls(adjacency, is_weighted=False)

    def bellman_ford(self, start: int) -> Tuple[List[float], List[Optional[int]]]:
        n = self.n
        dist = [INF] * n
        parent: List[Optional[int]] = [None] * n
        dist[start] = 0.0

        for _ in range(n - 1):
            updated = False
            for u in range(n):
                if dist[u] == INF:
                    continue
                for v in range(n):
                    w = self.weights[u][v]
                    if w != INF and dist[u] + w < dist[v]:
                        dist[v] = dist[u] + w
                        parent[v] = u
                        updated = True
            if not updated:
                break

        return dist, parent

    def bfs_shortest_path(self, start: int, end: int) -> Tuple[Optional[List[int]], Optional[int]]:
        from collections import deque

        n = self.n
        dist = [-1] * n
        parent: List[Optional[int]] = [None] * n
        q = deque()
        dist[start] = 0
        q.append(start)

        while q:
            u = q.popleft()
            if u == end:
                break
            for v in range(n):
                if self.weights[u][v] != INF and dist[v] == -1:
                    dist[v] = dist[u] + 1
                    parent[v] = u
                    q.append(v)

        if dist[end] == -1:
            return None, None

        path = []
        cur = end
        while cur is not None:
            path.append(cur)
            cur = parent[cur]
        path.reverse()
        return path, dist[end]


def format_path(path: List[int]) -> str:
    return " -> ".join(f"v{v+1}" for v in path)


def adjacency_to_weight_simple(adjacency: List[List[int]]) -> List[List[float]]:
    n = len(adjacency)
    w = [[INF] * n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            if adjacency[i][j] == 1:
                w[i][j] = 1.0
    return w


def bellman_ford_simple(weights: List[List[float]], start: int) -> Tuple[List[float], List[Optional[int]]]:
    n = len(weights)
    dist = [INF] * n
    parent: List[Optional[int]] = [None] * n
    dist[start] = 0.0

    for _ in range(n - 1):
        updated = False
        for u in range(n):
            if dist[u] == INF:
                continue
            for v in range(n):
                w = weights[u][v]
                if w != INF and dist[u] + w < dist[v]:
                    dist[v] = dist[u] + w
                    parent[v] = u
                    updated = True
        if not updated:
            break

    return dist, parent


def bfs_simple(adjacency: List[List[int]], start: int, end: int) -> Tuple[Optional[List[int]], Optional[int]]:
    from collections import deque

    n = len(adjacency)
    dist = [-1] * n
    parent: List[Optional[int]] = [None] * n
    q = deque([start])
    dist[start] = 0

    while q:
        u = q.popleft()
        if u == end:
            break
        for v in range(n):
            if adjacency[u][v] and dist[v] == -1:
                dist[v] = dist[u] + 1
                parent[v] = u
                q.append(v)

    if dist[end] == -1:
        return None, None

    path = []
    cur = end
    while cur is not None:
        path.append(cur)
        cur = parent[cur]
    path.reverse()
    return path, dist[end]


def format_matrix(matrix: List[List[float]], is_weighted: bool) -> str:
    lines = []
    for row in matrix:
        parts = []
        for v in row:
            if v == INF:
                parts.append("∞")
            else:
                parts.append(str(int(v)) if (not is_weighted or float(v).is_integer()) else f"{v}")
        lines.append("\t".join(parts))
    return "\n".join(lines)


def print_result_unweighted(title: str, adjacency: List[List[int]]) -> None:
    print("\n" + "-" * 70)
    print(title)
    g = DirectedGraph.from_adjacency(adjacency)
    start, end = 0, 6
    path, length = g.bfs_shortest_path(start, end)
    if path is None:
        print("Результат: шлях не існує")
    else:
        print(f"Найкоротший шлях (за кількістю дуг): {format_path(path)}; довжина = {length}")


def print_result_weighted(title: str, weights: List[List[float]]) -> None:
    print("\n" + "-" * 70)
    print(title)
    g = DirectedGraph(weights)
    start, end = 0, 6
    dist, parent = g.bellman_ford(start)
    if dist[end] == INF:
        print("Результат: шлях не існує")
        return
    path = []
    cur = end
    while cur is not None:
        path.append(cur)
        cur = parent[cur]
    path.reverse()
    print(f"Найкоротший шлях: {format_path(path)}; вага = {dist[end]}")


def main() -> None:
    adj_a = [
        [0, 0, 0, 1, 1, 0, 0],
        [1, 0, 1, 1, 0, 1, 1],
        [1, 1, 0, 1, 1, 0, 1],
        [0, 0, 0, 0, 1, 1, 0],
        [1, 0, 0, 1, 0, 1, 0],
        [0, 1, 1, 1, 0, 0, 0],
        [1, 1, 0, 0, 1, 1, 0],
    ]

    adj_b = [
        [0, 0, 0, 0, 1, 1, 0],
        [1, 0, 1, 0, 1, 0, 1],
        [1, 1, 0, 1, 1, 0, 0],
        [0, 0, 1, 0, 1, 1, 0],
        [1, 0, 0, 1, 0, 1, 0],
        [0, 0, 1, 0, 1, 0, 0],
        [1, 1, 0, 1, 1, 0, 0],
    ]

    adj_c = [
        [0, 0, 0, 1, 0, 1, 0],
        [1, 0, 1, 0, 1, 0, 1],
        [0, 1, 0, 1, 1, 1, 0],
        [1, 0, 0, 0, 1, 0, 0],
        [1, 0, 1, 1, 0, 1, 0],
        [0, 0, 0, 1, 1, 0, 0],
        [1, 0, 1, 0, 1, 1, 0],
    ]

    w_a = [
        [INF, INF, 5,   4,   2,   2,   9],
        [INF, INF, 1,   1,   INF, 1,   1],
        [2,   INF, INF, 1,   1,   INF, 3],
        [INF, 2,   1,   INF, 1,   INF, INF],
        [INF, INF, 2,   2,   INF, 1,   6],
        [1,   5,   INF, 1,   1,   INF, INF],
        [2,   INF, 1,   INF, 1,   2,   INF],
    ]

    w_b = [
        [INF, 4,   INF, INF, 5,   4,   INF],
        [9,   INF, 2,   1,   INF, INF, INF],
        [4,   4,   INF, INF, INF, INF, 3],
        [INF, 3,   1,   INF, 1,   INF, INF],
        [INF, INF, 2,   INF, INF, 4,   5],
        [INF, 3,   INF, 2,   2,   INF, INF],
        [INF, INF, 2,   INF, INF, 2,   INF],
    ]

    w_c = [
        [INF, INF, 9,   INF, INF, 2,   12],
        [1,   INF, INF, INF, 1,   2,   4],
        [2,   1,   INF, INF, 1,   INF, 2],
        [INF, 4,   1,   INF, INF, 1,   INF],
        [1,   2,   INF, 2,   INF, INF, INF],
        [INF, INF, INF, INF, 4,   INF, 5],
        [INF, 2,   1,   INF, 1,   2,   INF],
    ]

    print("ЛАБОРАТОРНА РОБОТА №5 — РОЗШИРЕНА ВЕРСІЯ")
    print("Пошук мінімального шляху з v1 у v7")

    print_result_unweighted("Варіант 1а (матриця суміжності)", adj_a)
    print_result_unweighted("Варіант 1б (матриця суміжності)", adj_b)
    print_result_unweighted("Варіант 1в (матриця суміжності)", adj_c)

    print_result_weighted("Варіант 2а (зважена матриця)", w_a)
    print_result_weighted("Варіант 2б (зважена матриця)", w_b)
    print_result_weighted("Варіант 2в (зважена матриця)", w_c)


def get_variants():
    adj_a = [
        [0, 0, 0, 1, 1, 0, 0],
        [1, 0, 1, 1, 0, 1, 1],
        [1, 1, 0, 1, 1, 0, 1],
        [0, 0, 0, 0, 1, 1, 0],
        [1, 0, 0, 1, 0, 1, 0],
        [0, 1, 1, 1, 0, 0, 0],
        [1, 1, 0, 0, 1, 1, 0],
    ]

    adj_b = [
        [0, 0, 0, 0, 1, 1, 0],
        [1, 0, 1, 0, 1, 0, 1],
        [1, 1, 0, 1, 1, 0, 0],
        [0, 0, 1, 0, 1, 1, 0],
        [1, 0, 0, 1, 0, 1, 0],
        [0, 0, 1, 0, 1, 0, 0],
        [1, 1, 0, 1, 1, 0, 0],
    ]

    adj_c = [
        [0, 0, 0, 1, 0, 1, 0],
        [1, 0, 1, 0, 1, 0, 1],
        [0, 1, 0, 1, 1, 1, 0],
        [1, 0, 0, 0, 1, 0, 0],
        [1, 0, 1, 1, 0, 1, 0],
        [0, 0, 0, 1, 1, 0, 0],
        [1, 0, 1, 0, 1, 1, 0],
    ]

    w_a = [
        [INF, INF, 5,   4,   2,   2,   9],
        [INF, INF, 1,   1,   INF, 1,   1],
        [2,   INF, INF, 1,   1,   INF, 3],
        [INF, 2,   1,   INF, 1,   INF, INF],
        [INF, INF, 2,   2,   INF, 1,   6],
        [1,   5,   INF, 1,   1,   INF, INF],
        [2,   INF, 1,   INF, 1,   2,   INF],
    ]

    w_b = [
        [INF, 4,   INF, INF, 5,   4,   INF],
        [9,   INF, 2,   1,   INF, INF, INF],
        [4,   4,   INF, INF, INF, INF, 3],
        [INF, 3,   1,   INF, 1,   INF, INF],
        [INF, INF, 2,   INF, INF, 4,   5],
        [INF, 3,   INF, 2,   2,   INF, INF],
        [INF, INF, 2,   INF, INF, 2,   INF],
    ]

    w_c = [
        [INF, INF, 9,   INF, INF, 2,   12],
        [1,   INF, INF, INF, 1,   2,   4],
        [2,   1,   INF, INF, 1,   INF, 2],
        [INF, 4,   1,   INF, INF, 1,   INF],
        [1,   2,   INF, 2,   INF, INF, INF],
        [INF, INF, INF, INF, 4,   INF, 5],
        [INF, 2,   1,   INF, 1,   2,   INF],
    ]

    return {
        "1a": (False, adj_a),
        "1b": (False, adj_b),
        "1c": (False, adj_c),
        "2a": (True, w_a),
        "2b": (True, w_b),
        "2c": (True, w_c),
    }


def draw_graph(canvas: tk.Canvas, matrix: List[List[float]], is_weighted: bool, highlight: Optional[List[int]] = None):
    canvas.delete("all")
    n = 7
    w = canvas.winfo_width()
    h = canvas.winfo_height()
    cx, cy = w // 2, h // 2
    radius = min(w, h) // 3
    coords = []
    for i in range(n):
        angle = 2 * math.pi * i / n - math.pi / 2
        x = cx + radius * math.cos(angle)
        y = cy + radius * math.sin(angle)
        coords.append((x, y))

    node_r = 18
    highlight_edges = set()
    if highlight and len(highlight) >= 2:
        for a, b in zip(highlight, highlight[1:]):
            highlight_edges.add((a, b))

    for u in range(n):
        for v in range(n):
            if matrix[u][v] == INF:
                continue
            x1, y1 = coords[u]
            x2, y2 = coords[v]
            dx = x2 - x1
            dy = y2 - y1
            dist = math.hypot(dx, dy) or 1.0
            sx = x1 + (node_r / dist) * dx
            sy = y1 + (node_r / dist) * dy
            ex = x2 - (node_r / dist) * dx
            ey = y2 - (node_r / dist) * dy
            is_high = (u, v) in highlight_edges
            color = "red" if is_high else "black"
            width = 3 if is_high else 1
            canvas.create_line(sx, sy, ex, ey, arrow=tk.LAST, fill=color, width=width)
            if is_weighted:
                mx = (sx + ex) / 2
                my = (sy + ey) / 2
                wt = matrix[u][v]
                canvas.create_text(mx, my - 10, text=str(wt), fill=color, font=(None, 9))

    for i, (x, y) in enumerate(coords):
        canvas.create_oval(x - node_r, y - node_r, x + node_r, y + node_r, fill="lightblue", outline="black")
        canvas.create_text(x, y, text=f"v{i+1}")


def gui_main() -> None:
    root = tk.Tk()
    root.title("Лаба5")
    root.geometry("800x520")

    variants = get_variants()
    var_names = list(variants.keys())

    top_frame = tk.Frame(root)
    top_frame.pack(side=tk.TOP, fill=tk.X, padx=8, pady=6)

    tk.Label(top_frame, text="Варіант:").pack(side=tk.LEFT)
    sel = tk.StringVar(value=var_names[0])
    opt = tk.OptionMenu(top_frame, sel, *var_names)
    opt.pack(side=tk.LEFT, padx=6)

    middle_frame = tk.Frame(root)
    middle_frame.pack(fill=tk.BOTH, expand=True, padx=8, pady=6)

    canvas = tk.Canvas(middle_frame, bg="white", width=560, height=420)
    canvas.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

    matrix_frame = tk.Frame(middle_frame)
    matrix_frame.pack(side=tk.RIGHT, fill=tk.Y)
    tk.Label(matrix_frame, text=" матриця:").pack(anchor=tk.N)
    matrix_text = tk.Text(matrix_frame, width=28, height=22, wrap=tk.NONE)
    matrix_text.pack(side=tk.TOP, padx=4)
    matrix_text.configure(state=tk.DISABLED)

    bottom_frame = tk.Frame(root)
    bottom_frame.pack(side=tk.BOTTOM, fill=tk.X, padx=8, pady=6)

    result_lbl = tk.Label(bottom_frame, text='Оберіть варіант та натисніть "Показати"')
    result_lbl.pack(side=tk.LEFT)

    def on_show():
        key = sel.get()
        is_weighted, mat = variants[key]
        matrix_text.configure(state=tk.NORMAL)
        matrix_text.delete("1.0", tk.END)
        matrix_text.insert(tk.END, format_matrix(mat, is_weighted))
        matrix_text.configure(state=tk.DISABLED)

        if not is_weighted:
            path, length = bfs_simple(mat, 0, 6)
            wmat = adjacency_to_weight_simple(mat)
            draw_graph(canvas, wmat, False, highlight=path)
            if path is None:
                result_lbl.config(text=f"{key}: шлях не існує")
            else:
                result_lbl.config(text=f"{key}: {format_path(path)} (довжина={length})")
        else:
            dist, parent = bellman_ford_simple(mat, 0)
            if dist[6] == INF:
                draw_graph(canvas, mat, True, highlight=None)
                result_lbl.config(text=f"{key}: шлях не існує")
            else:
                p = []
                cur = 6
                while cur is not None:
                    p.append(cur)
                    cur = parent[cur]
                p.reverse()
                draw_graph(canvas, mat, True, highlight=p)
                result_lbl.config(text=f"{key}: {format_path(p)} (вага={dist[6]})")

    show_btn = tk.Button(bottom_frame, text="Показати", command=on_show)
    show_btn.pack(side=tk.RIGHT)

    on_show()
    root.mainloop()


if __name__ == "__main__":
    gui_main()
