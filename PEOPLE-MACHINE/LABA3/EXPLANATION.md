# Пояснення до коду laba3.py

Цей файл містить детальний опис роботи програми для управління розкладом занять, написаної на Python з використанням бібліотеки `tkinter`. Програма дозволяє переглядати, фільтрувати, додавати, редагувати та видаляти записи про заняття.

## 1. Використані бібліотеки та модулі

```python
import tkinter as tk
from tkinter import ttk, simpledialog, messagebox
```

- **`tkinter` (як `tk`)**: Стандартна бібліотека Python для створення графічного інтерфейсу користувача (GUI). Вона надає базові елементи, такі як вікна, кнопки, мітки тощо.
- **`tkinter.ttk`**: Підмодуль `tkinter`, який надає доступ до "тематичних" віджетів (Themed Tkinter). Вони виглядають більш сучасно та нативно для операційної системи (наприклад, `Combobox`, `Treeview`, `Scrollbar`).
- **`tkinter.simpledialog`**: Модуль для створення простих діалогових вікон введення даних (наприклад, запит рядка тексту).
- **`tkinter.messagebox`**: Модуль для виведення повідомлень користувачеві (попереджень, запитань, помилок).

## 2. Глобальні змінні та константи

У програмі визначено кілька важливих глобальних списків та констант, які використовуються для зберігання даних та налаштувань інтерфейсу:

```python
groups   = [u'КН-11', u'КН-12', u'ІПЗ-21']
teachers = [u'Іваненко І.І.',
            u'Петренко О.В.',
            u'Коваль М.С.']
lessons  = [
    (u'Математика',
     u'Пн 08:30', '201', u'КН-11', '1',
     u'Іваненко І.І.',
     u'Лекція'),
    # ... інші записи ...
]

COLS   = (u'Предмет', u'Час',
          u'Аудиторія', u'Група',
          u'Тиждень', u'Викладач',
          u'Тип')
WIDTHS = (180, 110, 80, 80, 70, 160, 110)
LEK    = u'Лекція'
PRAK   = u'Практика'
LAB    = u'Лабораторна'
TYPE_BG = {LEK: '#cce0f5', PRAK: '#ccefd4', LAB: '#fde4b0'}
DAYS   = ['', u'Пн', u'Вт', u'Ср',
          u'Чт', u'Пт', u'Сб']
TYPES  = ['', LEK, PRAK, LAB]
WEEKS  = ['', '1', '2', u'Обидва']
```

- **`groups`, `teachers`**: Списки, що містять перелік доступних груп та викладачів. Вони використовуються для заповнення випадаючих списків.
- **`lessons`**: Основний список, що зберігає кортежі з даними про заняття (Предмет, Час, Аудиторія, Група, Тиждень, Викладач, Тип).
- **`COLS`, `WIDTHS`**: Назви колонок таблиці та їх ширина в пікселях.
- **`LEK`, `PRAK`, `LAB`**: Константи для типів занять.
- **`TYPE_BG`**: Словник, що ставить у відповідність типу заняття певний колір фону (для візуального виділення рядків у таблиці).

## 3. Детальний опис функцій

### Основні функції логіки

#### `apply_filter(tree, fvars, status)`

Ця функція відповідає за відображення відфільтрованих даних у таблиці ('tree').

```python
def apply_filter(tree, fvars, status):
    subj, day, grp, week, tchr, typ = (v.get() for v in fvars)
    res = [r for r in lessons
           if subj.lower() in r[0].lower()
           and day  in r[1] and grp  in r[3]
           and week in r[4] and tchr in r[5] and typ in r[6]]
    tree.delete(*tree.get_children())
    for r in res:
        tree.insert('', 'end', values=r, tags=(r[6],))
    status.set(u'Показано: %d із %d' % (len(res), len(lessons)))
```

1.  Зчитує поточні значення з полів фільтру (`fvars`).
2.  Використовує **List Comprehension** (генератор списків) для створення нового списку `res`, до якого потрапляють лише ті уроки з `lessons`, що відповідають усім критеріям фільтру.
    - Перевірка предмету (`subj`) нечутлива до регістру (`.lower()`).
3.  Очищає таблицю (`tree.delete`).
4.  Заповнює таблицю знайденими записами (`tree.insert`). При цьому додається тег (наприклад, 'Лекція'), який використовується для розфарбовування рядка.
5.  Оновлює статусний рядок, показуючи кількість знайдених записів.

#### `reset_filter(fvars, tree, status)`

Скидає всі значення полів фільтрації до початкового стану (порожні рядки) і викликає `apply_filter`, щоб показати повний список занять.

```python
def reset_filter(fvars, tree, status):
    for v in fvars: v.set('')
    apply_filter(tree, fvars, status)
```

### Робота з діалоговими вікнами

#### `lesson_dialog(root, title, initial=None)`

Створює модальне вікно (`tk.Toplevel`) для додавання нового або редагування існуючого заняття.

```python
def lesson_dialog(root, title, initial=None):
    init = initial or ('', '', '', groups[0] if groups else '', '1',
                       teachers[0] if teachers else '', LEK)
    dlg = tk.Toplevel(root)
    # ... налаштування вікна ...

    # ... створення полів вводу ...

    saved = [False]
    def ok(): saved[0] = True; dlg.destroy()
    tk.Button(dlg, text='OK', command=ok, width=12, bg='#4a90d9', fg='white').grid(
        row=7, column=0, pady=12, padx=10)

    # ... очікування закриття вікна ...
    dlg.wait_window()

    if not saved[0]: return None
    if not dvars[3].get():
        messagebox.showwarning('!', u'Вкажіть групу', parent=root); return None
    return tuple(v.get() for v in dvars)
```

- **Аргументи**: `root` (головне вікно), `title` (заголовок вікна), `initial` (дані для заповнення полів при редагуванні).
- Динамічно створює поля вводу (`Entry` або `Combobox`) на основі списків `LBLS` та `OPTS`.
- Блокує взаємодію з основним вікном (`dlg.grab_set()`) до закриття діалогу.
- Повертає кортеж введених даних або `None`, якщо користувач натиснув "Скасувати".

### Функції дій (Actions)

#### `add_group(root, grp_combos)`, `add_teacher(root, tchr_combos)`

Використовують `simpledialog.askstring` для отримання нової назви групи або імені викладача.

```python
def add_group(root, grp_combos):
    name = simpledialog.askstring(
        u'Нова група',
        u'Назва групи:', parent=root) or ''
    if not name: return
    if name in groups: messagebox.showwarning('!', name + ' already exists', parent=root); return
    groups.append(name)
    for c in grp_combos: c['values'] = [''] + groups
```

- Перевіряють, чи існує вже такий запис.
- Додають новий запис до відповідного глобального списку (`groups` або `teachers`).
- Оновлюють випадаючі списки (`Combobox`) у фільтрі та діалогах.

#### `add_lesson(root, tree, fvars, status)`

Викликає `lesson_dialog` для створення нового заняття. Якщо дані отримано, додає їх до списку `lessons` і оновлює таблицю.

```python
def add_lesson(root, tree, fvars, status):
    row = lesson_dialog(root, u'Додати заняття')
    if row: lessons.append(row); apply_filter(tree, fvars, status)
```

#### `edit_lesson(root, tree, fvars, status)`

Редагує вибраний у таблиці запис.

```python
def edit_lesson(root, tree, fvars, status):
    sel = tree.selection()
    if not sel:
        messagebox.showwarning('!', u'Виберіть запис',
                               parent=root); return
    vals = tree.item(sel[0], 'values')
    idx  = next((i for i, r in enumerate(lessons) if tuple(r) == tuple(vals)), None)
    if idx is None: return
    row = lesson_dialog(root,
                        u'Редагувати заняття',
                        initial=lessons[idx])
    if row: lessons[idx] = row; apply_filter(tree, fvars, status)
```

1.  Отримує вибраний рядок (`tree.selection()`).
2.  Шукає індекс цього запису в глобальному списку `lessons`.
3.  Викликає `lesson_dialog`, передаючи поточні дані як `initial`.
4.  Якщо дані змінено, оновлює запис у списку `lessons` і оновлює таблицю.

#### `delete_lesson(root, tree, fvars, status)`

Видаляє вибраний запис після підтвердження користувачем (`messagebox.askyesno`).

```python
def delete_lesson(root, tree, fvars, status):
    sel = tree.selection()
    if not sel:
        messagebox.showwarning('!', u'Виберіть запис', parent=root); return
    if not messagebox.askyesno('?', u'Видалити вибраний запис?', parent=root): return
    # ... пошук та видалення ...
    vals = tree.item(sel[0], 'values')
    idx  = next((i for i, r in enumerate(lessons) if tuple(r) == tuple(vals)), None)
    if idx is not None: lessons.pop(idx)
    apply_filter(tree, fvars, status)
```

## 4. Головна функція `main()`

Це точка входу в програму. Вона виконує наступні дії:

```python
def main():
    root = tk.Tk()
    root.title(u'Розклад занять')
    # ...

    # Створення панелі фільтрів
    fp = tk.LabelFrame(root, text=u'  Пошук / Фільтр  ', ...)

    # ... створення віджетів фільтрації ...

    # Створення таблиці
    tree = ttk.Treeview(tv, columns=COLS, show='headings', selectmode='browse')

    # ... налаштування колонок ...

    # Прив'язка подій
    tree.bind('<Double-1>', lambda e: edit_lesson(root, tree, fvars, status))

    # Панель інструментів (кнопки)
    tk.Button(bar, text=u'+ Група', bg='#f0f0f0', **BTN,
              command=lambda: add_group(root, grp_combos)).pack(side='left', padx=3)

    # ... інші кнопки ...

    apply_filter(tree, fvars, status)
    root.mainloop()
```

1.  **Ініціалізація**: Створює головне вікно `root`, встановлює заголовок та розміри.
2.  **Панель фільтрів**: Створює `LabelFrame` з полями вводу та випадаючими списками для пошуку.
    - Прив'язує змінні `fvars` до віджетів.
    - Додає "trace" (відстеження змін) до змінних фільтру: при зміні будь-якого поля автоматично викликається `apply_filter`.
3.  **Таблиця (Treeview)**:
    - Налаштовує колонки та їх заголовки.
    - Додає сортування при кліку на заголовок колонки (`sort_col`).
    - Налаштовує кольори рядків за допомогою `tag_configure`.
    - Додає смуги прокрутки (`Scrollbar`).
    - Прив'язує подію подвійного кліку `<Double-1>` до функції редагування.
4.  **Панель інструментів (Toolbar)**: Створює кнопки для додавання груп, викладачів та занять, а також редагування та видалення.
    - Використовує `lambda` функції для передачі аргументів у callback-функції кнопок.
5.  **Запуск**: Запускає головний цикл обробки подій `root.mainloop()`, який тримає вікно відкритим.

## 5. Ключові аспекти Python у коді

### Lambda-функції

У коді часто використовуються `lambda` вирази — це невеликі анонімні функції. У `tkinter` вони часто використовуються для передачі функцій з аргументами у параметр `command`.

```python
command=lambda: apply_filter(tree_ref[0], fvars, status)
```

- _Приклад:_ `command=lambda: apply_filter(...)` дозволяє викликати функцію `apply_filter` з потрібними аргументами при натисканні кнопки, а не робити це відразу при створенні кнопки.

### List Comprehensions (Генератори списків)

Використовуються для лаконічної фільтрації даних.

```python
res = [r for r in lessons
       if subj.lower() in r[0].lower()
       and day  in r[1] and grp  in r[3]
       and week in r[4] and tchr in r[5] and typ in r[6]]
```

- _Приклад:_ `res = [r for r in lessons if ...]` — створює новий список `res`, що містить лише елементи з `lessons`, які задовольняють умові.

### Розпакування списків (Unpacking)

У функції `apply_filter`:

```python
subj, day, grp, week, tchr, typ = (v.get() for v in fvars)
```

...дозволяє одним рядком отримати значення всіх змінних фільтру і присвоїти їх відповідним локальним змінним.

### Віджет Treeview

Це потужний віджет для відображення табличних або ієрархічних даних.

- `show='headings'`: Приховує перший стовпець "дерева", залишаючи лише звичайну таблицю.
- `values=r`: Визначає дані, що будуть показані у клітинках рядка.
- `tags=(r[6],)`: Присвоює рядку тег, який відповідає типу заняття (наприклад, "Лекція"), що дозволяє задати колір фону через `tag_configure`.

```python
for tag, bg in TYPE_BG.items():
    tree.tag_configure(tag, background=bg)
```
