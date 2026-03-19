#include <iostream>
#include <math.h>
#include <string>
#include <time.h>

using namespace std;

struct POINT {
    int x, y, z;
} p;

struct SPHERE {
    POINT center;
    double r;
 } a , b , s;

void contains(SPHERE s, POINT p) {
    double length;
    length = sqrt(pow((p.x - s.center.x), 2) + pow((p.y - s.center.y), 2) + pow((p.z - s.center.z), 2));
    if (length <= s.r) {
         cout << "Your point in the sphere";
    } else {
         cout << "Your point out of sphere";
    }
}
void cross(SPHERE a, SPHERE b) {
    double length_r, length_cen;
    length_r = a.r + b.r;
    length_cen = sqrt(pow((a.center.x- b.center.x), 2) + pow((a.center.y - b.center.y), 2) + pow((a.center.z - b.center.z), 2));
    if (length_r < length_cen ) {
         cout << "Your spheres are not crossed\n"; 
    } else if (length_r == length_cen) {
         cout << "Your spheres make a dot!\n";
    } else {
         cout << "Your spheres are crossed!\n";
    }
}
int main() {
    SPHERE a, b;
    POINT p;
    int checker;
    string yn;
    srand(time(NULL));
    while(true) {
        cout << "What would you like to observe?\n";
        cout << "1: Check if spheres are crossed\n";
        cout << "2: Check if the point is in a sphere\n";
        cout << "(1/2) ";
        cin >> checker;
        if (checker == 2) {
            string yn;
            while(true) {
               cout << "Would you like to generate the point?\n";
               cout << "Yes/no ";
               cin >> yn;

               if (yn == "Yes"  || yn == "yes") {
                   p.x = -100 + rand() % 200;
                   p.y = -100 + rand() % 200;
                   p.z = -100 + rand() % 200;
                   break;
                } else if (yn == "No" || yn == "no") {
                    cout << "Put your point (x y z)\n";
                    cin >> p.x >> p.y >> p.z;
                    break;
                } else {
                    cout << "Unenxpected result. Try again\n";
                }
            }
            cout << "Your point is: " <<  p.x << " " << p.y << " " << p.z << "\n";
            while(true) {
                cout << "Would you like to generate your sphere\n";
                cout << "Yes/No ";
                yn = " ";
                cin >> yn;
                if (yn == "Yes" || yn == "yes") {
                    s.center.x = -100 + rand() % 200;
                    s.center.y = -100 + rand() % 200;
                    s.center.z = -100 + rand() % 200;
                    s.r = 1 + rand() % 20;
                    break;
                } else if (yn == "No" || yn == "no") {
                    cout << "Put the coordinate of the center  (x y z):\n";
                    cin >> s.center.x >> s.center.y >> s.center.z;
                    cout << "Put the radius of your sphere: \n";
                    cin >> s.r;
                    break;
                } else {
                    cout << "Unenxpected result. Try again\n";
                }
            }
        cout << "The center is: " << s.center.x << " " << s.center.y << " " <<  s.center.z << " and the radius is: " << s.r;
        cout << endl;
        cout << "THE RESULT: ";
        contains(s, p); 
        cout << endl;
    } else if ( checker == 1) {
        //
        while(true) {
            cout << "Would you like to generate the {a} sphere?\n";
            cout << "Yes/No ";
            yn = " ";
            cin >> yn;
            if (yn == "Yes" || yn == "yes") {
                a.center.x = -100 + rand() % 200;
                a.center.y = -100 + rand() % 200;
                a.center.z = -100 + rand() % 200;
                a.r = 1 + rand() % 20;
                 break;
            } else if (yn == "No" || yn == "no") {
                cout << "Put the coordinate of the ceneter of {a} sphere (x y z): \n";
                cin >> a.center.x >> a.center.y >> a.center.z;
                cout << "Put the radius of sphere {a}: \n";
                cin >> a.r;
                break;
            } else {
                cout << "Unenxpected result. Try again\n";
            }
        }
        //
        while(true) {
            cout << "Would you like to generate the {b} sphere?\n";
            cout << "Yes/No ";
            yn = " ";
            cin >> yn;
            if (yn == "Yes" || yn == "yes") {
                b.center.x = -100 + rand() % 200;
                b.center.y = -100 + rand() % 200;
                b.center.z = -100 + rand() % 200;
                b.r = 1 + rand() % 20;
                 break;
            } else if (yn == "No" || yn == "no") {
                cout << "Put the coordinate of the ceneter of {b} sphere (x y z): \n";
                cin >> b.center.x >> b.center.y >> b.center.z;
                cout << "Put the radius of sphere {b}: \n";
                cin >> b.r;
                break;
            } else {
                cout << "Unenxpected result. Try again\n";
            }
        }
        cout << "{a} center: " << a.center.x << " " << a.center.y << " " << a.center.z << " radius: " << a.r << "\n";
        cout << "{b} center: " << b.center.x << " " << b.center.y << " " << b.center.z << " radius: " << b.r << "\n";
        cout << endl;
        cout << "THE RESULT: ";
        cross(a,b);
    } else {

        cout << "Tip: Choose 1 or 2 \n";
        return 33;
}
    int checker2;
    cout << "Choose the next steps: \n";
    cout << "1: Out \n";
    cout << "2: Restart \n";
    cout << "Put your answer: ";
    cin >> checker2;
    if (checker2 == 1) {
        break;
    } else {
        system("cls");
    }
}
    cout << endl;
    cout << "Press Enter to exit..." << endl;
    cin.ignore();
    cin.get();
    return 0;
}