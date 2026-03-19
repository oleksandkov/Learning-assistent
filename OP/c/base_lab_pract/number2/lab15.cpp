#include <stdio.h>
#include <iostream> 
#include <math.h>

using namespace std;

int main() {
    int k, i,  sum = 0;
    cout << "Entre the number: ";
    cin >> k;
    for (i = 1; i <= k; i++) {
        sum += pow(-1, k + 1)/ k*(k + 1);
    }
    cout << "Result: " << sum << "\n";
    return 0;
}
// by me