step = 0


def hanoi(n, start, helper, end):
	global step

	if n == 1:
		step += 1
		print(f"Крок {step}: {start} -> {end}")
		return

	hanoi(n - 1, start, end, helper)
	step += 1
	print(f"Крок {step}: {start} -> {end}")
	hanoi(n - 1, helper, start, end)


n = int(input("Введіть кількість дисків N: "))

print("\nПослідовність переміщень:")
hanoi(n, 1, 2, 3)

print(f"\nМінімальна кількість ходів (2^N - 1): {2 ** n - 1}")
print(f"Надруковано ходів: {step}")
