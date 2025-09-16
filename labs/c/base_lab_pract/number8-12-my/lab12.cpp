
// Завдання: Знайти аеропорт, який є проміжною (не початковою і не кінцевою) точкою маршруту
// для найбільшої кількості різних рейсів.

// Покрокова інструкція (детально):
// 1. Відкрити файл flight.csv для читання.
//    - Перевірити, чи файл відкрився успішно. Якщо ні — завершити програму з повідомленням про помилку.
// 2. Пропустити перший рядок (заголовок CSV).
// 3. Зчитати всі рядки файлу по черзі:
//    - Для кожного рядка розбити його на поля (номер рейсу, часи, аеропорти).
//    - Зберігати всі сегменти (перельоти) для кожного рейсу у map: ключ — номер рейсу, значення — вектор сегментів.
// 4. Для кожного рейсу:
//    - Відсортувати його сегменти за часом відправлення (depTime), щоб отримати правильний порядок маршруту.
//    - Побудувати маршрут: перший аеропорт — це depAirport першого сегмента, далі додаємо всі arrAirport по черзі.
//    - Визначити проміжні аеропорти (усі, крім першого та останнього у маршруті).
//    - Додати ці аеропорти у set (щоб не рахувати один і той самий аеропорт двічі для одного рейсу).
//    - Для кожного проміжного аеропорту збільшити лічильник у map: airport -> кількість рейсів, де він проміжний.
// 5. Знайти аеропорт із найбільшим значенням лічильника (тобто через який пролітає найбільше різних рейсів).
// 6. Вивести результат: назву аеропорту та кількість рейсів.

// Пояснення до кожного кроку див. у коментарях до коду нижче.

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;


struct Segment {
    string depTime;
    string arrTime;
    string depAirport;
    string arrAirport;
};

bool parseLine(const string &line, string &flightNo, Segment &seg) {
    stringstream ss(line);
    if (!getline(ss, flightNo, ',')) return false;
    if (!getline(ss, seg.depTime, ',')) return false;
    if (!getline(ss, seg.arrTime, ',')) return false;
    if (!getline(ss, seg.depAirport, ',')) return false;
    if (!getline(ss, seg.arrAirport, ',')) return false;
    return true;
}

vector<string> buildRoute(vector<Segment> &segments) {
    vector<string> route;
    if (segments.empty()) return route;
    sort(segments.begin(), segments.end(), [](const Segment &a, const Segment &b) {
        return a.depTime < b.depTime;
    });
    route.push_back(segments.front().depAirport);
    for (auto &s : segments) {
        route.push_back(s.arrAirport);
    }
    return route;
}

int main() {
    // 1. Відкриваємо файл для читання
    ifstream file("flight.csv");

    if (!file.is_open()) {
        // Якщо файл не відкрився — повідомляємо про помилку і завершуємо
        cerr << "Error opening file flight.csv" << endl;
        return 1;
    }


    // 2. Пропускаємо перший рядок (заголовок)
    string header;
    getline(file, header);


    // 3. Зчитуємо всі рядки, групуємо сегменти по рейсам
    map<string, vector<Segment>> flights; // номер рейсу -> всі його сегменти
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string flightNo;
        Segment seg;
        // Розбираємо рядок на поля
        if (parseLine(line, flightNo, seg)) {
            flights[flightNo].push_back(seg);
        }
    }
    file.close();


    // 4. Для кожного рейсу визначаємо проміжні аеропорти
    map<string, int> intermediateCount; // аеропорт -> кількість рейсів

    for (auto &kv : flights) {
        // Сортуємо сегменти та будуємо маршрут
        auto route = buildRoute(kv.second);
        if (route.size() <= 2) continue; // якщо менше 3 аеропортів — немає проміжних
        set<string> uniqueMids;
        // Додаємо всі проміжні аеропорти (без першого і останнього)
        for (size_t i = 1; i + 1 < route.size(); ++i) {
            uniqueMids.insert(route[i]);
        }
        // Для кожного проміжного аеропорту збільшуємо лічильник
        for (auto &ap : uniqueMids) {
            intermediateCount[ap]++;
        }
    }


    // 5. Якщо не знайдено жодного проміжного аеропорту — повідомляємо
    if (intermediateCount.empty()) {
        cout << "No intermediate airports found." << endl;
        return 0;
    }


    // 6. Знаходимо аеропорт із найбільшою кількістю рейсів
    string bestAirport;
    int bestCount = -1;
    for (auto &p : intermediateCount) {
        if (p.second > bestCount) {
            bestCount = p.second;
            bestAirport = p.first;
        }
    }

    // Виводимо результат
    cout << "Проміжний аеропорт з найбільшою кількістю рейсів: "
         << bestAirport << " (" << bestCount << ")" << endl;

    return 0;
}