#include <stdio.h>

int main() {
    int n, sum;
    printf("Введіть тризначне число: ");
    scanf("%d", &n);
    if (n < 100 || n > 999) {
        printf("Число не тризначне.\n");
    } else {
    sum = n/100 + (n/10 % 10) +(n % 100 % 10);
    printf("Сума цифр = %d\n", sum);
    }
    return 0;

}