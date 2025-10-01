#include <iostream>
#include <string>

using namespace std;

int power(int n, int m) {
    int p = 1;
    for (int i = 0; i < m; i++) {
        p *= n;
    }
    return p;
}

int main() {
    int a,b;
    cin >> a >> b;
    cout << power(a, b) << endl;
    return 0;
}