

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

// Визначаємо елемент сегмента польоту
struct Segment { // good 
    string depTime;
    string arrTime;
    string depAirport;
    string arrAirport;
};

// "Витягуємо" елементи у вектор
bool parseLine(const string& line, string& flightNo, Segment& seg) { // +-
    size_t start = 0, pos = line.find(',', start);
    if (pos == string::npos) return false;
    flightNo = line.substr(start, pos - start);
    start = pos + 1;

// "Витягуємо" елементи у вектор
    pos = line.find(',', start); // шукаємо кому
    if (pos == string::npos) return false; // якщо коми немає, повертаємо false
    seg.depTime = line.substr(start, pos - start); // витягуємо підрядок з початку до коми
    start = pos + 1; // оновлюємо початок на наступний символ після коми

    pos = line.find(',', start);
    if (pos == string::npos) return false;
    seg.arrTime = line.substr(start, pos - start);
    start = pos + 1;

    pos = line.find(',', start);
    if (pos == string::npos) return false;
    seg.depAirport = line.substr(start, pos - start);
    start = pos + 1;

    seg.arrAirport = line.substr(start);
    if (seg.arrAirport.empty()) return false;

    return true;
}

// Будуємо маршрут ( шляхом сортування польотів за часом вильоту, а потім створенням послідовності аеропортів -- 
// додаємо перший аеропорт відбуття, потім додаємо всі аеропорти прибуття)
vector<string> buildRoute(vector<Segment>& segments) {
    vector<string> route;
    if (segments.empty()) return route;

    // Сортуємо сегменти за часом вильоту
    sort(segments.begin(), segments.end(), [](const Segment& a, const Segment& b) {
        return a.depTime < b.depTime;
    });
// Додаємо до вертора "route" перший аеропорт відбуття, потім додаємо всі аеропорти прибуття
    route.push_back(segments[0].depAirport);
    for (const auto& seg : segments) {
        route.push_back(seg.arrAirport);
    }
    return route;
}

int main() {
        // cout << "Which file to open? \n";
        // cout << "Print your file name:\n";
        // string fileName;
        // cin >> fileName;
        // ifstream file("flight.csv");
        ifstream file(R"(c:\Users\muaro\Documents\GitHub\Learning-assistent\labs\c\base_lab_pract\number8-12-my\flight.csv)");
        if (!file.is_open()) {
            cerr << "Error opening file flight.csv" << endl;
            cin.get();
            cin.ignore(10000, '\n');
            return 33;
        }

    string header;
    getline(file, header);  // Пропускаємо заголовок таблиць

    map<string, vector<Segment>> flights; // створюємо словник, де ключ - номер рейсу, а значення - вектор сегментів
    string line;
    while (getline(file, line)) { // читаємо файл рядок за рядком і додаємо до словника
        if (line.empty()) continue;
        string flightNo;
        Segment seg;
        if (parseLine(line, flightNo, seg)) {
            flights[flightNo].push_back(seg);
        }
    }
    file.close();

    map<string, int> intermediateCount; // створюємо словник для підрахунку проміжних аеропортів (ключ - назва аеропорту, значення - кількість разів, коли через аеропорт пройшов рейс)

    for (const auto& pair : flights) {
        vector<Segment> segs = pair.second;  // Копіюємо, щоб не змінювати оригінал
        vector<string> route = buildRoute(segs);
        if (route.size() <= 2) continue; // Якщо маршрут має 2 або менше транзитних аеропортів, пропускаємо його

        set<string> uniqueMids; // Використовуємо set для унікальних проміжних аеропортів 
        for (int i = 1; i < route.size() - 1; ++i) { // Проміжні аеропорти - це всі, крім першого і останнього
            uniqueMids.insert(route[i]);
        }

        for (const string& mid : uniqueMids) { // Підраховуємо кожен унікальний проміжний аеропорт
            // ++intermediateCount[mid];
            intermediateCount[mid] += 1;

        }
    }

    if (intermediateCount.empty()) {
        cout << "No intermediate airports found." << endl;
        return 33;
    }

    string bestAirport;
    int bestCount = 0;
    for (const auto& pair : intermediateCount) {
        if (pair.second > bestCount) {
            bestCount = pair.second; 
            bestAirport = pair.first;
        }
    }

    cout << "Проміжний аеропорт з найбільшою кількістю рейсів: "
         << bestAirport << " (" << bestCount << ")" << endl;
    cin.get();
    cin.ignore(10000, '\n');
    return 0;
}