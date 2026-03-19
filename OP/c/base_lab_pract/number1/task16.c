/*
Завдання 16. Перевірка, чи можна зі сторін трьох введених трикутника скласти прямокутний трикутник.
*/
#include <stdio.h>
#include <math.h>
int main() {
    double a, b, c;
    printf("Введіть три сторони: ");
    scanf("%lf %lf %lf", &a, &b, &c);
    if (a + b > c && a + c > b && b + c > a && (fabs(a*a + b*b - c*c) < 1e-6 || fabs(a*a + c*c - b*b) < 1e-6 || fabs(b*b + c*c - a*a) < 1e-6)) {
        printf("Можна скласти прямокутний трикутник.\n");
    } else {
        printf("Не можна скласти прямокутний трикутник.\n");
    }
    return 0;
}
