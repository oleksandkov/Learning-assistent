# Блок-схема алгоритму (UML Activity Diagram)

## Лабораторна робота №4. Варіант 12

**Тема:** Алгоритми сортування: сортування вставкою (Insertion sort) та сортування двійковим деревом (Tree sort).  
**Задача:** Порівняльний аналіз часу виконання двох алгоритмів сортування для різних розмірностей та розподілів даних.

---

## 1. UML Activity Diagram (Mermaid)

```mermaid
flowchart TD
    Start(["● Початок (main)"]) --> RunTests["Виклик run_tests() (assert валідація)"]
    RunTests --> Menu["Вибір режиму: 1 - Бенчмарк, 2 - Одиночний тест, 3 - Тести, 0 - Вихід"]
    Menu --> CheckChoice{"choice == 0?"}
    
    CheckChoice -- "Так" --> Stop(["◉ Кінець (return 0)"])
    CheckChoice -- "Ні" --> Subprograms

    subgraph SubInsertion ["Підпрограма insertion_sort(a)"]
        direction TB
        IS_Loop["Зовнішній цикл: for i = 1 .. n-1"] --> IS_Key["key = a[i]; j = i - 1;"]
        IS_Key --> IS_Cond{"j >= 0 && a[j] > key ?"}
        IS_Cond -- "Так (Зсув)" --> IS_Shift["a[j+1] = a[j]; --j;"]
        IS_Shift --> IS_Cond
        IS_Cond -- "Ні" --> IS_Insert["Вставка: a[j+1] = key;"]
        IS_Insert --> IS_Done["Масив впорядковано in-place"]
    end

    subgraph SubTree ["Підпрограма tree_sort(a)"]
        direction TB
        TS_Init["root = nullptr;"] --> TS_Build["Побудова BST: insert_bst(root, x)"]
        TS_Build --> TS_Inorder["Центрований обхід inorder(root, a, idx)"]
        TS_Inorder --> TS_Free["Звільнення пам'яті: free_tree(root)"]
        TS_Free --> TS_Done["Масив повністю відсортовано"]
    end

    Subprograms --> PrintTable["Форматування та друк результатів (chrono)"]
    PrintTable --> Menu
```

---

## 2. Специфікація діаграми за стандартом UML (PlantUML)

```plantuml
@startuml
title Діаграма діяльності (UML Activity Diagram) - Лабораторна робота №4, Варіант 12

start
:Виклик run_tests() [assert];
:Вибір режиму роботи меню;

partition "Підпрограма insertion_sort(a)" {
  :for (i = 1; i < n; ++i);
  :key = a[i]; j = i - 1;
  while (j >= 0 && a[j] > key?) is (так)
    :a[j + 1] = a[j];
    :--j;
  endwhile (ні)
  :a[j + 1] = key;
}

partition "Підпрограма tree_sort(a)" {
  :root = nullptr;
  :Побудова двійкового дерева пошуку BST;
  :Центрований обхід Inorder (Left -> Root -> Right);
  :Звільнення динамічної пам'яті free_tree(root);
}

:Вимірювання часу std::chrono та друк зведеної таблиці;
stop
@enduml
```
