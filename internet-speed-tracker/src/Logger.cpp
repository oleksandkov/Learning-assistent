#include "Logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

Logger::Logger(const std::string& filepath) 
    : logFilePath(filepath) {
    initializeLogFile();
}

void Logger::initializeLogFile() {
    std::ofstream file(logFilePath, std::ios::app);
    if (file.is_open()) {
        file.seekp(0, std::ios::end);
        if (file.tellp() == 0) {
            file << "Timestamp,Download(Mbps),Upload(Mbps),Ping(ms)\n";
        }
        file.close();
    }
}

void Logger::logResult(const SpeedResult& result) {
    std::ofstream file(logFilePath, std::ios::app);
    if (file.is_open()) {
        auto time = std::chrono::system_clock::to_time_t(result.timestamp);
        std::tm* tm = std::localtime(&time);
        
        file << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << ","
             << result.downloadSpeed << ","
             << result.uploadSpeed << ","
             << result.ping << "\n";
        file.close();
    }
}

std::vector<SpeedResult> Logger::readLog() {
    std::vector<SpeedResult> results;
    std::ifstream file(logFilePath);
    std::string line;
    
    if (file.is_open()) {
        std::getline(file, line);
        
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string timestamp, download, upload, ping;
            
            if (std::getline(ss, timestamp, ',') &&
                std::getline(ss, download, ',') &&
                std::getline(ss, upload, ',') &&
                std::getline(ss, ping)) {
                
                SpeedResult result;
                try {
                    result.downloadSpeed = std::stod(download);
                    result.uploadSpeed = std::stod(upload);
                    result.ping = std::stod(ping);
                    result.isValid = true;
                    results.push_back(result);
                } catch (...) {
                    continue;
                }
            }
        }
        file.close();
    }
    
    return results;
}
