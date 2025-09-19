#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>
using namespace std;


void randomStringVector(vector<string>* x, int length) {
    string word;
    x->clear();
    for (int j = 0; j < length; ++j) {
        word.clear();
        int wordLength = 2 + rand() % 12; 
        for (int i = 0; i < wordLength; ++i) {
            char letter = 'a' + rand() % 26;
            word += letter;
        }
        x->push_back(word);
    }
}

void randomVecotorElement(vector<string>* x) {
    char letter;
    float number_float = static_cast<float>(rand()) / RAND_MAX * 100.0f;
    number_float = round(number_float * 1000) / 1000.0;
    int number_int = rand() % 100; 
    int checker = rand() % 2;

    if (checker == 0) {
        x->push_back(to_string(number_float));
    } else {
        int length = 2 + rand() % 12;
        string word;
        for (int i = 0; i < length; ++i) {
            int random = rand() % 3;
            if(random == 0) {
                letter = 'a' + rand() % 26;
                word += letter;
            } else if (random == 1) {
                letter = 'A' + rand() % 26;
                word += letter;
            } else {
                word += to_string(number_int % 10);
                number_int /= 10;
                if (number_int == 0) number_int = rand() % 100; // refill if run out
            }
        }
        x->push_back(word);
    }
}

void RandomNumber(double* x ) {
    srand(time(NULL));
    double number = 1 + static_cast<double>(rand()) / RAND_MAX * 99;
    *x = round(number * 1000) / 1000.0;
}

#endif // FUNCTIONS_HPP