#include <iostream>
#include <string>
using namespace std;

string ask(string prompt) { cout << prompt; string s; getline(cin, s); return s; }

void doStat(string drive) {
    if (drive.empty()) drive = "C:";
    if (drive.back() == ':') drive += '\\';
    string cmd = "dir /-C \"" + drive + "\"";
    system(cmd.c_str());
}

void doDiskcomp(string f1, string f2) {
    system(("FC /b \"" + f1 + "\" \"" + f2 + "\"").c_str());
}

void doEra(string path, bool confirmed) {
    if (!confirmed) { cout << "Скасовано.\n"; return; }
    system(("DEL /f \"" + path + "\"").c_str());
}

void menuMode() {
    while (true) {
        cout << "\n[Меню] 1-STAT  2-DISKCOMP  3-ERA  0-Назад\n";
        string ch = ask("Вибір: ");
        if      (ch == "0") return;
        else if (ch == "1") doStat(ask("Диск: "));
        else if (ch == "2") doDiskcomp(ask("Файл 1: "), ask("Файл 2: "));
        else if (ch == "3") {
            string file = ask("Файл: ");
            if (ask("Підтвердити? (y/n): ") == "y")
                doEra(file, true);
            else
                cout << "Скасовано.\n";
        }
        else cout << "Невірний вибір.\n";
    }
}

void qaMode() {
    cout << "\n[Q&A] 1-STAT  2-DISKCOMP  3-ERA  0-Назад\n";
    string ch = ask("Вибір: ");
    if      (ch == "0") return;
    else if (ch == "1") doStat(ask("Диск: "));
    else if (ch == "2") doDiskcomp(ask("Файл 1: "), ask("Файл 2: "));
    else if (ch == "3") {
        string file = ask("Файл: ");
        if (ask("Підтвердити? (y/n): ") == "y")
            doEra(file, true);
        else
            cout << "Скасовано.\n";
    }
    else cout << "Невірний вибір.\n";
}

void commandMode() {
    cout << "[Командний] STAT / DISKCOMP / ERA / EXIT\n";
    cout << "Приклади:\n";
    cout << "STAT C:                         -> інфо про диск C:\n";
    cout << "STAT                            -> запитає диск окремо\n";
    cout << "DISKCOMP                        -> запитає обидва файли окремо\n";
    cout << "ERA C:\\test\\old.txt             -> видалити файл (з підтвердженням)\n";
    cout << "ERA                             -> запитає шлях окремо\n";
    while (true) {
        string line = ask("A:\\> ");
        if      (line == "EXIT")                  return;
        else if (line.substr(0,4) == "STAT") {
            string arg = (line.size() > 5 ? line.substr(5) : ask("Диск:"));
            size_t first = arg.find_first_not_of(" \t");
            if (first != string::npos) arg = arg.substr(first);
            else arg = "";
            doStat(arg);
        }
        else if (line.substr(0,8) == "DISKCOMP")  doDiskcomp(ask("Файл 1: "), ask("Файл 2: "));
        else if (line.substr(0,3) == "ERA")       doEra(line.size() > 4 ? line.substr(4) : ask("Файл: "), ask("Підтвердити? (y/n): ") == "y");
        else cout << "Невідома команда.\n";
    }
}

void formMode() {
    cout << "  Приклад заповнення форми:\n";
    cout << "    Оператор:  Іванов\n";
    cout << "    Команда:   1 (STAT), 2 (DISKCOMP), 3 (ERA)\n";
    cout << "    Параметр 1: C:  або  C:\\test\\a.txt\n";
    cout << "    Параметр 2: C:\\test\\b.txt  (тільки для DISKCOMP)\n";
    string op  = ask("Оператор: ");
    string cmd = ask("Команда (1-STAT 2-DISKCOMP 3-ERA): ");
    string p1  = ask("Параметр 1: ");
    string p2  = (cmd == "2" ? ask("Параметр 2: ") : "");
    bool   ok  = (cmd == "3" && ask("Підтвердити? (y/n): ") == "y");
    cout << "Оператор=" << op << " Команда=" << cmd << " P1=" << p1 << " P2=" << p2 << "\n";
    if (ask("Виконати? (y/n): ") != "y") { cout << "Скасовано.\n"; return; }
    if      (cmd == "1") doStat(p1);
    else if (cmd == "2") doDiskcomp(p1, p2);
    else if (cmd == "3") doEra(p1, ok);
    else cout << "Невірна команда.\n";
}

int main() {
    system("chcp 65001 >nul");
    cout << "Лаб 1-2, Варіант 9: STAT, DISKCOMP, ERA\n";
    while (true) {
        cout << "\n1-Командний  2-Q&A  3-Меню  4-Форма  0-Вихід\n";
        string ch = ask("Вибір: ");
        if      (ch == "1") commandMode();
        else if (ch == "2") qaMode();
        else if (ch == "3") menuMode();
        else if (ch == "4") formMode();
        else if (ch == "0") { cout << "by)\n"; return 0; }
        else cout << "Невірний вибір.\n";
    }
}
