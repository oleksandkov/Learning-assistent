"""
Лабораторна робота №5 (проста версія)
Основні поняття теорії графів.

Завдання:
1) Знайти мінімальний шлях з v1 у v7 за матрицею суміжності (варіанти а, б, в).
2) Знайти мінімальний шлях з v1 у v7 у зваженому графі (варіанти а, б, в).

Скрипт зроблено максимально простим, у стилі попередніх лабораторних.
"""

INF = float("inf")


def bellman(matrix, start, end):
    """Простий алгоритм Беллмана-Форда."""
    n = len(matrix)
    dist = [INF] * n
    parent = [-1] * n
    dist[start] = 0

    for _ in range(n - 1):
        for u in range(n):
            for v in range(n):
                w = matrix[u][v]
                if w != INF and dist[u] != INF and dist[u] + w < dist[v]:
                    dist[v] = dist[u] + w
                    parent[v] = u

    if dist[end] == INF:
        return None, INF

    path = []
    current = end
    while current != -1:
        path.append(current)
        current = parent[current]
    path.reverse()

    return path, dist[end]


def adjacency_to_weight(adjacency):
    """Перетворює матрицю суміжності у вагову (1 або INF)."""
    n = len(adjacency)
    weighted = []
    for i in range(n):
        row = []
        for j in range(n):
            if adjacency[i][j] == 1:
                row.append(1)
            else:
                row.append(INF)
        weighted.append(row)
    return weighted


def print_result(title, matrix, is_weighted):
    start = 0  # v1
    end = 6    # v7

    if not is_weighted:
        matrix = adjacency_to_weight(matrix)

    path, cost = bellman(matrix, start, end)

    print("\n" + "-" * 70)
    print(title)
    print("Шлях: v1 -> v7")
    if path is None:
        print("Результат: шлях не існує")
        return

    text_path = " -> ".join(f"v{v + 1}" for v in path)
    print(f"Найкоротший шлях: {text_path}")
    if is_weighted:
        print(f"Мінімальна вага: {cost}")
    else:
        print(f"Мінімальна кількість дуг: {int(cost)}")


# ------------------ Дані із завдання ------------------

# 1) Матриці суміжності
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

# 2) Матриці ваг
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


print("=" * 70)
print("ЛАБОРАТОРНА РОБОТА №5 (ПРОСТА ВЕРСІЯ)")
print("Пошук мінімального шляху з v1 у v7 алгоритмом Беллмана")
print("=" * 70)

print("\nЗАВДАННЯ 1: Матриці суміжності")
print_result("Варіант 1а", adj_a, is_weighted=False)
print_result("Варіант 1б", adj_b, is_weighted=False)
print_result("Варіант 1в", adj_c, is_weighted=False)

print("\nЗАВДАННЯ 2: Зважені матриці")
print_result("Варіант 2а", w_a, is_weighted=True)
print_result("Варіант 2б", w_b, is_weighted=True)
print_result("Варіант 2в", w_c, is_weighted=True)

print("\nГотово. Оброблено всі 6 варіантів із лабораторної роботи №5.")