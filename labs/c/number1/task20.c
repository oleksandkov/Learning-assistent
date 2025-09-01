/*
Завдання 20. Обчислити значення виразу:
P = sin(a + b) / (3 * sqrt(a - b) - π)
*/
#include <stdio.h>
#include <math.h>
int main() {
    double a, b, P;
    printf("Введіть a та b: ");
    scanf("%lf %lf", &a, &b);
    P = sin(a + b) / (3 * sqrt(a - b) - M_PI);
    printf("P = %.4lf\n", P);
    return 0;
}
