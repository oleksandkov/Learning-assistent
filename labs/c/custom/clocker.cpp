#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <conio.h>

using namespace std;

#include "clocker.hpp"

int main() {
    
    int seconds;
    cout << "Enter timer seconds: ";
    cin >> seconds;
    timer(seconds);
    // Your main code starts here after the timer
    cout << "Main code running!" << endl;
    return 0;
}   