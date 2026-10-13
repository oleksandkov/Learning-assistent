// Лабораторна робота №5. Хеш-функції та хеш-таблиці. Варіант 1.12.
// 1.12 — хеш-функція SHABAL, застосування 3б) — контрольна сума файлу.
// Варіант 12 парний, тому власна хеш-функція реалізована методом множення.
//
// Структура програми відповідає завданню (див. методичний вказівку):
//   п.1 — відомий метод: SHABAL-512 (блок 1);
//   п.2 — власна хеш-функція методом множення (блок 2);
//   п.3 — застосування 3б): контрольна сума файлу, дописана в його кінець (блок 3);
//   п.4 — зручне текстове меню для роботи користувача;
//   п.5 — порівняльний експеримент: кількість колізій у хеш-таблиці (блок 4).

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>
#include <random>
#include <iomanip>
#include <unordered_set>
#include <cstdint>

using namespace std;

// ================================================================
// БЛОК 1. ХЕШ-ФУНКЦІЯ SHABAL-512  (пункт 1 завдання)
// ---------------------------------------------------------------
// Shabal — криптографічна хеш-функція (кандидат на конкурс SHA-3, 2-й тур).
// Блок повідомлення — 64 байти (512 біт), хеш — 64 байти (512 біт).
//
// Внутрішній стан — три масиви 32-бітних слів:
//   A[12] — основний "змішувач": воно ж і найхаотичніше;
//   B[16] — робочий масив: до нього додається блок повідомлення,
//           і він же оновлюється всередині перестановки P;
//   C[16] — "резервний" масив: з нього віднімається блок повідомлення;
//   W     — 64-бітний лічильник блоків повідомлення (Wlow, Whigh).
//
// Що відбувається з кожним блоком M (16 слів):
//   1) B[i] += M[i]      — додаємо повідомлення в стан;
//   2) A[0]^=Wlow; A[1]^=Whigh — підмішуємо номер блоку (захист від
//                                перестановок блоків);
//   3) P(A,B,C,M)        — головна нелінійна перестановка;
//   4) C[i] -= M[i]      — віднімаємо повідомлення іншим шляхом;
//   5) B <-> C           — міняємо масиви місцями;
//   6) W++               — лічильник блоків +1.
//
// Перестановка P — три частини:
//   а) кожне слово B циклічно зсувається вліво на 17 бітів (ROTL17);
//   б) 48 кроків — три групи по 16 (j = 0,1,2 при i = 0..15):
//        k = (i + 4*j) mod 12                    — індекс у масиві A
//        A[k] = 3·( A[k] ^ C[(8-i) mod 16]
//                    ^ 5·ROTL(A[(k+11) mod 12], 15) )
//               ^ M[i] ^ B[(i+13) mod 16] ^ (B[(i+9) mod 16] & ~B[(i+6) mod 16]);
//        B[i] = ~(ROTL(B[i], 1) ^ A[k]);
//      Множення на 3 (U) і на 5 (V) дають НЕЛІНІЙНІСТЬ,
//      зсуги на 15 і 1 біти руйнують суцільні біти,
//      A, B, C і повідомлення M "заплутуються" між собою.
//   в) заключні 36 додавань: A[i mod 12] += C[(i mod 12 + 11 + 4·(i div 12)) mod 16],
//      i = 0..35 — три раунди з кроком зсуву індексу C на 4 (той самий
//      крок 4, що й у частині б).
//
// Доповнення останнього блоку (padding): дописуємо 0x80 (біт 1 + нулі)
// і нулі до межі 64 байтів. Якщо повідомлення вже кратне 64 — додається
// цілий новий блок із 0x80. Довжину повідомлення НЕ додаємо — замість неї
// працює лічильник W.
//
// Фіналізація: доповнений останній блок пропускаємо через кроки 1–5,
// потім тричі повторюємо { B <-> C; A[0]^=Wlow; A[1]^=Whigh; P }.
// Хеш = 16 слів масиву B (для файлу записуємо у little-endian).
// ================================================================

uint32_t rotl(uint32_t x, int n) {
    return (x << n) | (x >> (32 - n));
}

uint32_t U(uint32_t x) { return x * 3; }   // нелінійні множення P
uint32_t V(uint32_t x) { return x * 5; }

// 32-бітний ключ -> 4 байти (little endian)
void keyBytes(uint32_t key, unsigned char b[4]) {
    for (int j = 0; j < 4; j++)
        b[j] = (unsigned char)(key >> (8*j));
}

// 16 слів -> 64 байти (little endian), для запису хеша у файл
void encodeBlock(const uint32_t M[16], unsigned char* buf) {
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 4; j++)
            buf[4*i + j] = (unsigned char)(M[i] >> (8*j));
}

// головна перестановка P: (A, B, C, M) -> (A', B', C')
void shabalP(uint32_t A[], uint32_t B[], uint32_t C[], const uint32_t M[]) {
    // а) зсув усіх слів B вліво на 17 бітів
    for (int i = 0; i < 16; i++)
        B[i] = rotl(B[i], 17);

    // б) 48 кроків: три групи по 16 (j = 0..2, i = 0..15)
    for (int j = 0; j < 3; j++)
        for (int i = 0; i < 16; i++) {
            int k = (i + 4*j) % 12;                 // індекс у масиві A
            A[k] = U(A[k] ^ C[(8 - i + 16) % 16] ^ V(rotl(A[(k + 11) % 12], 15)))
                   ^ M[i] ^ B[(i + 13) % 16] ^ (B[(i + 9) % 16] & ~B[(i + 6) % 16]);
            B[i] = ~(rotl(B[i], 1) ^ A[k]);         // нелінійний зворотний елемент
        }

    // в) заключні 36 додавань (три раунди, зсув індексу C +4 за раунд)
    for (int i = 0; i < 36; i++)
        A[i % 12] += C[(i % 12 + 11 + 4*(i / 12)) % 16];
}

// обробка одного повного блоку M (16 слів): кроки 1–6 з опису вище
void shabalBlock(uint32_t A[], uint32_t B[], uint32_t C[], const uint32_t M[],
                 uint32_t& Wlow, uint32_t& Whigh) {
    for (int i = 0; i < 16; i++) B[i] += M[i];      // 1) додаємо повідомлення
    A[0] ^= Wlow;                                   // 2) підмішуємо номер блоку
    A[1] ^= Whigh;
    shabalP(A, B, C, M);                            // 3) перестановка P
    for (int i = 0; i < 16; i++) C[i] -= M[i];      // 4) віднімаємо повідомлення
    for (int i = 0; i < 16; i++) { uint32_t t = B[i]; B[i] = C[i]; C[i] = t; } // 5) B <-> C
    if (++Wlow == 0) ++Whigh;                       // 6) лічильник блоків +1
}

// SHABAL-512 для довільного повідомлення msg; результат — 16 слів out
void shabal512(const vector<unsigned char>& msg, uint32_t out[16]) {
    // початковий стан (IV) Shabal-512
    uint32_t A[12] = {0x20728DFDu,0x46C0BD53u,0xE782B699u,0x55304632u,0x71B4EF90u,0x0EA9E82Cu,
                      0xDBB930F1u,0xFAD06B8Bu,0xBE0CAE40u,0x8BD14410u,0x76D2ADACu,0x28ACAB7Fu};
    uint32_t B[16] = {0xC1099CB7u,0x07B385F3u,0xE7442C26u,0xCC8AD640u,0xEB6F56C7u,0x1EA81AA9u,
                      0x73B9D314u,0x1DE85D08u,0x48910A5Au,0x893B22DBu,0xC5A0DF44u,0xBBC4324Eu,
                      0x72D2F240u,0x75941D99u,0x6D8BDE82u,0xA1A7502Bu};
    uint32_t C[16] = {0xD9BF68D1u,0x58BAD750u,0x56028CB2u,0x8134F359u,0xB5D469D8u,0x941A8CC2u,
                      0x418B2A6Eu,0x04052780u,0x7F07D787u,0x5194358Fu,0x3C60D665u,0xBE97D79Au,
                      0x950C3434u,0xAED9A06Du,0x2537DC8Du,0x7CDB5969u};
    uint32_t Wlow = 1, Whigh = 0;                   // лічильник блоків
    uint32_t M[16];

    size_t pos = 0;
    // повні 512-бітні блоки
    while (msg.size() - pos >= 64) {
        for (int i = 0; i < 16; i++)                // decode little endian
            M[i] = msg[pos + 4*i] | (uint32_t(msg[pos + 4*i+1]) << 8) |
                   (uint32_t(msg[pos + 4*i+2]) << 16) | (uint32_t(msg[pos + 4*i+3]) << 24);
        shabalBlock(A, B, C, M, Wlow, Whigh);
        pos += 64;
    }

    // останній блок: залишок повідомлення + 0x80 + нулі
    unsigned char buf[64] = {};
    for (size_t i = 0; pos + i < msg.size(); i++) buf[i] = msg[pos + i];
    buf[msg.size() - pos] = 0x80;
    for (int i = 0; i < 16; i++)
        M[i] = buf[4*i] | (uint32_t(buf[4*i+1]) << 8) |
               (uint32_t(buf[4*i+2]) << 16) | (uint32_t(buf[4*i+3]) << 24);

    // останній блок подається трішки інакше, ніж повні блоки:
    // add -> XOR_W -> P (без C-=M, без swap B<->C і без збільшення W)
    for (int i = 0; i < 16; i++) B[i] += M[i];      // додаємо повідомлення
    A[0] ^= Wlow;                                   // підмішуємо номер блоку
    A[1] ^= Whigh;
    shabalP(A, B, C, M);

    for (int i = 0; i < 3; i++) {                   // фіналізація: три раунди
        for (int k = 0; k < 16; k++) { uint32_t t = B[k]; B[k] = C[k]; C[k] = t; }
        A[0] ^= Wlow;
        A[1] ^= Whigh;
        shabalP(A, B, C, M);
    }
    for (int i = 0; i < 16; i++) out[i] = B[i];     // хеш = B
}

// ================================================================
// БЛОК 2. ВЛАСНА ХЕШ-ФУНКЦІЯ — МЕТОД МНОЖЕННЯ  (пункт 2 завдання)
// ---------------------------------------------------------------
// Варіант 12 парний, тому власну функцію будуємо методом множення.
// Формула з підручника:  h(k) = [ m * (k*A mod 1) ],
// A = (sqrt(5) - 1)/2 ≈ 0.6180339887 ("золоте січення").
// Щоб не рахувати з дробовими числами при w = 32 бітах:
//   S = round(A * 2^32) = 2654435769.
// Ключ k множимо на ціле S, отримуємо 64-бітний добуток k*S = r1*2^32 + r0.
// Саме МОЛОДШЕ слово r0 — це і є (k*A mod 1) * 2^32 (дробова частина),
// а номер комірки таблиці на m = 2^p комірок — старші p бітів r0:
//   h(k) = (код k*S по модулю 2^32) >> (32 - p).
// Приклад із підручника: k=123456 -> r0=17612864, старші 14 бітів = 67.
// Для довільних даних (файл, рядок) спочатку "склеюємо" їх у число k
// тим самим множенням на S — це вся власна хеш-функція.
// ================================================================

const uint32_t A_MUL = 2654435769u;                 // S = A * 2^32, A = (sqrt(5)-1)/2

// дробова частина k*A як ціле число: r0 = молодше слово добутку k*S,
// тобто (k*A mod 1) * 2^32 (приклад: k=123456 -> r0=17612864,
// старші 14 бітів дають h(k)=67 — див. підручник)
uint32_t mulHash(uint32_t k) {
    return (uint32_t)(((uint64_t)k * A_MUL));
}

int mulIndex(uint32_t k, int bits) {                // номер комірки таблиці на m = 2^bits
    return (int)(mulHash(k) >> (32 - bits));
}

uint32_t mulKey(const unsigned char* data, size_t len) {  // довільні дані -> число k
    uint32_t k = 0;
    for (size_t i = 0; i < len; i++)
        k = k * A_MUL + data[i];
    return k;
}

uint32_t mulHashBytes(const unsigned char* data, size_t len) {
    return mulHash(mulKey(data, len));              // власний хеш за методом множення
}

// ================================================================
// БЛОК 3. ЗАСТОСУВАННЯ 3б): КОНТРОЛЬНА СУМА ФАЙЛУ
// ---------------------------------------------------------------
// 1: обчислюємо суму SHABAL-512 і власною функцією — показуємо;
// 2: дописуємо 64 байти суми SHABAL-512 в кінець файлу;
// 3: перевіряємо дописану суму — свідчить про зміну файлу.
// ================================================================

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

void calcChecksum(const string& name) {
    bool ok;
    vector<unsigned char> data = readFile(name, ok);
    if (!ok) { cout << "Не вдалося відкрити файл: " << name << endl; return; }

    uint32_t h[16];
    shabal512(data, h);
    uint32_t mine = mulHashBytes(data.data(), data.size());

    cout << "Файл: " << name << "   (" << data.size() << " байт)" << endl;
    cout << "SHABAL-512:" << endl << "  " << hex << setfill('0');
    for (int i = 0; i < 16; i++) cout << setw(8) << h[i];
    cout << dec << setfill(' ') << endl;
    cout << "Власна, метод множення (32 біт): " << hex << setfill('0')
         << setw(8) << mine << dec << setfill(' ') << endl;
}

void appendChecksum(const string& name) {
    bool ok;
    vector<unsigned char> data = readFile(name, ok);
    if (!ok) { cout << "Не вдалося відкрити файл: " << name << endl; return; }

    uint32_t h[16];
    shabal512(data, h);
    unsigned char sum[64];
    encodeBlock(h, sum);                            // слова хешу в байти little endian

    ofstream f(name, ios::binary | ios::app);       // дописуємо в кінець
    f.write((char*)sum, 64);
    cout << "Контрольну суму (64 байти) дописано в кінець файлу " << name << endl;
}

void verifyChecksum(const string& name) {
    bool ok;
    vector<unsigned char> data = readFile(name, ok);
    if (!ok) { cout << "Не вдалося відкрити файл: " << name << endl; return; }
    if (data.size() < 64) { cout << "Файл закороткий, суми в ньому немає." << endl; return; }

    size_t n = data.size() - 64;                    // частина файлу без суми
    uint32_t h[16];
    shabal512(vector<unsigned char>(data.begin(), data.begin() + n), h);

    uint32_t saved[16];
    for (int i = 0; i < 16; i++)                    // зчитана сума з кінця файлу
        saved[i] = data[n + 4*i] | (uint32_t(data[n + 4*i+1]) << 8) |
                   (uint32_t(data[n + 4*i+2]) << 16) | (uint32_t(data[n + 4*i+3]) << 24);

    bool same = true;
    for (int i = 0; i < 16; i++)
        if (saved[i] != h[i]) same = false;
    cout << (same ? "Перевірка УСПІШНА — контрольна сума збігається."
                  : "Перевірка НЕВДАЛА — файл було змінено!") << endl;
}

// ================================================================
// БЛОК 4. ПОРІВНЯННЯ ХЕШ-ФУНКЦІЙ  (пункт 5 завдання)
// ---------------------------------------------------------------
// Хеш-таблиця на m комірок (m лежить у діапазоні 10^3..10^4),
// заповнюємо її на 60% (допустимо 50–70%), додаємо 700 нових ключів
// (допустимо 500–1000) і рахуємо, скільки з них потрапило в зайняту
// комірку (колізії). Діапазон ключів варіюємо — від нього суттєво
// залежать результати (особливо для простої власної функції).
// ================================================================

// унікальні випадкові ключі з [0, range)
vector<uint32_t> makeKeys(uint32_t range, int count, mt19937& gen) {
    vector<uint32_t> keys;
    unordered_set<uint32_t> used;
    uniform_int_distribution<uint32_t> dist(0, range - 1);
    while ((int)keys.size() < count) {
        uint32_t k = dist(gen);
        if (used.insert(k).second) keys.push_back(k);
    }
    return keys;
}

// SHABAL-512 від 32-бітного ключа (4 байти), беремо старші біти першого слова
int shabalIndexOf(uint32_t key, int bits) {
    unsigned char b[4]; keyBytes(key, b);
    vector<unsigned char> msg(b, b + 4);
    uint32_t h[16];
    shabal512(msg, h);
    return (int)(h[0] >> (32 - bits));
}

void collisions() {
    const int bits = 13;                        // m = 2^13 = 8192 (10^3..10^4)
    const int m = 1 << bits;
    const int fill = m * 60 / 100;              // 60% заповнення
    const int keys = 700;                       // нових ключів (500..1000)
    const uint32_t ranges[3] = {10000u, 1000000u, 1000000000u};
    const char* rname[3] = {"0..10^4", "0..10^6", "0..10^9"};

    cout << "Хеш-таблиця: " << m << " комірок, зайнято " << fill
         << " (" << (fill*100 + m/2)/m << "%), додається " << keys << " нових ключів." << endl;
    // очікувана кількість колізій: keys * (1 - (1 - 1/m)^fill)
    double frac = 1.0 - exp(-(double)fill / m);
    cout << "Очікувано колізій при рівномірному хешуванні: " << (int)(keys * frac) << endl << endl;

    for (int r = 0; r < 3; r++) {
        mt19937 gen(1000 + r);
        vector<uint32_t> all = makeKeys(ranges[r], fill + keys, gen);
        // таблиця заповнюється першими fill ключами, решта — нові
        vector<uint32_t> base(all.begin(), all.begin() + fill);
        vector<uint32_t> test(all.begin() + fill, all.end());

        cout << "Діапазон ключів " << rname[r] << ":" << endl;
        const char* names[2] = {"SHABAL-512            ", "Власна, метод множення"};
        for (int f = 0; f < 2; f++) {
            vector<char> used(m, 0);
            auto idx = [&](uint32_t k) { return f == 0 ? shabalIndexOf(k, bits)
                                                       : mulIndex(k, bits); };
            for (int i = 0; i < fill; i++) used[idx(base[i])] = 1;
            int hit = 0;
            for (int i = 0; i < keys; i++) {
                int at = idx(test[i]);
                if (used[at]) hit++;                   // комірка вже зайнята — колізія
                else used[at] = 1;
            }
            cout << "    " << names[f] << ": " << hit << " колізій з " << keys << endl;
        }
        cout << endl;
    }
}

// ================================================================
// МЕНЮ ТА ГОЛОВНА ФУНКЦІЯ  (пункт 4 завдання)
// ================================================================

// перевірка SHABAL-512 за відомими контрольними векторами
// (узгоджено з референсною реалізацією sphlib 3.0)
bool selftest() {
    // шістнадцятковий вигляд 16 слів хеша
    auto hexOf = [](const uint32_t h[16]) {
        char t[16];
        string s;
        for (int i = 0; i < 16; i++) { snprintf(t, sizeof t, "%08x", h[i]); s += t; }
        return s;
    };

    // хеш порожнього повідомлення
    uint32_t h[16];
    shabal512(vector<unsigned char>(), h);
    if (hexOf(h) != "ff5d2dfcf6b7705dfcc2f8b1fef9c1c842e534990ceded57a239b5f2cc190aef"
                   "8d4fa8ffe435a19ff5093cbd27a9f390ac03d6eb9e72eb29f09a2a6fc68dad31")
        return false;

    // хеш повідомлення "Hello!"
    const unsigned char hello[] = {'H','e','l','l','o','!'};
    vector<unsigned char> msg(hello, hello + 6);
    shabal512(msg, h);
    if (hexOf(h) != "c3b0c54d73871d4f9dbdf6dc59270dbd673f24ed506cf41c00d8fe1c032838ea4"
                   "62545fb9a9f3253c012ba4d9c91ea1fe7887f8c0fab330b6d123c1623af9e2c")
        return false;

    return true;
}

int main() {
    string file = "getfile.pdf";
    int choice = -1;

    cout << "Лабораторна робота №5. Хеш-функції та хеш-таблиці." << endl;
    cout << "Варіант 1.12 — SHABAL, застосування 3б), власна функція методом множення." << endl;
    cout << "Перевірка SHABAL-512 за контрольною сумою: "
         << (selftest() ? "УСПІШНО" : "ПОМИЛКА") << endl;
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
            case 2: cout << "Файл буде змінено (дописування 64 байтів)!\n";
                    appendChecksum(file); break;
            case 3: verifyChecksum(file); break;
            case 4: collisions(); break;
            case 0: break;
            default: cout << "Введіть число від 0 до 4." << endl;
        }
    }
    cout << "Кінець" << endl;
    return 0;
}
