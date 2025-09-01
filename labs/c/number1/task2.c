#include <stdio.h>
#include <math.h>

/*
Завдання 2. Напишіть програму, яка обчислює корені квадратного рівняння.
*/

#include <stdio.h>
#include <math.h>

int main() {
    double a, b, c, d, x1, x2;
    printf("Введіть коефіцієнти a, b, c квадратного рівняння: ");
    scanf("%lf %lf %lf", &a, &b, &c);
    d = b * b - 4 * a * c;
    if (d > 0) {
        x1 = (-b + sqrt(d)) / (2 * a);
        x2 = (-b - sqrt(d)) / (2 * a);
        printf("Два корені: x1 = %.2lf, x2 = %.2lf\n", x1, x2);
    } else if (d == 0) {
        x1 = -b / (2 * a);
        printf("Один корінь: x = %.2lf\n", x1);
    } else {
        printf("Коренів немає (дискримінант < 0)\n");
    }
    return 0;
}
