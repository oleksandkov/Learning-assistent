# Quick Start Guide

## 1. Run the Program

### Option A: Using Batch File (Easiest)

```batch
cd internet-speed-tracker
compile.bat
```

### Option B: Using PowerShell

```powershell
cd internet-speed-tracker
$sourceFiles = "src/SpeedTester.cpp src/Logger.cpp src/Visualizer.cpp src/main.cpp"
$flags = "-Iinclude -IC:/msys64/ucrt64/include -LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-system -pthread"
g++ $sourceFiles $flags -o bin/speed_tracker.exe
.\bin\speed_tracker.exe
```

## 2. What You'll See

1. **Console Window**:

   - Status messages: "Running speed test..."
   - Speed results: Download/Upload/Ping values

2. **SFML Visualization Window** (1200x700):

   - Green line = Download speed trend
   - Red line = Upload speed trend
   - Grid background for reference
   - Statistics at bottom: test count and averages

3. **CSV Log File** (speed_log.csv):
   - Created automatically in project directory
   - Records all test results with timestamps

## 3. How It Works

**Continuous Monitoring Loop:**

```
Start → Run Speed Test (Download, Upload, Ping)
       ↓
   Log to CSV → Update Visualization
       ↓
   Wait 60 seconds → Run Next Test
       ↓
   Close window to stop
```

## 4. Files Created/Used

```
internet-speed-tracker/
├── src/
│   ├── SpeedTester.cpp    → Internet speed testing
│   ├── Visualizer.cpp     → SFML window & graphs
│   ├── Logger.cpp         → CSV logging
│   └── main.cpp           → Threading & main loop
├── include/               → Header files
├── bin/
│   └── speed_tracker.exe  → Compiled executable
└── speed_log.csv          → Test results (auto-created)
```

## 5. Key Code Sections

### Speed Measurement

```cpp
SpeedResult result = tester.runTest();
// Returns: downloadSpeed, uploadSpeed, ping, isValid
```

### Logging Results

```cpp
logger.logResult(result);
// Appends to speed_log.csv
```

### Graph Rendering

```cpp
visualizer.update(globalResults);  // Update data
visualizer.render();               // Draw graphs
```

## 6. Customization

### Change Test Interval (default: 60 seconds)

Edit `src/main.cpp` line ~46:

```cpp
std::thread monitor(monitoringThread, std::ref(tester), std::ref(logger), 120);  // 120 seconds
```

### Change Window Size

Edit `include/Visualizer.h`:

```cpp
const int WINDOW_WIDTH = 1400;   // Change width
const int WINDOW_HEIGHT = 800;   // Change height
```

### Use Different Test Server

Edit `src/SpeedTester.cpp` constructor:

```cpp
SpeedTester tester("http://speedtest.ftp.otenet.gr/files/test10Mb.db");
```

## 7. Understanding the Visualization

**Graph Coordinates:**

- **X-axis**: Time progression (left to right)
- **Y-axis**: Speed in Mbps (bottom to top)
- **Grid lines**: Reference for speed values
- **Points**: Individual test results
- **Lines**: Speed trend over time

**Statistics Panel:**

- Tests: Total number of tests run
- Avg Down: Average download speed
- Avg Up: Average upload speed
- Avg Ping: Average latency

## 8. Data Analysis

### Read Historical Data

```cpp
Logger logger;
auto results = logger.readLog();
// Returns vector of all SpeedResult entries
```

### Calculate Statistics

```cpp
double avgSpeed = tester.getAverageDownload();
double avgUpload = tester.getAverageUpload();
double avgPing = tester.getAveragePing();
```

## 9. Troubleshooting

| Issue              | Solution                                             |
| ------------------ | ---------------------------------------------------- |
| Window won't open  | Check SFML libraries installed correctly             |
| Speed test shows 0 | Internet might be offline, check connectivity        |
| Font not found     | Arial.ttf should be at C:/Windows/Fonts/             |
| Compilation error  | Ensure g++ path and SFML path are correct            |
| CSV file empty     | Check file permissions, ensure /bin directory exists |

## 10. Learning Takeaways

This project demonstrates:

1. **Multi-threading**: Background monitoring with mutex locks
2. **External Processes**: Executing PowerShell/curl commands
3. **File I/O**: Reading/writing CSV data
4. **Graphics**: SFML rendering and event handling
5. **Data Structures**: Storing and processing time-series data
6. **OOP Design**: Separate classes for each responsibility
7. **Synchronization**: Thread-safe data sharing

## 11. Next Steps

After running successfully:

1. **Analyze Data**: Open speed_log.csv in Excel/Sheets
2. **Customize**: Modify test interval and window size
3. **Enhance**: Add features like alerts, email notifications
4. **Extend**: Create web dashboard for historical data
5. **Optimize**: Improve speed test accuracy with multiple servers

## 12. Contact/Support

For issues or questions about the code, refer to README.md for detailed documentation.

---

**Happy Speed Monitoring!** 🚀
