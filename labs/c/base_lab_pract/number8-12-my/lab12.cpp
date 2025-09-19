
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Segment {
    string depTime;
    string arrTime;
    string depAirport;
    string arrAirport;
};

bool parseLine(const string &line, string &flightNo, Segment &seg) {
    size_t start = 0, pos;

    pos = line.find(',', start);
    if (pos == string::npos) return false;
    flightNo = line.substr(start, pos - start);
    start = pos + 1;

    pos = line.find(',', start);
    if (pos == string::npos) return false;
    seg.depTime = line.substr(start, pos - start);
    start = pos + 1;

    pos = line.find(',', start);
    if (pos == string::npos) return false;
    seg.arrTime = line.substr(start, pos - start);
    start = pos + 1;

    pos = line.find(',', start);
    if (pos == string::npos) return false;
    seg.depAirport = line.substr(start, pos - start);
    start = pos + 1;

    // last field to end of line
    seg.arrAirport = line.substr(start);
    if (seg.arrAirport.empty()) return false;

    return true;
}

void insertionSortByDepTime(vector<Segment> &segments) {
    for (size_t i = 1; i < segments.size(); ++i) {
        Segment key = segments[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0 && segments[j].depTime > key.depTime) {
            segments[j + 1] = segments[j];
            --j;
        }
        segments[j + 1] = key;
    }
}

vector<string> buildRoute(vector<Segment> &segments) {
    vector<string> route;
    if (segments.empty()) return route;

    insertionSortByDepTime(segments);

    route.push_back(segments[0].depAirport);
    for (size_t i = 0; i < segments.size(); ++i) {
        route.push_back(segments[i].arrAirport);
    }
    return route;
}

bool contains(const vector<string> &v, const string &x) {
    for (size_t i = 0; i < v.size(); ++i) {
        if (v[i] == x) return true;
    }
    return false;
}

int main() {
    ifstream file("flight.csv");
    if (!file.is_open()) {
        cerr << "Error opening file flight.csv" << endl;
        return 1;
    }

    string header;
    getline(file, header);

    map<string, vector<Segment>> flights;
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string flightNo;
        Segment seg;
        if (parseLine(line, flightNo, seg)) {
            flights[flightNo].push_back(seg);
        }
    }
    file.close();

    map<string, int> intermediateCount;

    for (map<string, vector<Segment>>::iterator it = flights.begin(); it != flights.end(); ++it) {
        vector<Segment> &segs = it->second;
        vector<string> route = buildRoute(segs);
        if (route.size() <= 2) continue;

        vector<string> uniqueMids;
        for (size_t i = 1; i + 1 < route.size(); ++i) {
            if (!contains(uniqueMids, route[i])) {
                uniqueMids.push_back(route[i]);
            }
        }

        for (size_t i = 0; i < uniqueMids.size(); ++i) {
            intermediateCount[uniqueMids[i]] += 1;
        }
    }

    if (intermediateCount.empty()) {
        cout << "No intermediate airports found." << endl;
        return 0;
    }

    string bestAirport;
    int bestCount = -1;
    for (map<string, int>::iterator it = intermediateCount.begin(); it != intermediateCount.end(); ++it) {
        if (it->second > bestCount) {
            bestCount = it->second;
            bestAirport = it->first;
        }
    }

    cout << "Проміжний аеропорт з найбільшою кількістю рейсів: "
         << bestAirport << " (" << bestCount << ")" << endl;

    return 0;
}