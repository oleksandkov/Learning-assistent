#ifndef SPEED_TESTER_H
#define SPEED_TESTER_H

#include <string>
#include <vector>
#include <chrono>

struct SpeedResult {
    std::chrono::system_clock::time_point timestamp;
    double downloadSpeed;
    double uploadSpeed;
    double ping;
    bool isValid;
};

class SpeedTester {
private:
    std::string testServerUrl;
    std::vector<SpeedResult> results;
    
    double measureDownloadSpeed();
    double measureUploadSpeed();
    double measurePing();
    
public:
    SpeedTester(const std::string& serverUrl = "http://speedtest.ftp.otenet.gr/files/test10Mb.db");
    
    SpeedResult runTest();
    const std::vector<SpeedResult>& getResults() const;
    
    double getAverageDownload() const;
    double getAverageUpload() const;
    double getAveragePing() const;
    
    void clearResults();
};

#endif
