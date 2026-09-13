"""
Модульне тестування алгоритму розрахунку чека (тестові сценарії для звіту).
"""

from algorithm import ProductItem, ReceiptAlgorithm


def test_scenario_1_no_discounts():
    """Сценарій 1: Без жодних знижок (усі товари < 500 грн, сума чека < 1500 грн)"""
    items = [
        ProductItem("Хліб", 30.0, 2),
        ProductItem("Молоко", 45.0, 1),
        ProductItem("Сир", 180.0, 1)
    ]
    algo = ReceiptAlgorithm()
    res, trace = algo.calculate(items)

    assert res["subtotal_raw"] == 285.0
    assert res["total_item_discount"] == 0.0
    assert res["subtotal_after_item"] == 285.0
    assert res["receipt_discount"] == 0.0
    assert not res["receipt_discount_applied"]
    assert res["final_payable"] == 285.0
    print("[OK] Сценарій 1 (Без знижок) пройдено успішно!")


def test_scenario_2_only_item_discount():
    """Сценарій 2: Тільки індивідуальна знижка на товар (один товар >= 500 грн, чек < 1500 грн)"""
    items = [
        ProductItem("Кава преміум", 600.0, 1), # 600 >= 500 -> знижка 10% (60 грн), вартість 540
        ProductItem("Цукор", 40.0, 2)          # 80 грн
    ]
    algo = ReceiptAlgorithm()
    res, trace = algo.calculate(items)

    assert res["subtotal_raw"] == 680.0
    assert res["total_item_discount"] == 60.0
    assert res["subtotal_after_item"] == 620.0
    assert res["receipt_discount"] == 0.0
    assert not res["receipt_discount_applied"]
    assert res["final_payable"] == 620.0
    print("[OK] Сценарій 2 (Тільки знижка на товар) пройдено успішно!")


def test_scenario_3_only_receipt_discount():
    """Сценарій 3: Тільки накопичувальна знижка чека (усі товари < 500 грн, але сума >= 1500 грн)"""
    items = [
        ProductItem("Шоколадні набори", 400.0, 4) # 1600 грн (жоден окремий товар не перевищує 500)
    ]
    algo = ReceiptAlgorithm()
    res, trace = algo.calculate(items)

    assert res["subtotal_raw"] == 1600.0
    assert res["total_item_discount"] == 0.0
    assert res["subtotal_after_item"] == 1600.0
    assert res["receipt_discount"] == 80.0 # 5% від 1600 = 80 грн
    assert res["receipt_discount_applied"]
    assert res["final_payable"] == 1520.0
    print("[OK] Сценарій 3 (Тільки знижка на чек) пройдено успішно!")


def test_scenario_4_both_discounts():
    """Сценарій 4: Комплексний випадок (товари >= 500 грн та фінальний чек >= 1500 грн)"""
    items = [
        ProductItem("Кавомашина капсульна", 1200.0, 1), # 1200 >= 500 -> знижка 10% (120 грн), після = 1080 грн
        ProductItem("Набір посуду", 700.0, 1)            # 700 >= 500 -> знижка 10% (70 грн), після = 630 грн
    ]
    # Підсумок після товарів: 1080 + 630 = 1710 грн
    # 1710 >= 1500 -> знижка 5% від 1710 = 85.50 грн
    # До сплати: 1710 - 85.50 = 1624.50 грн
    # Загальна економія: 120 + 70 + 85.50 = 275.50 грн
    algo = ReceiptAlgorithm()
    res, trace = algo.calculate(items)

    assert res["subtotal_raw"] == 1900.0
    assert res["total_item_discount"] == 190.0
    assert res["subtotal_after_item"] == 1710.0
    assert res["receipt_discount"] == 85.50
    assert res["receipt_discount_applied"]
    assert res["final_payable"] == 1624.50
    assert res["total_saved"] == 275.50
    print("[OK] Сценарій 4 (Обидві знижки одночасно) пройдено успішно!")


if __name__ == "__main__":
    test_scenario_1_no_discounts()
    test_scenario_2_only_item_discount()
    test_scenario_3_only_receipt_discount()
    test_scenario_4_both_discounts()
    print("\nУсі 4 тестові сценарії виконані з 100% успіхом!")
