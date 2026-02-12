#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ЛАБОРАТОРНА РОБОТА №1. МНОЖИНИ. ОСНОВНІ ПОНЯТТЯ
Програма для роботи з множинами та операціями над ними
"""

import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.patches import Circle
import numpy as np

# Налаштування для українського тексту
plt.rcParams['font.family'] = ['DejaVu Sans', 'Arial Unicode MS', 'Lucida Grande']
plt.rcParams['axes.unicode_minus'] = False

def create_venn_diagram(set_a, set_b, title, operation_result=None, operation_name=""):
    """Створює діаграму Венна для двох множин"""
    fig, ax = plt.subplots(1, 1, figsize=(10, 8))
    
    # Параметри кіл
    center_a = (-0.5, 0)
    center_b = (0.5, 0)
    radius = 1.2
    
    # Створення кіл
    circle_a = Circle(center_a, radius, alpha=0.3, color='red', label=f'A = {set_a}')
    circle_b = Circle(center_b, radius, alpha=0.3, color='blue', label=f'B = {set_b}')
    
    # Якщо є результат операції, підсвічуємо його
    if operation_result is not None:
        if operation_name == "union":
            circle_a = Circle(center_a, radius, alpha=0.5, color='green')
            circle_b = Circle(center_b, radius, alpha=0.5, color='green')
        elif operation_name == "intersection":
            # Створюємо переріз
            intersection_patch = patches.Wedge(center=(0, 0), r=radius*0.8, 
                                             theta1=30, theta2=150, alpha=0.7, color='green')
            ax.add_patch(intersection_patch)
        elif operation_name == "difference_a_b":
            circle_a = Circle(center_a, radius, alpha=0.5, color='green')
            # Маскуємо частину A, що перетинається з B
        elif operation_name == "difference_b_a":
            circle_b = Circle(center_b, radius, alpha=0.5, color='green')
    
    ax.add_patch(circle_a)
    ax.add_patch(circle_b)
    
    # Додаємо елементи множин
    if set_a:
        elements_a_only = set_a - set_b
        elements_common = set_a & set_b
        
        # Елементи тільки в A
        y_pos = 0.3
        for i, elem in enumerate(elements_a_only):
            ax.text(-1.2, y_pos - i*0.2, str(elem), fontsize=10, ha='center')
        
        # Спільні елементи
        y_pos = 0.3
        for i, elem in enumerate(elements_common):
            ax.text(0, y_pos - i*0.2, str(elem), fontsize=10, ha='center')
    
    if set_b:
        elements_b_only = set_b - set_a
        # Елементи тільки в B
        y_pos = 0.3
        for i, elem in enumerate(elements_b_only):
            ax.text(1.2, y_pos - i*0.2, str(elem), fontsize=10, ha='center')
    
    # Налаштування осей
    ax.set_xlim(-2.5, 2.5)
    ax.set_ylim(-2, 2)
    ax.set_aspect('equal')
    ax.set_title(title, fontsize=14, pad=20)
    
    # Додаємо підписи
    ax.text(-0.5, -1.7, 'A', fontsize=16, ha='center', weight='bold')
    ax.text(0.5, -1.7, 'B', fontsize=16, ha='center', weight='bold')
    
    if operation_result is not None:
        result_text = f"{operation_name}: {operation_result}"
        ax.text(0, -2.3, result_text, fontsize=12, ha='center', 
               bbox=dict(boxstyle="round,pad=0.3", facecolor="lightgray"))
    
    ax.axis('off')
    plt.tight_layout()
    plt.show()

def visualize_subsets(main_set, subsets):
    """Візуалізує всі підмножини основної множини"""
    n = len(subsets)
    cols = min(4, n)
    rows = (n + cols - 1) // cols
    
    fig, axes = plt.subplots(rows, cols, figsize=(15, 4*rows))
    if rows == 1:
        axes = [axes] if cols == 1 else axes
    else:
        axes = axes.flatten()
    
    for i, subset in enumerate(subsets):
        if i < len(axes):
            ax = axes[i]
            
            # Створюємо коло для основної множини
            circle = Circle((0, 0), 1, alpha=0.3, color='lightblue')
            ax.add_patch(circle)
            
            # Підсвічуємо елементи підмножини
            if subset:
                elements = list(main_set)
                for j, elem in enumerate(elements):
                    angle = 2 * np.pi * j / len(elements)
                    x = 0.7 * np.cos(angle)
                    y = 0.7 * np.sin(angle)
                    
                    if elem in subset:
                        ax.plot(x, y, 'ro', markersize=12)
                    else:
                        ax.plot(x, y, 'ko', markersize=8, alpha=0.3)
                    
                    ax.text(x*1.3, y*1.3, str(elem), ha='center', va='center', fontsize=10)
            
            ax.set_xlim(-1.5, 1.5)
            ax.set_ylim(-1.5, 1.5)
            ax.set_aspect('equal')
            
            if not subset:
                ax.set_title(f"∅", fontsize=12)
            else:
                ax.set_title(f"{subset}", fontsize=10)
            ax.axis('off')
    
    # Приховуємо зайві осі
    for i in range(n, len(axes)):
        axes[i].axis('off')
    
    plt.suptitle(f'Всі підмножини множини {main_set}', fontsize=16)
    plt.tight_layout()
    plt.show()

def input_set(name):
    """Вводить множину від користувача"""
    print(f"Введіть елементи множини {name} через пробіл:")
    elements = input().strip().split()
    # Перетворюємо в числа, якщо можливо
    result = set()
    for elem in elements:
        try:
            result.add(int(elem))
        except ValueError:
            result.add(elem)
    return result

def task1():
    """Завдання 1: Порівняння множин та перевірка підмножини"""
    print("=== Завдання 1: Порівняння множин ===")
    
    set_a = input_set("A")
    set_b = input_set("B")
    
    print(f"Множина A: {set_a}")
    print(f"Множина B: {set_b}")
    
    if set_a == set_b:
        print("Множини A і B рівні")
        create_venn_diagram(set_a, set_b, "Рівні множини A = B")
    elif set_a.issubset(set_b):
        print("A є підмножиною B")
        create_venn_diagram(set_a, set_b, "A ⊆ B (A є підмножиною B)")
    elif set_b.issubset(set_a):
        print("B є підмножиною A")
        create_venn_diagram(set_a, set_b, "B ⊆ A (B є підмножиною A)")
    else:
        print("Множини не рівні і жодна не є підмножиною іншої")
        create_venn_diagram(set_a, set_b, "Множини A і B (не рівні, без підмножин)")

def task2():
    """Завдання 2: Генерація всіх підмножин"""
    print("=== Завдання 2: Всі підмножини множини ===")
    
    main_set = input_set("A")
    print(f"Множина A: {main_set}")
    
    # Обмежуємо кількість елементів для візуалізації
    if len(main_set) > 4:
        print("Попередження: Для множин з більш ніж 4 елементами візуалізація може бути складною.")
        show_visual = input("Показати візуалізацію? (y/n): ").lower() == 'y'
    else:
        show_visual = True
    
    # Генеруємо всі підмножини
    elements = list(main_set)
    n = len(elements)
    subsets = []
    
    # Використовуємо бітові маски для генерації всіх підмножин
    for i in range(2**n):
        subset = set()
        for j in range(n):
            if i & (1 << j):
                subset.add(elements[j])
        subsets.append(subset)
    
    print(f"Всі підмножини множини A (всього {len(subsets)}):")
    for i, subset in enumerate(subsets):
        if not subset:
            print(f"{i+1}: ∅ (порожня множина)")
        else:
            print(f"{i+1}: {subset}")
    
    if show_visual:
        print("\nВізуалізація підмножин...")
        visualize_subsets(main_set, subsets)

def task3():
    """Завдання 3: Операції над множинами"""
    print("=== Завдання 3: Операції над множинами ===")
    
    set_a = input_set("A")
    set_b = input_set("B")
    
    print(f"Множина A: {set_a}")
    print(f"Множина B: {set_b}")
    print()
    
    # Операції над множинами
    union = set_a | set_b  # Об'єднання
    intersection = set_a & set_b  # Переріз
    difference_a_b = set_a - set_b  # Різниця A - B
    difference_b_a = set_b - set_a  # Різниця B - A
    symmetric_diff = set_a ^ set_b  # Симетрична різниця
    
    print("Результати операцій:")
    print(f"A ∪ B (об'єднання):           {union}")
    print(f"A ∩ B (переріз):              {intersection}")
    print(f"A - B (різниця):              {difference_a_b}")
    print(f"B - A (різниця):              {difference_b_a}")
    print(f"A ⊕ B (симетрична різниця):   {symmetric_diff}")
    
    # Додаткова інформація
    print(f"\nДодаткова інформація:")
    print(f"Кількість елементів в A: {len(set_a)}")
    print(f"Кількість елементів в B: {len(set_b)}")
    print(f"Кількість елементів в A ∪ B: {len(union)}")
    print(f"Кількість елементів в A ∩ B: {len(intersection)}")
    
    # Візуалізація операцій
    print("\nВізуалізація операцій...")
    
    # Показуємо всі операції
    create_venn_diagram(set_a, set_b, f"A ∪ B = {union}", union, "union")
    create_venn_diagram(set_a, set_b, f"A ∩ B = {intersection}", intersection, "intersection")
    create_venn_diagram(set_a, set_b, f"A - B = {difference_a_b}", difference_a_b, "difference_a_b")
    create_venn_diagram(set_a, set_b, f"B - A = {difference_b_a}", difference_b_a, "difference_b_a")
    create_venn_diagram(set_a, set_b, f"A ⊕ B = {symmetric_diff}", symmetric_diff, "symmetric_diff")

def main():
    """Головна функція"""
    print("ЛАБОРАТОРНА РОБОТА №1. МНОЖИНИ. ОСНОВНІ ПОНЯТТЯ")
    print("=" * 50)
    
    while True:
        print("\nОберіть завдання:")
        print("1. Порівняння множин та перевірка підмножини")
        print("2. Генерація всіх підмножин")
        print("3. Операції над множинами")
        print("0. Вихід")
        
        try:
            choice = int(input("Ваш вибір: "))
            
            if choice == 1:
                task1()
            elif choice == 2:
                task2()
            elif choice == 3:
                task3()
            elif choice == 0:
                print("Програма завершена.")
                break
            else:
                print("Неправильний вибір. Спробуйте ще раз.")
                
        except ValueError:
            print("Введіть число від 0 до 3.")
        
        print()

if __name__ == "__main__":
    main()
