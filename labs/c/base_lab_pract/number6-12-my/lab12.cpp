
#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>

using namespace std;


float strToFloat(const char *str) {
    // Convert C-string to float using strtof
    char* endptr;
    float n = std::strtof(str, &endptr);
    if (endptr == str || *endptr != '\0') {
        std::cout << "Input is not a valid number!" << std::endl;
        return 0.0f;
    }
    std::cout << "Your n is: " << n << std::endl;
    return n;
}
void test(const char* str, float n) {
    float val = std::strtof(str, nullptr);
    if (n == val) {
        cout << "You pass the test" << endl;
    } else {
        cout << "Test is not passed" << endl;
    }
}

int main() {
    char str[100];
    cout << "Enter a number: ";
    cin >> str;
    float n = strToFloat(str);
    test(str, n);
    std::cin.ignore();
    std::cin.get();
}