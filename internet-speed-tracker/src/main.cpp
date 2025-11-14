#include "SpeedTester.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <iomanip>

std::mutex dataMutex;
std::vector<SpeedResult> globalResults;
bool isRunning = true;

void monitoringThread(SpeedTester& tester, Logger& logger, int intervalSeconds) {
    int testCount = 0;
    while (isRunning) {
        testCount++;
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "Speed Test #" << testCount << "\n";
        std::cout << std::string(50, '=') << "\n";
        
        SpeedResult result = tester.runTest();
        
        if (result.isValid) {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Download: " << result.downloadSpeed << " Mbps\n"
                      << "Upload:   " << result.uploadSpeed << " Mbps\n"
                      << "Ping:     " << result.ping << " ms\n";
            
            logger.logResult(result);
            
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                globalResults = tester.getResults();
            }
        } else {
            std::cout << "Test failed - connection issue\n";
        }
        
        if (testCount < 5) {
            std::cout << "\nWaiting " << intervalSeconds << " seconds for next test...\n";
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
        } else {
            isRunning = false;
            std::cout << "\n5 tests completed. Stopping.\n";
        }
    }
}

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║   Internet Speed Tracker - Console Version             ║\n";
    std::cout << "║   Running 5 continuous speed tests with CSV logging    ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    
    SpeedTester tester;
    Logger logger;
    
    std::cout << "\nStarting speed tests (60 seconds interval)...\n";
    std::cout << "Results saved to: speed_log.csv\n";
    
    std::thread monitor(monitoringThread, std::ref(tester), std::ref(logger), 60);
    monitor.join();
    
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        if (!globalResults.empty()) {
            double avgDown = tester.getAverageDownload();
            double avgUp = tester.getAverageUpload();
            double avgPing = tester.getAveragePing();
            
            std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
            std::cout << "║                  FINAL STATISTICS                      ║\n";
            std::cout << "╚═══════════════════════════════════════════════════════╝\n\n";
            
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Total Tests:            " << globalResults.size() << "\n";
            std::cout << "Average Download Speed: " << avgDown << " Mbps\n";
            std::cout << "Average Upload Speed:   " << avgUp << " Mbps\n";
            std::cout << "Average Ping Latency:   " << avgPing << " ms\n\n";
        }
    }
    
    std::cout << "All results saved to speed_log.csv\n";
    std::cout << "Program completed!\n\n";
    
    return 0;
}
