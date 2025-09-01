/*
Завдання 5. Обчислити значення виразу:
P = sin(a + b) / (3 * cos^3(a - b) - π)
*/
#include <stdio.h>
#include <math.h>
// Студентське визначення числа Пі
const double PI = 3.1415926535;
int main() {
    double a, b, P;
    printf("Введіть a та b: ");
    scanf("%lf %lf", &a, &b);
    P = sin(a + b) / (3 * pow(cos(a - b), 3) - PI);
    printf("P = %.4lf\n", P);
    return 0;
}
