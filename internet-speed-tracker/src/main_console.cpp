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
        std::cout << "\n========================================\n";
        std::cout << "Running speed test #" << testCount << "...\n";
        std::cout << "========================================\n";
        
        SpeedResult result = tester.runTest();
        
        if (result.isValid) {
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "✓ Download: " << result.downloadSpeed << " Mbps\n";
            std::cout << "✓ Upload:   " << result.uploadSpeed << " Mbps\n";
            std::cout << "✓ Ping:     " << result.ping << " ms\n";
            
            logger.logResult(result);
            std::cout << "✓ Result logged to speed_log.csv\n";
            
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                globalResults = tester.getResults();
            }
        } else {
            std::cout << "✗ Test failed - connection issue\n";
        }
        
        if (testCount < 5) {
            std::cout << "\nWaiting " << intervalSeconds << " seconds for next test...\n";
            std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
        } else {
            std::cout << "\n5 tests completed. Stopping monitoring.\n";
            isRunning = false;
        }
    }
}

int main() {
    std::cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   Internet Speed Tracker - Console Version                   ║\n";
    std::cout << "║   Running continuous speed tests with logging                ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";
    
    SpeedTester tester;
    Logger logger;
    
    std::cout << "Starting 5 consecutive speed tests (60 seconds interval)...\n";
    std::cout << "Results will be saved to: speed_log.csv\n\n";
    
    std::thread monitor(monitoringThread, std::ref(tester), std::ref(logger), 60);
    
    monitor.join();
    
    // Display statistics
    {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        if (!globalResults.empty()) {
            double avgDown = tester.getAverageDownload();
            double avgUp = tester.getAverageUpload();
            double avgPing = tester.getAveragePing();
            
            std::cout << "\n╔════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                      FINAL STATISTICS                        ║\n";
            std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";
            
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "Total Tests Completed:  " << globalResults.size() << "\n";
            std::cout << "Average Download Speed: " << avgDown << " Mbps\n";
            std::cout << "Average Upload Speed:   " << avgUp << " Mbps\n";
            std::cout << "Average Ping Latency:   " << avgPing << " ms\n\n";
        }
    }
    
    std::cout << "Monitoring stopped. All results saved to speed_log.csv\n";
    std::cout << "\nProgram completed successfully!\n\n";
    
    return 0;
}
