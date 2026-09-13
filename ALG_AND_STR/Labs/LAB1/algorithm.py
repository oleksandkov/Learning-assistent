"""
Модуль: algorithm.py
Тема: Лабораторна робота №1. Основні поняття та загальні складові алгоритмів.
Призначення: Реалізація алгоритму 'Касовий чек із накопичувальною знижкою'
з явним виділенням трьох базових керуючих структур (ділянок):
  1. Лінійна ділянка (Linear section)
  2. Циклічна ділянка (Loop section)
  3. Розгалужена ділянка (Branching section)
"""

from dataclasses import dataclass
from typing import List, Dict, Any, Tuple


@dataclass
class ProductItem:
    """Модель даних товару в чеку."""
    name: str
    price: float
    quantity: int

    @property
    def cost_raw(self) -> float:
        """Початкова вартість позиції без знижки."""
        return self.price * self.quantity


@dataclass
class ProcessedItem:
    """Результат обробки товару з урахуванням знижок."""
    name: str
    price: float
    quantity: int
    cost_raw: float
    discount_percent: float
    discount_amount: float
    final_cost: float


class ReceiptAlgorithm:
    """
    Клас, що реалізує обчислювальний алгоритм формування чека.
    Містить три чітко розмежовані ділянки алгоритму.
    """

    def __init__(self, item_discount_threshold: float = 500.0,
                 item_discount_rate: float = 0.10,
                 receipt_discount_threshold: float = 1500.0,
                 receipt_discount_rate: float = 0.05,
                 vat_rate: float = 0.20):
        # Налаштування алгоритму (константи / параметри)
        self.item_discount_threshold = item_discount_threshold
        self.item_discount_rate = item_discount_rate
        self.receipt_discount_threshold = receipt_discount_threshold
        self.receipt_discount_rate = receipt_discount_rate
        self.vat_rate = vat_rate

    def calculate(self, items: List[ProductItem]) -> Tuple[Dict[str, Any], List[str]]:
        """
        Головний метод виконання алгоритму.
        Повертає підсумкові дані чека та покроковий лог (трасування) виконання ділянок.
        """
        trace_log = []

        # =========================================================================
        # ДІЛЯНКА 1: ЛІНІЙНА (ПОЧАТОК ТА ІНІЦІАЛІЗАЦІЯ)
        # =========================================================================
        trace_log.append("[ЛІНІЙНА ДІЛЯНКА] Крок 1: Ініціалізація початкових змінних-накопичувачів.")
        subtotal_raw = 0.0           # Загальна вартість товарів без знижок
        total_item_discount = 0.0    # Сума індивідуальних знижок на товари
        subtotal_after_item = 0.0    # Проміжна вартість після знижок на позиції
        processed_items: List[ProcessedItem] = []
        n = len(items)

        trace_log.append(f"[ЛІНІЙНА ДІЛЯНКА] Крок 2: Зчитано кількість позицій у чеку N = {n}.")

        # =========================================================================
        # ДІЛЯНКА 2: ЦИКЛІЧНА (ОБРОБКА КОЖНОГО ТОВАРУ У ЧЕКУ)
        # =========================================================================
        trace_log.append(f"[ЦИКЛІЧНА ДІЛЯНКА] Вхід у цикл for i = 1..{n} для обробки кожної позиції.")
        for idx, item in enumerate(items, start=1):
            trace_log.append(f"  [ЦИКЛ: Ітерація {idx}/{n}] Обробка товару '{item.name}', ціна: {item.price:.2f} грн, к-сть: {item.quantity}.")
            
            raw_cost = item.cost_raw
            subtotal_raw += raw_cost

            # =====================================================================
            # ДІЛЯНКА 3 (А): РОЗГАЛУЖЕНА (ПЕРЕВІРКА ПОРЕДУМОВИ ДЛЯ ЗНИЖКИ НА ТОВАР)
            # =====================================================================
            if item.price >= self.item_discount_threshold:
                # Гілка ТАК: товар дорожчий за поріг (преміум/гурт) -> знижка 10%
                disc_rate = self.item_discount_rate
                disc_val = raw_cost * disc_rate
                trace_log.append(
                    f"    [РОЗГАЛУЖЕННЯ - ТАК] Ціна {item.price:.2f} >= {self.item_discount_threshold:.2f} грн. "
                    f"Застосовано знижку {disc_rate * 100:.0f}% (-{disc_val:.2f} грн)."
                )
            else:
                # Гілка НІ: товар без індивідуальної знижки
                disc_rate = 0.0
                disc_val = 0.0
                trace_log.append(
                    f"    [РОЗГАЛУЖЕННЯ - НІ] Ціна {item.price:.2f} < {self.item_discount_threshold:.2f} грн. "
                    f"Індивідуальна знижка: 0%."
                )

            item_final = raw_cost - disc_val
            total_item_discount += disc_val
            subtotal_after_item += item_final

            processed_items.append(
                ProcessedItem(
                    name=item.name,
                    price=item.price,
                    quantity=item.quantity,
                    cost_raw=raw_cost,
                    discount_percent=disc_rate * 100,
                    discount_amount=disc_val,
                    final_cost=item_final
                )
            )

        trace_log.append(f"[ЦИКЛІЧНА ДІЛЯНКА] Завершення циклу. Всі {n} позицій успішно опрацьовано.")

        # =========================================================================
        # ДІЛЯНКА 3 (Б): РОЗГАЛУЖЕНА (ПЕРЕВІРКА НАКОПИЧУВАЛЬНОЇ ЗНИЖКИ НА ВЕСЬ ЧЕК)
        # =========================================================================
        trace_log.append(
            f"[РОЗГАЛУЖЕНА ДІЛЯНКА] Перевірка умови чека: Проміжна сума {subtotal_after_item:.2f} >= "
            f"Поріг {self.receipt_discount_threshold:.2f} грн?"
        )
        if subtotal_after_item >= self.receipt_discount_threshold:
            # Гілка ТАК: чек перевищує поріг -> знижка на чек 5%
            receipt_discount = subtotal_after_item * self.receipt_discount_rate
            receipt_discount_applied = True
            trace_log.append(
                f"  [РОЗГАЛУЖЕННЯ - ТАК] Умова виконана! Нараховано знижку на чек "
                f"{self.receipt_discount_rate * 100:.0f}% (-{receipt_discount:.2f} грн)."
            )
        else:
            # Гілка НІ: поріг не досягнуто -> знижка 0%
            receipt_discount = 0.0
            receipt_discount_applied = False
            trace_log.append(
                f"  [РОЗГАЛУЖЕННЯ - НІ] Сума недостатня для знижки на чек. Знижка на чек: 0 грн."
            )

        # =========================================================================
        # ДІЛЯНКА 1 (ФІНАЛ): ЛІНІЙНА (ОСТАТОЧНІ РОЗРАХУНКИ ТА ВИВЕДЕННЯ)
        # =========================================================================
        trace_log.append("[ЛІНІЙНА ДІЛЯНКА] Крок 3: Фінальні послідовні математичні обчислення.")
        final_payable = subtotal_after_item - receipt_discount
        total_saved = total_item_discount + receipt_discount
        # Розрахунок ПДВ 20% (включеного у фінальну вартість за законодавством: ПДВ = Сума * 20 / 120)
        vat_amount = final_payable * (self.vat_rate / (1.0 + self.vat_rate))

        trace_log.append(f"[ЛІНІЙНА ДІЛЯНКА] Крок 4: Сума до сплати = {final_payable:.2f} грн, ПДВ = {vat_amount:.2f} грн.")
        trace_log.append("[ЛІНІЙНА ДІЛЯНКА] Крок 5: Формування та виведення чека завершено.")

        results = {
            "processed_items": processed_items,
            "items_count": n,
            "subtotal_raw": subtotal_raw,
            "total_item_discount": total_item_discount,
            "subtotal_after_item": subtotal_after_item,
            "receipt_discount": receipt_discount,
            "receipt_discount_applied": receipt_discount_applied,
            "total_saved": total_saved,
            "final_payable": final_payable,
            "vat_amount": vat_amount,
            "item_threshold": self.item_discount_threshold,
            "receipt_threshold": self.receipt_discount_threshold
        }

        return results, trace_log


def format_receipt_text(res: Dict[str, Any]) -> str:
    """Форматування результатів у вигляд друкованого фіскального чека."""
    lines = []
    lines.append("==========================================")
    lines.append("           СУПЕРМАРКЕТ 'АЛГОРИТМ'         ")
    lines.append("       Касовий термінал (Лаб. роб. №1)    ")
    lines.append("==========================================")
    lines.append(f"{'№':<3} {'Назва':<14} {'Ціна':>7} {'К-сть':>5} {'Сума':>9}")
    lines.append("------------------------------------------")

    for idx, it in enumerate(res["processed_items"], start=1):
        name_trunc = (it.name[:12] + "..") if len(it.name) > 14 else it.name
        lines.append(f"{idx:<3} {name_trunc:<14} {it.price:>7.2f} {it.quantity:>5} {it.cost_raw:>9.2f}")
        if it.discount_amount > 0:
            lines.append(f"    * Знижка на товар ({it.discount_percent:.0f}%):  -{it.discount_amount:>7.2f}")

    lines.append("------------------------------------------")
    lines.append(f"Початкова сума товарів:         {res['subtotal_raw']:>10.2f} грн")
    lines.append(f"Знижки за окремі товари:       -{res['total_item_discount']:>10.2f} грн")
    lines.append(f"Проміжний підсумок:            {res['subtotal_after_item']:>10.2f} грн")
    
    if res["receipt_discount_applied"]:
        lines.append(f"Накопичувальна знижка чека (5%):-{res['receipt_discount']:>10.2f} грн")
    else:
        lines.append(f"Накопичувальна знижка чека:          0.00 грн")
        
    lines.append("==========================================")
    lines.append(f"ДО СПЛАТИ:                     {res['final_payable']:>10.2f} грн")
    lines.append("==========================================")
    lines.append(f"У тому числі ПДВ (20%):          {res['vat_amount']:>10.2f} грн")
    lines.append(f"ЕКОНОМІЯ ЗА ЧЕКОМ:              {res['total_saved']:>10.2f} грн")
    lines.append("------------------------------------------")
    lines.append("      Дякуємо за покупку! Гарного дня!    ")
    lines.append("==========================================")

    return "\n".join(lines)

