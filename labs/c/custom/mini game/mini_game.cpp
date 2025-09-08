#include <iostream>
#include <math.h>
#include <time.h>
#include <chrono>
#include <string>
#include <windows.h>

#include "clocker.hpp"

using namespace std;
 
void print_centered(const std::string& message) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns = 80; // default width
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    int padding = (columns - message.length()) / 2;
    if (padding > 0) std::cout << std::string(padding, ' ');
    std::cout << message << std::endl;
}

int main() {
    char play_again;
    do {
        int number;
        while (true) {
            couit << "The number of equlations: ";
            std::string input;
            cin >> input;
            try {
                number = std::stoi(input);
                if (number > 0) 
                break;
            } catch (...) {
            }
            cout << "Please enter a valid positive integer!" << endl;
        }
        int tim = 5;
        timer(5);
        auto start = std::chrono::high_resolution_clock::now();
        cout << "Put your answers!\n";
        cout << endl;
        // Generation 
        srand(time(NULL));
        int num_1, num_2;
        char operator_in;  
        char op_char;  
        int check = 0;
        for (int i = 0; i < number; i++) {
            // OPERATOR INDEX
            operator_in = rand() % 4;
            if (operator_in == 0) {
                op_char = '+';
                num_1 = rand() % 20;
                num_2 = rand() % 20;
            } else if (operator_in == 1) { 
                op_char = '-';
                num_1 = rand() % 20;
                num_2 = rand() % 20;
            } else if (operator_in == 2) {
                op_char = '*';
                num_1 = rand() % 20;
                num_2 = rand() % 20;
            } else {
                op_char = '/';
                num_2 = 1 + rand() % 100;
                num_1 = rand() % 1000;
                while(num_1 % num_2 != 0) {
                    num_1 = rand() % 1000;
                }
            }
            cout << num_1 << " " << op_char << " " << num_2 << " = ";
            std::string input;
            cin >> input;
            // If input is not a number, skip to next question
            try {
                int answer = std::stoi(input);
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
                    if (answer == num_1 / num_2) {
                        check++;
                    }
                }
            } catch (...) {
            }
            cout << endl;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        cout << "Correct answers: " << check << "/" << number << endl;
        cout << "Elapsed time: " << duration.count() << " ms" << endl;
        cout << "Do you want to play again? (y/n): ";
        cin >> play_again;
        cin.ignore();
        system("cls"); // optional: clear screen for next round
    } while (play_again == 'y' || play_again == 'Y');
    return 0;
}
