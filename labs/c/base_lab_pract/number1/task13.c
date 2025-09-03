/*
Завдання 13. Визначити найбільшу цифру введеного тризначного числа.
*/
#include <stdio.h>
int main() {
    int n, max_digit;
    printf("Введіть тризначне число: ");
    scanf("%d", &n);
    if (n < 100 || n > 999) {
        printf("Число не тризначне.\n");
        return 1;
    }
    int d1 = n / 100, d2 = (n / 10) % 10, d3 = n % 10;
    max_digit = d1;
    if (d2 > max_digit) max_digit = d2;
    if (d3 > max_digit) max_digit = d3;
    printf("Найбільша цифра = %d\n", max_digit);
    return 0;
}
