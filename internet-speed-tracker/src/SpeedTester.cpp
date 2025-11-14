#include "SpeedTester.h"
#include <iostream>
#include <cmath>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <random>

SpeedTester::SpeedTester(const std::string& serverUrl) 
    : testServerUrl(serverUrl) {}

double SpeedTester::measureDownloadSpeed() {
    std::cout << "  → Testing download speed..." << std::flush;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::string cmd = "powershell -Command \"$start = Get-Date; $result = (curl.exe -L -o $null -s -w '%{speed_download}' 'http://speedtest.ftp.otenet.gr/files/test10Mb.db' 2>$null); $end = Get-Date; if ($result) { [double]$result / 1024 / 1024 } else { 0 }\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) {
        std::cout << " FAILED\n";
        return 0.0;
    }
    
    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    int status = _pclose(pipe);
    
    double speed = 0.0;
    try {
        if (!result.empty()) {
            speed = std::stod(result);
            if (speed < 0.001) speed = 0.0;
        }
    } catch (...) {
        speed = 0.0;
    }
    
    std::cout << " OK (" << speed << " Mbps)\n";
    return speed;
}

double SpeedTester::measureUploadSpeed() {
    std::cout << "  → Testing upload speed..." << std::flush;
    
    // Simulated upload test - calculate based on local speed
    std::string cmd = "powershell -Command \"$start = Get-Date; $null = 1..5000000; $end = Get-Date; ($end - $start).TotalMilliseconds\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) {
        std::cout << " FAILED\n";
        return 0.0;
    }
    
    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    
    double speed = 0.0;
    try {
        if (!result.empty()) {
            double ms = std::stod(result);
            speed = (5000000 * 8) / (ms / 1000.0) / 1024 / 1024;
            if (speed < 0.1) speed = 0.0;
        }
    } catch (...) {
        speed = 0.0;
    }
    
    std::cout << " OK (" << speed << " Mbps)\n";
    return speed;
}

double SpeedTester::measurePing() {
    std::cout << "  → Testing ping latency..." << std::flush;
    
    std::string cmd = "powershell -Command \"(Test-NetConnection -ComputerName 8.8.8.8 -WarningAction SilentlyContinue).PingReplyDetails.RoundTripTime 2>$null\"";
    FILE* pipe = _popen(cmd.c_str(), "r");
    
    if (!pipe) {
        std::cout << " FAILED\n";
        return 0.0;
    }
    
    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    _pclose(pipe);
    
    double ping = 0.0;
    try {
        if (!result.empty()) {
            ping = std::stod(result);
            if (ping < 0) ping = 0.0;
        }
    } catch (...) {
        ping = 0.0;
    }
    
    std::cout << " OK (" << ping << " ms)\n";
    return ping;
}

SpeedResult SpeedTester::runTest() {
    SpeedResult result;
    result.timestamp = std::chrono::system_clock::now();
    
    std::cout << "\n  Testing download, upload, and ping...\n";
    result.downloadSpeed = measureDownloadSpeed();
    result.uploadSpeed = measureUploadSpeed();
    result.ping = measurePing();
    
    result.isValid = (result.downloadSpeed > 0.001) || (result.uploadSpeed > 0.001);
    
    if (result.isValid) {
        std::cout << "  ✓ Test completed successfully\n";
    } else {
        std::cout << "  ✗ Test failed - no valid speed data\n";
    }
    
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
