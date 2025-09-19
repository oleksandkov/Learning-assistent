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
    vector<string> input;
    vector<float> value;
    while(true ) {
        string checker;
        cout << "Do you want to generated the elements\n";
        cout << "Yes/No ";
        cin >> checker;
        if (checker == "Yes" || checker == "yes")
        {
            cout << "Put the number of elemnts\n";
            int length;
            cin >> length;
            for(int i = 0; i < length; ++i) {
                randomVecotorElement(&input);
            }
            break;
        } else if ( checker == "No" || checker == "no") 
        {
            cout << "Enter float values separated by space: ";
            string temp;
            while (cin >> temp) {
                input.push_back(temp);
                if (cin.peek() == '\n' ) break;
            }
            break;
        } else {
            continue;
        }
    }
    strToFloat(input, value);
    cout << "Tap to see the result\n";
    cin.get();
    cin.ignore();
    for (const auto& element : value) {
        cout << element << " ";
    }
    cout << endl;
    cout << "The number of elements is: " << value.size() << "\n";
    cin.get();
    cin.ignore();

    
}




