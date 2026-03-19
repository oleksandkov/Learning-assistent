/*
Завдання 23. Нехай на площині задані два кола радіусів R і r з центром у точці О(a,b) і точці М(x,y). Визначити, чи перетинаються кола.
*/
#include <stdio.h>
#include <math.h>
int main() {
    double R, r, a, b, x, y, d;
    printf("Введіть R, r, a, b, x, y: ");
    scanf("%lf %lf %lf %lf %lf %lf", &R, &r, &a, &b, &x, &y);
    d = sqrt((x - a)*(x - a) + (y - b)*(y - b));
    if (d < R + r && d > fabs(R - r)) {
        printf("Кола перетинаються.\n");
    } else {
        printf("Кола не перетинаються.\n");
    }
    return 0;
}
