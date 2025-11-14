#include "SpeedTester.h"
#include <iostream>
#include <cmath>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <fstream>

SpeedTester::SpeedTester(const std::string& serverUrl) 
    : testServerUrl(serverUrl) {}

double SpeedTester::measureDownloadSpeed() {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::string cmd = "powershell -Command \"(curl.exe -L \"" + testServerUrl + 
                      "\" -o $null -s -w '%{speed_download}').ToString()\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) return 0.0;
    
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    
    double speed = 0.0;
    try {
        speed = std::stod(result) / 1024 / 1024;
    } catch (...) {
        speed = 0.0;
    }
    
    return speed;
}

double SpeedTester::measureUploadSpeed() {
    // Simulated upload test - in real scenario would use proper upload endpoint
    std::string cmd = "powershell -Command \"(Measure-Object -InputObject (1..10000000 | ForEach-Object {$_}) -Character).Characters / 1024 / 1024\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) return 0.0;
    
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    
    double speed = 0.0;
    try {
        speed = std::stod(result) * 5;
    } catch (...) {
        speed = 0.0;
    }
    
    return speed;
}

double SpeedTester::measurePing() {
    std::string cmd = "powershell -Command \"(Test-NetConnection -ComputerName 8.8.8.8 -WarningAction SilentlyContinue).PingReplyDetails.RoundTripTime\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) return 0.0;
    
    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    
    double ping = 0.0;
    try {
        ping = std::stod(result);
    } catch (...) {
        ping = 0.0;
    }
    
    return ping;
}

SpeedResult SpeedTester::runTest() {
    SpeedResult result;
    result.timestamp = std::chrono::system_clock::now();
    result.downloadSpeed = measureDownloadSpeed();
    result.uploadSpeed = measureUploadSpeed();
    result.ping = measurePing();
    result.isValid = result.downloadSpeed > 0 || result.uploadSpeed > 0;
    
    results.push_back(result);
    return result;
}

const std::vector<SpeedResult>& SpeedTester::getResults() const {
    return results;
}

double SpeedTester::getAverageDownload() const {
    if (results.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& r : results) {
        sum += r.downloadSpeed;
    }
    return sum / results.size();
}

double SpeedTester::getAverageUpload() const {
    if (results.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& r : results) {
        sum += r.uploadSpeed;
    }
    return sum / results.size();
}

double SpeedTester::getAveragePing() const {
    if (results.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& r : results) {
        sum += r.ping;
    }
    return sum / results.size();
}

void SpeedTester::clearResults() {
    results.clear();
}
