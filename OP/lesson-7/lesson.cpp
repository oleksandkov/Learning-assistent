#include <iostream> 

using namespace std;


long long fac(int n) {
    long long res = 1;
for (int i=1; i<=n; i++)
res *= i;
return res;
}

void showBitwiseShifts(int n) {
    cout << "Original n: " << n << endl;
    cout << "n << 2 (left shift by 2): " << (n << 2) << endl;
    cout << "n >> 2 (right shift by 2): " << (n >> 2) << endl;
}

long long fac1(int n)
{
    if (n==0) return 1;
    return fac1(n-1)*n;
}


int fib(int n)
{
    if(n==0) return 0;
    if (n == 1) return 1;
    return fib(n-1) + fib(n-2);
}


inline int min1(int a, int b)
{
    if(a<b) return a;
    return b;
}
int main() {
    char s[2][5] = {{"Yes"}, {"No"}};
    int year = 2024;

    int a = 11, b = 6;
    cout << s[(bool)(year & 3)]; 
    cin.get();
    cin.get();
}

// n! = 1*2*3....*n