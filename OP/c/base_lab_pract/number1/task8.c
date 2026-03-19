/*
Завдання 8. Дано додатні a, b, c, x. З'ясувати чи пройде цегла з ребрами a, b, c у квадратний отвір зі стороною x.
*/
#include <stdio.h>
int main() {
    double a, b, c, x;
    printf("Введіть a, b, c, x: ");
    scanf("%lf %lf %lf %lf", &a, &b, &c, &x);
    if ((a <= x && b <= x) || (a <= x && c <= x) || (b <= x && c <= x)) {
        printf("Можна.\n");
    } else {
        printf("Не можна.\n");
    }
    return 0;
}
