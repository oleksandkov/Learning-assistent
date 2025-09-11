#include <cassert>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>

using namespace std;

int calculate_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    float s;
    s = 0.5*abs(x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2);
    return s;
}
// void test(int x1, int y1, int x2, int y2, int x3, int y3) {
//     int x1 = 1, y1 = 4;
//     int x2 = 5, y2 = 19;
//     int x3 = 0, y3 = 4;
//     double s = 0.5 * abs(x1 * y2 + x2 * y3 + x3 * y1 - x1 * y3 - x2 * y1 - x3 * y2);
//     assert(s == 30.0 && "Test failed! Area does not match expected value.");
// }
int main() {
    int x1,x2, x3,x4, y1,y2,y3,y4;
    cout << "Enter the coordinate of first apex (x;y): \n";
    cin >> x1 >> y1;
    cout << "Enter the coordinate of second apex(x;y): \n";
    cin >> x2 >> y2;
    cout << "Enter the coordinate of third apex(x;y): \n";
    cin >> x3 >>  y3;
    cout << "Enter the coordinate of forth apex(x;y): \n";
    cin >> x4 >> y4;
    // Unit test number 1
    // test(x1,y1,x2,y2,x3,y3);
    float s1 = calculate_area(x1,y1,x2,y2,x3,y3);
    // Unit test number 2
    //  test(x3,y3,x4,y4,x1,y1);
    float s2 = calculate_area(x3,y3,x4,y4,x1,y1);
    float total_s = s1 + s2;
    cout << "Press to see the result.";
    cin.get();
    cin.ignore();
    cout << "The area of figure: " << total_s;
    cin.get();
    cin.ignore();

}