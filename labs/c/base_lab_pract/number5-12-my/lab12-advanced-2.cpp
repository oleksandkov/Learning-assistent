#include <iostream>
#include <string>
#include <assert.h>
#include <math.h>
#include <time.h>

using namespace std;


// Test if a square matrix is symmetric (C-style array version, fixed size for columns)
void test(int n, int m, double* matrix) {
    // Unit test to check if the matrix is symmetric
    if (n != m) {
        cout << "Matrix is not square, cannot be symmetric.\n";
        return;
    }
    bool symmetric = true;
    for (int i = 0; i < n && symmetric; ++i) {
        for (int j = 0; j < n; ++j) {
            if (matrix[i * m + j] != matrix[j * m + i]) {
                symmetric = false;
                break;
            }
        }
    }
    if (symmetric) {
        cout << "You pass the test\n";
    } else {
        cout << "Test is not passed \n";
    }
}


int main () {
    int  n, m;
	int i, j;
    // Declaring the size of matrix
    cout << "Type the size of your matrix\n";
    cout << "Type the number of rows: ";
    cin >> n;   
    cout << "Type the number of columns: ";
    cin >> m;
    if (n != m){
        cout << "Your size of matrix is not correct!\n";
        cout << "You need to create a square matrix.\n";
        return 33;
    }
    double matrix[n][m];
	// Declaring the matrix
    while (true) {
        string path;
        cout << "Would you like to generate the matrix automatically?(Yes/No)\n";
        cin >> path;
        if (path == "Yes" || path == "yes") {
            srand(time(NULL));
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    matrix[i][j] = rand() % 100;
                }
            }
            break;
        } else if (path == "No" || path == "no") {
            cout << "Enter the matrix (nxm):\n";
            for (i = 0; i < n; ++i) {
                for (j = 0; j < m; ++j) {
                    cout << "Element [" << i << "][" << j << "]: ";
                    cin >> matrix[i][j];
                }
            }
            break;
        } else {
            cout << "Try again, print `Yes` or `No`\n";
        }
    }


	// Checker
    int check = 0;
    int null = 0;
    if( m == 3 && n == 3) {
        if(matrix[1][0] == matrix[1][2] && matrix[2][0] == matrix[0][2] && matrix[2][1] == matrix[0][1]) {
            check = 1;
        }
    }  else {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (i != j) {
                    if (matrix[i][j] == matrix[j][i]) {
                        check++;
                    }
                } else {
                    null++;
                }
            }
        }
    }
	// Showing the matrix
	cout << "Press to see the matrix..\n";
	cin.get();
    cin.ignore();
	int count = 0;
	for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
	}
    // Showing the result
	cout << "See the result: \n";
	if(check == 1 || check == m * n - null) {
        cout << "Your matrix is symmetric!\n";
	} else {
        // Creating the symmetric matrix
    double  matrix_sym[n][m];
        for(int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                matrix_sym[i][j] = matrix[i][j];
            }
        }
        string fixer;
        check = 0;
        null = 0;
        float avg;
        cout << "It seems that your matrix is not symetric, i can fix it (Yes/No)\n";
        cin >> fixer;
        cout << endl;
        
        if (fixer == "Yes" || fixer == "yes") {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    if (i != j) {
                        if (matrix_sym[i][j] == matrix_sym[j][i]) {
                            check++;
                        } else {
                            // Make symmetric by copying one value to the other
                            matrix_sym[j][i] = matrix_sym[i][j];
                            check++;
                        }
                    } else {
                        null++;
                    }
                }
            }
            // Showing the symetric matrix
            test(n, m, &matrix_sym[0][0]);
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < m; j++) {
                    cout << matrix_sym[i][j] << " ";
                }
                cout << endl;
            }

        if( check == n * m - null) {
        cout << "Now your matrix  is symmetric, cogratulations!\n";
	} else {
        cout << "Your matrix still not symmetric, Sorry, bad code \n";
	}
        } else  {
            cout << "   \\   /" << endl;
            cout << "    | |" << endl;
            cout << "  _     _" << endl;
            cout << " |  o o  |" << endl;
            cout << " |   -   |" << endl;
            cout << "  /_____\\ " << endl;
        }
	}
 
	// End of the code
	cin.get();
	cin.ignore();
	}
