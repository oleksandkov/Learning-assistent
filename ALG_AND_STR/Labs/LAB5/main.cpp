// Лабораторна робота №5. Хеш-функції та хеш-таблиці. Варіант 1.12.
// 1.12 — хеш-функція SHABAL, застосування 3б) — контрольна сума файлу.
// Варіант 12 парний, тому власна хеш-функція реалізована методом множення.

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <unordered_set>
#include <cstdint>

using namespace std;

// ---------- ДОПОМІЖНЕ ----------

// циклічний зсув 32-бітного слова вліво на n бітів
uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

// нелінійні перестановки U і V з алгоритму SHABAL (множення за модулем 2^32)
uint32_t U(uint32_t x) { return x * 3; }
uint32_t V(uint32_t x) { return x * 5; }

// перетворення 64 байт у 16 слів у порядку "little endian"
void decodeBlock(const unsigned char* buf, uint32_t M[16]) {
    for (int i = 0; i < 16; i++)
        M[i] = buf[4*i] | (buf[4*i+1] << 8) |
               (buf[4*i+2] << 16) | (uint32_t(buf[4*i+3]) << 24);
}

// ---------- 1. ХЕШ-ФУНКЦІЯ SHABAL-512 ----------

// ключова перестановка P: 48 раундів з нелінійними множеннями U і V
void shabalP(uint32_t A[], uint32_t B[], uint32_t C[], const uint32_t M[]) {
    for (int i = 0; i < 16; i++)
        B[i] = rotl(B[i], 17);                       // початковий зсув B на 17 бітів

    for (int j = 0; j < 3; j++)
        for (int i = 0; i < 16; i++) {
            int k = (i + 4*j) % 12;                 // індекс у змінній A
            A[k] = U(A[k] ^ C[(8 - i + 16) % 16] ^ V(rotl(A[(k + 11) % 12], 15)))
                   ^ M[i] ^ B[(i + 13) % 16] ^ (B[(i + 9) % 16] & ~B[(i + 6) % 16]);
            B[i] = ~(rotl(B[i], 1) ^ A[k]);         // зворотний нелінійний елемент
        }

    for (int j = 0; j < 36; j++)
        A[j % 12] += C[(j + 3) % 16];               // фінальний цикл додавання
}

// функція стиснення одного 512-бітного блоку
void shabalBlock(uint32_t A[], uint32_t B[], uint32_t C[], const uint32_t M[],
                 uint32_t& Wlow, uint32_t& Whigh) {
    for (int i = 0; i < 16; i++) B[i] += M[i];      // додаємо блок повідомлення
    A[0] ^= Wlow;                                   // 64-бітний лічильник у перші два слова
    A[1] ^= Whigh;
    shabalP(A, B, C, M);
    for (int i = 0; i < 16; i++) C[i] -= M[i];      // віднімаємо блок повідомлення
    for (int i = 0; i < 16; i++) { uint32_t t = B[i]; B[i] = C[i]; C[i] = t; }
    if (++Wlow == 0) ++Whigh;                       // збільшуємо лічильник
}

// обчислення SHABAL-512 (512 біт = 16 слів) для довільного повідомлення
void shabal512(const vector<unsigned char>& msg, uint32_t out[16]) {
    // початкові значення внутрішнього стану (Shabal-512)
    uint32_t A[12] = {0x20728DFDu,0x46C0BD53u,0xE782B699u,0x55304632u,0x71B4EF90u,0x0EA9E82Cu,
                      0xDBB930F1u,0xFAD06B8Bu,0xBE0CAE40u,0x8BD14410u,0x76D2ADACu,0x28ACAB7Fu};
    uint32_t B[16] = {0xC1099CB7u,0x07B385F3u,0xE7442C26u,0xCC8AD640u,0xEB6F56C7u,0x1EA81AA9u,
                      0x73B9D314u,0x1DE85D08u,0x48910A5Au,0x893B22DBu,0xC5A0DF44u,0xBBC4324Eu,
                      0x72D2F240u,0x75941D99u,0x6D8BDE82u,0xA1A7502Bu};
    uint32_t C[16] = {0xD9BF68D1u,0x58BAD750u,0x56028CB2u,0x8134F359u,0xB5D469D8u,0x941A8CC2u,
                      0x418B2A6Eu,0x04052780u,0x7F07D787u,0x5194358Fu,0x3C60D665u,0xBE97D79Au,
                      0x950C3434u,0xAED9A06Du,0x2537DC8Du,0x7CDB5969u};
    uint32_t Wlow = 1, Whigh = 0;
    uint32_t M[16];

    size_t pos = 0;
    while (msg.size() - pos >= 64) {                // обробляємо повні 512-бітні блоки
        decodeBlock(&msg[pos], M);
        shabalBlock(A, B, C, M, Wlow, Whigh);
        pos += 64;
    }

    // останній блок: залишок повідомлення, потім біт 1 і нулі
    unsigned char buf[64] = {};
    for (size_t i = 0; pos + i < msg.size(); i++) buf[i] = msg[pos + i];
    buf[msg.size() - pos] = 0x80;
    decodeBlock(buf, M);

    for (int i = 0; i < 16; i++) B[i] += M[i];      // блок з доповненням
    A[0] ^= Wlow;
    A[1] ^= Whigh;
    shabalP(A, B, C, M);

    for (int i = 0; i < 3; i++) {                   // фіналізація: три додаткові раунди
        for (int k = 0; k < 16; k++) { uint32_t t = B[k]; B[k] = C[k]; C[k] = t; }
        A[0] ^= Wlow;
        A[1] ^= Whigh;
        shabalP(A, B, C, M);
    }
    for (int i = 0; i < 16; i++) out[i] = B[i];     // хеш-значення — це 16 слів B
}

// ---------- 2. ВЛАСНА ХЕШ-ФУНКЦІЯ (МЕТОД МНОЖЕННЯ) ----------

// метод множення за Кнутом: h = (h * A + символ) mod 2^32,
// де A = 2^32 / phi — наближення до числа Фібоначчі
uint32_t myHash(const unsigned char* data, size_t len) {
    const uint32_t A = 0x9E3779B9u;
    uint32_t h = 0x811C9DC5u;
    for (size_t i = 0; i < len; i++)
        h = h * A + data[i] + 1;
    return h;
}

// метод дішення — для порівняння з методом множення
uint32_t divHash(uint32_t h, uint32_t m) {
    return h % m;
}

// ---------- 3. РОБОТА З ФАЙЛАМИ ----------

// читання файлу в масив байтів
vector<unsigned char> readFile(const string& name, bool& ok) {
    ifstream f(name, ios::binary);
    ok = (bool)f;
    if (!ok) return {};
    vector<unsigned char> d;
    char buf[4096];
    while (f.read(buf, 4096) || f.gcount())
        d.insert(d.end(), buf, buf + f.gcount());
    return d;
}

// ---------- 4. ЗАСТОСУВАННЯ 3б): КОНТРОЛЬНА СУМА ФАЙЛУ ----------

// обчислення контрольної суми файлу двома хеш-функціями
void calcChecksum(const string& name) {
    bool ok;
    vector<unsigned char> data = readFile(name, ok);
    if (!ok) { cout << "Не вдалося відкрити файл: " << name << endl; return; }

    uint32_t h[16];
    shabal512(data, h);
    uint32_t mine = myHash(data.data(), data.size());

    cout << "Файл: " << name << "   (" << data.size() << " байт)" << endl;
    cout << "SHABAL-512 (512 біт):" << endl << "  " << hex << setfill('0');
    for (int i = 0; i < 16; i++) cout << setw(8) << h[i];
    cout << dec << setfill(' ') << endl;
    cout << "Власна, метод множення (32 біт): " << hex << setfill('0')
         << setw(8) << mine << dec << setfill(' ') << endl;
    cout << "Той самий хеш за методом дішення: " << divHash(mine, 8192) << endl;
}

// дописування контрольної суми SHABAL-512 в кінець файлу
void appendChecksum(const string& name) {
    bool ok;
    vector<unsigned char> data = readFile(name, ok);
    if (!ok) { cout << "Не вдалося відкрити файл: " << name << endl; return; }

    uint32_t h[16];
    shabal512(data, h);

    unsigned char sum[64];
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 4; j++)
            sum[4*i + j] = (unsigned char)(h[i] >> (8*j));    // у порядку little endian

    ofstream f(name, ios::binary | ios::app);               // дописуємо в кінець
    f.write((char*)sum, 64);
    f.close();
    cout << "Контрольну суму 64 байти дописано в кінець файлу " << name << endl;
}

// перевірка дописаної контрольної суми
void verifyChecksum(const string& name) {
    bool ok;
    vector<unsigned char> data = readFile(name, ok);
    if (!ok) { cout << "Не вдалося відкрити файл: " << name << endl; return; }
    if (data.size() < 64) { cout << "Файл закороткий, суми в ньому немає." << endl; return; }

    size_t n = data.size() - 64;                            // частина файлу без суми
    vector<unsigned char> body(data.begin(), data.begin() + n);

    uint32_t h[16];
    shabal512(body, h);

    bool same = true;
    for (int i = 0; i < 16; i++) {
        uint32_t saved = data[n + 4*i] | (uint32_t(data[n + 4*i+1]) << 8) |
                         (uint32_t(data[n + 4*i+2]) << 16) |
                         (uint32_t(data[n + 4*i+3]) << 24);
        if (saved != h[i]) same = false;
    }
    cout << (same ? "Перевірка УСПІШНА — контрольна сума збігається."
                  : "Перевірка НЕВДАЛА — файл було змінено!") << endl;
}

// ---------- 5. ПОРІВНЯННЯ ХЕШ-ФУНКЦІЙ ----------

// генерація count різних ключів з діапазону [0, range)
vector<uint32_t> makeKeys(uint32_t range, int count, mt19937& gen) {
    vector<uint32_t> keys;
    unordered_set<uint32_t> used;
    uniform_int_distribution<uint32_t> dist(0, range - 1);
    while ((int)keys.size() < count) {
        uint32_t k = dist(gen);
        if (used.insert(k).second) keys.push_back(k);       // пропускаємо повтори
    }
    return keys;
}

// SHABAL-512 від 4-байтового ключа, беремо молодші 32 біти
uint32_t shabal512of(uint32_t key) {
    unsigned char b[4] = {(unsigned char)key, (unsigned char)(key >> 8),
                         (unsigned char)(key >> 16), (unsigned char)(key >> 24)};
    uint32_t h[16];
    shabal512(vector<unsigned char>(b, b + 4), h);
    return h[0];
}

// власна хеш-функція від 4-байтового ключа
uint32_t myHashOf(uint32_t key) {
    unsigned char b[4] = {(unsigned char)key, (unsigned char)(key >> 8),
                         (unsigned char)(key >> 16), (unsigned char)(key >> 24)};
    return myHash(b, 4);
}

// підрахунок колізій: заповнюємо таблицю на 60%, потім додаємо 700 нових ключів
void collisions() {
    const int bits = 13;                  // розмір таблиці 2^13 = 8192 (межа 10^3..10^4)
    const int m = 1 << bits;
    const int fill = m * 60 / 100;        // заповнюємо на 60%
    const int keys = 700;                 // кількість додаваних ключів (межа 500..1000)

    const char* names[3] = {"SHABAL-512", "Множення", "Дішення"};
    uint32_t ranges[3] = {10000u, 1000000u, 1000000000u};
    const char* rname[3] = {"0..10^4", "0..10^6", "0..10^9"};

    cout << "Розмір таблиці " << m << " комірок, заповнення " << fill
         << " (" << fill*100/m << "%), додається " << keys << " нових ключів." << endl;
    cout << "Очікувано колізій: " << keys * fill / m << " з " << keys
         << " (зайнято " << fill*100/m << "% комірок)." << endl << endl;

    for (int r = 0; r < 3; r++) {
        mt19937 gen(1000 + r);
        vector<uint32_t> base = makeKeys(ranges[r], fill, gen);      // таблиця заповнюється
        vector<uint32_t> test = makeKeys(ranges[r], keys, gen);      // нові ключі

        cout << "Діапазон ключів " << rname[r] << ":" << endl;
        for (int f = 0; f < 3; f++) {
            vector<char> table(m, 0);
            for (int i = 0; i < fill; i++) {
                uint32_t h = (f == 0) ? shabal512of(base[i]) : myHashOf(base[i]);
                int idx = (f == 2) ? (int)divHash(h, m) : (int)(h >> (32 - bits));
                table[idx] = 1;
            }
            int hit = 0;
            for (int i = 0; i < keys; i++) {
                uint32_t h = (f == 0) ? shabal512of(test[i]) : myHashOf(test[i]);
                int idx = (f == 2) ? (int)divHash(h, m) : (int)(h >> (32 - bits));
                if (table[idx]) hit++;                      // комірка вже зайнята
                else table[idx] = 1;
            }
            cout << "    " << names[f] << ": " << hit << " колізій ("
                 << hit*1000/keys << "‰)" << endl;
        }
        cout << endl;
    }
}

// ---------- ГОЛОВНА ФУНКЦІЯ ----------

// перевірка SHABAL-512 за еталонним значенням хешу порожнього повідомлення
bool selftest() {
    uint32_t h[16], ref[16] = {0xff5d2dfc, 0xf6b7705d, 0xfcc2f8b1, 0xfef9c1c8,
                               0x42e53499, 0x0ceded57, 0xa239b5f2, 0xcc190aef,
                               0x8d4fa8ff, 0xe435a19f, 0xf5093cbd, 0x27a9f390,
                               0xac03d6eb, 0x9e72eb29, 0xf09a2a6f, 0xc68dad31};
    shabal512(vector<unsigned char>(), h);
    for (int i = 0; i < 16; i++)
        if (h[i] != ref[i]) return false;
    return true;
}

int main() {
    string file = "getfile.pdf";
    int choice = -1;

    cout << "Лабораторна робота №5. Хеш-функції та хеш-таблиці." << endl;
    cout << "Варіант 1.12 — SHABAL, застосування 3б), власна функція методом множення." << endl;
    cout << "Перевірка SHABAL-512 за еталоном: " << (selftest() ? "УСПІШНО" : "ПОМИЛКА") << endl;
    cout << "Файл для обробки (Enter — " << file << "): ";
    getline(cin, file);
    if (file.empty()) file = "getfile.pdf";

    while (choice != 0) {
        cout << "\n1. Контрольна сума файлу\n2. Дописати суму в кінець файлу\n"
                "3. Перевірити дописану суму\n4. Порівняння хеш-функцій\n0. Вихід\n"
                "Вибір: ";
        if (!(cin >> choice)) break;
        cin.ignore(1000, '\n');                // відкидаємо решту рядка після числа

        switch (choice) {
            case 1: calcChecksum(file); break;
            case 2: appendChecksum(file); break;
            case 3: verifyChecksum(file); break;
            case 4: collisions(); break;
            case 0: break;
            default: cout << "Введіть число від 0 до 4." << endl;
        }
    }
    cout << "Кінець" << endl;
    return 0;
}