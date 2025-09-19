#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "functions.hpp"

using namespace std;
 
void strToFloat(const vector<string>& str, vector<float>& validNumbers) {
    for (int i = 0; i < str.size(); ++i) {
        const string& s = str[i];
        char* endptr = nullptr;
        float val = strtof(s.c_str(), &endptr);
        // float val = strtof(s.c_str(),; nullptr);
        
        if (endptr != s.c_str() && *endptr == '\0') {
            validNumbers.push_back(val);
            cout << "Valid float: " << val << endl;
        } else {
             cout << "Invalid float detected in: '" << s << "'" << endl;
        }
    }
}




int main() {
    // vector<string> input;
    // vector<float> value;
    // cout << "Enter float values separated by space: ";
    // string temp;
    // while (cin >> temp) {
    //     input.push_back(temp);
    //     if (cin.peek() == '\n' ) break;
    // }
    // strToFloat(input, value);
    // cout << "Tap to see the result\n";
    // cin.get();
    // cin.ignore();
    // for (const auto& element : value) {
    //     cout << element << "";
    // }
    // cin.get();
    // cin.ignore();

    vector<string> input;
    randomStringVector(&input, 5);
    for (const auto& str : input) {
        cout << str << endl;
    }
    double number;
    RandomNumber(&number);
    cout << "Random number: " << number << endl;
    cin.get();
    cin.ignore();
    
}




