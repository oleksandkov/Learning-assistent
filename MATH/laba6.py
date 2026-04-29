

from typing import List
import random
import tkinter as tk
from tkinter import ttk, messagebox



def adj_to_inc(adj: List[List[int]], gtype: str) -> List[List[int]]:
    """Матриця суміжності → матриця інцидентності."""
    n = len(adj)
    cols: List[List[int]] = []

    if gtype == 'directed':
        for i in range(n):
            for j in range(n):
                if i == j and adj[i][j]:
                    col = [0]*n; col[i] = 1; cols.append(col)
                elif i != j and adj[i][j]:
                    col = [0]*n; col[i] = 1; col[j] = -1; cols.append(col)

    elif gtype == 'undirected':
        for i in range(n):
            if adj[i][i]:
                col = [0]*n; col[i] = 2; cols.append(col)
        for i in range(n):
            for j in range(i+1, n):
                if adj[i][j]:
                    col = [0]*n; col[i] = 1; col[j] = 1; cols.append(col)

    elif gtype == 'mixed':
        for i in range(n):
            if adj[i][i]:
                col = [0]*n; col[i] = 2; cols.append(col)
        for i in range(n):
            for j in range(i+1, n):
                a, b = adj[i][j], adj[j][i]
                if a and b:
                    col = [0]*n; col[i] = 1; col[j] = 1; cols.append(col)
                elif a:
                    col = [0]*n; col[i] = 1; col[j] = -1; cols.append(col)
                elif b:
                    col = [0]*n; col[j] = 1; col[i] = -1; cols.append(col)
    else:
        raise ValueError(f'Невідомий тип графа: {gtype}')

    if not cols:
        return [[] for _ in range(n)]

    m = len(cols)
    inc = [[0]*m for _ in range(n)]
    for j, col in enumerate(cols):
        for i in range(n):
            inc[i][j] = col[i]
    return inc


def inc_to_adj(inc: List[List[int]]) -> List[List[int]]:
    """Матриця інцидентності → матриця суміжності."""
    n = len(inc)
    if not n or not inc[0]:
        return [[0]*n for _ in range(n)] if n else []
    m = len(inc[0])
    adj = [[0]*n for _ in range(n)]
    for j in range(m):
        col = [inc[i][j] for i in range(n)]
        nz = [(i, v) for i, v in enumerate(col) if v != 0]
        if len(nz) == 1:
            adj[nz[0][0]][nz[0][0]] = 1
        elif len(nz) == 2:
            (i1, v1), (i2, v2) = nz
            if v1 == 1 and v2 == 1:
                adj[i1][i2] = adj[i2][i1] = 1
            elif v1 == 1 and v2 == -1:
                adj[i1][i2] = 1
            elif v1 == -1 and v2 == 1:
                adj[i2][i1] = 1
            else:
                adj[i1][i2] = adj[i2][i1] = 1
    return adj


# ── Допоміжні функції ────────────────────────────────────────────────────────

def parse_matrix(text: str) -> List[List[int]]:
    rows = []
    for idx, line in enumerate(text.strip().splitlines(), 1):
        line = line.strip()
        if not line:
            continue
        parts = line.replace(',', ' ').split()
        try:
            rows.append([int(x) for x in parts])
        except ValueError:
            raise ValueError(f'Рядок {idx}: очікуються цілі числа — "{line}"')
    if not rows:
        raise ValueError('Порожній ввід — введіть матрицю у текстове поле')
    L = len(rows[0])
    for idx, r in enumerate(rows, 1):
        if len(r) != L:
            raise ValueError(f'Рядок {idx} має {len(r)} значень, очікується {L}')
    return rows


def mat_to_str(mat: List[List[int]], rpfx: str = 'x', cpfx: str = 'a') -> str:
    """Форматування матриці з заголовками рядків і стовпців."""
    if not mat or not mat[0]:
        return '(порожня матриця)'
    n, m = len(mat), len(mat[0])
    val_w   = max((len(str(v)) for row in mat for v in row), default=1)
    label_w = max(len(f'{cpfx}{j+1}') for j in range(m))
    w = max(val_w, label_w) + 1
    hdr = ' ' * 5 + ''.join(f'{cpfx+str(j+1):>{w}}' for j in range(m))
    lines = [hdr]
    for i, row in enumerate(mat):
        lines.append(f'{rpfx}{i+1:<4}' + ''.join(f'{v:>{w}}' for v in row))
    return '\n'.join(lines)


def graph_stats(adj: List[List[int]], gtype: str) -> str:
    """Статистика графа за матрицею суміжності."""
    n = len(adj)
    if not n:
        return 'Граф порожній'
    loops = sum(1 for i in range(n) if adj[i][i])
    if gtype == 'undirected':
        edges = sum(adj[i][j] for i in range(n) for j in range(i+1, n))
    else:
        edges = sum(adj[i][j] for i in range(n) for j in range(n) if i != j)

    lines = ['Аналіз графа', '─' * 35]
    lines.append(f'Вершин (n): {n}   Ребер/дуг: {edges}   Петель: {loops}')
    lines.append('─' * 35)
    lines.append('Степені вершин:')

    for i in range(n):
        lp = ' ⟲' if adj[i][i] else ''
        if gtype == 'directed':
            out_d = sum(adj[i][j] for j in range(n) if j != i)
            in_d  = sum(adj[k][i] for k in range(n) if k != i)
            G      = [f'x{j+1}' for j in range(n) if j != i and adj[i][j]]
            G_inv  = [f'x{k+1}' for k in range(n) if k != i and adj[k][i]]
            lines.append(f'  x{i+1}{lp}: вихід={out_d} Г={G}')
            lines.append(f'       захід={in_d} Г⁻¹={G_inv}')
        else:
            deg  = sum(adj[i][j] for j in range(n) if j != i)
            nbrs = [f'x{j+1}' for j in range(n) if j != i and adj[i][j]]
            lines.append(f'  x{i+1}{lp}: deg={deg}  Г={nbrs}')

    if gtype == 'undirected' and edges > 0:
        total_deg = sum(sum(adj[i][j] for j in range(n) if j != i) for i in range(n))
        lines.append('─' * 35)
        lines.append(f'Сума степенів = {total_deg} = 2 × {edges} ✓')
    return '\n'.join(lines)


def random_adj(n: int, directed: bool, density: float = 0.4) -> List[List[int]]:
    adj = [[0]*n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            if i == j:
                continue
            if directed:
                if random.random() < density:
                    adj[i][j] = 1
            elif j > i and random.random() < density:
                adj[i][j] = adj[j][i] = 1
    return adj


# ── Готові приклади (з підручника) ──────────────────────────────────────────

EXAMPLES = {
    'Орієнт. 3×3 (цикл)': {
        'gtype': 'directed',
        'adj': [[0,1,0],[0,0,1],[1,0,0]],
    },
    'Неорієнт. 4×4': {
        'gtype': 'undirected',
        'adj': [[0,1,1,0],[1,0,0,1],[1,0,0,0],[0,1,0,0]],
    },
    'Орієнт. 6×6 (рис.6.3)': {
        'gtype': 'directed',
        'adj': [
            [0, 1, 1, 0, 0, 0],
            [0, 1, 0, 0, 1, 0],
            [0, 0, 0, 0, 0, 0],
            [0, 0, 1, 0, 0, 0],
            [1, 0, 0, 1, 0, 0],
            [1, 0, 0, 0, 1, 1],
        ],
    },
    'Неорієнт. 6×6 (рис.6.4)': {
        'gtype': 'undirected',
        'adj': [
            [0, 1, 1, 0, 0, 1],
            [1, 0, 1, 0, 0, 0],
            [1, 1, 0, 1, 0, 0],
            [0, 0, 1, 0, 1, 1],
            [0, 0, 0, 1, 0, 1],
            [1, 0, 0, 1, 1, 0],
        ],
    },
}




# ── GUI ─────────────────────────────────────────────────────────────────────

def build_gui():
    root = tk.Tk()
    root.title('Лабораторна №6 — Матричні представлення графів')
    root.minsize(760, 520)

    tab_conv = ttk.Frame(root, padding=6)
    tab_conv.pack(fill='both', expand=True)
    tab_conv.columnconfigure(0, weight=1)
    tab_conv.columnconfigure(1, weight=2)
    tab_conv.rowconfigure(1, weight=1)

    # ── ЛІВА ПАНЕЛЬ: параметри та ввід ───────────────────────────────────────
    left = ttk.Frame(tab_conv)
    left.grid(row=0, column=0, rowspan=3, sticky='nsew', padx=(0, 6))
    left.columnconfigure(0, weight=1)

    # --- 1. Тип графа ---
    lf1 = ttk.LabelFrame(left, text=' Тип графа ', padding=6)
    lf1.grid(row=0, column=0, sticky='ew', pady=(0, 4))

    gtype_var = tk.StringVar(value='directed')
    for val, lbl in [
        ('directed',   'Орієнтований'),
        ('undirected', 'Неорієнтований'),
        ('mixed',      'Змішаний'),
    ]:
        ttk.Radiobutton(lf1, text=lbl, variable=gtype_var, value=val).pack(anchor='w')

    # --- 2. Напрямок конвертації ---
    lf2 = ttk.LabelFrame(left, text=' Конвертувати ', padding=6)
    lf2.grid(row=1, column=0, sticky='ew', pady=(0, 4))

    direction_var = tk.StringVar(value='adj2inc')
    ttk.Radiobutton(
        lf2, text='Суміжності  →  Інцидентності',
        variable=direction_var, value='adj2inc',
    ).pack(anchor='w')
    ttk.Radiobutton(
        lf2, text='Інцидентності  →  Суміжності',
        variable=direction_var, value='inc2adj',
    ).pack(anchor='w')

    # --- 3. Генератор ---
    lf3 = ttk.LabelFrame(left, text=' Випадковий граф ', padding=6)
    lf3.grid(row=2, column=0, sticky='ew', pady=(0, 4))

    rng_row = ttk.Frame(lf3)
    rng_row.pack(fill='x')
    ttk.Label(rng_row, text='n вершин =').pack(side='left')
    n_var = tk.StringVar(value='4')
    ttk.Entry(rng_row, textvariable=n_var, width=3).pack(side='left', padx=4)
    btn_rand = ttk.Button(rng_row, text='Генерувати')
    btn_rand.pack(side='left', padx=6)

    # --- 4. Введення матриці ---
    lf4 = ttk.LabelFrame(left, text=' Матриця ', padding=4)
    lf4.grid(row=3, column=0, sticky='nsew', pady=(0, 4))
    lf4.columnconfigure(0, weight=1)
    lf4.rowconfigure(0, weight=1)
    left.rowconfigure(3, weight=1)

    txt_in = tk.Text(lf4, width=30, height=10, font=('Courier New', 10))
    scr_in = ttk.Scrollbar(lf4, command=txt_in.yview)
    txt_in.configure(yscrollcommand=scr_in.set)
    txt_in.grid(row=0, column=0, sticky='nsew')
    scr_in.grid(row=0, column=1, sticky='ns')

    # --- Кнопка Конвертувати ---
    btn_convert = ttk.Button(left, text='  ▶  Конвертувати  ')
    btn_convert.grid(row=4, column=0, pady=6, sticky='ew')

    # ── ПРАВА ПАНЕЛЬ: вивід ──────────────────────────────────────────────────
    right = ttk.Frame(tab_conv)
    right.grid(row=0, column=1, rowspan=3, sticky='nsew')
    right.columnconfigure(0, weight=1)
    right.rowconfigure(1, weight=2)
    right.rowconfigure(3, weight=1)

    ttk.Label(right, text='Результат конвертації:', font=('Arial', 9, 'bold')).grid(
        row=0, column=0, sticky='w', pady=(0, 2))

    txt_out = tk.Text(right, width=42, height=14, font=('Courier New', 10),
                      state='disabled', bg='#f8f9fa')
    scr_out = ttk.Scrollbar(right, command=txt_out.yview)
    txt_out.configure(yscrollcommand=scr_out.set)
    txt_out.grid(row=1, column=0, sticky='nsew')
    scr_out.grid(row=1, column=1, sticky='ns')

    ttk.Label(right, text='Аналіз графа:', font=('Arial', 9, 'bold')).grid(
        row=2, column=0, sticky='w', pady=(6, 2))

    txt_stats = tk.Text(right, width=42, height=8, font=('Courier New', 9),
                        state='disabled', bg='#eef2f7')
    scr_st = ttk.Scrollbar(right, command=txt_stats.yview)
    txt_stats.configure(yscrollcommand=scr_st.set)
    txt_stats.grid(row=3, column=0, sticky='nsew')
    scr_st.grid(row=3, column=1, sticky='ns')


    # ── Функції ──────────────────────────────────────────────────────────────

    def _set(widget, text):
        widget.configure(state='normal')
        widget.delete('1.0', tk.END)
        widget.insert(tk.END, text)
        widget.configure(state='disabled')

    def adj_rows_to_text(adj):
        return '\n'.join(' '.join(str(v) for v in row) for row in adj)

    def gen_random():
        try:
            n = int(n_var.get())
            if not (2 <= n <= 15):
                raise ValueError
        except ValueError:
            messagebox.showerror('Помилка', 'n — ціле від 2 до 15')
            return
        directed = (gtype_var.get() != 'undirected')
        adj = random_adj(n, directed, density=0.4)
        if direction_var.get() == 'adj2inc':
            data = adj_rows_to_text(adj)
        else:
            inc  = adj_to_inc(adj, gtype_var.get())
            data = adj_rows_to_text(inc) if inc and inc[0] else ''
        txt_in.delete('1.0', tk.END)
        txt_in.insert(tk.END, data)

    def do_convert():
        raw = txt_in.get('1.0', tk.END)
        try:
            mat = parse_matrix(raw)
        except ValueError as e:
            messagebox.showerror('Помилка вводу', str(e))
            return

        gtype     = gtype_var.get()
        direction = direction_var.get()

        try:
            if direction == 'adj2inc':
                n = len(mat)
                if any(len(r) != n for r in mat):
                    raise ValueError('Матриця суміжності має бути квадратною (n×n)')
                result   = adj_to_inc(mat, gtype)
                in_lbl   = 'Матриця суміжності'
                out_lbl  = 'Матриця інцидентності'
                adj_for_stats = mat
                in_str  = mat_to_str(mat, rpfx='x', cpfx='x')
                out_str = mat_to_str(result, rpfx='x', cpfx='a')
            else:
                result   = inc_to_adj(mat)
                in_lbl   = 'Матриця інцидентності'
                out_lbl  = 'Матриця суміжності'
                adj_for_stats = result
                in_str  = mat_to_str(mat, rpfx='x', cpfx='a')
                out_str = mat_to_str(result, rpfx='x', cpfx='x')
        except Exception as e:
            messagebox.showerror('Помилка конвертації', str(e))
            return

        nr, nc = len(mat), (len(mat[0]) if mat else 0)
        rr, rc = len(result), (len(result[0]) if result and result[0] else 0)

        in_block  = f'── {in_lbl} ({nr}×{nc}) ──\n{in_str}'
        if result and result[0]:
            out_block = f'\n\n── {out_lbl} ({rr}×{rc}) ──\n{out_str}'
        else:
            out_block = f'\n\n── {out_lbl} ──\n(порожня — немає ребер)'

        _set(txt_out, in_block + out_block)
        _set(txt_stats, graph_stats(adj_for_stats, gtype))

    btn_convert.configure(command=do_convert)
    btn_rand.configure(command=gen_random)

    root.mainloop()


if __name__ == '__main__':
    build_gui()
