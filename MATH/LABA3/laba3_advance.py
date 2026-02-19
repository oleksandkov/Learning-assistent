import tkinter as tk
from tkinter import ttk
from tkinter import messagebox


def build_hanoi_moves(n, start, helper, end, moves):
	if n == 1:
		moves.append((start, end))
		return

	build_hanoi_moves(n - 1, start, end, helper, moves)
	moves.append((start, end))
	build_hanoi_moves(n - 1, helper, start, end, moves)


def solve_hanoi():
	raw_value = disks_var.get().strip()

	if not raw_value.isdigit():
		messagebox.showerror("Помилка", "Введіть додатне ціле число N.")
		return

	n = int(raw_value)
	if n <= 0:
		messagebox.showerror("Помилка", "N має бути більше 0.")
		return

	if n > 15:
		answer = messagebox.askyesno(
			"Багато кроків",
			"Для такого N буде дуже багато кроків. Продовжити?",
		)
		if not answer:
			return

	moves = []
	build_hanoi_moves(n, 1, 2, 3, moves)

	output_text.config(state="normal")
	output_text.delete("1.0", tk.END)
	output_text.insert(tk.END, "Послідовність переміщень:\n\n")

	for step, (from_rod, to_rod) in enumerate(moves, start=1):
		output_text.insert(tk.END, f"Крок {step}: {from_rod} -> {to_rod}\n")

	output_text.config(state="disabled")

	minimum_moves = 2 ** n - 1
	total_moves = len(moves)

	min_moves_var.set(f"Мінімум за формулою 2^N - 1: {minimum_moves}")
	printed_moves_var.set(f"Надруковано ходів: {total_moves}")

	if total_moves == minimum_moves:
		status_var.set("✅ Розв'язок мінімальний і коректний.")
	else:
		status_var.set("⚠️ Перевірка: кількість ходів не збігається.")


def clear_all():
	disks_var.set("")
	min_moves_var.set("Мінімум за формулою 2^N - 1: -")
	printed_moves_var.set("Надруковано ходів: -")
	status_var.set("Готово до нового обчислення.")

	output_text.config(state="normal")
	output_text.delete("1.0", tk.END)
	output_text.config(state="disabled")


root = tk.Tk()
root.title("Лабораторна №3 — Ханойські башти (Advanced GUI)")
root.geometry("900x620")
root.minsize(760, 500)

style = ttk.Style()
style.theme_use("clam")
style.configure("TLabel", font=("Segoe UI", 11))
style.configure("Title.TLabel", font=("Segoe UI", 16, "bold"))
style.configure("Info.TLabel", font=("Segoe UI", 10), foreground="#334155")
style.configure("Accent.TButton", font=("Segoe UI", 10, "bold"))

main_frame = ttk.Frame(root, padding=16)
main_frame.pack(fill="both", expand=True)

header_label = ttk.Label(main_frame, text="Задача Ханойських башт", style="Title.TLabel")
header_label.pack(anchor="w")

subtitle_label = ttk.Label(
	main_frame,
	text="",
	style="Info.TLabel",
)
subtitle_label.pack(anchor="w", pady=(0, 12))

controls_frame = ttk.Frame(main_frame)
controls_frame.pack(fill="x", pady=(0, 10))

ttk.Label(controls_frame, text="Кількість дисків N:").grid(row=0, column=0, sticky="w")

disks_var = tk.StringVar()
entry_n = ttk.Entry(controls_frame, textvariable=disks_var, width=12)
entry_n.grid(row=0, column=1, padx=(8, 10), sticky="w")
entry_n.focus()

solve_button = ttk.Button(controls_frame, text="Розв'язати", style="Accent.TButton", command=solve_hanoi)
solve_button.grid(row=0, column=2, padx=(0, 8))

clear_button = ttk.Button(controls_frame, text="Очистити", command=clear_all)
clear_button.grid(row=0, column=3)

stats_frame = ttk.Frame(main_frame)
stats_frame.pack(fill="x", pady=(0, 10))

min_moves_var = tk.StringVar(value="Мінімум за формулою 2^N - 1: -")
printed_moves_var = tk.StringVar(value="Надруковано ходів: -")
status_var = tk.StringVar(value="Введіть N і натисніть «Розв'язати».")

ttk.Label(stats_frame, textvariable=min_moves_var).pack(anchor="w")
ttk.Label(stats_frame, textvariable=printed_moves_var).pack(anchor="w")
ttk.Label(stats_frame, textvariable=status_var, style="Info.TLabel").pack(anchor="w", pady=(4, 0))

output_frame = ttk.LabelFrame(main_frame, text="Результат")
output_frame.pack(fill="both", expand=True)

scrollbar = ttk.Scrollbar(output_frame)
scrollbar.pack(side="right", fill="y")

output_text = tk.Text(
	output_frame,
	wrap="none",
	font=("Consolas", 11),
	yscrollcommand=scrollbar.set,
	padx=10,
	pady=10,
)
output_text.pack(fill="both", expand=True)
output_text.config(state="disabled")

scrollbar.config(command=output_text.yview)

root.bind("<Return>", lambda event: solve_hanoi())
root.bind("<Escape>", lambda event: clear_all())

root.mainloop()
