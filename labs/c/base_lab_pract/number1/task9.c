/*
Завдання 9. Нехай на площині задані два концентричних кола радіусів R і r з центром у точці О(0,0) і точка М(x,y). Визначити, до якого кола вона ближча.
*/
#include <stdio.h>
#include <math.h>
int main() {
    double R, r, x, y, d;
    printf("Введіть R, r, x, y: ");
    scanf("%lf %lf %lf %lf", &R, &r, &x, &y);
    d = sqrt(x*x + y*y);
    if (fabs(d - R) < fabs(d - r)) {
        printf("Точка ближча до кола R.\n");
    } else {
        printf("Точка ближча до кола r.\n");
    }
    return 0;
}
