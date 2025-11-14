#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include "SpeedTester.h"

class Logger {
private:
    std::string logFilePath;
    
public:
    Logger(const std::string& filepath = "speed_log.csv");
    
    void logResult(const SpeedResult& result);
    void initializeLogFile();
    std::vector<SpeedResult> readLog();
};

#endif
