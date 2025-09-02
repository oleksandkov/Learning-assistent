/*
Завдання 21. Трикутник заданий координатами x1, y1, x2, y2, x3, y3. Знайти площу трикутника.
*/
#include <stdio.h>
#include <math.h>
#include <locale.h>
int main() {
    double x1, y1, x2, y2, x3, y3, S;
    printf("Введіть координати x1, y1, x2, y2, x3, y3: ");
    scanf("%lf %lf %lf %lf %lf %lf", &x1, &y1, &x2, &y2, &x3, &y3);
    S = fabs((x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2))/2);
    printf("Площа = %.4lf\n", S);
    return 0;
}
