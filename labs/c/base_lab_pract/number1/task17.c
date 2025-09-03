/*
Завдання 17. Обчислення коренів бі-квадратного рівняння.
*/
#include <stdio.h>
#include <math.h>
int main() {
    double a, b, c, D, x1, x2;
    printf("Введіть a, b, c: ");
    scanf("%lf %lf %lf", &a, &b, &c);
    D = b*b - 4*a*c;
    if (D < 0) {
        printf("Коренів немає.\n");
    } else {
        double y1 = (-b + sqrt(D)) / (2*a);
        double y2 = (-b - sqrt(D)) / (2*a);
        if (y1 >= 0) {
            x1 = sqrt(y1);
            x2 = -sqrt(y1);
            printf("x1 = %.2lf, x2 = %.2lf\n", x1, x2);
        }
        if (y2 >= 0) {
            x1 = sqrt(y2);
            x2 = -sqrt(y2);
            printf("x3 = %.2lf, x4 = %.2lf\n", x1, x2);
        }
        if (y1 < 0 && y2 < 0) printf("Дійсних коренів немає.\n");
    }
    return 0;
}
