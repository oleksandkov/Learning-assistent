"""
Графічний застосунок (Desktop App) для Лабораторної роботи №1.
Тема: 'Основні поняття та загальні складові алгоритмів'
Варіант: 'Касовий чек із накопичувальною знижкою'
Автор: Студент
Використання: python app.py
"""

import os
import webbrowser
import tkinter as tk
from tkinter import ttk, messagebox
from typing import List

from algorithm import ProductItem, ReceiptAlgorithm, format_receipt_text
from draw_flowchart import generate_svg


class CashRegisterApp(tk.Tk):
    """Головне вікно настільного касового терміналу."""

    def __init__(self):
        super().__init__()
        self.title("Лабораторна робота №1 — Касовий чек із накопичувальною знижкою")
        self.geometry("1080x700")
        self.minsize(950, 600)

        # Список товарів
        self.items: List[ProductItem] = []

        # Налаштування стилів ttk
        self._setup_styles()

        # Створення компонентів інтерфейсу
        self._create_layout()

        # Завантаження початкових демонстраційних даних
        self._load_demo_data()

    def _setup_styles(self):
        self.style = ttk.Style(self)
        try:
            self.style.theme_use("clam")
        except tk.TclError:
            pass

        # Налаштування кольорів та шрифтів
        self.style.configure(".", font=("Segoe UI", 10))
        self.style.configure("Treeview.Heading", font=("Segoe UI", 10, "bold"))
        self.style.configure("Action.TButton", font=("Segoe UI", 10, "bold"), padding=6)
        self.style.configure("Calculate.TButton", font=("Segoe UI", 11, "bold"), foreground="#0d47a1", padding=8)
        self.style.configure("Header.TLabel", font=("Segoe UI", 13, "bold"), foreground="#1a237e")

    def _create_layout(self):
        # Головний горизонтальний розділювач PanedWindow
        main_pane = ttk.PanedWindow(self, orient=tk.HORIZONTAL)
        main_pane.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # -------------------------------------------------------------
        # ЛІВА ПАНЕЛЬ: ВВЕДЕННЯ ТА ТАБЛИЦЯ ТОВАРІВ
        # -------------------------------------------------------------
        left_frame = ttk.Frame(main_pane, padding=5)
        main_pane.add(left_frame, weight=5)

        # Заголовок
        lbl_left_title = ttk.Label(
            left_frame,
            text="🛒 Формування кошика покупок (Вхідні дані)",
            style="Header.TLabel"
        )
        lbl_left_title.pack(anchor=tk.W, pady=(0, 10))

        # Фрейм введення нового товару
        input_group = ttk.LabelFrame(left_frame, text="Додати новий товар до чека", padding=10)
        input_group.pack(fill=tk.X, pady=(0, 10))

        # Рядок введення: Назва, Ціна, Кількість
        f_row1 = ttk.Frame(input_group)
        f_row1.pack(fill=tk.X, pady=2)

        ttk.Label(f_row1, text="Назва:").grid(row=0, column=0, sticky=tk.W, padx=2)
        self.ent_name = ttk.Entry(f_row1, width=22)
        self.ent_name.grid(row=0, column=1, sticky=tk.W, padx=4)

        ttk.Label(f_row1, text="Ціна (грн):").grid(row=0, column=2, sticky=tk.W, padx=(10, 2))
        self.ent_price = ttk.Entry(f_row1, width=10)
        self.ent_price.grid(row=0, column=3, sticky=tk.W, padx=4)

        ttk.Label(f_row1, text="К-сть (шт):").grid(row=0, column=4, sticky=tk.W, padx=(10, 2))
        self.ent_qty = ttk.Entry(f_row1, width=6)
        self.ent_qty.grid(row=0, column=5, sticky=tk.W, padx=4)
        self.ent_qty.insert(0, "1")

        # Кнопки для роботи з товарами
        f_btns = ttk.Frame(input_group)
        f_btns.pack(fill=tk.X, pady=(8, 0))

        btn_add = ttk.Button(f_btns, text="➕ Додати товар", command=self._add_item, style="Action.TButton")
        btn_add.pack(side=tk.LEFT, padx=3)

        btn_del = ttk.Button(f_btns, text="🗑️ Видалити вибране", command=self._delete_selected)
        btn_del.pack(side=tk.LEFT, padx=3)

        btn_clear = ttk.Button(f_btns, text="🧹 Очистити все", command=self._clear_items)
        btn_clear.pack(side=tk.LEFT, padx=3)

        btn_demo = ttk.Button(f_btns, text="🎲 Тестовий набір", command=self._load_demo_data)
        btn_demo.pack(side=tk.RIGHT, padx=3)

        # Таблиця товарів (Treeview)
        table_frame = ttk.Frame(left_frame)
        table_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 10))

        columns = ("id", "name", "price", "qty", "cost")
        self.tree = ttk.Treeview(table_frame, columns=columns, show="headings", height=10)
        self.tree.heading("id", text="№")
        self.tree.heading("name", text="Назва товару")
        self.tree.heading("price", text="Ціна за од. (грн)")
        self.tree.heading("qty", text="Кількість")
        self.tree.heading("cost", text="Вартість (грн)")

        self.tree.column("id", width=35, anchor=tk.CENTER)
        self.tree.column("name", width=180, anchor=tk.W)
        self.tree.column("price", width=110, anchor=tk.E)
        self.tree.column("qty", width=70, anchor=tk.CENTER)
        self.tree.column("cost", width=110, anchor=tk.E)

        scrollbar = ttk.Scrollbar(table_frame, orient=tk.VERTICAL, command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # Параметри алгоритму (Пороги знижок)
        thresh_group = ttk.LabelFrame(left_frame, text="⚙️ Параметри алгоритму (Правила розгалуження)", padding=8)
        thresh_group.pack(fill=tk.X, pady=(0, 10))

        f_thr1 = ttk.Frame(thresh_group)
        f_thr1.pack(fill=tk.X, pady=2)
        ttk.Label(f_thr1, text="Поріг знижки на товар (10%):").pack(side=tk.LEFT, padx=2)
        self.ent_item_thresh = ttk.Entry(f_thr1, width=8)
        self.ent_item_thresh.pack(side=tk.LEFT, padx=5)
        self.ent_item_thresh.insert(0, "500.0")
        ttk.Label(f_thr1, text="грн").pack(side=tk.LEFT)

        ttk.Label(f_thr1, text="Поріг знижки на чек (5%):").pack(side=tk.LEFT, padx=(20, 2))
        self.ent_receipt_thresh = ttk.Entry(f_thr1, width=8)
        self.ent_receipt_thresh.pack(side=tk.LEFT, padx=5)
        self.ent_receipt_thresh.insert(0, "1500.0")
        ttk.Label(f_thr1, text="грн").pack(side=tk.LEFT)

        # Кнопки дій алгоритму
        btn_calc = ttk.Button(
            left_frame,
            text="⚡ ВИКОНАТИ АЛГОРИТМ ТА РОЗРАХУВАТИ ЧЕК",
            command=self._execute_algorithm,
            style="Calculate.TButton"
        )
        btn_calc.pack(fill=tk.X, ipady=4, pady=(0, 4))

        btn_flowchart = ttk.Button(
            left_frame,
            text="📐 Відкрити векторну блок-схему (SVG / ГОСТ)",
            command=self._open_flowchart
        )
        btn_flowchart.pack(fill=tk.X, ipady=2)

        # -------------------------------------------------------------
        # ПРАВА ПАНЕЛЬ: РЕЗУЛЬТАТИ ТА ВІЗУАЛІЗАЦІЯ ДІЛЯНОК
        # -------------------------------------------------------------
        right_frame = ttk.Frame(main_pane, padding=5)
        main_pane.add(right_frame, weight=5)

        lbl_right_title = ttk.Label(
            right_frame,
            text="📊 Вихідні результати та трасування ділянок",
            style="Header.TLabel"
        )
        lbl_right_title.pack(anchor=tk.W, pady=(0, 10))

        # Вкладки Notebook
        self.notebook = ttk.Notebook(right_frame)
        self.notebook.pack(fill=tk.BOTH, expand=True)

        # Вкладка 1: Касовий чек
        tab_receipt = ttk.Frame(self.notebook, padding=8)
        self.notebook.add(tab_receipt, text="🧾 Фіскальний чек")

        self.txt_receipt = tk.Text(
            tab_receipt,
            font=("Consolas", 10),
            bg="#fdfefe",
            fg="#1b2631",
            relief=tk.SOLID,
            borderwidth=1,
            padx=10,
            pady=10
        )
        self.txt_receipt.pack(fill=tk.BOTH, expand=True)

        # Вкладка 2: Трасування виконання 3 ділянок
        tab_trace = ttk.Frame(self.notebook, padding=8)
        self.notebook.add(tab_trace, text="🔍 Трасування трьох ділянок")

        # Панель легенди
        legend_frame = ttk.Frame(tab_trace)
        legend_frame.pack(fill=tk.X, pady=(0, 6))

        lbl_leg1 = tk.Label(legend_frame, text=" 🟢 Лінійна ділянка ", bg="#e8f5e9", fg="#1b5e20", font=("Segoe UI", 9, "bold"))
        lbl_leg1.pack(side=tk.LEFT, padx=3)
        lbl_leg2 = tk.Label(legend_frame, text=" 🔄 Циклічна ділянка ", bg="#e3f2fd", fg="#0d47a1", font=("Segoe UI", 9, "bold"))
        lbl_leg2.pack(side=tk.LEFT, padx=3)
        lbl_leg3 = tk.Label(legend_frame, text=" 🔀 Розгалужена ділянка ", bg="#fff3e0", fg="#e65100", font=("Segoe UI", 9, "bold"))
        lbl_leg3.pack(side=tk.LEFT, padx=3)

        self.txt_trace = tk.Text(
            tab_trace,
            font=("Consolas", 9),
            bg="#ffffff",
            fg="#212121",
            relief=tk.SOLID,
            borderwidth=1,
            padx=8,
            pady=8
        )
        self.txt_trace.pack(fill=tk.BOTH, expand=True)

        # Налаштування кольорових тегів для тексту логу
        self.txt_trace.tag_config("linear", foreground="#1b5e20", font=("Consolas", 9, "bold"))
        self.txt_trace.tag_config("loop", foreground="#0d47a1", font=("Consolas", 9, "bold"))
        self.txt_trace.tag_config("branch_yes", foreground="#e65100", font=("Consolas", 9, "bold"))
        self.txt_trace.tag_config("branch_no", foreground="#546e7a", font=("Consolas", 9))
        self.txt_trace.tag_config("normal", foreground="#37474f", font=("Consolas", 9))

        # Вкладка 3: Довідка за лабораторною роботою
        tab_info = ttk.Frame(self.notebook, padding=10)
        self.notebook.add(tab_info, text="ℹ️ Про лаб. роботу")

        info_text = (
            "ЛАБОРАТОРНА РОБОТА №1\n"
            "Тема: Основні поняття та загальні складові алгоритмів\n\n"
            "У даній програмі реалізовано три базові керуючі структури алгоритму:\n\n"
            "1. ЛІНІЙНА ДІЛЯНКА (послідовне виконання дій):\n"
            "   - Зчитування початкових параметрів і порогів знижок.\n"
            "   - Ініціалізація підсумкових змінних-накопичувачів.\n"
            "   - Фінальний розрахунок суми до сплати, ПДВ (20%) та підготовка друку чека.\n\n"
            "2. ЦИКЛІЧНА ДІЛЯНКА (повторення операцій):\n"
            "   - Цикл for i = 1..N для послідовного обходу всіх N товарів у списку.\n"
            "   - Розрахунок вартості кожної позиції та накопичення загальної суми.\n\n"
            "3. РОЗГАЛУЖЕНА ДІЛЯНКА (прийняття рішень if-else):\n"
            "   - Умова 1 (у циклі): Якщо ціна товару >= 500 грн -> знижка 10% на цей товар.\n"
            "   - Умова 2 (після циклу): Якщо проміжна вартість чека >= 1500 грн ->\n"
            "     надання накопичувальної знижки 5% на весь чек.\n\n"
            "Форми представлення алгоритму у звіті: Чорна/біла скринька, Блок-схема, Код з коментарями."
        )
        lbl_info = tk.Label(tab_info, text=info_text, justify=tk.LEFT, font=("Segoe UI", 10), anchor=tk.NW)
        lbl_info.pack(fill=tk.BOTH, expand=True)

    def _add_item(self):
        name = self.ent_name.get().strip()
        if not name:
            messagebox.showwarning("Помилка введення", "Будь ласка, введіть назву товару!")
            self.ent_name.focus()
            return

        try:
            price = float(self.ent_price.get().replace(",", "."))
            if price <= 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Помилка введення", "Ціна повинна бути додатним числовим значенням!")
            self.ent_price.focus()
            return

        try:
            qty = int(self.ent_qty.get())
            if qty <= 0:
                raise ValueError
        except ValueError:
            messagebox.showerror("Помилка введення", "Кількість повинна бути цілим додатним числом!")
            self.ent_qty.focus()
            return

        item = ProductItem(name=name, price=price, quantity=qty)
        self.items.append(item)
        self._refresh_table()

        # Очищення полів введення
        self.ent_name.delete(0, tk.END)
        self.ent_price.delete(0, tk.END)
        self.ent_qty.delete(0, tk.END)
        self.ent_qty.insert(0, "1")
        self.ent_name.focus()

    def _delete_selected(self):
        selected = self.tree.selection()
        if not selected:
            messagebox.showinfo("Підказка", "Виберіть у таблиці товар, який бажаєте видалити.")
            return

        for item_id in selected:
            idx = int(self.tree.item(item_id, "values")[0]) - 1
            if 0 <= idx < len(self.items):
                del self.items[idx]
        self._refresh_table()

    def _clear_items(self):
        self.items.clear()
        self._refresh_table()
        self.txt_receipt.delete("1.0", tk.END)
        self.txt_trace.delete("1.0", tk.END)

    def _refresh_table(self):
        # Очищення таблиці
        for row in self.tree.get_children():
            self.tree.delete(row)

        # Заповнення актуальними даними
        for idx, it in enumerate(self.items, start=1):
            self.tree.insert(
                "",
                tk.END,
                values=(idx, it.name, f"{it.price:.2f}", it.quantity, f"{it.cost_raw:.2f}")
            )

    def _load_demo_data(self):
        """Завантаження репрезентативного набору даних для демонстрації всіх трьох гілок."""
        self.items = [
            ProductItem(name="Кава зернова 1кг", price=620.0, quantity=1),   # >= 500 (знижка на товар)
            ProductItem(name="Шоколад чорний", price=85.0, quantity=3),     # < 500
            ProductItem(name="Оливкова олія", price=540.0, quantity=2),     # >= 500 (знижка на товар)
            ProductItem(name="Чай зелений", price=120.0, quantity=1)        # < 500
        ]
        self._refresh_table()
        self._execute_algorithm()

    def _execute_algorithm(self):
        """Запуск алгоритму розрахунку та оновлення відображення."""
        if not self.items:
            messagebox.showwarning("Увага", "Список товарів порожній. Додайте товари для розрахунку.")
            return

        try:
            item_th = float(self.ent_item_thresh.get().replace(",", "."))
            rec_th = float(self.ent_receipt_thresh.get().replace(",", "."))
        except ValueError:
            messagebox.showerror("Помилка налаштувань", "Пороги знижок мають бути числовими значеннями!")
            return

        # Створення екземпляра алгоритму та виконання
        algo = ReceiptAlgorithm(
            item_discount_threshold=item_th,
            receipt_discount_threshold=rec_th
        )
        res, trace_log = algo.calculate(self.items)

        # Оновлення тексту чека
        receipt_str = format_receipt_text(res)
        self.txt_receipt.delete("1.0", tk.END)
        self.txt_receipt.insert(tk.END, receipt_str)

        # Оновлення тексту трасування ділянок із підсвічуванням
        self.txt_trace.delete("1.0", tk.END)
        for line in trace_log:
            if "[ЛІНІЙНА ДІЛЯНКА]" in line:
                self.txt_trace.insert(tk.END, line + "\n", "linear")
            elif "[ЦИКЛІЧНА ДІЛЯНКА]" in line or "[ЦИКЛ:" in line:
                self.txt_trace.insert(tk.END, line + "\n", "loop")
            elif "[РОЗГАЛУЖЕННЯ - ТАК]" in line or "[РОЗГАЛУЖЕНА ДІЛЯНКА]" in line:
                self.txt_trace.insert(tk.END, line + "\n", "branch_yes")
            elif "[РОЗГАЛУЖЕННЯ - НІ]" in line:
                self.txt_trace.insert(tk.END, line + "\n", "branch_no")
            else:
                self.txt_trace.insert(tk.END, line + "\n", "normal")

        # Перемикання на вкладку чека
        self.notebook.select(0)

    def _open_flowchart(self):
        """Відкриття векторної блок-схеми у системній програмі перегляду або браузері."""
        svg_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "flowchart.svg")
        if not os.path.exists(svg_path):
            generate_svg(svg_path)

        try:
            # На Windows os.startfile відкриває асоційовану програму (Edge, Chrome тощо)
            if hasattr(os, "startfile"):
                os.startfile(svg_path)
            else:
                webbrowser.open(f"file://{svg_path}")
        except Exception as e:
            messagebox.showerror("Помилка відкриття", f"Не вдалося відкрити файл блок-схеми:\n{e}")


if __name__ == "__main__":
    app = CashRegisterApp()
    app.mainloop()
