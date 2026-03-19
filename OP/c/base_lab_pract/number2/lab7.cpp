#include <stdio.h>
#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

void loop_vector (vector<int> result) {
    for (int i = 0; i < result.size(); i++) {
        cout << result[i] << " ";
    }
}
int main() {
    int m, i, sum = 0;
    vector<int> result;
    cout << "Enter your number: \n";
    cin >> m;
    for (i = 99; i <= 999; i++) {
        int r1,r2,r3;
        r1 = i / 100;
        r2 = (i/10) % 10;
        r3 = i % 10;
        if (m == r1 + r2 + r3) {
            result.push_back(i);
            sum++;
        } else {
            continue;
        }
    }
    cout << "Result: \n";
    loop_vector(result);
    
    cout << "\nNumber of positive results: " << sum << "\n";
    cin.ignore();
    cin.get();
}

// by me