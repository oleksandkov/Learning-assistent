#include <stdio.h>
#include <iostream> 
#include <math.h>
#include <vector>

using namespace std;

void loop_vector (vector<int> x) {
    int i;
    for ( i = 0; i < x.size(); i++ ) {
        cout << x[i] << " ";
    }
}
int main () {
    int a, i;
    vector<int> numbers;
    cout << "Enter your number:\n";
    cin >> a;
    for (int i = 1; i <= a; i++) {
        if (a % i == 0) {
            numbers.push_back(i);    
        }
    }
    cout << "These are the numbers: "; 
    loop_vector(numbers);
    cin.get();
    cin.ignore();
}

// by me