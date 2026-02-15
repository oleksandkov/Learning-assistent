import tkinter as tk
from tkinter import ttk


# Дані з лабораторної (завдання 1)
FUNCTIONS_MINTERMS = [
    [0, 1, 2, 4, 5, 7, 8, 10, 11, 13, 14, 15],
    [0, 1, 2, 3, 4, 5, 7, 8, 10, 11, 13, 14],
    [0, 1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15],
    [1, 2, 3, 4, 6, 7, 8, 9, 11, 12, 14, 15],
    [1, 3, 4, 5, 6, 8, 9, 10, 12, 13, 15],
    [2, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14],
]

MINIMIZED_EXPRESSIONS = [
    "!x2!x4 + x2x4 + !x1!x3 + x1x3",
    "!x2!x4 + !x2x3 + x2!x3x4 + !x1x4 + !x1!x3 + x1x3!x4",
    "x4 + x3 + !x1!x2",
    "x3x4 + !x2x4 + x2!x4 + !x1x3 + x1!x3!x4",
    "!x3x4 + x2!x3 + !x1!x2x4 + !x1x2!x4 + x1!x2!x4 + x1x2x4",
    "x2!x4 + !x1x3!x4 + !x1x2 + x1!x3 + x1!x2x4",
]


def minterm_number(x1, x2, x3, x4):
    return (x1 << 3) | (x2 << 2) | (x3 << 1) | x4


def karnaugh_layout_numbers():

    row_pairs = [(1, 0), (1, 1), (0, 1), (0, 0)]  # (x2, x4)
    col_pairs = [(1, 0), (1, 1), (0, 1), (0, 0)]  # (x1, x3)

    grid = []
    for x2, x4 in row_pairs:
        row = []
        for x1, x3 in col_pairs:
            row.append(minterm_number(x1, x2, x3, x4))
        grid.append(row)

    return grid


def truth_table_text(minterms):
    mts = set(minterms)
    lines = ["x1 x2 x3 x4 | f"]
    for m in range(16):
        x1 = (m >> 3) & 1
        x2 = (m >> 2) & 1
        x3 = (m >> 1) & 1
        x4 = m & 1
        f = 1 if m in mts else 0
        lines.append(f"{x1}  {x2}  {x3}  {x4}  | {f}")
    return "\n".join(lines)


def draw_kmap(canvas, minterms):
    canvas.delete("all")

    # Геометрія
    x0 = 170
    y0 = 140
    cell = 95
    rows = 4
    cols = 4

    # Заголовок
    canvas.create_text(
        520,
        28,
        text="Карта Карно / Вейча (4 змінні)",
        font=("Segoe UI", 18, "bold"),
        fill="#0f172a",
    )

    # Довідка по осях
    canvas.create_text(
        520,
        58,
        text="Рядки: x2x4 = 10, 11, 01, 00      |      Стовпці: x1x3 = 10, 11, 01, 00",
        font=("Segoe UI", 11),
        fill="#334155",
    )

    # Зовнішня рамка
    canvas.create_rectangle(x0, y0, x0 + cols * cell, y0 + rows * cell, width=2)

    # Внутрішня сітка
    for i in range(1, cols):
        canvas.create_line(x0 + i * cell, y0, x0 + i * cell, y0 + rows * cell, width=1)
    for i in range(1, rows):
        canvas.create_line(x0, y0 + i * cell, x0 + cols * cell, y0 + i * cell, width=1)

    # Підписи x1 / !x1 зверху (по 2 стовпці)
    canvas.create_text(x0 + cell, y0 - 32, text="x1", font=("Segoe UI", 14, "bold"))
    canvas.create_text(x0 + 3 * cell, y0 - 32, text="!x1", font=("Segoe UI", 14, "bold"))
    canvas.create_line(x0, y0 - 15, x0 + 2 * cell, y0 - 15, width=2)
    canvas.create_line(x0 + 2 * cell, y0 - 15, x0 + 4 * cell, y0 - 15, width=2)

    # Підписи x2 / !x2 зліва (по 2 рядки)
    canvas.create_text(x0 - 42, y0 + cell, text="x2", font=("Segoe UI", 14, "bold"))
    canvas.create_text(x0 - 46, y0 + 3 * cell, text="!x2", font=("Segoe UI", 14, "bold"))

    # Підписи x3 / !x3 знизу (по 2 стовпці)
    canvas.create_text(x0 + cell, y0 + rows * cell + 28, text="x3", font=("Segoe UI", 14, "bold"))
    canvas.create_text(x0 + 3 * cell, y0 + rows * cell + 28, text="!x3", font=("Segoe UI", 14, "bold"))

    # Підписи x4 / !x4 справа (по рядках у Gray-порядку 10,11,01,00)
    # Для рядків: 10 -> !x4, 11 -> x4, 01 -> x4, 00 -> !x4
    right_labels = ["!x4", "x4", "x4", "!x4"]
    for r in range(4):
        yc = y0 + r * cell + cell / 2
        canvas.create_text(x0 + cols * cell + 36, yc, text=right_labels[r], font=("Segoe UI", 12, "bold"))

    

    # Заповнення клітинок
    layout = karnaugh_layout_numbers()
    mts = set(minterms)

    for r in range(4):
        for c in range(4):
            m = layout[r][c]
            val = 1 if m in mts else 0

            x1 = x0 + c * cell
            y1 = y0 + r * cell
            x2 = x1 + cell
            y2 = y1 + cell

            if val == 1:
                fill_color = "#d1fae5"  # зелений для 1
            else:
                fill_color = "#f8fafc"  # світлий для 0

            canvas.create_rectangle(x1 + 1, y1 + 1, x2 - 1, y2 - 1, fill=fill_color, outline="")

            # Бітовий код і номер мінтерма (як у PDF)
            bits = f"{(m >> 3) & 1}{(m >> 2) & 1}{(m >> 1) & 1}{m & 1}"
            canvas.create_text(x1 + 8, y1 + 10, text=bits, anchor="nw", font=("Consolas", 10), fill="#0f172a")
            canvas.create_text(x1 + 8, y1 + 28, text=str(m), anchor="nw", font=("Consolas", 11, "bold"), fill="#0f172a")

            # Значення функції (велике)
            canvas.create_text(
                x1 + cell / 2,
                y1 + cell / 2 + 12,
                text=str(val),
                font=("Segoe UI", 20, "bold"),
                fill="#111827" if val == 0 else "#065f46",
            )


def refresh_view(*_):
    idx = function_var.get() - 1
    minterms = FUNCTIONS_MINTERMS[idx]
    expression = MINIMIZED_EXPRESSIONS[idx]

    equation_var.set(f"f(x1,x2,x3,x4) = Σm{tuple(minterms)}")
    minimal_var.set(f"Мінімізована ДНФ:  {expression}")

    draw_kmap(canvas, minterms)

    tt.delete("1.0", tk.END)
    tt.insert(tk.END, truth_table_text(minterms))


# --------------------------- GUI ---------------------------
root = tk.Tk()
root.geometry("1280x760")
root.minsize(1180, 700)

style = ttk.Style()
style.theme_use("clam")
style.configure("TLabel", font=("Segoe UI", 11))
style.configure("Title.TLabel", font=("Segoe UI", 16, "bold"))
style.configure("TButton", font=("Segoe UI", 10, "bold"))

main = ttk.Frame(root, padding=14)
main.pack(fill="both", expand=True)

header = ttk.Label(main, text="Лабораторна №4 — Мінімізація методом Карно/Вейча", style="Title.TLabel")
header.pack(anchor="w")

sub = ttk.Label(
    main,
    text="Advanced-версія: графічна карта Карно + перевірка правильного розташування x і !x як у PDF",
)
sub.pack(anchor="w", pady=(0, 10))

controls = ttk.Frame(main)
controls.pack(fill="x", pady=(0, 8))

function_var = tk.IntVar(value=1)

ttk.Label(controls, text="Оберіть функцію:").pack(side="left")
selector = ttk.Spinbox(
    controls,
    from_=1,
    to=6,
    textvariable=function_var,
    width=5,
    command=refresh_view,
)
selector.pack(side="left", padx=8)

refresh_btn = ttk.Button(controls, text="Оновити", command=refresh_view)
refresh_btn.pack(side="left")

layout_info = ttk.Label(
    controls,
    text="Розміщення осей: зверху x1/!x1, зліва x2/!x2, знизу x3/!x3, справа x4/!x4",
)
layout_info.pack(side="left", padx=(16, 0))

equation_var = tk.StringVar()
minimal_var = tk.StringVar()

ttk.Label(main, textvariable=equation_var, font=("Consolas", 12, "bold")).pack(anchor="w", pady=(2, 2))
ttk.Label(main, textvariable=minimal_var, font=("Consolas", 12)).pack(anchor="w", pady=(0, 8))

body = ttk.Frame(main)
body.pack(fill="both", expand=True)

# Ліва панель: графіка карти
left = ttk.Frame(body)
left.pack(side="left", fill="both", expand=True)

canvas = tk.Canvas(left, bg="white", highlightthickness=1, highlightbackground="#cbd5e1")
canvas.pack(fill="both", expand=True)

# Права панель: таблиця істинності
right = ttk.LabelFrame(body, text="Таблиця істинності")
right.pack(side="right", fill="y", padx=(10, 0))

scroll = ttk.Scrollbar(right)
scroll.pack(side="right", fill="y")

tt = tk.Text(right, width=28, height=36, font=("Consolas", 11), yscrollcommand=scroll.set)
tt.pack(side="left", fill="both", expand=True)
scroll.config(command=tt.yview)

root.bind("<Return>", refresh_view)

refresh_view()
root.mainloop()
