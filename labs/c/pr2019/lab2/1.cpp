#include <iostream>
#include <stdio.h>

int main() {
    int a,a_square, a_3;
    printf("Enter your numbers:\n");
    scanf("%d", &a);
    a_square = a * a;
    a_3 = a*a*a;
    printf("%d\n", a);
    printf("%d  %d\n", a_square,a_3);
    printf("%d  %d  %d\n",a_3, a_square, a);
    printf("Press any boton....");
    std::cin.get();
    std::cin.ignore();

}