from itertools import combinations
import tkinter as tk

print("ЛАБОРАТОРНА РОБОТА №1. МНОЖИНИ. ОСНОВНІ ПОНЯТТЯ")

while True:
    print("\nОберіть дію:")
    print("1 - Завдання 1 (рівність і підмножина)")
    print("2 - Завдання 2 (всі підмножини)")
    print("3 - Завдання 3 (графічні операції)")
    print("0 - Вихід")

    choice = input("Ваш вибір: ").strip()

    if choice == "1":
        print("\n=== Завдання 1 ===")
        raw_a = input("Введіть множину A (через пробіл): ").strip()
        raw_b = input("Введіть множину B (через пробіл): ").strip()

        a = set(raw_a.split()) if raw_a else set()
        b = set(raw_b.split()) if raw_b else set()

        a_text = "{" + ", ".join(sorted(a)) + "}"
        b_text = "{" + ", ".join(sorted(b)) + "}"

        print(f"A = {a_text}")
        print(f"B = {b_text}")

        if a == b:
            print("Висновок: A і B рівні.")
        elif a.issubset(b):
            print("Висновок: A є підмножиною B.")
        elif b.issubset(a):
            print("Висновок: B є підмножиною A.")
        else:
            print("Висновок: множини різні, і жодна не є підмножиною іншої.")

    elif choice == "2":
        print("\n=== Завдання 2 ===")
        raw_a = input("Введіть множину A (через пробіл): ").strip()
        a = set(raw_a.split()) if raw_a else set()

        items = list(a)
        subsets = []
        for r in range(len(items) + 1):
            for combo in combinations(items, r):
                subsets.append(set(combo))

        a_text = "{" + ", ".join(sorted(a)) + "}"
        print(f"A = {a_text}")
        print(f"Кількість підмножин P(A): {len(subsets)}")
        print("Список всіх підмножин:")

        for i, sub in enumerate(subsets, start=1):
            sub_text = "{" + ", ".join(sorted(sub)) + "}"
            print(f"{i:>3}. {sub_text}")

    elif choice == "3":
        print("\n=== Завдання 3 ===")
        raw_a = input("Введіть множину A (через пробіл): ").strip()
        raw_b = input("Введіть множину B (через пробіл): ").strip()

        a = set(raw_a.split()) if raw_a else set()
        b = set(raw_b.split()) if raw_b else set()

        union_ab = a | b
        inter_ab = a & b
        diff_ab = a - b
        symdiff_ab = a ^ b

        a_text = "{" + ", ".join(sorted(a)) + "}"
        b_text = "{" + ", ".join(sorted(b)) + "}"
        union_text = "{" + ", ".join(sorted(union_ab)) + "}"
        inter_text = "{" + ", ".join(sorted(inter_ab)) + "}"
        diff_text = "{" + ", ".join(sorted(diff_ab)) + "}"
        symdiff_text = "{" + ", ".join(sorted(symdiff_ab)) + "}"

        print(f"A = {a_text}")
        print(f"B = {b_text}")
        print("Результати:")
        print(f"A ∪ B = {union_text}")
        print(f"A ∩ B = {inter_text}")
        print(f"A - B = {diff_text}")
        print(f"A ⊕ B = {symdiff_text}")

        root = tk.Tk()
        root.title("ЛР №1: Операції над множинами")
        root.configure(bg="#0f172a")
        root.resizable(False, False)

        # Простий вигляд у 1 колонку
        canvas = tk.Canvas(root, width=760, height=980, bg="#0f172a", highlightthickness=0)
        canvas.pack(padx=12, pady=12)

        # Header
        canvas.create_rectangle(18, 16, 742, 86, fill="#111827", outline="#334155", width=2)
        canvas.create_text(
            380,
            38,
            text="ОПЕРАЦІЇ НАД МНОЖИНАМИ",
            font=("Segoe UI", 15, "bold"),
            fill="#e2e8f0",
        )
        canvas.create_text(
            380,
            62,
            text=f"A = {a_text}    |    B = {b_text}",
            font=("Segoe UI", 10, "bold"),
            fill="#93c5fd",
        )

        # ===== Card 1: A ∪ B =====
        card_top = 106
        canvas.create_rectangle(28, card_top, 732, card_top + 200, fill="#1e293b", outline="#334155", width=2)
        canvas.create_text(380, card_top + 24, text="A ∪ B", font=("Segoe UI", 12, "bold"), fill="#f8fafc")
        x = 280
        y = card_top + 62
        canvas.create_oval(x, y, x + 120, y + 120, fill="#86efac", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y, x + 190, y + 120, fill="#86efac", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y + 8, x + 120, y + 112, fill="#4ade80", outline="")
        canvas.create_text(x + 34, y + 60, text="A", font=("Segoe UI", 11, "bold"), fill="#0f172a")
        canvas.create_text(x + 156, y + 60, text="B", font=("Segoe UI", 11, "bold"), fill="#0f172a")

        # ===== Card 2: A ∩ B =====
        card_top = 320
        canvas.create_rectangle(28, card_top, 732, card_top + 200, fill="#1e293b", outline="#334155", width=2)
        canvas.create_text(380, card_top + 24, text="A ∩ B", font=("Segoe UI", 12, "bold"), fill="#f8fafc")
        x = 280
        y = card_top + 62
        canvas.create_oval(x, y, x + 120, y + 120, fill="#e2e8f0", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y, x + 190, y + 120, fill="#e2e8f0", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y + 8, x + 120, y + 112, fill="#22c55e", outline="")
        canvas.create_text(x + 34, y + 60, text="A", font=("Segoe UI", 11, "bold"), fill="#0f172a")
        canvas.create_text(x + 156, y + 60, text="B", font=("Segoe UI", 11, "bold"), fill="#0f172a")

        # ===== Card 3: A - B =====
        card_top = 534
        canvas.create_rectangle(28, card_top, 732, card_top + 200, fill="#1e293b", outline="#334155", width=2)
        canvas.create_text(380, card_top + 24, text="A - B", font=("Segoe UI", 12, "bold"), fill="#f8fafc")
        x = 280
        y = card_top + 62
        canvas.create_oval(x, y, x + 120, y + 120, fill="#86efac", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y, x + 190, y + 120, fill="#e2e8f0", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y + 8, x + 120, y + 112, fill="#e2e8f0", outline="")
        canvas.create_text(x + 34, y + 60, text="A", font=("Segoe UI", 11, "bold"), fill="#0f172a")
        canvas.create_text(x + 156, y + 60, text="B", font=("Segoe UI", 11, "bold"), fill="#0f172a")

        # ===== Card 4: A ⊕ B =====
        card_top = 748
        canvas.create_rectangle(28, card_top, 732, card_top + 200, fill="#1e293b", outline="#334155", width=2)
        canvas.create_text(380, card_top + 24, text="A ⊕ B", font=("Segoe UI", 12, "bold"), fill="#f8fafc")
        x = 280
        y = card_top + 62
        canvas.create_oval(x, y, x + 120, y + 120, fill="#86efac", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y, x + 190, y + 120, fill="#86efac", outline="#0f172a", width=2)
        canvas.create_oval(x + 70, y + 8, x + 120, y + 112, fill="#1e293b", outline="")
        canvas.create_text(x + 34, y + 60, text="A", font=("Segoe UI", 11, "bold"), fill="#0f172a")
        canvas.create_text(x + 156, y + 60, text="B", font=("Segoe UI", 11, "bold"), fill="#0f172a")

        # Bottom result strip
        canvas.create_rectangle(18, 952, 742, 970, fill="#111827", outline="#334155", width=1)
        canvas.create_text(
            380,
            961,
            text=f"A∪B={union_text}; A∩B={inter_text}; A-B={diff_text}; A⊕B={symdiff_text}",
            font=("Consolas", 8),
            fill="#cbd5e1",
        )

        root.mainloop()

    elif choice == "0":
        print("Готово")
        break
    else:
        print("Невірний вибір.")
