

#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>   

using namespace std;
using namespace chrono;


void insertionSort(vector<int>& a) {
    for (int i = 1; i < (int)a.size(); i++) {
        int key = a[i];      
        int j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1]; 
            j--;
        }
        a[j] = key;          
    }
}

struct Node {
    int key;         
    Node* left;      
    Node* right;     

    explicit Node(int k) : key(k), left(nullptr), right(nullptr) {}
};


Node* insert(Node* root, int key) {
    if (root == nullptr) return new Node(key);   
    if (key < root->key)
        root->left  = insert(root->left, key);
    else
        root->right = insert(root->right, key);
    return root;
}


void inorder(Node* v, vector<int>& a, int& k) {
    if (v == nullptr) return;                     
    inorder(v->left, a, k);                       
    a[k++] = v->key;                              
    inorder(v->right, a, k);                      
}

void freeTree(Node* v) {
    if (v == nullptr) return;
    freeTree(v->left);
    freeTree(v->right);
    delete v;
}


void treeSort(vector<int>& a) {
    Node* root = nullptr;
    for (int x : a) root = insert(root, x);       
    int k = 0;                                    
    inorder(root, a, k);                          
    freeTree(root);                               
}


vector<int> makeArray(int n) {
    mt19937 gen(12345);                 
    uniform_int_distribution<int> dist(0, 1000000000);
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = dist(gen);
    return a;
}

int main() {
    cout << "n\tInsertion (ms)\tTree (ms)\n";

    vector<int> a1;
    vector<int> a2;

    for (int n : {10000, 100000}) {
        a1 = makeArray(n);
        a2 = a1;

        auto t1 = high_resolution_clock::now();
        insertionSort(a1);
        auto t2 = high_resolution_clock::now();
        treeSort(a2);
        auto t3 = high_resolution_clock::now();

        double ms1 = duration<double, milli>(t2 - t1).count();
        double ms2 = duration<double, milli>(t3 - t2).count();

        if (!is_sorted(a1.begin(), a1.end()) || !is_sorted(a2.begin(), a2.end())) {
            cout << "масив не відсортовано!" << endl;
            return 1;
        }

        cout << n << "\t" << ms1 << "\t\t" << ms2 << "\n";
    }

    cout << "\nПісля сортування вставкою: ";
    for (int i = 0; i < 20 && i < (int)a1.size(); i++)
        cout << a1[i] << " ";
    cout << endl;

    cout << "\nПісля сортування деревом: ";
    for (int i = 0; i < 20 && i < (int)a2.size(); i++)
        cout << a2[i] << " ";
    cout << endl;

    cout << "\nНатисніть Enter..." << flush;
    cin.get();

    return 0;
}