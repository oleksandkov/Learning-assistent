"""
main.py
Графічний інтерфейс користувача (GUI) для Лабораторної роботи №1.
Тема: Методи переведення чисел з однієї системи числення в іншу.
Завдання 2: Переведення цілих або дробових чисел з двійкової у десяткову систему числення.
"""

import tkinter as tk
from tkinter import ttk, messagebox
from decimal import Decimal
from converter import convert_binary_to_decimal, format_decimal_result, ConversionResult


class BinaryDecimalConverterApp:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("Лабораторна робота №1 | Конвертер двійкових чисел у десяткові")
        self.root.geometry("820x720")
        self.root.minsize(700, 600)

        # Налаштування кольорової схеми та стилів
        self.bg_color = "#f8fafc"
        self.card_bg = "#ffffff"
        self.primary_color = "#2563eb"
        self.primary_hover = "#1d4ed8"
        self.secondary_color = "#64748b"
        self.text_color = "#1e293b"
        self.text_muted = "#64748b"
        self.border_color = "#cbd5e1"
        self.success_color = "#16a34a"
        self.error_color = "#dc2626"

        self.root.configure(bg=self.bg_color)
        self.setup_styles()
        self.create_widgets()

    def setup_styles(self):
        self.style = ttk.Style()
        try:
            self.style.theme_use("clam")
        except Exception:
            pass

        # Загальні стилі віджетів ttk
        self.style.configure(".", background=self.bg_color, foreground=self.text_color, font=("Segoe UI", 10))
        self.style.configure("Card.TFrame", background=self.card_bg, relief="flat")
        self.style.configure("Header.TLabel", font=("Segoe UI", 15, "bold"), foreground="#0f172a", background=self.card_bg)
        self.style.configure("SubHeader.TLabel", font=("Segoe UI", 9), foreground=self.text_muted, background=self.card_bg)
        self.style.configure("FieldLabel.TLabel", font=("Segoe UI", 10, "bold"), foreground=self.text_color, background=self.card_bg)
        self.style.configure("ResultLabel.TLabel", font=("Segoe UI", 11, "bold"), foreground="#0f172a", background=self.card_bg)
        self.style.configure("Formula.TLabel", font=("Consolas", 10), foreground="#0369a1", background="#f0f9ff")

        # Стиль таблиці Treeview
        self.style.configure(
            "Treeview",
            background="#ffffff",
            foreground=self.text_color,
            fieldbackground="#ffffff",
            rowheight=26,
            font=("Segoe UI", 9)
        )
        self.style.configure(
            "Treeview.Heading",
            font=("Segoe UI", 9, "bold"),
            background="#e2e8f0",
            foreground="#334155"
        )
        self.style.map("Treeview", background=[("selected", "#dbeafe")], foreground=[("selected", "#1e3a8a")])

    def create_widgets(self):
        # Головний скрольований контейнер
        main_frame = tk.Frame(self.root, bg=self.bg_color)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=16)

        # ================= 1. ШАПКА =================
        header_card = tk.Frame(main_frame, bg=self.card_bg, bd=1, relief=tk.SOLID, highlightbackground=self.border_color)
        header_card.pack(fill=tk.X, pady=(0, 12), ipady=8, ipadx=14)

        title_lbl = ttk.Label(
            header_card,
            text="Калькулятор переведення з двійкової у десяткову систему",
            style="Header.TLabel"
        )
        title_lbl.pack(anchor="w")

        subtitle_lbl = ttk.Label(
            header_card,
            text="Лабораторна робота №1 з дисципліни «Архітектура комп'ютера» | Метод розкладу за поліномом степенів 2",
            style="SubHeader.TLabel"
        )
        subtitle_lbl.pack(anchor="w", pady=(2, 0))

        # ================= 2. БЛОК ВВЕДЕННЯ =================
        input_card = tk.Frame(main_frame, bg=self.card_bg, bd=1, relief=tk.SOLID, highlightbackground=self.border_color)
        input_card.pack(fill=tk.X, pady=(0, 12), padx=0, ipady=10, ipadx=14)

        input_title = ttk.Label(
            input_card,
            text="Введіть двійкове число (ціле або дробове):",
            style="FieldLabel.TLabel"
        )
        input_title.pack(anchor="w")

        # Поле вводу та кнопки
        entry_row = tk.Frame(input_card, bg=self.card_bg)
        entry_row.pack(fill=tk.X, pady=(8, 8))

        self.input_var = tk.StringVar(value="110100101")
        self.entry = tk.Entry(
            entry_row,
            textvariable=self.input_var,
            font=("Consolas", 13),
            bd=1,
            relief=tk.SOLID,
            highlightthickness=1,
            highlightcolor=self.primary_color,
            highlightbackground=self.border_color,
            fg="#0f172a",
            bg="#ffffff"
        )
        self.entry.pack(side=tk.LEFT, fill=tk.X, expand=True, ipady=6, padx=(0, 8))
        self.entry.bind("<Return>", lambda event: self.calculate())
        self.entry.focus_set()

        # Кнопка переведення
        calc_btn = tk.Button(
            entry_row,
            text="Перевести ➔",
            command=self.calculate,
            bg=self.primary_color,
            fg="#ffffff",
            activebackground=self.primary_hover,
            activeforeground="#ffffff",
            font=("Segoe UI", 10, "bold"),
            relief=tk.FLAT,
            cursor="hand2",
            padx=16,
            pady=6
        )
        calc_btn.pack(side=tk.LEFT, padx=(0, 6))

        # Кнопка очищення
        clear_btn = tk.Button(
            entry_row,
            text="Очистити",
            command=self.clear_input,
            bg="#f1f5f9",
            fg="#475569",
            activebackground="#e2e8f0",
            activeforeground="#1e293b",
            font=("Segoe UI", 10),
            relief=tk.FLAT,
            cursor="hand2",
            padx=12,
            pady=6
        )
        clear_btn.pack(side=tk.LEFT)

        # Рядок швидких прикладів
        presets_row = tk.Frame(input_card, bg=self.card_bg)
        presets_row.pack(fill=tk.X, pady=(4, 0))

        preset_lbl = tk.Label(presets_row, text="Швидкі приклади:", bg=self.card_bg, fg=self.text_muted, font=("Segoe UI", 8))
        preset_lbl.pack(side=tk.LEFT, padx=(0, 8))

        samples = [
            ("110100101 (Методичка)", "110100101"),
            ("1011.1101 (Дріб)", "1011.1101"),
            ("-101.011 (Від'ємне)", "-101.011"),
            ("0.0011 (Малий дріб)", "0.0011"),
            ("11111111 (255)", "11111111")
        ]
        for title, val in samples:
            btn = tk.Button(
                presets_row,
                text=title,
                command=lambda v=val: self.load_preset(v),
                bg="#f8fafc",
                fg="#2563eb",
                activebackground="#e0f2fe",
                font=("Segoe UI", 8),
                relief=tk.FLAT,
                cursor="hand2",
                padx=6,
                pady=2
            )
            btn.pack(side=tk.LEFT, padx=3)

        # ================= 3. БЛОК РЕЗУЛЬТАТУ =================
        result_card = tk.Frame(main_frame, bg=self.card_bg, bd=1, relief=tk.SOLID, highlightbackground=self.border_color)
        result_card.pack(fill=tk.X, pady=(0, 12), ipady=10, ipadx=14)

        res_header_row = tk.Frame(result_card, bg=self.card_bg)
        res_header_row.pack(fill=tk.X)

        res_title = ttk.Label(res_header_row, text="Результат переведення:", style="FieldLabel.TLabel")
        res_title.pack(side=tk.LEFT)

        self.copy_btn = tk.Button(
            res_header_row,
            text="📋 Скопіювати результат",
            command=self.copy_result,
            bg="#f8fafc",
            fg="#0f172a",
            activebackground="#e2e8f0",
            font=("Segoe UI", 9),
            relief=tk.FLAT,
            cursor="hand2",
            padx=8,
            pady=2
        )
        self.copy_btn.pack(side=tk.RIGHT)

        # Головне поле виведення результату
        self.result_display_var = tk.StringVar(value="Введіть число та натисніть «Перевести»")
        self.result_label = tk.Label(
            result_card,
            textvariable=self.result_display_var,
            font=("Segoe UI", 16, "bold"),
            bg="#f0fdf4",
            fg="#15803d",
            padx=12,
            pady=8,
            anchor="w",
            relief=tk.SOLID,
            bd=1
        )
        self.result_label.pack(fill=tk.X, pady=(8, 8))

        # Формула розкладу за поліномом
        poly_title = tk.Label(result_card, text="Математичний розклад за поліномом:", bg=self.card_bg, fg=self.text_muted, font=("Segoe UI", 9, "bold"))
        poly_title.pack(anchor="w", pady=(2, 2))

        self.formula_text = tk.Text(
            result_card,
            height=2,
            font=("Consolas", 10),
            bg="#f8fafc",
            fg="#0369a1",
            bd=1,
            relief=tk.SOLID,
            wrap=tk.WORD,
            padx=8,
            pady=6
        )
        self.formula_text.pack(fill=tk.X)
        self.formula_text.insert(tk.END, "N₁₀ = ∑ (bᵢ · 2ⁱ)")
        self.formula_text.config(state=tk.DISABLED)

        # ================= 4. ПОКРОКОВА ТАБЛИЦЯ РОЗКЛАДУ =================
        table_card = tk.Frame(main_frame, bg=self.card_bg, bd=1, relief=tk.SOLID, highlightbackground=self.border_color)
        table_card.pack(fill=tk.BOTH, expand=True, pady=(0, 8), ipady=8, ipadx=14)

        table_title = ttk.Label(table_card, text="Покрокова деталізація внеску кожного розряду:", style="FieldLabel.TLabel")
        table_title.pack(anchor="w", pady=(0, 6))

        columns = ("part", "pos", "bit", "power_formula", "power_val", "contrib")
        self.tree = ttk.Treeview(table_card, columns=columns, show="headings", height=8)

        self.tree.heading("part", text="Частина числа")
        self.tree.heading("pos", text="Степінь (i)")
        self.tree.heading("bit", text="Біт (bᵢ)")
        self.tree.heading("power_formula", text="Степінь двійки")
        self.tree.heading("power_val", text="Вага розряду (2ⁱ)")
        self.tree.heading("contrib", text="Внесок (bᵢ · 2ⁱ)")

        self.tree.column("part", width=120, anchor="center")
        self.tree.column("pos", width=90, anchor="center")
        self.tree.column("bit", width=80, anchor="center")
        self.tree.column("power_formula", width=110, anchor="center")
        self.tree.column("power_val", width=150, anchor="e")
        self.tree.column("contrib", width=150, anchor="e")

        tree_scroll = ttk.Scrollbar(table_card, orient="vertical", command=self.tree.yview)
        self.tree.configure(yscrollcommand=tree_scroll.set)

        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tree_scroll.pack(side=tk.RIGHT, fill=tk.Y)

        # ================= 5. СТАТУСНИЙ РЯДОК =================
        self.status_var = tk.StringVar(value="Готово до роботи.")
        status_bar = tk.Label(
            self.root,
            textvariable=self.status_var,
            font=("Segoe UI", 9),
            bg="#e2e8f0",
            fg="#475569",
            anchor="w",
            padx=12,
            pady=4
        )
        status_bar.pack(fill=tk.X, side=tk.BOTTOM)

        # Розраховуємо початкове значення
        self.calculate()

    def load_preset(self, val: str):
        self.input_var.set(val)
        self.calculate()

    def clear_input(self):
        self.input_var.set("")
        self.result_display_var.set("Введіть число та натисніть «Перевести»")
        self.result_label.configure(bg="#f8fafc", fg=self.text_color)
        self.formula_text.config(state=tk.NORMAL)
        self.formula_text.delete("1.0", tk.END)
        self.formula_text.insert(tk.END, "N₁₀ = ∑ (bᵢ · 2ⁱ)")
        self.formula_text.config(state=tk.DISABLED)
        for row in self.tree.get_children():
            self.tree.delete(row)
        self.status_var.set("Поле очищено.")
        self.entry.focus_set()

    def copy_result(self):
        res_text = self.result_display_var.get()
        if res_text and not res_text.startswith("Помилка") and not res_text.startswith("Введіть"):
            # Витягуємо тільки числовий результат
            clean_res = res_text.replace("Результат:", "").replace("₁₀", "").strip()
            self.root.clipboard_clear()
            self.root.clipboard_append(clean_res)
            self.status_var.set(f"Результат {clean_res} скопійовано в буфер обміну! ✓")
        else:
            self.status_var.set("Немає результату для копіювання.")

    def calculate(self):
        val = self.input_var.get().strip()
        if not val:
            self.result_display_var.set("Помилка: введіть двійкове число!")
            self.result_label.configure(bg="#fef2f2", fg=self.error_color)
            self.status_var.set("Поле введення порожнє.")
            return

        try:
            res: ConversionResult = convert_binary_to_decimal(val)
        except ValueError as ex:
            self.result_display_var.set(f"{ex}")
            self.result_label.configure(bg="#fef2f2", fg=self.error_color)
            self.status_var.set(f"Помилка: {ex}")
            # Очищуємо формулу та таблицю
            self.formula_text.config(state=tk.NORMAL)
            self.formula_text.delete("1.0", tk.END)
            self.formula_text.insert(tk.END, "Помилка валідації вхідних даних.")
            self.formula_text.config(state=tk.DISABLED)
            for row in self.tree.get_children():
                self.tree.delete(row)
            return

        # Відображення успішного результату
        formatted_dec = format_decimal_result(res.total_decimal)
        self.result_display_var.set(f"Результат:  {formatted_dec}₁₀  (з {val}₂)")
        self.result_label.configure(bg="#f0fdf4", fg="#15803d")

        # Оновлення формули
        self.formula_text.config(state=tk.NORMAL)
        self.formula_text.delete("1.0", tk.END)
        self.formula_text.insert(
            tk.END,
            f"Формула: {res.polynomial_formula}\n"
            f"Значення: {res.polynomial_with_values} = {formatted_dec}₁₀"
        )
        self.formula_text.config(state=tk.DISABLED)

        # Оновлення таблиці
        for row in self.tree.get_children():
            self.tree.delete(row)

        for c in res.integer_contributions:
            self.tree.insert("", tk.END, values=(
                "Ціла частина",
                f"2^{c.index}",
                c.bit,
                f"2^{c.index}",
                format_decimal_result(c.power_value),
                format_decimal_result(c.contribution)
            ))

        for c in res.fractional_contributions:
            self.tree.insert("", tk.END, values=(
                "Дробова частина",
                f"2^{c.index}",
                c.bit,
                f"2^{c.index} (1/2^{abs(c.index)})",
                format_decimal_result(c.power_value),
                format_decimal_result(c.contribution)
            ))

        self.status_var.set(f"Успішно переведено {val}₂ = {formatted_dec}₁₀ ✓")


def main():
    root = tk.Tk()
    app = BinaryDecimalConverterApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()

