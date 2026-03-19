#include <iostream>
#include <string>
using namespace std;

string resul(int n) {
    char arr[32];
    int idx = 0;
    bool negative = false;

    if (n < 0) {
        negative = true;
        n = -n;
    }

    if (n == 0) {
        arr[idx++] = '0';
    }

    while (n > 0) {
        arr[idx++] = char('0' + (n % 8));
        n /= 8;
    }

    string result;
    if (negative) {
        result += '-';
    }
    for (int i = idx - 1; i >= 0; --i) {
        result += arr[i];
    }
    return result;
}

