/*
Завдання 14. Обчислення напруги на кожному з послідовно сполучених ділянок електричного ланцюга опором R1, R2, R3.
*/
#include <stdio.h>
int main() {
    double R1, R2, R3, I, U1, U2, U3;
    printf("Введіть R1, R2, R3, силу струму I: ");
    scanf("%lf %lf %lf %lf", &R1, &R2, &R3, &I);
    U1 = I * R1;
    U2 = I * R2;
    U3 = I * R3;
    printf("U1 = %.2lf, U2 = %.2lf, U3 = %.2lf\n", U1, U2, U3);
    return 0;
}
