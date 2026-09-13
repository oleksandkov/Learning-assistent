"""
test_converter.py
Юніт-тести для перевірки коректності модуля converter.py.
"""

import unittest
from decimal import Decimal
from converter import convert_binary_to_decimal, validate_and_normalize, format_decimal_result


class TestConverter(unittest.TestCase):

    def test_example_from_methodology(self):
        # Приклад зі сторінки 2 методички: 110100101_2 = 421_10
        res = convert_binary_to_decimal("110100101")
        self.assertEqual(res.total_decimal, Decimal(421))
        self.assertEqual(format_decimal_result(res.total_decimal), "421")
        self.assertEqual(res.is_negative, False)

    def test_simple_integers(self):
        self.assertEqual(convert_binary_to_decimal("0").total_decimal, Decimal(0))
        self.assertEqual(convert_binary_to_decimal("1").total_decimal, Decimal(1))
        self.assertEqual(convert_binary_to_decimal("10").total_decimal, Decimal(2))
        self.assertEqual(convert_binary_to_decimal("1111").total_decimal, Decimal(15))
        self.assertEqual(convert_binary_to_decimal("10000000").total_decimal, Decimal(128))

    def test_fractional_numbers(self):
        # 0.1_2 = 0.5_10
        res = convert_binary_to_decimal("0.1")
        self.assertEqual(res.total_decimal, Decimal("0.5"))

        # 0.01_2 = 0.25_10
        res = convert_binary_to_decimal("0.01")
        self.assertEqual(res.total_decimal, Decimal("0.25"))

        # 1011.1101_2 = 11 + (0.5 + 0.25 + 0.0625) = 11.8125
        res = convert_binary_to_decimal("1011.1101")
        self.assertEqual(res.total_decimal, Decimal("11.8125"))

        # Перевірка коми замість крапки: 1101,101 -> 13.625
        res_comma = convert_binary_to_decimal("1101,101")
        self.assertEqual(res_comma.total_decimal, Decimal("13.625"))

    def test_negative_numbers(self):
        res = convert_binary_to_decimal("-101.011")
        # 101_2 = 5, .011_2 = 0.25 + 0.125 = 0.375 -> -5.375
        self.assertEqual(res.total_decimal, Decimal("-5.375"))
        self.assertTrue(res.is_negative)

    def test_normalization_and_leading_zeros(self):
        # .101 -> 0.101
        res = convert_binary_to_decimal(".101")
        self.assertEqual(res.total_decimal, Decimal("0.625"))

        # 000101.1000 -> 5.5
        res = convert_binary_to_decimal("000101.1000")
        self.assertEqual(res.total_decimal, Decimal("5.5"))

    def test_invalid_inputs(self):
        invalid_cases = [
            "",               # Порожній рядок
            "   ",            # Лише пробіли
            "1021",           # Цифра 2
            "10a1",           # Літера a
            "1.0.1",          # Дві крапки
            ".",              # Тільки крапка
            "-",              # Тільки мінус
            "+-101",          # Невірний знак
        ]
        for item in invalid_cases:
            with self.subTest(case=item):
                is_valid, err_msg, _ = validate_and_normalize(item)
                self.assertFalse(is_valid)
                with self.assertRaises(ValueError):
                    convert_binary_to_decimal(item)


if __name__ == "__main__":
    unittest.main()

