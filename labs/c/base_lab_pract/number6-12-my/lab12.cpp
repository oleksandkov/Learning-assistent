
#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cstring>
#include <assert.h>


using namespace std;

float strToFloat(const char *str, float *n) {
    char *end;
    float val = strtof(str, &end);
    if ( *end != '\0') {
        return -1.0f;
    }
     *n = val;
    return val;
}

void test_strToFloat() {
    float value;
    assert(strToFloat("123.45", &value) == 123.45f);
    assert(strToFloat("abc", &value) == -1.0f);
    assert(strToFloat("12.34abc", &value) == -1.0f);
    cout << "All tests passed!" << endl;
}

int main() {
    char input[100];
    float value;

    cout << "Enter a float value: ";
    cin >> input;

    test_strToFloat();

    float res = strToFloat(input, &value);
    if (res == -1.0f) {
        cout << "Invalid float input\n";
    } else {
        cout << "Valid float: " << value << '\n';
    }
    cin.get();
    cin.ignore();
    return 0;
}