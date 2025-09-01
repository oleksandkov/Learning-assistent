/*
Завдання 12. Обчислити суму цифр введеного тризначного числа.
*/
#include <stdio.h>
int main() {
    int n, sum = 0;
    printf("Введіть тризначне число: ");
    scanf("%d", &n);
    if (n < 100 || n > 999) {
        printf("Число не тризначне.\n");
        return 1;
    }
    sum = n / 100 + (n / 10) % 10 + n % 10;
    printf("Сума цифр = %d\n", sum);
    return 0;
}
