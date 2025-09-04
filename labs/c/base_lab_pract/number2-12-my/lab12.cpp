#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

void loop_vector(vector<int> v) {
    for (int i = 0; i < v.size(); i++) {
        cout << v[i] << " ";
    }
}

int main() {
vector<int> v;
int i, num = 0;
for (i = 2000; i >= 2000 && i <= 3000; i ++ ) {
    int r1, r2, r3, r4;
        r1 = i / 1000;
        r2 = i /100 % 10;
        r3 = i /10 % 10;
        r4 = i % 10;
        if (r1 != r2 && r1 != r3 && r1 != r4 && r2 != r3 && r2 != r4 && r3 != r4) {
            num++;
        }
        
        if (r1 != r2 && r1 != r3 && r1 != r4 && r2 != r3 && r2 != r4 && r3 != r4) {
            v.push_back(i); 
        }
        
    }
    std::cout << ("Кількість чисел у запису яких немає двх однакових цифр:") << num << "\n";
    cout << "Натисни щоб побачити ці числа: \n";
    cin.get();
    cout << "Числа: \n";
    loop_vector(v);
    return 0; 
}

