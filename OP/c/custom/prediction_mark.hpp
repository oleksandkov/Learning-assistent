#ifndef PREDICTION_MARK_HPP
#define PREDICTION_MARK_HPP

#include <iostream>
#include <string>
#include <time.h>

using namespace std;
void prediction() {
    int mark;
    srand(time(NULL));
    mark = rand() % 5;
     if (mark == 0) {
        cout << "You will get F-" << endl;
    } else if (mark == 1) {
        cout << "You will get D" << endl;
    } else if (mark == 2) {
        cout << "You will get C" << endl;
    } else if (mark == 3) {
        cout << "You will get B, not bad" << endl;
    } else {
        cout << "You will get A+, good job!" << endl;
    }
}

#endif