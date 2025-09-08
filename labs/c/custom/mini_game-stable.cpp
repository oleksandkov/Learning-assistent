#include <iostream>
#include <math.h>
#include <time.h>
#include <chrono>
#include <string>

#include "clocker.hpp"

using namespace std;
 


int main() {
    int number;
    cout << "The number of equlations: ";
    cin >> number;
    int tim = 5;
    timer(5);
    auto start = std::chrono::high_resolution_clock::now();
    // Generation 
    srand(time(NULL));
    int num_1, num_2;
    char operator_in;  
    char op_char;  
    int check = 0;
    for (int i = 0; i < number; i++) {
        num_1 = rand() % 20;
        num_2 = rand() % 20;
        // OPERATOR INDEX
    if( num_2 != 0 && num_1 % num_2 == 0 && num_1 > num_2) {

        operator_in = rand() % 6;
        if (operator_in == 0) {
            op_char = '+';
        } else if (operator_in == 1) { 
            op_char = '-';
        } else if (operator_in == 2) {
            op_char = '*';
        } else {
            op_char = '/';
        }
        cout << num_1 << " " << op_char << " " << num_2 << " = ";
        int answer;
        cin >> answer;
        // OPERATOR USAGE
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
        if (op_char == '*') {
            if (answer == num_1 * num_2) {
                check++;
            }
        } 
        if (op_char == '/') {
            if ( answer == num_1 / num_2) {
                check++;
            }
        }
        cout << endl;
    } else {
        operator_in = rand() % 3;
        if (operator_in == 0) {
            op_char = '+';
        } else if (operator_in == 1) { 
            op_char = '-';
        } else {
            op_char = '*';
        }
        cout << num_1 << " " << op_char << " " << num_2 << " = ";
        int answer;
        cin >> answer;
        // OPERATOR USAGE
        if (op_char == '+') {
            if (answer == num_1 + num_2) {
                check++;
            }
        }
        if (op_char == '-' ) {
            if (answer == num_1 - num_2) {
                check++;
            }
        }
        if (op_char == '*') {
            if (answer == num_1 * num_2) {
                check++;
            }
        }
        cout << endl;

    }
}
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout << "Correct answers: " << check << "/" << number << endl;
    cout << "Elapsed time: " << duration.count() << " ms" << endl;

    cin.get();
    cin.ignore();
}
