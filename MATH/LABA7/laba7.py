import tkinter as tk
from tkinter import ttk, messagebox
import random

INF = float('inf')

# Default 4x4 example: arc x3→x2 is backward → demonstrates topological sort
DEFAULT_MATRIX = """\
0 3 8 0
0 0 0 4
0 2 0 5
0 0 0 0"""


# ── Algorithms ───────────────────────────────────────────────────────────────

def topological_sort(adj):
    """
    Textbook algorithm (Lab 7 PDF):
      i = n
      find xk with |Γ(xk)| = 0  →  assign number i  →  remove xk  →  i -= 1
    Returns new_order where new_order[k] = orig_idx of vertex numbered k+1.
    Returns None if a cycle is detected.
    """
    n = len(adj)
    out_deg = [sum(1 for j in range(n) if adj[i][j] > 0) for i in range(n)]
    remaining = list(range(n))
    order_desc = []  # first appended → gets highest number n

    while remaining:
        found = None
        for k in remaining:
            if out_deg[k] == 0:
                found = k
                break
        if found is None:
            return None  # cycle detected
        order_desc.append(found)
        remaining.remove(found)
        # Remove arcs coming into 'found' from the remaining graph
        for i in remaining:
            if adj[i][found] > 0:
                out_deg[i] -= 1

    # order_desc[0] gets number n, order_desc[-1] gets number 1
    return list(reversed(order_desc))  # new_order[k] = orig_idx of vertex k+1


def dp_shortest_path(adj, s, t, topo_order):
    """
    Forward DP iteration on DAG (formula 3 from PDF):
      λ(xj) = min over xi ∈ Γ⁻¹(xj) of [ λ(xi) + c_ij ]
    Γ⁻¹(xj) is found via the j-th COLUMN of the adjacency matrix.
    Returns (lam, path):
      lam[orig_idx]  = λ label for that vertex
      path           = list of orig indices from s to t, or None if unreachable
    """
    n = len(adj)
    lam = [INF] * n
    lam[s] = 0

    for new_j in range(n):
        j = topo_order[new_j]
        if j == s:
            continue
        # Scan column j to find predecessors
        for i in range(n):
            if adj[i][j] > 0 and lam[i] < INF:
                candidate = lam[i] + adj[i][j]
                if candidate < lam[j]:
                    lam[j] = candidate

    if lam[t] == INF:
        return lam, None

    # Back-trace (formula 4): arc (xi,xj) is on path iff λ(xj) = λ(xi) + c_ij
    path = [t]
    current = t
    while current != s:
        found_prev = None
        for i in range(n):
            if adj[i][current] > 0 and abs(lam[i] + adj[i][current] - lam[current]) < 1e-9:
                found_prev = i
                break
        if found_prev is None:
            return lam, None
        path.append(found_prev)
        current = found_prev

    path.reverse()
    return lam, path


# ── Utilities ─────────────────────────────────────────────────────────────────

def parse_matrix(text):
    rows = []
    for line in text.strip().splitlines():
        line = line.strip()
        if not line:
            continue
        try:
            row = [float(x) for x in line.split()]
        except ValueError:
            return None
        rows.append(row)
    if not rows:
        return None
    n = len(rows)
    if any(len(r) != n for r in rows):
        return None
    return rows


def random_dag(n, max_weight=9):
    adj = [[0] * n for _ in range(n)]
    # Random forward arcs (i < j guarantees DAG structure)
    for i in range(n):
        for j in range(i + 1, n):
            if random.random() < 0.4:
                adj[i][j] = random.randint(1, max_weight)
    # Ensure at least one valid path from 0 to n-1
    for k in range(n - 1):
        if not any(adj[k][j] > 0 for j in range(k + 1, n)):
            adj[k][k + 1] = random.randint(1, max_weight)
    return adj


def fmt(v):
    if v == INF:
        return "∞"
    return str(int(v)) if v == int(v) else str(v)


# ── GUI ───────────────────────────────────────────────────────────────────────

def build_gui():
    root = tk.Tk()
    root.title("Лаб. №7 — Найкоротший шлях (динамічне програмування)")
    root.resizable(True, True)

    FONT = ("Consolas", 10)
    BOLD = ("Consolas", 10, "bold")
    PAD = 8

    # ── Left panel ───────────────────────────────────────────────────────────
    left = tk.Frame(root, padx=PAD, pady=PAD)
    left.pack(side=tk.LEFT, fill=tk.Y)

    tk.Label(left, text="Початкова вершина s:", font=FONT).grid(row=0, column=0, sticky="w")
    sv_var = tk.StringVar(value="1")
    tk.Entry(left, textvariable=sv_var, width=5, font=FONT).grid(
        row=0, column=1, sticky="w", padx=(4, 0))

    tk.Label(left, text="Кінцева вершина t:", font=FONT).grid(
        row=1, column=0, sticky="w", pady=(4, 0))
    tv_var = tk.StringVar(value="4")
    tk.Entry(left, textvariable=tv_var, width=5, font=FONT).grid(
        row=1, column=1, sticky="w", padx=(4, 0), pady=(4, 0))

    ttk.Separator(left, orient="horizontal").grid(
        row=2, column=0, columnspan=2, sticky="ew", pady=PAD)

    tk.Label(left, text="n вершин =", font=FONT).grid(row=3, column=0, sticky="w")
    n_var = tk.StringVar(value="7")
    tk.Entry(left, textvariable=n_var, width=5, font=FONT).grid(
        row=3, column=1, sticky="w", padx=(4, 0))

    gen_btn = tk.Button(left, text="Генерувати", font=FONT)
    gen_btn.grid(row=4, column=0, columnspan=2, pady=4, sticky="ew")

    ttk.Separator(left, orient="horizontal").grid(
        row=5, column=0, columnspan=2, sticky="ew", pady=PAD)

    tk.Label(left, text="Матриця суміжності:", font=BOLD).grid(
        row=6, column=0, columnspan=2, sticky="w")
    mat_in = tk.Text(left, width=36, height=12, font=FONT)
    mat_in.grid(row=7, column=0, columnspan=2, pady=4)
    mat_in.insert(tk.END, DEFAULT_MATRIX)

    solve_btn = tk.Button(left, text="Знайти найкоротший шлях", font=BOLD)
    solve_btn.grid(row=8, column=0, columnspan=2, pady=4, sticky="ew")

    # ── Right panel ──────────────────────────────────────────────────────────
    right = tk.Frame(root, padx=PAD, pady=PAD)
    right.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

    tk.Label(right, text="Результат:", font=BOLD).pack(anchor="w")
    result = tk.Text(right, width=58, height=32, font=FONT,
                     state="disabled", bg="#f8f8f8", relief="flat", wrap="none")
    sb_y = ttk.Scrollbar(right, orient="vertical", command=result.yview)
    result.configure(yscrollcommand=sb_y.set)
    sb_y.pack(side=tk.RIGHT, fill=tk.Y)
    result.pack(fill=tk.BOTH, expand=True)

    def set_result(text):
        result.config(state="normal")
        result.delete("1.0", tk.END)
        result.insert(tk.END, text)
        result.config(state="disabled")

    # ── Callbacks ────────────────────────────────────────────────────────────

    def do_generate():
        try:
            n = int(n_var.get())
            assert 2 <= n <= 15
        except Exception:
            messagebox.showerror("Помилка", "n має бути цілим числом від 2 до 15")
            return
        adj = random_dag(n)
        mat_in.delete("1.0", tk.END)
        for row in adj:
            mat_in.insert(tk.END, " ".join(str(v) for v in row) + "\n")
        sv_var.set("1")
        tv_var.set(str(n))
        set_result("Матрицю згенеровано.\nНатисніть «Знайти найкоротший шлях».")

    def do_solve():
        raw = mat_in.get("1.0", tk.END)
        adj = parse_matrix(raw)
        if adj is None:
            messagebox.showerror("Помилка", "Неправильний формат матриці суміжності!")
            return
        n = len(adj)
        try:
            s = int(sv_var.get()) - 1
            t = int(tv_var.get()) - 1
            assert 0 <= s < n and 0 <= t < n
        except Exception:
            messagebox.showerror(
                "Помилка", f"Вершини s і t мають бути цілими числами від 1 до {n}")
            return
        if s == t:
            set_result("Початкова і кінцева вершини співпадають.")
            return

        topo = topological_sort(adj)
        lines = []

        if topo is None:
            lines.append("ПОМИЛКА: Граф містить цикл!")
            lines.append("Алгоритм динамічного програмування не застосовується.")
            set_result("\n".join(lines))
            return

        # ── Topological sort output ──────────────────────────────────────────
        orig_to_new = {orig: k + 1 for k, orig in enumerate(topo)}

        lines.append("1. Топологічне сортування:")
        lines.append("   Порядок виключення вершин (від n до 1):")
        lines.append("   " + " → ".join(f"x{o+1}" for o in reversed(topo)))
        lines.append("   Нова нумерація:")
        lines.append("   " + ", ".join(f"x{o+1}→{orig_to_new[o]}" for o in range(n)))
        lines.append("")

        # ── DP output ────────────────────────────────────────────────────────
        lam, path = dp_shortest_path(adj, s, t, topo)

        lines.append(f"2. Мітки λ(xi)  [s = x{s+1},  t = x{t+1}]:")
        for i in range(n):
            lines.append(f"   λ(x{i+1}) = {fmt(lam[i])}")
        lines.append("")

        if path is None:
            lines.append(f"Шлях від x{s+1} до x{t+1} не існує!")
        else:
            path_str = " → ".join(f"x{v+1}" for v in path)
            lines.append("3. Найкоротший шлях:")
            lines.append(f"   μ(x{s+1}, x{t+1}) = {{ {path_str} }}")
            lines.append(f"   L(μ) = {fmt(lam[t])}")
            lines.append("")
            lines.append("4. Перевірка (вираз 4):")
            for k in range(len(path) - 1):
                xi, xj = path[k], path[k + 1]
                c = adj[xi][xj]
                lines.append(
                    f"   λ(x{xj+1}) = λ(x{xi+1}) + c{xi+1}{xj+1}"
                    f"  ({fmt(lam[xj])} = {fmt(lam[xi])} + {fmt(c)})"
                )

        set_result("\n".join(lines))

    gen_btn.config(command=do_generate)
    solve_btn.config(command=do_solve)

    root.mainloop()


if __name__ == "__main__":
    build_gui()
