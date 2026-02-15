"""
Лабораторна робота №4 (спрощена версія)
Мінімізація логічних функцій методом Карно/Вейча.

Цей файл зроблено максимально простим:
- без складних алгоритмів мінімізації;
- без зовнішніх бібліотек;
- з готовими мінімізованими формулами для 6 функцій із завдання 1.
"""

# Порядок на карті Карно (код Грея): 00, 01, 11, 10
gray_order = [0, 1, 3, 2]

# 6 функцій із PDF (завдання 1)
functions_minterms = [
    [0, 1, 2, 4, 5, 7, 8, 10, 11, 13, 14, 15],
    [0, 1, 2, 3, 4, 5, 7, 8, 10, 11, 13, 14],
    [0, 1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15],
    [1, 2, 3, 4, 6, 7, 8, 9, 11, 12, 14, 15],
    [1, 3, 4, 5, 6, 8, 9, 10, 12, 13, 15],
    [2, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14],
]

# Готові мінімізовані ДНФ (отримані для цих мінтермів)
minimized_expressions = [
    "!x2!x4 + x2x4 + !x1!x3 + x1x3",
    "!x2!x4 + !x2x3 + x2!x3x4 + !x1x4 + !x1!x3 + x1x3!x4",
    "x4 + x3 + !x1!x2",
    "x3x4 + !x2x4 + x2!x4 + !x1x3 + x1!x3!x4",
    "!x3x4 + x2!x3 + !x1!x2x4 + !x1x2!x4 + x1!x2!x4 + x1x2x4",
    "x2!x4 + !x1x3!x4 + !x1x2 + x1!x3 + x1!x2x4",
]


print("=" * 80)
print("ЛАБОРАТОРНА РОБОТА №4 (ПРОСТА ВЕРСІЯ)")
print("Мінімізація методом Карно/Вейча для 4 змінних")
print("=" * 80)


for index in range(6):
    minterms = functions_minterms[index]
    minterms_set = set(minterms)
    expr = minimized_expressions[index]

    print("\n" + "-" * 80)
    print(f"Функція {index + 1}")
    print(f"f(x1,x2,x3,x4) = Σm{tuple(minterms)}")

    # Таблиця істинності
    print("\nТаблиця істинності:")
    print("x1 x2 x3 x4 | f")
    for m in range(16):
        x1 = (m >> 3) & 1
        x2 = (m >> 2) & 1
        x3 = (m >> 1) & 1
        x4 = m & 1
        f = 1 if m in minterms_set else 0
        print(f"{x1}  {x2}  {x3}  {x4}  | {f}")

    # Карта Карно 4x4
    print("\nКарта Карно (рядки: x1x2, стовпці: x3x4; порядок 00,01,11,10):")
    print("        00 01 11 10")
    row_labels = ["00", "01", "11", "10"]

    for r in range(4):
        row_code = gray_order[r]
        row_values = []

        for c in range(4):
            col_code = gray_order[c]
            m = (row_code << 2) | col_code
            value = 1 if m in minterms_set else 0
            row_values.append(str(value))

        print(f"{row_labels[r]}   ->  " + "  ".join(row_values))

    # Мінімізована функція + коротка схема
    print("\nМінімізована ДНФ:")
    print(f"f = {expr}")

    terms = [part.strip() for part in expr.split("+")]
    print("Схема AND-OR:")
    print(f"- AND елементів: {len(terms)}")
    print("- Виходи AND подати на один OR")
    print("- Для !xi використати NOT")


print("\nГотово. Оброблено всі 6 функцій із завдання 1.")
