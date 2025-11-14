#include "SpeedTester.h"
#include "Visualizer.h"
#include "Logger.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

std::mutex dataMutex;
std::vector<SpeedResult> globalResults;
bool isRunning = true;

void monitoringThread(SpeedTester& tester, Logger& logger, int intervalSeconds) {
    while (isRunning) {
        std::cout << "Running speed test...\n";
        SpeedResult result = tester.runTest();
        
        if (result.isValid) {
            std::cout << "Download: " << result.downloadSpeed << " Mbps, "
                      << "Upload: " << result.uploadSpeed << " Mbps, "
                      << "Ping: " << result.ping << " ms\n";
            
            logger.logResult(result);
            
            {
                std::lock_guard<std::mutex> lock(dataMutex);
                globalResults = tester.getResults();
            }
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    }
}

int main() {
    SpeedTester tester;
    Logger logger;
    Visualizer visualizer;
    
    std::cout << "Internet Speed Tracker - SFML Visualization\n";
    std::cout << "Starting continuous monitoring (Test every 60 seconds)...\n";
    std::cout << "Close window to stop\n\n";
    
    std::thread monitor(monitoringThread, std::ref(tester), std::ref(logger), 60);
    
    while (visualizer.isOpen()) {
        if (!visualizer.handleEvents()) {
            break;
        }
        
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            visualizer.update(globalResults);
        }
        
        visualizer.render();
    }
    
    isRunning = false;
    monitor.join();
    
    std::cout << "\nMonitoring stopped. Results saved to speed_log.csv\n";
    
    return 0;
}
