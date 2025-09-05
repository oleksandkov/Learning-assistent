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
    //  Умова перезапуску 1
    if (max <= min) {
        cout << "Try again!\n";
        return 33;
    }   
    //



    /*int max_to_use = max - min;*/
    for (int i = 0; i < 75; i++) {
         A1[i] = min + ((float)rand() / RAND_MAX) * (max - min);
        /* A1[i] = min + (rand() % max_to_use);*/
    }

    
    float A2[75] = {0.0}; 
    int sum = 0;
    for (int i = 0; i < 75; i++) {
        if (25.8 < A1[i] && A1[i] < 34.7) {
            A2[sum] = A1[i];
            sum++;
    }
    // Умова перезапуску 2
    }
    if (sum == 0) {
        cout << "A2 is empty, try again!\n";
        return 33;
    }
    //
    int newIndex = 0;
    for (int i = 0; i < 75; i++) {
        if (A2[i] != 0.0f) {
            A2[newIndex] = A2[i];
            newIndex++;
        }
    }
    sum = newIndex; 
    cout << "Натиcни щоб побачити масив A1: \n";
    cin.get();
    cin.ignore();
    for (int i = 0; i < 75; i++) {
        cout << A1[i] << " ";
    }
    cout << endl;
    cout << "Натисни щоб побачити масив A2: \n";
    cin.get();
    for (int i = 0; i < sum; i++) {
        cout << A2[i] << " ";
    }
    cout << endl;
    cout << "Кількість елементів у масиві A2: " << sum << "\n";
    cout << endl;
    return 0;
}
