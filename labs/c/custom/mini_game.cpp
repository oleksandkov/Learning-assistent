#include <iostream>
#include <math.h>
#include <time.h>
#include <chrono>

#include "clocker.hpp"

using namespace std;
 


int main() {
    int number;
    cout << "The number of equlations: ";
    cin >> number;
    int tim = 5;
    timer(5);
    auto start = std::chrono::high_resolution_clock::now();
    srand(time(NULL));
    int num_1, num_2;
    char operator_in;  
    char op_char;  // Use a different name than 'operator'
    int check = 0;
    for (int i = 0; i < number; i++) {
        num_1 = rand() % 20;
        num_2 = rand() % 20;
        operator_in = rand() % 2;
        if (operator_in == 0) {
            op_char = '+';
        } else {
            op_char = '-';
        }
        cout << num_1 << " " << op_char << " " << num_2 << " = ";
        int answer;
        cin >> answer;
        if (op_char == '+') {
            if (answer == num_1 + num_2) {
                check++;
            }
        }
        if (op_char == '-') {
            if (answer == num_1 - num_2) {
                check++;
            }
        }
        cout << endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout << "Correct answers: " << check << endl;
    cout << "Elapsed time: " << duration.count() << " ms" << endl;

    cin.get();
    cin.ignore();
}
