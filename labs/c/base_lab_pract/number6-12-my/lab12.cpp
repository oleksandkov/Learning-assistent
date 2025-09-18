
#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cstring>

using namespace std;

void strToFloat(char *str, float *n) {
    char *p;
    *n = strtof(str, &p);
    if (strlen(p) > 0) {
        cout << "Pointer p points to: '" << p << "'" << endl;
        cout << "Invalid float detected.\n" << endl;
        cout << -1 << endl;
    }
   if (strlen(p) == 0) {
        cout << "Valid float: " << *n << endl;
   }
}

int main() {
    char input[100]; 
    float value;

    cout << "Enter a float value: ";
    cin >> input;
    strToFloat(input, &value);

    cin.get();
    cin.ignore();

    return 0;
}