/*
Завдання 6. Трикутник заданий координатами x1, y1, x2, y2, x3, y3. Знайти периметр трикутника.
*/
#include <stdio.h>
#include <math.h>
int main() {
    double x1, y1, x2, y2, x3, y3, p;
    printf("Введіть координати x1, y1, x2, y2, x3, y3: ");
    scanf("%lf %lf %lf %lf %lf %lf", &x1, &y1, &x2, &y2, &x3, &y3);
    double a = hypot(x2 - x1, y2 - y1);
    double b = hypot(x3 - x2, y3 - y2);
    double c = hypot(x1 - x3, y1 - y3);
    p = a + b + c;
    printf("Периметр = %.4lf\n", p);
    return 0;
}
