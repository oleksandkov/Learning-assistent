#pragma once

#include "../core/Board.hpp"
#include "../core/GameState.hpp"
#include "../solvers/ISolver.hpp"
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include <ostream>

namespace sokoban::cli {

struct AlgorithmComparisonResult {
    std::string algorithmName;              // "BFS", "A* (Moves)", "A* (Pushes)", "AI"
    solvers::SolverKind kind;
    solvers::OptimizationMetric metric;
    std::string metricLabel;                // непорожній = показати замість Moves/Pushes ("AI-план")
    solvers::SearchStatus status;
    std::uint64_t moves = 0;
    std::uint64_t pushes = 0;
    std::uint64_t exploredStates = 0;       // розкриті вузли
    std::uint64_t generatedStates = 0;
    std::uint64_t deadlockPruned = 0;
    std::chrono::nanoseconds searchTime{0}; // чистий search_time (steady_clock)
    bool isOptimal = false;                 // прапорець оптимальності
    std::string optimalityDescription;      // "Yes (Moves)", "Yes (Pushes)", "No", "N/A"
    bool replayValid = false;               // підтверджено через ReplayValidator
    std::string replayErrorMessage;
    std::optional<solvers::Solution> solution;
};

struct LevelComparisonReport {
    std::string levelName;
    int width = 0;
    int height = 0;
    int boxCount = 0;
    std::vector<AlgorithmComparisonResult> results;
};

class AlgorithmComparator {
public:
    static LevelComparisonReport compare(const core::Board& board,
                                         const core::GameState& initialState,
                                         const std::string& levelName = "Level",
                                         std::chrono::milliseconds timeLimit = std::chrono::milliseconds(30000),
                                         std::size_t nodeLimit = 1000000);

    static LevelComparisonReport compareFile(const std::string& levelPath,
                                             std::chrono::milliseconds timeLimit = std::chrono::milliseconds(30000),
                                             std::size_t nodeLimit = 1000000);

    static void printTable(const LevelComparisonReport& report,
                           std::ostream& out,
                           bool ansiSupported = true,
                           int width = 0); // width>0 і <105 = компактна таблиця для вузької консолі

    // Ширина консолі, або 0 якщо вивід не термінал (pipe/файл) чи невідома.
    static int terminalWidth();
};

} // namespace sokoban::cli
