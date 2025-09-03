#include <iostream>
#include <vector>

using namespace std;

int main() {
    int n;
    cout << "Enter the rows of Pyramid: ";
    cin >> n;

    // Generate Fibonacci sequence
    vector<long long> fib;
    fib.push_back(0);
    if (n > 1) fib.push_back(1);
    for (int i = 2; i < n * (n + 1) / 2; i++) {
        fib.push_back(fib[i-1] + fib[i-2]);
    }

    // Print the triangle
    int index = 0;
    int temp = n;
    for (int row = 1; row <= n; row++) {
        // Print spaces for centering
        for (int c = 1; c < temp; c++) {
            cout << " ";
        }
        temp--;

        // Print Fibonacci numbers for this row
        for (int c = 1; c <= 2 * row - 1; c++) {
            if (index < fib.size()) {
                cout << fib[index++];
            }
        }
        cout << endl;
    }

    cout << "Press Enter to exit...";
    cin.ignore();
    cin.get();
    return 0;
}