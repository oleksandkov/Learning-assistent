#include <iostream>

 using namespace std;


 int main() {
   int x = 5;
   int* x_pointer;
   int** x_pointer_2;
   x_pointer = &x;
   x_pointer_2 = &x_pointer;

   cout << *x_pointer;
   cout << endl;
   **x_pointer_2 = 15;
   cout << x;


   cin.get();
   cin.ignore();
 }