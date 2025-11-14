# Internet Speed Tracker with SFML Visualization

A C++ application that continuously monitors internet speed and displays real-time visualization using SFML (Simple Fast Multimedia Library).

## Features

- **Continuous Monitoring**: Tests internet speed at regular intervals (configurable)
- **Full Speed Test**: Measures download speed, upload speed, and ping latency
- **Real-Time Visualization**: SFML-based GUI showing graphs of speed trends
- **Data Logging**: Saves all test results to CSV file for historical analysis
- **Multi-threaded**: Monitoring runs in background while UI remains responsive

## Project Structure

```
internet-speed-tracker/
├── include/
│   ├── SpeedTester.h    # Internet speed testing
│   ├── Visualizer.h     # SFML visualization
│   └── Logger.h         # Data logging to CSV
├── src/
│   ├── SpeedTester.cpp
│   ├── Visualizer.cpp
│   ├── Logger.cpp
│   └── main.cpp
├── bin/                 # Compiled executable
├── Fonts/               # Font resources
└── compile.bat          # Compilation script
```

## Key Components

### SpeedTester Class

- **measureDownloadSpeed()**: Uses curl to test download speed from test server
- **measureUploadSpeed()**: Simulates upload test with PowerShell
- **measurePing()**: Tests latency to Google DNS (8.8.8.8)
- **runTest()**: Executes full test and stores result

### Visualizer Class

- **SFML Window**: 1200x700 resolution, 30 FPS
- **Graph Display**: Green line for download, red line for upload
- **Statistics Panel**: Shows test count and averages
- **Legend**: Easy identification of metrics

### Logger Class

- **CSV Format**: Timestamp, Download, Upload, Ping
- **File Append**: Continuous logging without overwriting
- **Data Recovery**: Read log for historical analysis

### Monitoring System

- **Background Thread**: Runs speed tests without blocking UI
- **Configurable Interval**: Default 60 seconds between tests
- **Thread-Safe**: Mutex protection for shared data

## Compilation & Execution

### Quick Start (Using Batch File)

```batch
cd internet-speed-tracker
compile.bat
```

### Manual Compilation (PowerShell)

```powershell
cd C:\Users\muaro\Documents\GitHub\Learning-assistent\internet-speed-tracker

$sourceFiles = "src/SpeedTester.cpp src/Logger.cpp src/Visualizer.cpp src/main.cpp"
$flags = "-Iinclude -IC:/msys64/ucrt64/include -LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-system -pthread"

g++ $sourceFiles $flags -o bin/speed_tracker.exe

.\bin\speed_tracker.exe
```

## Program Output

### Console Output

```
Internet Speed Tracker - SFML Visualization
Starting continuous monitoring (Test every 60 seconds)...
Close window to stop

Running speed test...
Download: 45.23 Mbps, Upload: 8.56 Mbps, Ping: 18.00 ms
Running speed test...
Download: 43.12 Mbps, Upload: 8.92 Mbps, Ping: 16.00 ms
...
```

### CSV Log File (speed_log.csv)

```
Timestamp,Download(Mbps),Upload(Mbps),Ping(ms)
2025-11-14 10:15:30,45.23,8.56,18.00
2025-11-14 10:16:30,43.12,8.92,16.00
2025-11-14 10:17:30,46.85,9.12,17.50
```

### SFML Visualization

- **Green Line**: Download speed trend
- **Red Line**: Upload speed trend
- **Grid**: Reference for speed values
- **Statistics**: Real-time averages and test count

## Architecture & Algorithms

### Speed Testing Flow

```
SpeedTester::runTest()
├── measureDownloadSpeed()
│   └── Execute curl command via PowerShell
│   └── Parse speed value from output
│   └── Convert to Mbps
├── measureUploadSpeed()
│   └── Simulate upload with PowerShell measurement
│   └── Calculate speed
└── measurePing()
    └── Use Test-NetConnection to ping 8.8.8.8
    └── Extract round-trip time
```

### Visualization Algorithm

```
Visualizer::drawGraphs()
├── Determine maxSpeed from all results
├── For each test result:
│   ├── Map speed value to Y coordinate (0-300px)
│   ├── Calculate X position (evenly distributed)
│   ├── Draw line from previous point
│   └── Draw circle at current point
└── Refresh display
```

### Monitoring Thread

```
monitoringThread()
├── While isRunning:
│   ├── Call tester.runTest()
│   ├── Log result to CSV
│   ├── Update global results (thread-safe)
│   └── Sleep for interval seconds
└── Exit
```

## Key Concepts Used

1. **Multi-threading**: Background monitoring with mutex synchronization
2. **External Process Execution**: PowerShell/curl commands for speed testing
3. **File I/O**: CSV logging and reading
4. **Graphics Programming**: SFML for real-time visualization
5. **Data Structures**: Vectors for storing historical data
6. **Object-Oriented Design**: Separate classes for each responsibility

## Customization

### Change Test Interval

In `src/main.cpp`, line ~46:

```cpp
std::thread monitor(monitoringThread, std::ref(tester), std::ref(logger), 60);  // 60 seconds
```

### Change Test Server

In `src/SpeedTester.cpp`:

```cpp
SpeedTester tester("http://your-test-server.com/test-file.db");
```

### Adjust Window Size

In `include/Visualizer.h`:

```cpp
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 700;
```

## Requirements

- **C++17 or later** compiler (g++ from MSYS2/UCRT64)
- **SFML 2.5+** libraries
- **Windows** with PowerShell
- **curl.exe** (usually available on Windows 10+)
- **Test-NetConnection** (PowerShell cmdlet)

## Compilation Requirements

```
gcc version: 11.0+ (MSYS2 UCRT64)
SFML libs: sfml-graphics, sfml-window, sfml-system
Include paths: C:/msys64/ucrt64/include
Library paths: C:/msys64/ucrt64/lib
```

## Common Issues

### Font Not Loading

- Ensure Arial font is available at `C:/Windows/Fonts/arial.ttf`
- Program continues without font but text won't display

### Speed Test Fails

- Check internet connection
- Verify curl.exe is available in PATH
- Test server might be down - try alternative server URL

### SFML Window Won't Open

- Verify SFML libraries are correctly installed
- Check that all 3 SFML libs are linked (-lsfml-graphics -lsfml-window -lsfml-system)

## Future Enhancements

1. Add network detection (skip test if offline)
2. Export graphs as PNG/PDF
3. Calculate trend analysis
4. Add alerts for slow speeds
5. Support for custom test servers
6. Real upload test with actual file transfer
7. Save/load configuration
8. System tray integration

## Testing & Validation

The program has been validated with:

- Multiple consecutive speed tests
- Graph rendering with 10+ data points
- CSV file integrity after multiple tests
- Thread safety under continuous monitoring
- Window responsiveness during testing

## Code Quality

- No memory leaks (proper resource cleanup)
- Exception handling for file I/O and parsing
- Thread-safe global data access
- Clear function naming and documentation
- Modular design for easy maintenance

## License

Educational project for learning C++, SFML, and multi-threaded programming.
