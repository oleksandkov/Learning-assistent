#include <stdio.h>


int main() {
    int i, num = 0;
    for (i = 2000; i >= 2000 && i <= 3000; i ++ ) {
        int r1, r2, r3, r4;
        r1 = i / 1000;
        r2 = i /100 % 10;
        r3 = i /10 % 10;
        r4 = i % 10;
        if (r1 != r2 && r1 != r3 && r1 != r4 && r2 != r3 && r2 != r4 && r3 != r4) {
            num++;
        }
    }
    printf("Кількість чисел у запису яких немає двх однакових цифр: %d\n", num);
    return 0;

}