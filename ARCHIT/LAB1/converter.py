"""
converter.py
Модуль для переведення двійкових чисел (цілих та дробових) у десяткову систему числення.
Підтримує точні обчислення за допомогою модуля decimal.
"""

from decimal import Decimal, getcontext
from typing import NamedTuple, List, Tuple

# Встановлюємо достатню точність для роботи з довгими дробами
getcontext().prec = 50


class BitContribution(NamedTuple):
    index: int          # Позиція степеня (наприклад: 3, 2, 1, 0 для цілої, -1, -2 для дробової)
    bit: str            # Двійковий символ ('0' або '1')
    power_value: Decimal # Значення 2^index
    contribution: Decimal # bit * 2^index


class ConversionResult(NamedTuple):
    original_input: str
    is_negative: bool
    integer_part_binary: str
    fractional_part_binary: str
    integer_contributions: List[BitContribution]
    fractional_contributions: List[BitContribution]
    integer_decimal: Decimal
    fractional_decimal: Decimal
    total_decimal: Decimal
    polynomial_formula: str
    polynomial_with_values: str


def validate_and_normalize(binary_str: str) -> Tuple[bool, str, str]:
    """
    Перевіряє коректність введеного двійкового числа та нормалізує його.
    Повертає: (чи_валідне, повідомлення_про_помилку, нормалізований_рядок)
    """
    if not binary_str:
        return False, "Помилка: рядок введення порожній. Будь ласка, введіть число.", ""

    s = binary_str.strip().replace(" ", "")
    if not s:
        return False, "Помилка: введено лише пробіли.", ""

    # Заміна коми на крапку для підтримки обох роздільників
    s = s.replace(",", ".")

    # Обробка знаку
    sign = ""
    if s[0] in ("+", "-"):
        sign = s[0]
        s = s[1:]

    if not s:
        return False, "Помилка: після знаку відсутні цифри.", ""

    # Перевірка кількості крапок
    dot_count = s.count(".")
    if dot_count > 1:
        return False, "Помилка: число містить більше однієї крапки/коми.", ""

    # Перевірка на неприпустимі символи
    valid_chars = {"0", "1", "."}
    invalid_chars = set(s) - valid_chars
    if invalid_chars:
        return False, f"Помилка: двійкове число може містити лише 0 та 1. Знайдено недопустимі символи: {', '.join(sorted(invalid_chars))}", ""

    # Якщо крапка є єдиним символом
    if s == ".":
        return False, "Помилка: введено лише розділовий знак без цифр.", ""

    # Нормалізація вигляду: наприклад, .101 -> 0.101, 110. -> 110.0
    if "." in s:
        parts = s.split(".")
        int_p = parts[0] if parts[0] else "0"
        frac_p = parts[1] if parts[1] else "0"
        normalized = f"{sign}{int_p}.{frac_p}"
    else:
        normalized = f"{sign}{s}"

    return True, "", normalized


def convert_binary_to_decimal(binary_str: str) -> ConversionResult:
    """
    Виконує переведення двійкового числа у десяткове з покроковим розкладом за поліномом.
    Викликає ValueError у разі некоректного введення.
    """
    is_valid, err_msg, normalized = validate_and_normalize(binary_str)
    if not is_valid:
        raise ValueError(err_msg)

    is_negative = normalized.startswith("-")
    clean_num = normalized.lstrip("+-")

    if "." in clean_num:
        int_str, frac_str = clean_num.split(".")
    else:
        int_str, frac_str = clean_num, ""

    # Видалення зайвих ведучих нулів у цілій частині (залишаємо хоча б один '0')
    int_str_stripped = int_str.lstrip("0")
    if not int_str_stripped:
        int_str = "0"
    else:
        int_str = int_str_stripped

    # Видалення зайвих нулів у кінці дробової частини
    frac_str = frac_str.rstrip("0")

    # 1. Обчислення цілої частини: sum_{i=0..n-1} a_i * 2^i
    int_contributions: List[BitContribution] = []
    int_decimal = Decimal(0)
    n = len(int_str)

    for idx, char in enumerate(int_str):
        power = n - 1 - idx
        power_val = Decimal(2) ** power
        bit_val = Decimal(1) if char == "1" else Decimal(0)
        contrib = bit_val * power_val
        int_contributions.append(BitContribution(
            index=power,
            bit=char,
            power_value=power_val,
            contribution=contrib
        ))
        int_decimal += contrib

    # 2. Обчислення дробової частини: sum_{j=1..m} b_j * 2^(-j)
    frac_contributions: List[BitContribution] = []
    frac_decimal = Decimal(0)

    for j, char in enumerate(frac_str, start=1):
        power = -j
        power_val = Decimal(2) ** power
        bit_val = Decimal(1) if char == "1" else Decimal(0)
        contrib = bit_val * power_val
        frac_contributions.append(BitContribution(
            index=power,
            bit=char,
            power_value=power_val,
            contribution=contrib
        ))
        frac_decimal += contrib

    # Підсумкове значення
    total_abs = int_decimal + frac_decimal
    total_decimal = -total_abs if is_negative else total_abs

    # 3. Формування математичного розкладу за поліномом
    poly_terms = []
    poly_calc_terms = []

    # Додаємо цілі розряди
    for c in int_contributions:
        poly_terms.append(f"{c.bit}·2^{{{c.index}}}")
        poly_calc_terms.append(f"{c.contribution}")

    # Додаємо дробові розряди
    for c in frac_contributions:
        poly_terms.append(f"{c.bit}·2^{{{c.index}}}")
        poly_calc_terms.append(f"{c.contribution}")

    if not poly_terms:
        polynomial_formula = "0·2^{0}"
        polynomial_with_values = "0"
    else:
        polynomial_formula = " + ".join(poly_terms)
        polynomial_with_values = " + ".join(poly_calc_terms)

    if is_negative:
        polynomial_formula = f"-({polynomial_formula})"
        polynomial_with_values = f"-({polynomial_with_values})"

    return ConversionResult(
        original_input=binary_str,
        is_negative=is_negative,
        integer_part_binary=int_str,
        fractional_part_binary=frac_str,
        integer_contributions=int_contributions,
        fractional_contributions=frac_contributions,
        integer_decimal=int_decimal,
        fractional_decimal=frac_decimal,
        total_decimal=total_decimal,
        polynomial_formula=polynomial_formula,
        polynomial_with_values=polynomial_with_values
    )


def format_decimal_result(val: Decimal) -> str:
    """
    Красиво форматує Decimal у рядок без експоненційного запису та без зайвих нулів у кінці.
    """
    s = f"{val:f}"
    if "." in s:
        s = s.rstrip("0").rstrip(".")
    return s if s else "0"

