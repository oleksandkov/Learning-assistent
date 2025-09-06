#include <iostream>
#include <string>
#include <assert.h>

using namespace std;


int main () {
	int matrix[3][3];
	int i, j;
	// Declaring the matrix
	cout << "Enter the matrix (3x3):\n";
	for ( i = 0; i < 3; ++i) {
		for (j = 0; j < 3; ++j) {
			cout << "Element [" << i << "][" << j << "]: ";
			cin >> matrix[i][j];
		}
	}
	// Checker
	int check = 0;
	if(matrix[1][0] == matrix[1][2] && matrix[2][0] == matrix[0][2] && matrix[2][1] == matrix[0][1]) {
			check = 1;
		}
	// Showing the matrix
	cout << "Press to see the matrix..\n";
	cin.get();
	int count = 0;
	for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
        cout << matrix[i][j] << " ";
    }
    cout << endl;
	}
	cout << "See the result: \n";
	if(check == 1) {
		cout << "Your matrix is symmetric!\n";
	} else {
		cout << "Your matrix is not symmetric\n";
	}
	// End of the code
	cin.get();
	cin.ignore();
	}