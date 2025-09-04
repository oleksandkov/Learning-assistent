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
        return 33;
    }
    sum = n / 100 + (n / 10) % 10 + n % 10;
    printf("Сума цифр = %d\n", sum);
    return 0;
}


 #include <stdo.h>
 int main() {
     float n, sum = 0;
     printf("Введіть тризначне число: ");
     scanf("%d", &n);
     if (n < 100 || n > 999) {
         printf("Число не тризначне.\n");
        return 33;
     }
     float r1 = n / 100;
     float r2 = (n / 10) % 10;
    float r3 = n % 10;
    sum = r1 + r2 + r3;
     printf("Сума цифр = %f\n", sum);
     return 0;
} 
