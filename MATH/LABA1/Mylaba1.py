import tkinter as tk
from tkinter import messagebox
from itertools import combinations


def parse_value(s):
    try:
        return int(s)
    except ValueError:
        try:
            return float(s)
        except ValueError:
            return s


# Завдання 1
print("Обери завдання:")
x = set(map(parse_value, input("Множина перша: (внести множину довільних елементів через пробіл)").split()))
print("Множина перша:", x)

y = set(map(parse_value, input("Множина друга: (внести множину довільних елементів через пробіл)").split()))
print("Множина друга:", y)


if x == y:
    print("Множини еквівалентні.")
elif x <= y:
    print("Перша множина є підмножиною другої.")
elif y <= x:
    print("Друга множина є підмножиною першої.")
else:
    print("Множини різні і жодна не є підмножиною іншої.")

# Завдання 2
print("Завдання 2")

s = set(map(parse_value, input("Множина: (внести множину довільних елементів через пробіл)").split()))

subsets = [set()]
for element in s:
    new_subsets = []
    for subset in subsets:
        new_subset = subset | {element}
        new_subsets.append(new_subset)
    subsets += new_subsets

for subset in subsets:
    print(subset)

# Завдання 3
print("\n" + "="*60)
print("ЗАВДАННЯ 3: Моделювання операцій над множинами")
print("="*60)

# АЛГОРИТМ:
# Крок 1: Взяти множини X та Y з попереднього завдання
# Крок 2: Виконати базові операції над множинами
# Крок 3: Відобразити результати у графічному інтерфейсі

# Обчислення операцій
union_set = x | y  # Об'єднання
intersection_set = x & y  # Перетин
difference_set = x - y  # Різниця
symmetric_difference = x ^ y  # Симетрична різниця

# Консольний вивід
print(f"\n📌 Вхідні дані:")
print(f"   X = {x}")
print(f"   Y = {y}")
print(f"\n📊 Результати операцій:")
print(f"   X ∪ Y = {union_set}")
print(f"   X ∩ Y = {intersection_set}")
print(f"   X - Y = {difference_set}")
print(f"   X ⊕ Y = {symmetric_difference}")
print(f"\n🖼️  Графічна візуалізація відкривається...")
print("="*60)

# Створення GUI вікна
root = tk.Tk()
root.title("🔬 Операції над множинами | Лабораторна робота №1")
root.configure(bg="#1a1a2e")

# Створення полотна
canvas = tk.Canvas(root, width=920, height=750, bg="#f0f0f0")
canvas.pack(padx=10, pady=10)

# Заголовок з вихідними множинами
title_text = f"Множина X = {x}    |    Множина Y = {y}"
canvas.create_text(460, 20, text=title_text, font=("Arial", 12, "bold"), fill="navy")
canvas.create_line(10, 40, 910, 40, fill="navy", width=2)

# ОПЕРАЦІЯ 1: X ∪ Y (Об'єднання)
canvas.create_oval(60, 80, 200, 220, fill="red", outline="black", width=2)
canvas.create_oval(140, 80, 280, 220, fill="skyblue", outline="black", width=2)
label = tk.Label(root, text="X", bg="red", font=("Arial", 18, "bold"))
label.place(x=95, y=130)
label = tk.Label(root, text="Y", bg="skyblue", font=("Arial", 18, "bold"))
label.place(x=215, y=130)
label = tk.Label(root, text="X ∪ Y (Об'єднання)", bg="#f0f0f0", font=("Arial", 13, "bold"))
label.place(x=70, y=230)
result_text = f"Результат: {union_set}"
label = tk.Label(root, text=result_text, bg="#f0f0f0", font=("Arial", 10), wraplength=200)
label.place(x=60, y=250)

# ОПЕРАЦІЯ 2: X - Y (Різниця)
canvas.create_oval(60, 320, 200, 460, fill="lightcoral", outline="darkred", width=2)
canvas.create_oval(140, 320, 280, 460, fill="white", outline="gray", width=2)
label = tk.Label(root, text="X", bg="lightcoral", font=("Arial", 18, "bold"))
label.place(x=95, y=370)
label = tk.Label(root, text="Y", bg="white", font=("Arial", 18, "bold"))
label.place(x=215, y=370)
label = tk.Label(root, text="X - Y (Різниця)", bg="#f0f0f0", font=("Arial", 13, "bold"))
label.place(x=80, y=470)
result_text = f"Результат: {difference_set}"
label = tk.Label(root, text=result_text, bg="#f0f0f0", font=("Arial", 10), wraplength=200)
label.place(x=60, y=490)

# ОПЕРАЦІЯ 3: X ∩ Y (Перетин)
canvas.create_oval(480, 80, 620, 220, fill="white", outline="darkgreen", width=2)
canvas.create_oval(560, 80, 700, 220, fill="white", outline="darkgreen", width=2)
canvas.create_oval(560, 90, 615, 210, fill="lightgreen", outline="darkgreen", width=2)
label = tk.Label(root, text="X", bg="white", font=("Arial", 18, "bold"))
label.place(x=510, y=130)
label = tk.Label(root, text="Y", bg="white", font=("Arial", 18, "bold"))
label.place(x=645, y=130)
label = tk.Label(root, text="X ∩ Y (Перетин)", bg="#f0f0f0", font=("Arial", 13, "bold"))
label.place(x=505, y=230)
result_text = f"Результат: {intersection_set}"
label = tk.Label(root, text=result_text, bg="#f0f0f0", font=("Arial", 10), wraplength=200)
label.place(x=480, y=250)

# ОПЕРАЦІЯ 4: X ⊕ Y (Симетрична різниця)
canvas.create_oval(480, 320, 620, 460, fill="gold", outline="orange", width=2)
canvas.create_oval(560, 320, 700, 460, fill="gold", outline="orange", width=2)
canvas.create_oval(560, 330, 615, 450, fill="white", outline="orange", width=2)
label = tk.Label(root, text="X", bg="gold", font=("Arial", 18, "bold"))
label.place(x=510, y=370)
label = tk.Label(root, text="Y", bg="gold", font=("Arial", 18, "bold"))
label.place(x=645, y=370)
label = tk.Label(root, text="X ⊕ Y (Симетрична різниця)", bg="#f0f0f0", font=("Arial", 13, "bold"))
label.place(x=445, y=470)
result_text = f"Результат: {symmetric_difference}"
label = tk.Label(root, text=result_text, bg="#f0f0f0", font=("Arial", 10), wraplength=200)
label.place(x=480, y=490)

# Легенда та пояснення
canvas.create_line(10, 560, 910, 560, fill="navy", width=2)
legend_text = "Легенда: Кольорові області показують елементи, що входять до результату операції"
canvas.create_text(460, 580, text=legend_text, font=("Arial", 10, "italic"), fill="gray")

# Підсумкова інформація
summary = f"Кількість елементів: X={len(x)}, Y={len(y)} | Об'єднання={len(union_set)}, Перетин={len(intersection_set)}, Різниця={len(difference_set)}, Симетрична різниця={len(symmetric_difference)}"
canvas.create_text(460, 600, text=summary, font=("Arial", 9), fill="navy")

root.mainloop()