#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdlib.h> // for rand() and srand()
#include <vector>

using namespace std;


int main() {
    float A1[75];
    float min, max;
    cout << "Введи мінімальну межу: \n";
    cin >> min;
    cout << "Введи максимальні межу: \n";
    cin >> max; 
    srand(time(NULL));
    if (max <= min) {
        cout << "Try again!\n";
        return 33;
    }   
    
    // int max_to_use = max - min;
    for (int i = 0; i < 75; i++) {
         A1[i] = min + ((float)rand() / RAND_MAX) * (max - min);
        // A1[i] = min + (rand() % max_to_use);
    }
    float A2[75] = {0.0};
    for (int i = 0; i < 75; i++) {
        if (25.8 < A1[i] && A1[i] < 34.7) A2[i] = A1[i];
    }

    cout << "Натисни щоб побачити масив A2: \n";
    cin.get();
    for (int i = 0; i < 75; i++) {
        cout << A2[i] << ", ";
    }
    cin.get();
    cin.ignore();
}
