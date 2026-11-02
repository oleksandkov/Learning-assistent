#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <cstdint>

using namespace std;

struct Record {
    string email;
    string password;
    double drive_data;
};

struct Node {
    Record data;
    Node *l, *r;
    Node(const Record& d) : data(d), l(nullptr), r(nullptr) {}
};

Node* insert(Node* t, const Record& d) {
    if (!t) return new Node(d);
    if (d.email < t->data.email) t->l = insert(t->l, d);
    else if (d.email > t->data.email) t->r = insert(t->r, d);
    return t;
}

Node* find(Node* t, const string& key) {
    if (!t) return nullptr;
    if (key == t->data.email) return t;
    if (key < t->data.email) return find(t->l, key);
    return find(t->r, key);
}

Node* minNode(Node* t) { while (t->l) t = t->l; return t; }

Node* remove(Node* t, const string& key) {
    if (!t) return nullptr;
    if (key < t->data.email) t->l = remove(t->l, key);
    else if (key > t->data.email) t->r = remove(t->r, key);
    else {
        if (!t->l) { Node* r = t->r; delete t; return r; }
        if (!t->r) { Node* l = t->l; delete t; return l; }
        Node* m = minNode(t->r);
        t->data = m->data;
        t->r = remove(t->r, m->data.email);
    }
    return t;
}

void inorder(Node* t) {
    if (!t) return;
    inorder(t->l);
    cout << t->data.email << " | " << t->data.password << " | " << t->data.drive_data << " GB\n";
    inorder(t->r);
}

void freeTree(Node* t) {
    if (!t) return;
    freeTree(t->l);
    freeTree(t->r);
    delete t;
}

const uint32_t A_MUL = 2654435769u;

uint32_t strKey(const string& s) {
    uint32_t h = 0;
    for (char c : s) h = h * 131 + (unsigned char)c;
    return h;
}

int tableBits = 13;
int tableSize = 1 << 13;

struct HashTable {
    vector<vector<Record>> chains;
    HashTable() : chains(tableSize) {}
    int idx(const string& key) {
        uint32_t k = strKey(key);
        uint32_t m = (uint32_t)((uint64_t)k * A_MUL);
        return (int)(m >> (32 - tableBits));
    }
    bool insert(const Record& d) {
        int i = idx(d.email);
        for (auto& r : chains[i]) if (r.email == d.email) return false;
        chains[i].push_back(d);
        return true;
    }
    Record* search(const string& key) {
        int i = idx(key);
        for (auto& r : chains[i]) if (r.email == key) return &r;
        return nullptr;
    }
    bool remove(const string& key) {
        int i = idx(key);
        for (size_t j = 0; j < chains[i].size(); j++)
            if (chains[i][j].email == key) { chains[i].erase(chains[i].begin() + j); return true; }
        return false;
    }
    void print() {
        for (int i = 0; i < tableSize; i++)
            if (!chains[i].empty()) {
                cout << i << ": ";
                for (auto& r : chains[i]) cout << r.email << " ";
                cout << "\n";
            }
    }
};

void experiment() {
    int n = 100000;
    mt19937 gen(42);
    vector<Record> recs;
    recs.reserve(n);
    for (int i = 0; i < n; i++) {
        Record r;
        r.email = "user" + to_string(gen()) + "@mail.com";
        r.password = "pass" + to_string(gen() % 10000);
        r.drive_data = (gen() % 1000) / 10.0;
        recs.push_back(r);
    }
    sort(recs.begin(), recs.end(), [](const Record& a, const Record& b){ return a.email < b.email; });
    recs.erase(unique(recs.begin(), recs.end(), [](const Record& a, const Record& b){ return a.email == b.email; }), recs.end());
    n = recs.size();
    shuffle(recs.begin(), recs.end(), gen);

    Node* root = nullptr;
    HashTable ht;
    for (auto& r : recs) { root = insert(root, r); ht.insert(r); }

    int c1 = 0, c2 = 0;
    auto t1 = chrono::high_resolution_clock::now();
    for (auto& r : recs) if (find(root, r.email)) c1++;
    auto t2 = chrono::high_resolution_clock::now();
    auto t3 = chrono::high_resolution_clock::now();
    for (auto& r : recs) if (ht.search(r.email)) c2++;
    auto t4 = chrono::high_resolution_clock::now();
    cout << "BST search ms: " << chrono::duration<double, milli>(t2 - t1).count() << " found " << c1 << "\n";
    cout << "Hash search ms: " << chrono::duration<double, milli>(t4 - t3).count() << " found " << c2 << "\n";
    freeTree(root);
}

int main() {
    Node* root = nullptr;
    HashTable ht;
    int choice = -1;
    while (choice != 0) {
        cout << "\n1. Додати запис\n2. Видалити запис\n3. Пошук у дереві\n"
             << "4. Пошук у хеш-таблиці\n5. Вивести дерево\n6. Вивести таблицю\n"
             << "7. Експеримент\n0. Вихід\nВибір: ";
        if (!(cin >> choice)) break;
        cin.ignore(1000, '\n');
        if (choice == 1) {
            Record r;
            cout << "Email: "; getline(cin, r.email);
            cout << "Пароль: "; getline(cin, r.password);
            cout << "Обсяг даних (ГБ): "; cin >> r.drive_data;
            cin.ignore(1000, '\n');
            root = insert(root, r);
            ht.insert(r);
        } else if (choice == 2) {
            string k; cout << "Email: "; getline(cin, k);
            root = remove(root, k);
            ht.remove(k);
        } else if (choice == 3) {
            string k; cout << "Email: "; getline(cin, k);
            Node* n = find(root, k);
            if (n) cout << n->data.email << " | " << n->data.password << " | " << n->data.drive_data << " GB\n";
            else cout << "Не знайдено\n";
        } else if (choice == 4) {
            string k; cout << "Email: "; getline(cin, k);
            Record* r = ht.search(k);
            if (r) cout << r->email << " | " << r->password << " | " << r->drive_data << " GB\n";
            else cout << "Не знайдено\n";
        } else if (choice == 5) {
            inorder(root);
        } else if (choice == 6) {
            ht.print();
        } else if (choice == 7) {
            experiment();
        }
    }
    freeTree(root);
    cout << "Кінець\n";
    return 0;
}
