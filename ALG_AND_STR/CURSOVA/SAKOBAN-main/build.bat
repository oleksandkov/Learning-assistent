@echo off
echo Compiling SOKOBAN CLI (C++20 Release)...
g++ -std=c++20 -O3 -Isrc src/core/Board.cpp src/core/GameRules.cpp src/core/GameSession.cpp src/core/GameState.cpp src/core/LevelParser.cpp src/solvers/AStarSolver.cpp src/solvers/BFSSolver.cpp src/solvers/DeadlockDetector.cpp src/solvers/Hungarian.cpp src/solvers/LevelGenerator.cpp src/solvers/ReplayValidator.cpp src/solvers/SolverRegistry.cpp src/solvers/Zobrist.cpp src/cli/AlgorithmComparator.cpp src/cli/BenchmarkRunner.cpp src/cli/AiClient.cpp src/cli/Menu.cpp src/cli/TerminalUI.cpp src/cli/main.cpp -o sokoban_cli.exe -static -static-libgcc -static-libstdc++
if %ERRORLEVEL% EQU 0 (
    echo [OK] Successfully built sokoban_cli.exe!
) else (
    echo [ERROR] Compilation failed.
)
