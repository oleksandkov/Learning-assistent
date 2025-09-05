#include <iostream> 
#include <string>

using namespace std;


int main() {
     const char* a = "aafjfdjfskjdfksjdfkjsdklfjsdlkfjalskdjfkajf";
    string s = "aafjfdjfskjdfksjdfkjsdklfjsdlkfjalskdjfkajf";
    // const char* massive[10];
    string  massive[10];
    for (int i = 0; i < 10; i++) {
        massive[i] = "aafjfdjfskjdfksjdfkjsdklfjsdlkfjalskdjfkajf";
        cout << massive[i] << endl;
    }
}