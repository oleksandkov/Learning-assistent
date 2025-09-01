/*
Завдання 10. Ввести значення трьох дійсних змінних: z1, z2, z3. Поміняти місцями значення так, щоб z1 було найменшим, а z3 – найбільшим.
*/
#include <stdio.h>
int main() {
    double z1, z2, z3, min, max, mid;
    printf("Введіть z1, z2, z3: ");
    scanf("%lf %lf %lf", &z1, &z2, &z3);
    min = z1; max = z1;
    if (z2 < min) min = z2;
    if (z3 < min) min = z3;
    if (z2 > max) max = z2;
    if (z3 > max) max = z3;
    mid = z1 + z2 + z3 - min - max;
    printf("z1 = %.2lf, z2 = %.2lf, z3 = %.2lf\n", min, mid, max);
    return 0;
}
