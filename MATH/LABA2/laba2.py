import random


def print_matrix(matrix):
	for row in matrix:
		print(" ".join(f"{value:2}" for value in row))


def task_1_demo():
	print("\n=== Завдання 1 ===")
	print("Довільний варіант ")
	print("Відношення : R(a, b) = 'a ділить b' на S = {1..n}")

	n = 6
	elements = list(range(1, n + 1))
	matrix = []
	edges = []

	# Будуємо матрицю та граф напряму (без додаткових складних функцій)
	for a in elements:
		row = []
		for b in elements:
			if b % a == 0:
				row.append(1)
				edges.append((a, b))
			else:
				row.append(0)
		matrix.append(row)

	print(f"Множина S: {elements}")
	print("Матриця відношення:")
	print_matrix(matrix)
	print("Орієнтований граф (список дуг):")
	print(edges)


def swap_ends_in_place(arr):
	left, right = 0, len(arr) - 1
	while left < right:
		arr[left], arr[right] = arr[right], arr[left]
		left += 1
		right -= 1


def task_2_demo():
	print("\n=== Завдання 2 ===")
	a = [random.randint(-50, 50) for _ in range(27)]
	print("Початковий A(27):")
	print(a)
	swap_ends_in_place(a)
	print("Після обміну елементів:")
	print(a)


def transpose_in_place(matrix):
	"""Транспонування квадратної матриці на місці (без додаткової матриці)."""
	n = len(matrix)
	for i in range(n):
		for j in range(i + 1, n):
			matrix[i][j], matrix[j][i] = matrix[j][i], matrix[i][j]


def task_3_demo():
	print("\n=== Завдання 3 ===")
	n = 10
	matrix = [[random.randint(0, 9) for _ in range(n)] for _ in range(n)]

	print("Початкова матриця A(10,10):")
	print_matrix(matrix)

	transpose_in_place(matrix)

	print("Транспонована матриця:")
	print_matrix(matrix)


def find_saddle_points(matrix):
	"""
	Повертає список сідлових точок у форматі (i, j, value),
	де i, j — індекси з 1.
	"""
	n = len(matrix)

	# Мінімум у кожному рядку
	row_mins = []
	for i in range(n):
		row_mins.append(min(matrix[i]))

	# Максимум у кожному стовпці
	col_maxs = []
	for j in range(n):
		column_values = []
		for i in range(n):
			column_values.append(matrix[i][j])
		col_maxs.append(max(column_values))

	saddle_points = []
	for i in range(n):
		for j in range(n):
			value = matrix[i][j]
			if value == row_mins[i] and value == col_maxs[j]:
				saddle_points.append((i + 1, j + 1, value))

	return saddle_points


def task_4_demo():
	print("\n=== Завдання 4 ===")
	n = 5
	matrix = [[random.randint(-9, 9) for _ in range(n)] for _ in range(n)]

	print("Матриця A(n,n):")
	print_matrix(matrix)

	points = find_saddle_points(matrix)
	if points:
		print("Знайдено сідлові точки (рядок, стовпець, значення):")
		for point in points:
			print(point)
	else:
		print("Матриця не має сідлової точки.")


def generate_m_first_k(k):
	"""
	Генерує перші k чисел множини M у зростаючому порядку:
	1 ∈ M; X ∈ M => 2X+1, 3X+1 ∈ M; без повторів.
	"""
	result = []
	candidates = {1}

	while len(result) < k:
		# Беремо найменше доступне число з множини кандидатів
		x = min(candidates)
		candidates.remove(x)
		result.append(x)

		# За правилом додаємо наступні числа
		candidates.add(2 * x + 1)
		candidates.add(3 * x + 1)

	return result


def task_5_demo():
	print("\n=== Завдання 5 ===")
	a = generate_m_first_k(100)
	print("Масив A(100) (перші 100 чисел множини M):")
	print(a)


def run_all_tasks():
	task_1_demo()
	task_2_demo()
	task_3_demo()
	task_4_demo()
	task_5_demo()


def main():
	random.seed(42)
	print("ЛАБОРАТОРНА РОБОТА №2. МАТРИЦІ ТА МАСИВИ")

	while True:
		print("\nОберіть дію:")
		print("1 - Завдання 1 (матриця і граф відношення)")
		print("2 - Завдання 2 (обмін елементів A(27))")
		print("3 - Завдання 3 (транспонування A(10,10))")
		print("4 - Завдання 4 (сідлова точка матриці)")
		print("5 - Завдання 5 (формування A(100) множини M)")
		print("6 - Запустити всі завдання")
		print("0 - Вихід")

		choice = input("Ваш вибір: ").strip()

		if choice == "1":
			task_1_demo()
		elif choice == "2":
			task_2_demo()
		elif choice == "3":
			task_3_demo()
		elif choice == "4":
			task_4_demo()
		elif choice == "5":
			task_5_demo()
		elif choice == "6":
			run_all_tasks()
		elif choice == "0":
			print("Готово")
			break
		else:
			print("Невірний вибір.")


if __name__ == "__main__":
	main()
