# Блок-схема алгоритму (UML Activity Diagram)

## Лабораторна робота №3. Варіант 12

**Тема:** Дослідження та реалізація базових динамічних структур даних: двозв‘язний список з розгалуженнями.  
**Задача:** Зберігання та операції над парами "Ім'я | Послідовність чисел (від 1 до $N$)". Для послідовностей довжиною понад $K$ організувати гілки ($K = 3$, $N = 10$).  
**Операції:** Додавання елементів, пошук і роздрукування, видалення, підрахунок кількості елементів, роздрукування всього списку.

---

## 1. UML Activity Diagram (Mermaid)

```mermaid
flowchart TD
    Start(["● Початок (main)"]) --> RunTests["Виклик run_tests() (assert самоперевірка)"]
    RunTests --> InitList["Ініціалізація DoublyBranchedList list (K=3, N=10)"]
    InitList --> MenuLoop["Вивід консольного меню та введення choice (0..6)"]
    MenuLoop --> ChoiceBranch{"choice == 0?"}

    %% Вихід з програми
    ChoiceBranch -- "Так (Вихід)" --> ClearMem["Очищення пам'яті list.clear()"]
    ClearMem --> StopProgram(["◉ Кінець (return 0)"])

    %% Основний робочий цикл операцій
    ChoiceBranch -- "Ні" --> PushBackSub

    %% Підпрограма push_back
    subgraph PushBackSub ["Підпрограма push_back(name, nums): Додавання елемента"]
        direction TB
        PB_Start["Створення Node*: node->name = name, total = min(nums.size, N)"] --> PB_Copy["Копіювання min(total, K) чисел у main_nums"]
        PB_Copy --> PB_BranchCheck{"total > K ?"}
        PB_BranchCheck -- "Так" --> PB_MakeBranch["Створення однозв'язного ланцюжка BranchNode для чисел K..total-1"]
        PB_BranchCheck -- "Ні" --> PB_NullBranch["node->branch = nullptr"]
        PB_MakeBranch --> PB_LinkTail["Вставка у двозв'язний хвіст: tail->next = node; node->prev = tail; tail = node; ++count;"]
        PB_NullBranch --> PB_LinkTail
        PB_LinkTail --> PB_Done["Успішне додавання вузла"]
    end

    PushBackSub --> RemoveSub

    %% Підпрограма remove
    subgraph RemoveSub ["Підпрограма remove(name): Видалення та вивільнення гілки"]
        direction TB
        RM_Find["target = find(name) (лінійний пошук за іменем)"] --> RM_Check{"target != nullptr?"}
        RM_Check -- "Так" --> RM_Unlink["Переприв'язка покажчиків: target->prev->next = target->next; target->next->prev = target->prev;"]
        RM_Unlink --> RM_FreeBranch["Вивільнення пам'яті гілки: free_branch(target->branch)"]
        RM_FreeBranch --> RM_Delete["Звільнення вузла: delete target; --count;"]
        RM_Check -- "Ні" --> RM_NotFound["Друк: 'Не знайдено'"]
    end

    RemoveSub --> ReturnLoop["Повернення до меню (наступна операція)"]
    ReturnLoop --> MenuLoop
```

---

## 2. Специфікація діаграми за стандартом UML (PlantUML)

```plantuml
@startuml
title Діаграма діяльності (UML Activity Diagram) - Лабораторна робота №3, Варіант 12

start
:Виклик run_tests() (assert перевірка);
:Ініціалізація DoublyBranchedList (K=3, N=10);

while (choice != 0?)
  :Виведення меню та зчитування choice;
  if (choice == 0?) then (так)
    :list.clear();
    stop
  else (ні)
    partition "Підпрограма push_back(name, nums)" {
      :Створення Node*;
      :Копіювання min(total, K) чисел у main_nums;
      if (total > K?) then (так)
        :Створення ланцюжка BranchNode;
      else (ні)
        :node->branch = nullptr;
      endif
      :tail->next = node; node->prev = tail; tail = node; ++count;
    }

    partition "Підпрограма remove(name)" {
      :target = find(name);
      if (target != nullptr?) then (так)
        :target->prev->next = target->next;
        :target->next->prev = target->prev;
        :free_branch(target->branch);
        :delete target;
        :--count;
      else (ні)
        :Повідомлення "Не знайдено";
      endif
    }
  endif
endwhile
stop
@enduml
```

---

## 3. Таблиця відповідності елементів UML-схеми та конструкцій мови C++

| Елемент діаграми UML | Конструкція у коді [`main.cpp`](file:///C:/Github/Learning-assistent/ALG_AND_STR/Labs/LAB3/main.cpp) | Призначення |
| :--- | :--- | :--- |
| **Початковий / Кінцевий вузол** (`Initial / Final Node`) | `int main(...)` / `return 0;` | Точка входу в програму та коректне завершення. |
| **Блок автотестування** (`Action Node`) | `run_tests();` | Автоматична валідація інваріантів списку за допомогою `assert()`. |
| **Розгалуження (Decision Node)** | `if (total > K)` | Перевірка потреби у винесенні надлишкових чисел у гілку. |
| **Створення гілки (Branch Creation)** | `*b_tail = new BranchNode{nums[i], nullptr};` | Динамічне виділення та зв'язування елементів однозв'язної гілки. |
| **Вставка у двозв'язний хвіст** | `tail->next = node; node->prev = tail;` | Додавання нового елемента в кінець списку за $O(1)$. |
| **Пошук та перевірка (Lookup)** | `for (Node* curr = head; curr; curr = curr->next)` | Лінійне траверсування списку за покажчиками $O(n)$. |
| **Вилучення та очищення пам'яті** | `free_branch(target->branch); delete target;` | Безпечне вивільнення динамічної пам'яті без витоків (RAII). |
| **Консольна диспетчеризація** | `switch (choice) { ... }` | Інтерактивна взаємодія з користувачем через меню. |
