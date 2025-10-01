#include <iostream>
#include <string> 

using namespace std;
//                                            Pointers 

// int main() {
//     int x = 3;
//     int y = 5;
//     int *px = &x; // pointer to x
//     y = *px + 1; // y = 3 + 1
//     cout << "Value of x: " << y << endl; // 3
//     cout << *px;
//     cin.get();
//     cin.ignore()
// }

// int main() {
//     int num1 = 1, num2 = 2, num3 = 3;
//     int *pnum2 = &num2, *pnum1 = &num1, *pnum3 = &num3;
//     pnum1++;
//     *pnum1 = 0;
//     cout << num1 << " " << num2 << " " << num3 << endl; // 1 2 0
// }


// Arrays and pointers

int main() {
    int a[10];
    int *pa;
    pa = &a[0]; // pa points to the first element of a
    int x = *pa;
    // or
    x = a[0];
    int **ppnum = &pnum2;
    **ppnum = 0;
    x = *(a + i); // a + i is the address of the i-th element of a


    cin.get();
    cin.ignore();
}