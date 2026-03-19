#include <stdio.h>
#include <iostream>

using namespace std;

int main() {
    int n, sum;
    cout << "Введіть тризначне число: \n";
    cin >> n;
    if (n < 100 || n > 999) {
        cout << "Число не тризначне.\n";
    } else {
    sum = n/100 + (n/10 % 10) +(n % 100 % 10);
    cout << "Сума розрядів це: " << sum << "\n";
    }
    return 0;

}