---
### Data Types in C / Типи даних у C

| Type / Тип   | Description / Опис                                                   | Example Declaration / Приклад оголошення |
|--------------|----------------------------------------------------------------------|------------------------------------------|
| int          | Integer (whole number) / Ціле число                                  | int a = 5;                               |
| float        | Single-precision float / Число з плаваючою комою (одинарна точність) | float b = 3.14f;                         |
| double       | Double-precision float / Число з плаваючою комою (подвійна точність) | double c = 2.718;                        |
| char         | Character / Символ                                                   | char d = 'A';                            |
| long         | Long integer / Довге ціле число                                      | long e = 123456L;                        |
| short        | Short integer / Коротке ціле число                                   | short f = 10;                            |

---
### printf() and scanf() Format Specifiers / Форматні специфікатори printf() і scanf()

| Specifier / Специфікатор | Meaning / Значення                                                  | Example Usage / Приклад використання   |
|--------------------------|---------------------------------------------------------------------|----------------------------------------|
| %d        | int (decimal integer) / int (десяткове ціле число)                                 | printf("%d", a);                       |
| %f        | float (decimal floating point) / float (десяткове число з плаваючою комою)         | printf("%f", b);                       |
| %.2f      | float with 2 decimal places / float з 2 знаками після коми                         | printf("%.2f", b);                     |
| %lf       | double (floating point) / double (число з плаваючою комою)                         | printf("%lf", c);                      |
| %c        | char (character) / char (символ)                                                   | printf("%c", d);                       |
| %s        | string (char array) / рядок (масив char)                                           | printf("%s", str);                     |
| %x        | int (hexadecimal) / int (шістнадцятковий)                                          | printf("%x", a);                       |
| %o        | int (octal) / int (вісімковий)                                                     | printf("%o", a);                       |
| %u        | unsigned int / unsigned int (беззнакове ціле)                                      | printf("%u", a);                       |
| %%        | print a percent sign / вивести знак відсотка                                       | printf("%%");                          |

#### Note on scanf() and & (address-of operator) / Примітка щодо scanf() та & (оператор взяття адреси)

In `scanf()`, you must pass the address of the variable where the input will be stored. This is done using the `&` operator. / У `scanf()` потрібно передавати адресу змінної, куди буде записано введене значення. Для цього використовується оператор `&`:

```c
int a;
scanf("%d", &a); // &a means "address of a" / &a означає "адреса a"
```

For strings (char arrays), you do not use `&` because the array name already represents its address. / Для рядків (масивів char) `&` не використовується, оскільки ім'я масиву вже є адресою:

```c
char str[100];
scanf("%s", str); // no & needed / без &
```

## C Operators Reference / Оператори мови C

---
This file lists all standard C operators for quick reference. / Цей файл містить усі стандартні оператори мови C для швидкого ознайомлення.
### Arithmetic Operators / Арифметичні оператори

| Operator / Оператор | Description / Опис                      | Example / Приклад   |
|---------------------|-----------------------------------------|---------------------|
| +        | Addition / Додавання                               | a + b               |
| -        | Subtraction / Віднімання                           | a - b               |
| *        | Multiplication / Множення                          | a * b               |
| /        | Division / Ділення                                 | a / b               |
| %        | Modulus (remainder) / Остача від ділення           | a % b               |

### Assignment Operators / Оператори присвоєння

| Operator / Оператор | Description / Опис                      | Example / Приклад   |
|---------------------|-----------------------------------------|---------------------|
| =        | Assignment / Присвоєння                            | a = b               |
| +=       | Add and assign / Додати і присвоїти                | a += b              |
| -=       | Subtract and assign / Відняти і присвоїти          | a -= b              |
| *=       | Multiply and assign / Помножити і присвоїти        | a *= b              |
| /=       | Divide and assign / Поділити і присвоїти           | a /= b              |
| %=       | Modulus and assign / Взяти остачу і присвоїти      | a %= b              |

### Increment/Decrement Operators / Оператори інкременту/декременту

| Operator / Оператор | Description / Опис                       | Example / Приклад   |
|---------------------|------------------------------------------|---------------------|
| ++       | Increment / Збільшити на 1                          | a++ or ++a          |
| --       | Decrement / Зменшити на 1                           | a-- or --a          |

### Relational Operators / Оператори відношення

| Operator / Оператор | Description / Опис                      | Example / Приклад   |
|---------------------|-----------------------------------------|---------------------|
| ==       | Equal to / Дорівнює                                | a == b              |
| !=       | Not equal to / Не дорівнює                         | a != b              |
| >        | Greater than / Більше                              | a > b               |
| <        | Less than / Менше                                  | a < b               |
| >=       | Greater or equal / Більше або дорівнює             | a >= b              |
| <=       | Less or equal / Менше або дорівнює                 | a <= b              |

### Logical Operators / Логічні оператори

| Operator / Оператор | Description / Опис                       | Example / Приклад   |
|---------------------|------------------------------------------|---------------------|
| &&       | Logical AND / Логічне І                             | a && b              |
| ||       | Logical OR / Логічне АБО                            | a || b              |
| !        | Logical NOT / Логічне НЕ                            | !a                  |

### Bitwise Operators / Побітові оператори

| Operator / Оператор | Description / Опис                        | Example / Приклад   |
|---------------------|-------------------------------------------|---------------------|
| &        | Bitwise AND / Побітове І                             | a & b               |
| |        | Bitwise OR / Побітове АБО                            | a | b               |
| ^        | Bitwise XOR / Побітове виключне АБО                  | a ^ b               |
| ~        | Bitwise NOT / Побітове НЕ                            | ~a                  |
| <<       | Left shift / Зсув вліво                              | a << 2              |
| >>       | Right shift / Зсув вправо                            | a >> 2              |

### Conditional (Ternary) Operator / Тернарний оператор (умовний)

| Operator / Оператор | Description / Опис                       | Example / Приклад   |
|---------------------|------------------------------------------|---------------------|
| ? :      | Ternary / Тернарний                                 | a ? b : c           |

### Comma Operator / Оператор коми

| Operator / Оператор | Description / Опис                      | Example / Приклад   |
|---------------------|-----------------------------------------|---------------------|
| ,        | Comma / Кома                                       | a = (b = 3, b+2)    |

### Other Operators / Інші оператори

| Operator / Оператор | Description / Опис                     | Example / Приклад   |
|---------------------|----------------------------------------|---------------------|
| sizeof | Size of data type / Розмір типу даних               | sizeof(int)         |
| &      | Address of / Адреса                                 | &a                  |
| *      | Pointer to / Вказівник                              | *ptr                |
| ->     | Structure pointer / Доступ до члена структури       | ptr->member         |
| .      | Structure member / Доступ до члена структури        | obj.member          |
