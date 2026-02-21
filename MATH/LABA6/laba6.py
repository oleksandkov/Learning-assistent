from typing import List
import random
import tkinter as tk
from tkinter import ttk


INF = float('inf')


def adjacency_to_incidence(adj: List[List[int]], graph_type: str) -> List[List[int]]:
    n = len(adj)
    cols: List[List[int]] = []

    if graph_type == 'directed':
        for i in range(n):
            for j in range(n):
                if adj[i][j]:
                    col = [0] * n
                    col[i] = -1
                    col[j] = 1
                    cols.append(col)

    elif graph_type == 'undirected':
        for i in range(n):
            if adj[i][i]:
                col = [0] * n
                col[i] = 2
                cols.append(col)
        for i in range(n):
            for j in range(i+1, n):
                if adj[i][j] or adj[j][i]:
                    col = [0] * n
                    col[i] = 1
                    col[j] = 1
                    cols.append(col)

    elif graph_type == 'mixed':
        for i in range(n):
            if adj[i][i]:
                col = [0] * n
                col[i] = 2
                cols.append(col)
        for i in range(n):
            for j in range(i+1, n):
                a = adj[i][j]
                b = adj[j][i]
                if a and b:
                    col = [0] * n
                    col[i] = 1
                    col[j] = 1
                    cols.append(col)
                elif a and not b:
                    col = [0] * n
                    col[i] = -1
                    col[j] = 1
                    cols.append(col)
                elif b and not a:
                    col = [0] * n
                    col[j] = -1
                    col[i] = 1
                    cols.append(col)
    else:
        raise ValueError('unknown graph_type')

    if not cols:
        return [ [] for _ in range(n) ]

    m = len(cols)
    inc = [[0]*m for _ in range(n)]
    for j, col in enumerate(cols):
        for i in range(n):
            inc[i][j] = col[i]
    return inc


def incidence_to_adjacency(inc: List[List[int]]) -> List[List[int]]:
    n = len(inc)
    if n == 0:
        return []
    m = len(inc[0])
    adj = [[0]*n for _ in range(n)]
    for j in range(m):
        col = [inc[i][j] for i in range(n)]
        nonzero = [(i, val) for i,val in enumerate(col) if val != 0]
        if len(nonzero) == 2:
            (i1, v1), (i2, v2) = nonzero[0], nonzero[1]
            if v1 == 1 and v2 == 1:
                adj[i1][i2] = 1
                adj[i2][i1] = 1
            elif v1 == -1 and v2 == 1:
                adj[i1][i2] = 1
            elif v1 == 1 and v2 == -1:
                adj[i2][i1] = 1
            else:
                adj[i1][i2] = 1
                adj[i2][i1] = 1
        elif len(nonzero) == 1:
            i1, v1 = nonzero[0]
            adj[i1][i1] = 1
        else:
            pass
    return adj


def read_matrix(rows: int, cols: int=None) -> List[List[int]]:
    mat: List[List[int]] = []
    for r in range(rows):
        while True:
            line = input(f'row {r+1}: ').strip()
            if not line:
                continue
            parts = line.replace(',', ' ').split()
            vals = [int(x) for x in parts]
            if cols is not None and len(vals) != cols:
                print(f'expected {cols} values')
                continue
            mat.append(vals)
            break
    return mat


def print_matrix(mat: List[List[int]]):
    for row in mat:
        print(' '.join(str(x) for x in row))


def random_adjacency(n: int, directed: bool=False, loops: bool=False, density: float=0.3) -> List[List[int]]:
    adj = [[0]*n for _ in range(n)]
    for i in range(n):
        for j in range(n):
            if i == j and not loops:
                continue
            if directed:
                if random.random() < density:
                    adj[i][j] = 1
            else:
                if j > i and random.random() < density:
                    adj[i][j] = adj[j][i] = 1
    if loops:
        for i in range(n):
            if random.random() < density/2:
                adj[i][i] = 1
    return adj


def format_matrix_str(mat: List[List[int]]) -> str:
    if not mat:
        return '<порожня матриця>'
    return '\n'.join(' '.join(str(x) for x in row) for row in mat)


def sample_variants():
    variants = {}
    variants['adj_1'] = [[0,1,0],[0,0,1],[1,0,0]]
    variants['adj_2'] = [[0,1,1,0],[1,0,0,1],[1,0,0,0],[0,1,0,0]]
    variants['inc_1'] = adjacency_to_incidence(variants['adj_1'],'directed')
    variants['inc_2'] = adjacency_to_incidence(variants['adj_2'],'undirected')
    return variants


def control_questions(adj: List[List[int]], inc: List[List[int]]):
    n = len(adj)
    answers = []
    answers.append('1. Основні способи представлення: матриця суміжності, матриця інцидентності, список суміжності.')
    if n>0:
        v = 1
        outgoing = [j+1 for j in range(n) if adj[v-1][j]]
        incoming = [i+1 for i in range(n) if adj[i][v-1]]
        answers.append(f'2. Для вершини {v}: вихідні ребра -> {outgoing}; вхідні ребра -> {incoming}.')
    else:
        answers.append('2. Немає вершин у графі.')
    answers.append('3. Сума степенів усіх вершин неорієнтованого графа дорівнює подвоєній кількості ребер (2E).')
    answers.append('4. Відмінності: матриця суміжності показує пари вершин, матриця інцидентності показує зв\'зок ребер з вершинами; матриця інцидентності краще при багатьох вершинах і мало ребрах.')
    return '\n\n'.join(answers)


def gui_main():
    variants = sample_variants()
    root = tk.Tk()
    root.title('Перекладач матриць графа — Лаба 6')

    frame = ttk.Frame(root, padding=10)
    frame.grid(row=0, column=0, sticky='nsew')

    input_type = tk.StringVar(value='adj')
    ttk.Label(frame, text='Одержано завдання у вигляді:').grid(row=0, column=0, sticky='w')
    ttk.Radiobutton(frame, text='a) матриця суміжності', variable=input_type, value='adj').grid(row=1, column=0, sticky='w')
    ttk.Radiobutton(frame, text='б) матриця інцидентності', variable=input_type, value='inc').grid(row=2, column=0, sticky='w')

    ttk.Label(frame, text='Варіант/приклад:').grid(row=3, column=0, sticky='w')
    var_choice = tk.StringVar(value='adj_1')
    choice_menu = ttk.OptionMenu(frame, var_choice, 'adj_1', *variants.keys())
    choice_menu.grid(row=3, column=1, sticky='w')

    ttk.Label(frame, text='Або згенерувати випадковий граф:').grid(row=4, column=0, sticky='w')
    ttk.Label(frame, text='n:').grid(row=5, column=0, sticky='e')
    n_entry = ttk.Entry(frame, width=5)
    n_entry.insert(0,'5')
    n_entry.grid(row=5, column=1, sticky='w')
    ttk.Label(frame, text='щільність (0-1):').grid(row=6, column=0, sticky='e')
    dens_entry = ttk.Entry(frame, width=5)
    dens_entry.insert(0,'0.3')
    dens_entry.grid(row=6, column=1, sticky='w')
    directed_var = tk.BooleanVar(value=False)
    ttk.Checkbutton(frame, text='орієнтований', variable=directed_var).grid(row=7, column=0, sticky='w')

    txt_orig = tk.Text(frame, width=40, height=10)
    txt_conv = tk.Text(frame, width=40, height=10)
    txt_q = tk.Text(frame, width=80, height=10)
    txt_orig.grid(row=8, column=0, columnspan=2)
    txt_conv.grid(row=8, column=2, columnspan=2)
    txt_q.grid(row=9, column=0, columnspan=4)

    def do_convert():
        it = input_type.get()
        txt_orig.delete('1.0', tk.END)
        txt_conv.delete('1.0', tk.END)
        if it == 'adj':
            key = var_choice.get()
            if key.startswith('adj_'):
                adj = variants[key]
            else:
                n = int(n_entry.get())
                dens = float(dens_entry.get())
                adj = random_adjacency(n, directed=directed_var.get(), loops=False, density=dens)
            inc = adjacency_to_incidence(adj, 'directed' if directed_var.get() else 'undirected')
            txt_orig.insert(tk.END, format_matrix_str(adj))
            txt_conv.insert(tk.END, format_matrix_str(inc))
            txt_q.delete('1.0', tk.END)
            txt_q.insert(tk.END, control_questions(adj, inc))
        else:
            key = var_choice.get()
            if key.startswith('inc_'):
                inc = variants[key]
            else:
                n = int(n_entry.get())
                dens = float(dens_entry.get())
                adj = random_adjacency(n, directed=directed_var.get(), loops=False, density=dens)
                inc = adjacency_to_incidence(adj, 'directed' if directed_var.get() else 'undirected')
            adj = incidence_to_adjacency(inc)
            txt_orig.insert(tk.END, format_matrix_str(inc))
            txt_conv.insert(tk.END, format_matrix_str(adj))
            txt_q.delete('1.0', tk.END)
            txt_q.insert(tk.END, control_questions(adj, inc))

    btn = ttk.Button(frame, text='Перетворити', command=do_convert)
    btn.grid(row=10, column=0)

    root.mainloop()


def main_gui():
    gui_main()


if __name__ == '__main__':
    main_gui()
