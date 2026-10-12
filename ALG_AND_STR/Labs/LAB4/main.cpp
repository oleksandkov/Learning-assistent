// Лабораторна робота №4. Алгоритми сортування. Варіант 12.
// Порівняння сортування вставкою (3) та сортування злиттям (8).

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>   // потрібен лише для перевірки is_sorted

using namespace std;
using namespace chrono;

// --- Сортування вставкою (Insertion sort) ---
// Кожен елемент по черзі береться з невпорядкованої частини
// і вставляється на своє місце у вже впорядкованій частині масиву.
void insertionSort(vector<int>& a) {
    for (int i = 1; i < (int)a.size(); i++) {
        int key = a[i];      // запам'ятовуємо поточний елемент
        int j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1]; // посуваємо більші елементи вправо, звільнюючи місце
            j--;
        }
        a[j] = key;          // вставляємо елемент на знайдену позицію
    }
}

// --- Сортування злиттям (Merge sort) ---
// Масив ділиться навпіл, кожна половинка сортується рекурсивно,
// а потім дві впорядковані половинки зливаються в одну.
void mergeSort(vector<int>& a, int left, int right) {
    if (right - left <= 1) return;   // один елемент (або порожній відрізок) вже впорядкований
    int mid = left + (right - left) / 2;
    mergeSort(a, left, mid);
    mergeSort(a, mid, right);

    vector<int> tmp(a.begin() + left, a.begin() + right); // копія відрізка, індекси від 0
    int n = right - left;   // довжина відрізка
    int m = mid - left;     // розмір лівої половинки
    int i = 0;              // вказівник на початок лівої половинки
    int j = m;              // вказівник на початок правої половинки
    for (int k = 0; k < n; k++) {
        if (i < m && (j >= n || tmp[i] <= tmp[j]))
            a[left + k] = tmp[i++];   // беремо менший з двох поточних елементів
        else
            a[left + k] = tmp[j++];
    }
}

// --- Заповнення масиву випадковими числами ---
// Один і той самий seed => обидва алгоритми отримують однакові вхідні дані.
vector<int> makeArray(int n) {
    mt19937 gen(12345);                 // генератор випадкових чисел з фіксованим зерном
    uniform_int_distribution<int> dist(0, 1000000000);
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = dist(gen);
    return a;
}

int main() {
    cout << "n\tInsertion (ms)\tMerge (ms)\n";

    vector<int> a1;   
    vector<int> a2;

    for (int n : {10000, 100000}) {
        a1 = makeArray(n);          
        a2 = a1;

        auto t1 = high_resolution_clock::now();
        insertionSort(a1);
        auto t2 = high_resolution_clock::now();
        mergeSort(a2, 0, n);
        auto t3 = high_resolution_clock::now();

        double ms1 = duration<double, milli>(t2 - t1).count();
        double ms2 = duration<double, milli>(t3 - t2).count();

        // перевірка коректності: масив має бути впорядкованим
        if (!is_sorted(a1.begin(), a1.end()) || !is_sorted(a2.begin(), a2.end())) {
            cout << "ПОМИЛКА: масив не відсортовано!" << endl;
            return 1;
        }

        cout << n << "\t" << ms1 << "\t\t" << ms2 << "\n";
    }

    
    cout << "\nПісля сортування вставкою (перші 20): ";
    for (int i = 0; i < 20 && i < (int)a1.size(); i++)
        cout << a1[i] << " ";
    cout << endl;

    cout << "\nПісля сортування злиттям (перші 20): ";
    for (int i = 0; i < 20 && i < (int)a2.size(); i++)
        cout << a2[i] << " ";
    cout << endl;

  
    cout << "\nНатисніть Enter..." << flush;
    cin.get();

    return 0;
}