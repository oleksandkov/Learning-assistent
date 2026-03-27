import random
import tkinter as tk
from tkinter import messagebox


INF = float("inf")


def parse_matrix(text):
    rows = []
    for line in text.strip().splitlines():
        line = line.strip()
        if not line:
            continue
        parts = line.split()
        row = []
        for value in parts:
            try:
                row.append(float(value))
            except ValueError:
                return None
        rows.append(row)
    if not rows:
        return None
    n = len(rows)
    if any(len(row) != n for row in rows):
        return None
    return rows


def matrix_to_text(matrix):
    lines = []
    for row in matrix:
        values = []
        for v in row:
            if abs(v - int(v)) < 1e-9:
                values.append(str(int(v)))
            else:
                values.append(str(v))
        lines.append(" ".join(values))
    return "\n".join(lines)


def random_dag(n, p=0.4, max_weight=9):
    g = [[0] * n for _ in range(n)]
    for i in range(n):
        for j in range(i + 1, n):
            if random.random() < p:
                g[i][j] = random.randint(1, max_weight)
    for i in range(n - 1):
        if not any(g[i][j] > 0 for j in range(i + 1, n)):
            g[i][i + 1] = random.randint(1, max_weight)
    return g


def renumber_vertices(matrix):
    n = len(matrix)
    perm = list(range(n))
    random.shuffle(perm)
    inv = [0] * n
    for new_i, old_i in enumerate(perm):
        inv[old_i] = new_i
    new_m = [[0] * n for _ in range(n)]
    for new_i in range(n):
        for new_j in range(n):
            old_i = perm[new_i]
            old_j = perm[new_j]
            new_m[new_i][new_j] = matrix[old_i][old_j]
    return new_m, perm, inv


def topo_sort(matrix):
    n = len(matrix)
    out_deg = [sum(1 for j in range(n) if matrix[i][j] > 0) for i in range(n)]
    remaining = set(range(n))
    order_desc = []
    while remaining:
        sinks = [v for v in remaining if out_deg[v] == 0]
        if not sinks:
            return None
        xk = random.choice(sinks)
        order_desc.append(xk)
        remaining.remove(xk)
        for i in list(remaining):
            if matrix[i][xk] > 0:
                out_deg[i] -= 1
    order = list(reversed(order_desc))
    return order


def shortest_path_dp(matrix, s, t, order):
    n = len(matrix)
    lam = [INF] * n
    lam[s] = 0

    for j in order:
        if j == s:
            continue
        for i in range(n):
            if matrix[i][j] > 0 and lam[i] < INF:
                cand = lam[i] + matrix[i][j]
                if cand < lam[j]:
                    lam[j] = cand

    if lam[t] == INF:
        return lam, None

    path = [t]
    cur = t
    while cur != s:
        prev = None
        for i in range(n):
            if matrix[i][cur] > 0 and abs((lam[i] + matrix[i][cur]) - lam[cur]) < 1e-9:
                prev = i
                break
        if prev is None:
            return lam, None
        path.append(prev)
        cur = prev
    path.reverse()
    return lam, path


def fmt_num(v):
    if v == INF:
        return "∞"
    if abs(v - int(v)) < 1e-9:
        return str(int(v))
    return str(v)


def build_app():
    root = tk.Tk()
    root.title("Лабораторна 7: найкоротший шлях у DAG")
    root.geometry("1000x650")

    left = tk.Frame(root, padx=10, pady=10)
    left.pack(side=tk.LEFT, fill=tk.Y)
    right = tk.Frame(root, padx=10, pady=10)
    right.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

    tk.Label(left, text="Початкова вершина s:").grid(row=0, column=0, sticky="w")
    s_var = tk.StringVar(value="1")
    tk.Entry(left, textvariable=s_var, width=6).grid(row=0, column=1, sticky="w", padx=5)

    tk.Label(left, text="Кінцева вершина t:").grid(row=1, column=0, sticky="w", pady=(6, 0))
    t_var = tk.StringVar(value="4")
    tk.Entry(left, textvariable=t_var, width=6).grid(row=1, column=1, sticky="w", padx=5, pady=(6, 0))

    tk.Label(left, text="Кількість вершин n:").grid(row=2, column=0, sticky="w", pady=(12, 0))
    n_var = tk.StringVar(value="7")
    tk.Entry(left, textvariable=n_var, width=6).grid(row=2, column=1, sticky="w", padx=5, pady=(12, 0))

    tk.Label(left, text="Матриця суміжності:").grid(row=3, column=0, columnspan=2, sticky="w", pady=(12, 0))
    matrix_text = tk.Text(left, width=42, height=18)
    matrix_text.grid(row=4, column=0, columnspan=2, pady=6)

    default = [
        [0, 3, 8, 0],
        [0, 0, 0, 4],
        [0, 2, 0, 5],
        [0, 0, 0, 0],
    ]
    matrix_text.insert("1.0", matrix_to_text(default))

    out = tk.Text(right, wrap="word", bg="#f7f7f7")
    out.pack(fill=tk.BOTH, expand=True)

    def set_out(lines):
        out.config(state="normal")
        out.delete("1.0", tk.END)
        out.insert("1.0", "\n".join(lines))
        out.config(state="disabled")

    def on_generate():
        try:
            n = int(n_var.get())
            if n < 2 or n > 20:
                raise ValueError
        except ValueError:
            messagebox.showerror("Помилка", "n має бути цілим числом від 2 до 20")
            return
        g = random_dag(n)
        matrix_text.delete("1.0", tk.END)
        matrix_text.insert("1.0", matrix_to_text(g))
        s_var.set("1")
        t_var.set(str(n))
        set_out(["Матрицю згенеровано.", "Натисніть кнопку розв'язання."])

    def on_renumber():
        matrix = parse_matrix(matrix_text.get("1.0", tk.END))
        if matrix is None:
            messagebox.showerror("Помилка", "Введіть коректну квадратну матрицю суміжності")
            return
        new_m, perm, inv = renumber_vertices(matrix)
        matrix_text.delete("1.0", tk.END)
        matrix_text.insert("1.0", matrix_to_text(new_m))
        try:
            s = int(s_var.get()) - 1
            t = int(t_var.get()) - 1
            n = len(matrix)
            if 0 <= s < n and 0 <= t < n:
                s_var.set(str(inv[s] + 1))
                t_var.set(str(inv[t] + 1))
        except ValueError:
            pass
        lines = ["Виконано випадкову нумерацію вершин:"]
        for new_i, old_i in enumerate(perm):
            lines.append(f"x{old_i + 1} -> x{new_i + 1}")
        set_out(lines)

    def on_solve():
        matrix = parse_matrix(matrix_text.get("1.0", tk.END))
        if matrix is None:
            messagebox.showerror("Помилка", "Введіть коректну квадратну матрицю суміжності")
            return

        n = len(matrix)
        try:
            s = int(s_var.get()) - 1
            t = int(t_var.get()) - 1
            if not (0 <= s < n and 0 <= t < n):
                raise ValueError
        except ValueError:
            messagebox.showerror("Помилка", f"s і t мають бути від 1 до {n}")
            return

        order = topo_sort(matrix)
        if order is None:
            set_out([
                "Граф містить цикл.",
                "Топологічне сортування і метод динамічного програмування для DAG не застосовні.",
            ])
            return

        lam, path = shortest_path_dp(matrix, s, t, order)
        lines = []
        lines.append("Топологічний порядок вершин:")
        lines.append(" -> ".join(f"x{v + 1}" for v in order))
        lines.append("")
        lines.append("Мітки λ(xi):")
        for i, value in enumerate(lam):
            lines.append(f"λ(x{i + 1}) = {fmt_num(value)}")
        lines.append("")

        if path is None:
            lines.append(f"Шлях від x{s + 1} до x{t + 1} не існує.")
        else:
            lines.append("Найкоротший шлях:")
            lines.append(" -> ".join(f"x{v + 1}" for v in path))
            lines.append(f"Довжина шляху: {fmt_num(lam[t])}")

        set_out(lines)

    btn_frame = tk.Frame(left)
    btn_frame.grid(row=5, column=0, columnspan=2, sticky="ew", pady=8)

    tk.Button(btn_frame, text="Згенерувати матрицю", command=on_generate).pack(fill=tk.X, pady=2)
    tk.Button(btn_frame, text="Випадкова нумерація вершин", command=on_renumber).pack(fill=tk.X, pady=2)
    tk.Button(btn_frame, text="Знайти найкоротший шлях", command=on_solve).pack(fill=tk.X, pady=2)

    set_out([
        "Введіть матрицю або згенеруйте її.",
        "Потім задайте s і t та натисніть кнопку розв'язання.",
    ])

    root.mainloop()


if __name__ == "__main__":
    build_app()