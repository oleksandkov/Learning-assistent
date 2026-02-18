#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
using namespace std;

// виводить підказку і повертає введений рядок
string ask(string p) { cout << p; string s; getline(cin, s); return s; }

// STAT — інфо про диск (всього / вільно / зайнято)
void doStat(string path) {
    if (path.empty()) path = "C:\\";
    ULARGE_INTEGER free, total;
    if (GetDiskFreeSpaceExA(path.c_str(), &free, &total, NULL))
        cout << "Диск: " << path
            << " | Всього: "  << total.QuadPart / 1073741824 << "GB"
            << " | Вільно: "  << free.QuadPart  / 1073741824 << "GB"
            << " | Зайнято: " << (total.QuadPart - free.QuadPart) / 1073741824 << "GB\n";
    else cout << "Помилка: диск не знайдено.\n";
}

// DISKCOMP — порівнює файли двох папок побайтово
void doDiskcomp(string p1, string p2) {
    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA((p1 + "\\*").c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) { cout << "Папку не знайдено.\n"; return; }
    bool ok = true;
    do {
        string name = fd.cFileName;
        if (name == "." || name == "..") continue;
        ifstream a(p1+"\\"+name, ios::binary), b(p2+"\\"+name, ios::binary);
        if (!b.is_open()) { cout << "  Тільки в Dir1: " << name << "\n"; ok = false; continue; }
        string c1((istreambuf_iterator<char>(a)),{}), c2((istreambuf_iterator<char>(b)),{});
        cout << "  " << (c1==c2 ? "[=]":"[!]") << " " << name << "\n";
        if (c1 != c2) ok = false;
    } while (FindNextFileA(h, &fd));
    FindClose(h);
    cout << (ok ? "ІДЕНТИЧНІ\n" : "ВІДРІЗНЯЮТЬСЯ\n");
}

// ERA — видаляє файл якщо ok=true (юзер підтвердив)
void doEra(string path, bool ok) {
    if (!ok) { cout << "Скасовано.\n"; return; }
    cout << (DeleteFileA(path.c_str()) ? "Файл видалено.\n" : "Помилка видалення.\n");
}

// Q&A і Меню: Q&A виходить після першої команди, Меню — крутиться в циклі
void selectMode(string title) {
    while (true) {
        cout << "[" << title << "] 1-STAT 2-DISKCOMP 3-ERA 0-Назад\n";
        string cmd = ask("Вибір: ");
        if      (cmd == "0") return;
        else if (cmd == "1") doStat(ask("Диск: "));
        else if (cmd == "2") doDiskcomp(ask("Dir1: "), ask("Dir2: "));
        else if (cmd == "3") doEra(ask("Файл: "), ask("Підтвердити? (y/n): ") == "y");
        else cout << "Невірний вибір.\n";
        if (title == "Q&A") return;
    }
}

/* Командний режим — команда і параметри вводяться в одному рядку,
якщо параметри не вказані — програма запитує їх окремо */
void commandMode() {
    cout << "[Командний] STAT / DISKCOMP / ERA / EXIT\n";
    while (true) {
        string line = ask("A:\\> ");
        if      (line == "EXIT")               return;
        else if (line.substr(0,4) == "STAT")   doStat(line.size()>5 ? line.substr(5) : ask("Диск: "));
        else if (line.substr(0,8) == "DISKCOMP") doDiskcomp(ask("Dir1: "), ask("Dir2: "));
        else if (line.substr(0,3) == "ERA")    doEra(line.size()>4 ? line.substr(4) : ask("Файл: "), ask("Підтвердити? (y/n): ")=="y");
        else cout << "Невідома команда.\n";
    }
}

// Форма — збирає всі поля спочатку, показує їх, потім виконує
void formMode() {
    string op  = ask("Оператор: ");
    string cmd = ask("Команда (1-STAT 2-DISKCOMP 3-ERA): ");
    string p1  = ask("Параметр 1: ");
    string p2  = (cmd=="2" ? ask("Параметр 2: ") : "");
    bool   ok  = (cmd=="3" && ask("Підтвердити? (y/n): ")=="y");
    cout << "Оператор=" << op << " Команда=" << cmd << " P1=" << p1 << " P2=" << p2 << "\n";
    if (ask("Виконати? (y/n): ") != "y") { cout << "Скасовано.\n"; return; }
    if      (cmd=="1") doStat(p1);
    else if (cmd=="2") doDiskcomp(p1, p2);
    else if (cmd=="3") doEra(p1, ok);
    else cout << "Невірна команда.\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8); 
    cout << "Лаб 1-2, Варіант 9: STAT, DISKCOMP, ERA\n";
    while (true) {
        cout << "\n1-Командний 2-Q&A 3-Меню 4-Форма 0-Вихід\n";
        string ch = ask("Вибір: ");
        if      (ch=="1") commandMode();
        else if (ch=="2") selectMode("Q&A");
        else if (ch=="3") selectMode("Меню");
        else if (ch=="4") formMode();
        else if (ch=="0") return 0;
        else cout << "Невірний вибір.\n";
    }
}