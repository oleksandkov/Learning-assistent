#include <iostream>
#include <string>

using namespace std;


void mutate_into_binary(long long x) {
    int code[64];
    int i = 0;
    int num = x;
    if(num == 0) {
        cout << 0 << endl;
        return;
    }
    while(num > 0) {
        code[i] = num % 2;
        num = num / 2;
        i++;
    }
    for(int j = i - 1; j >= 0; j--) {
        cout << code[j];
    }
    cout << endl;
}

int main() {
    cout << "Put your number: \n";
    int number;
    cin >> number;
    cout << "Press to see the binary code..\n";
    cin.get();
    cin.ignore();
     mutate_into_binary(number);
    cin.get();
}   