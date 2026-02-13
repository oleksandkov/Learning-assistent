import tkinter as tk
from tkinter import ttk, messagebox
import itertools, math

class SetTheoryLab:
    def __init__(self, root):
        self.root = root
        self.root.title("Лаб №1: Множини")
        self.root.geometry("850x650")
        self.nb = ttk.Notebook(root)
        self.nb.pack(expand=True, fill='both', padx=10, pady=10)

        self.setup_t1()
        self.setup_t2()
        self.setup_t3()

    def get_set(self, entry):
        try: return set(map(int, entry.get().split()))
        except: return None

    # ЗАВДАННЯ 1
    def setup_t1(self):
        tab = ttk.Frame(self.nb)
        self.nb.add(tab, text="1. Порівняння")

        ttk.Label(tab, text="Введіть елементи (через пробіл):", font=("Arial", 11)).pack(pady=10)
        f = ttk.Frame(tab); f.pack()
        self.e1, self.e2 = ttk.Entry(f, width=20), ttk.Entry(f, width=20)

        for i, (t, e) in enumerate([("A:", self.e1), ("B:", self.e2)]):
            ttk.Label(f, text=t).grid(row=i, column=0, padx=5, pady=5)
            e.grid(row=i, column=1, padx=5)

        ttk.Button(tab, text="Перевірити", command=self.check).pack(pady=10)
        self.lbl_r1 = ttk.Label(tab, text="", font=("Consolas", 12)); self.lbl_r1.pack()

    def check(self):
        A, B = self.get_set(self.e1), self.get_set(self.e2)
        if A is None or B is None: return messagebox.showerror("Помилка", "Тільки числа!")

        res = []
        res.append(f"Множини рівні (A = B): {'Так' if A == B else 'Ні'}")
        res.append(f"A є підмножиною B (A ⊆ B): {'Так' if A.issubset(B) else 'Ні'}")
        res.append(f"B є підмножиною A (B ⊆ A): {'Так' if B.issubset(A) else 'Ні'}")

        self.lbl_r1.config(text="\n".join(res))

    # ЗАВДАННЯ 2
    def setup_t2(self):
        tab = ttk.Frame(self.nb)
        self.nb.add(tab, text="2. Булеан")
        ttk.Label(tab, text="Множина:", font=("Arial", 11)).pack(pady=10)
        self.e_pow = ttk.Entry(tab, width=30); self.e_pow.pack()
        ttk.Button(tab, text="Генерувати", command=self.gen_pow).pack(pady=5)
        self.txt_r2 = tk.Text(tab, height=15, width=70, font=("Consolas", 10))
        self.txt_r2.pack(pady=10)

    def gen_pow(self):
        s = self.get_set(self.e_pow)
        if s is None: return messagebox.showerror("Помилка", "Дані некоректні")
        lst, n = sorted(list(s)), len(s)
        out = [f"Множина: {s}\nКількість підмножин (2^{n}): {2**n}\n{'='*40}"]
        subs = itertools.chain.from_iterable(itertools.combinations(lst, r) for r in range(n + 1))
        out.extend(f"{i+1}. " + ("∅" if not sub else f"{{{', '.join(map(str, sub))}}}") for i, sub in enumerate(subs))
        self.txt_r2.delete(1.0, tk.END); self.txt_r2.insert(tk.END, "\n".join(out))

    # ЗАВДАННЯ 3
    def setup_t3(self):
        tab = ttk.Frame(self.nb)
        self.nb.add(tab, text="3. Візуалізація")
        self.op = tk.StringVar(value="union")
        f = ttk.LabelFrame(tab, text="Операція"); f.pack(pady=5, fill="x")
        ops = [("A ∪ B", "union"), ("A ∩ B", "intersection"), ("A - B", "diff_ab"), ("B - A", "diff_ba"), ("A ⊕ B", "sym_diff")]
        for t, v in ops: ttk.Radiobutton(f, text=t, variable=self.op, value=v, command=self.draw).pack(side="left", padx=5)

        self.canv = tk.Canvas(tab, width=600, height=400, bg="white"); self.canv.pack(pady=5)
        self.lbl_d = ttk.Label(tab, text="", font=("Arial", 11, "italic")); self.lbl_d.pack()
        self.draw()

    def draw(self):
        self.canv.delete("all")
        op = self.op.get()
        r, cx_a, cx_b, cy = 100, 230, 370, 200
        deg = math.degrees(math.acos(((cx_b - cx_a)/2)/r))
        G, W = "#4CAF50", "white"

        def circle(cx, fill):
            if fill: self.canv.create_oval(cx-r, cy-r, cx+r, cy+r, fill=fill, outline="")

        def lens(fill):
            self.canv.create_arc(cx_a-r, cy-r, cx_a+r, cy+r, start=-deg, extent=2*deg, style=tk.CHORD, fill=fill, outline="")
            self.canv.create_arc(cx_b-r, cy-r, cx_b+r, cy+r, start=180-deg, extent=2*deg, style=tk.CHORD, fill=fill, outline="")

        if op == "union":
            circle(cx_a, G); circle(cx_b, G)
        elif op == "intersection":
            lens(G)
        elif op == "diff_ab":
            circle(cx_a, G); lens(W)
        elif op == "diff_ba":
            circle(cx_b, G); lens(W)
        elif op == "sym_diff":
            circle(cx_a, G); circle(cx_b, G); lens(W)

        self.canv.create_oval(cx_a-r, cy-r, cx_a+r, cy+r, outline="black", width=2)
        self.canv.create_oval(cx_b-r, cy-r, cx_b+r, cy+r, outline="black", width=2)
        self.canv.create_text(cx_a-40, cy, text="A", font="Arial 20 bold")
        self.canv.create_text(cx_b+40, cy, text="B", font="Arial 20 bold")

        descs = {"union": "Всі елементи", "intersection": "Спільні елементи", "diff_ab": "Тільки в A",
                 "diff_ba": "Тільки в B", "sym_diff": "Не спільні елементи"}
        self.lbl_d.config(text=descs.get(op, ""))

if __name__ == "__main__":
    root = tk.Tk()
    SetTheoryLab(root)
    root.mainloop()