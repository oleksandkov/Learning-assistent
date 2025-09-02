/*
Завдання 1. Напишіть програму, яка перевіряє, чи можна зі сторін трьох
введених трикутника скласти трикутник. Для цього необхідно виконати
умови: сума двох будь-яких сторін має бути більше третьої.
*/




#include <stdio.h>
#include <locale.h>

int main() {
    // Set locale to system default for Unicode (Cyrillic) support
    setlocale(LC_ALL, "");
    // If you see unreadable symbols, run 'chcp 65001' in your terminal and use a font with Cyrillic support.
    double a, b, c;
    printf("Введіть три сторони трикутника: ");
    scanf("%lf %lf %lf", &a, &b, &c);

    if (a + b > c && a + c > b && b + c > a) {
        printf("Можна скласти трикутник зі сторін %.2lf, %.2lf, %.2lf.\n", a, b, c);
    } else {
        printf("Не можна скласти трикутник зі сторін %.2lf, %.2lf, %.2lf.\n", a, b, c);
    }
    return 0;
}
