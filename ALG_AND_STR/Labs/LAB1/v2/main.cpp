#include <iostream>   
#include <iomanip>  
#include <cassert>   
#include <cmath>      
#include <string>     

const int START_YEAR = 1949;  
const int N = 50;            


using namespace std;

double calculate_average(const double *arr, int size) {


    double sum = 0.0;


    for (const double *p = arr; p < arr + size; ++p) {
        sum += *p;  
    }

    return sum / size;  
}


void print_report(const double *arr, int size, int start_year, double avg) {
    cout << "\n-----------------------------------------\n";
    cout << "  Рік  |  Опади (мм)  |  Відхилення (мм) \n";
    cout << "-----------------------------------------\n";

    for (int i = 0; i < size; ++i) {

        const double *p = arr + i;

        double deviation = *p - avg;


        cout << "  " << (start_year + i)
                  << " | " << std::setw(12) << std::fixed << std::setprecision(2) << *p
                  << " | " << std::setw(16) << std::showpos << deviation << std::noshowpos
                  << "\n";
    }

    cout << "Середня кількість опадів: " << std::fixed << std::setprecision(2) << avg << " мм\n\n";
}


int main() {

    

    double rain[N];

    std::cout << "Оберіть джерело даних:\n";
    std::cout << "1 - Використати випадкові\n";
    std::cout << "2 - Ввести 50 значень \n";
    std::cout << "1 або 2:  ";

    char choice = '1';  
    if (!(std::cin >> choice) || (choice != '1' && choice != '2')) {
        choice = '1';  
    }

    if (choice == '2') {
        std::cout << "Введіть " << N << " дійсних чисел (опади за 1949-1998 рр.):\n";

  
        for (double *p = rain; p < rain + N; ++p) {

            int year = START_YEAR + static_cast<int>(p - rain);
            cout << year << ": ";

            if (!(cin >> *p)) {
                cerr << "Помилка введення даних.\n";
                return 1;  
            }
        }
    } else {
        
        const double sample_data[N] = {
            540.2, 612.5, 489.1, 675.0, 520.4, 598.7, 630.1, 515.8, 580.3, 602.4,
            645.2, 532.0, 578.6, 690.4, 510.9, 625.3, 560.1, 595.0, 640.8, 512.3,
            670.5, 588.2, 545.6, 615.4, 530.7, 660.2, 575.0, 590.1, 622.8, 505.4,
            680.0, 565.3, 610.9, 535.2, 642.1, 580.0, 599.4, 618.7, 525.6, 672.3,
            550.8, 605.2, 633.0, 518.5, 665.4, 582.1, 540.0, 620.5, 595.6, 611.2
        };

        for (int i = 0; i < N; ++i) {
            *(rain + i) = *(sample_data + i);
        }
        cout << "Завантажено зразкові дані за 50 років.\n";
    }

    double avg = calculate_average(rain, N);

    print_report(rain, N, START_YEAR, avg);

    return 0;  
}
