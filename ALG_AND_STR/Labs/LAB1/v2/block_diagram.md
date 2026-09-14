# Блок-схема алгоритму (UML Activity Diagram)

## Лабораторна робота №1. Варіант 72

**Тема:** Опрацювання одновимірних масивів з використанням покажчиків.  
**Задача:** Розрахунок середньої кількості опадів у Вінниці за 50 років (1949–1998) та річних відхилень від середнього.

---

## 1. UML Activity Diagram (Mermaid)

```mermaid
flowchart TD
    Start(["● Початок (main)"]) --> RunTests["Виклик run_tests() (перевірка assert)"]
    RunTests --> TestBranch{"Аргумент --test?"}

    TestBranch -- "Так" --> PrintTestPassed["Вивід: 'Тести успішно пройдені'"]
    PrintTestPassed --> StopTest(["◉ Завершення (exit 0)"])

    TestBranch -- "Ні" --> InitConst["Оголошення: rain[50], START_YEAR = 1949, N = 50"]
    InitConst --> DisplayMenu["Вивід меню: 1 - Зразкові дані, 2 - Ручне введення"]
    DisplayMenu --> ReadChoice[/"Введення: choice"/]

    ReadChoice --> ChoiceBranch{"choice == '2'?"}

    %% Гілка ручного введення
    ChoiceBranch -- "Так (Ручне)" --> InitInputLoop["Ініціалізація покажчика: p = rain"]
    InitInputLoop --> LoopInputCond{"p < rain + N ?"}
    LoopInputCond -- "Так" --> PromptYear["Вивід року: 1949 + (p - rain)"]
    PromptYear --> ReadVal[/"Введення числа: *p"/]
    ReadVal --> CheckInputValid{"Ввід успішний?"}
    CheckInputValid -- "Ні" --> InputError["Вивід повідомлення про помилку"]
    InputError --> StopErr(["◉ Завершення з кодом 1"])
    CheckInputValid -- "Так" --> IncInputPtr["Інкремент покажчика: ++p"]
    IncInputPtr --> LoopInputCond
    LoopInputCond -- "Ні" --> CallCalcAvg

    %% Гілка завантаження зразкових даних
    ChoiceBranch -- "Ні (Зразкові дані)" --> CopySampleData["Копіювання 50 кліматичних значень у rain через *(rain + i)"]
    CopySampleData --> CallCalcAvg

    %% Обчислення середнього
    CallCalcAvg["Виклик функції: avg = calculate_average(rain, N)"] --> SubCalcAvg

    subgraph SubCalcAvg ["Підпрограма calculate_average(const double *arr, int size)"]
        direction TB
        AvgStart(["Початок функції"]) --> AssertCheck["assert(arr != nullptr && size > 0)"]
        AssertCheck --> InitSum["sum = 0.0, p = arr"]
        InitSum --> LoopSumCond{"p < arr + size ?"}
        LoopSumCond -- "Так" --> AddToSum["sum += *p\n++p"]
        AddToSum --> LoopSumCond
        LoopSumCond -- "Ні" --> ReturnAvg["Повернути: sum / size"]
        ReturnAvg --> AvgEnd(["Кінець функції"])
    end

    SubCalcAvg --> CallPrintReport["Виклик процедури: print_report(rain, N, START_YEAR, avg)"]

    subgraph SubPrintReport ["Підпрограма print_report(const double *arr, int size, int start_year, double avg)"]
        direction TB
        RepStart(["Початок процедури"]) --> PrintHeader["Виведення заголовка таблиці:\nРік | Опади (мм) | Відхилення (мм)"]
        PrintHeader --> InitRepLoop["i = 0"]
        InitRepLoop --> LoopRepCond{"i < size ?"}
        LoopRepCond -- "Так" --> CalcDev["p = arr + i\ndeviation = *p - avg"]
        CalcDev --> PrintRow[/"Виведення рядка: (start_year + i), *p, deviation"/]
        PrintRow --> IncRep["++i"]
        IncRep --> LoopRepCond
        LoopRepCond -- "Ні" --> PrintFooter[/"Виведення середнього значення: avg"/]
        PrintFooter --> RepEnd(["Кінець процедури"])
    end

    SubPrintReport --> Finish(["◉ Завершення (return 0)"])
```

---

## 2. Специфікація діаграми за стандартом UML (PlantUML)

```plantuml
@startuml
title Діаграма діяльності (UML Activity Diagram) - Лабораторна робота №1, Варіант 72

start
:Виклик run_tests();

if (Аргумент командного рядка == "--test"?) then (так)
  :Вивід повідомлення "Тести успішно пройдені";
  stop
else (ні)
endif

:Ініціалізація змінних:\n- double rain[50];\n- START_YEAR = 1949;\n- N = 50;

:Виведення меню вибору джерела даних;
:Введення вибору choice;

if (choice == '2'?) then (ручне введення)
  :Встановлення покажчика: p = rain;
  while (p < rain + N) is (так)
    :Обчислення поточного року:\nyear = START_YEAR + (p - rain);
    :Введення значення опадів у *p;
    if (Помилка введення?) then (так)
      :Повідомлення про помилку;
      stop
    else (ні)
    endif
    :Зсув покажчика: ++p;
  endwhile (ні)
else (зразкові дані)
  :Завантаження 50 кліматичних значень опадів у масив\nвикористовуючи зміщення *(rain + i);
endif

partition "Функція calculate_average(arr, size)" {
  :assert(arr != nullptr && size > 0);
  :sum = 0.0;
  :p = arr;
  while (p < arr + size) is (так)
    :Додати значення до суми: sum += *p;
    :Інкремент покажчика: ++p;
  endwhile (ні)
  :Обчислити середнє: avg = sum / size;
}

partition "Процедура print_report(arr, size, start_year, avg)" {
  :Вивести заголовок таблиці результатів;
  :i = 0;
  while (i < size) is (так)
    :Отримати адресу елемента: p = arr + i;
    :Обчислити відхилення: deviation = *p - avg;
    :Вивести рядок таблиці (рік, значення опадів, відхилення);
    :++i;
  endwhile (ні)
  :Вивести підсумкове значення середніх опадів (avg);
}

stop
@enduml
```

---

## 3. Таблиця відповідності елементів UML-схеми та конструкцій мови C++

| Елемент діаграми UML                                     | Конструкція у коді [`main.cpp`](file:///c:/Users/muaro/Documents/GitHub/Learning-assistent/ALG_AND_STR/Labs/LAB1/v2/main.cpp) | Призначення                                                                     |
| :------------------------------------------------------- | :---------------------------------------------------------------------------------------------------------------------------- | :------------------------------------------------------------------------------ |
| **Початковий / Кінцевий вузол** (`Initial / Final Node`) | `int main(...)` / `return 0;`                                                                                                 | Точка входу в програму та завершення роботи.                                    |
| **Дія (Action Node)**                                    | `sum += *p; ++p;`                                                                                                             | Обчислення значень за допомогою розіменування та арифметики покажчиків.         |
| **Розгалуження (Decision Node)**                         | `if (choice == '2') { ... } else { ... }`                                                                                     | Вибір режиму: ручне введення користувачем або типові дані Вінниці.              |
| **Цикл (Loop / Activity Partition)**                     | `for (const double *p = arr; p < arr + size; ++p)`                                                                            | Ітерація по масиву виключно через покажчики без індексації квадратними дужками. |
| **Вхід / Вихід (Input/Output)**                          | `std::cin >> *p;`, `std::cout << ...`                                                                                         | Зчитування та форматований вивід результатів таблиці.                           |
