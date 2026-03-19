#include <stdio.h>
#include <iostream>

int main() {
    int r1,r2,r3, num;
    printf("Write your number: \n");
    scanf("%d", &num);
    r1 = num /100;
    r2 = num / 10 % 10;
    r3 = num % 10;
    int var1 = r1 * 100 + (r3 * 10 + r3) + r2;
    int var2 = r3 * 100 + (r2 * 10 + r2) + r1;
    int var3 = r2 * 100 + (r1 * 10 + r1) + r3;
    int var4 = r2 * 100 + (r3 * 10 + r3) + r1;
    int var5 = r3 * 100 + (r1 * 10 + r1) + r2;
    printf("There are the all combinations: %d, %d, %d, %d, %d, %d", num, var1,var2,var3,var4,var5 );
    std::cin.get();
    std::cin.ignore();
}