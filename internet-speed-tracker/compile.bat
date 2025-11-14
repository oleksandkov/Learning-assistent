@echo off
REM Compile and Run Internet Speed Tracker with SFML

cd /d "%~dp0"
echo Compiling Internet Speed Tracker...

set INCLUDE_FLAGS=-Iinclude -IC:/msys64/ucrt64/include
set LIB_FLAGS=-LC:/msys64/ucrt64/lib -lsfml-graphics -lsfml-window -lsfml-system
set SRC_FILES=src/SpeedTester.cpp src/Logger.cpp src/Visualizer.cpp src/main.cpp

g++ %SRC_FILES% %INCLUDE_FLAGS% %LIB_FLAGS% -o bin/speed_tracker.exe -pthread

if %errorlevel% equ 0 (
    echo.
    echo Compilation successful!
    echo Running Internet Speed Tracker...
    echo.
    bin\speed_tracker.exe
) else (
    echo Compilation failed!
    pause
)
