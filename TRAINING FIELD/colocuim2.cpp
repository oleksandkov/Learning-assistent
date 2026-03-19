#include <iostream>
#include <string>

 using namespace std;

struct EXAMPLE {
    int number;
    float number_float;
    string sting;
};

 int main() {
    EXAMPLE x;
    x.number = 7;
    x.number_float = 8.8;
    x.sting = "blabla";

    cout << x.sting << " ";
    cout << endl;
    cout << x.number << " ";
    cout << endl;
    cout << x.number_float << " ";

    cout << endl;
    cout << "Pointer: " << endl;
    EXAMPLE* pointer = &x;
    cout << pointer->number;
    
    cin.get();
    cin.ignore();
 }


