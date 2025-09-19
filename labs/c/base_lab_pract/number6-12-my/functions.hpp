#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <ctime>
using namespace std;


void randomStringVector(vector<string>* x, int length) {
    srand(time(NULL));
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

void RandomNumber(double* x ) {
    srand(time(NULL));
    double number = 1 + static_cast<double>(rand()) / RAND_MAX * 99;
    *x = round(number * 1000) / 1000.0;
}

#endif // FUNCTIONS_HPP