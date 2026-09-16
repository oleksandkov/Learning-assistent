#pragma once

#include "../solvers/ISolver.hpp"
#include <string>
#include <vector>
#include <ostream>

namespace sokoban::cli {

enum class OutputFormat {
    Table,
    CSV,
    JSON
};

struct BenchmarkOptions {
    std::vector<std::string> levelPaths;
    int repeats = 5;
    bool runBFS = true;
    bool runAStarMoves = true;
    bool runAStarPushes = true;
    bool runIDAStar = true;
    bool runGreedy = true;
    std::chrono::milliseconds timeLimit{30000};
    std::size_t nodeLimit{1000000};
    OutputFormat format = OutputFormat::Table;
    std::string outputFile;
};

struct BenchmarkRecord {
    std::string levelName;
    std::string algorithm;
    std::string metric;
    std::string status;
    std::uint64_t moves = 0;
    std::uint64_t pushes = 0;
    std::uint64_t generated = 0;
    std::uint64_t explored = 0;
    std::uint64_t duplicatePruned = 0;
    std::uint64_t deadlockPruned = 0;
    std::uint64_t maxFrontier = 0;
    double medianPreprocessingMs = 0.0;
    double medianSearchMs = 0.0;
    double medianReconstructionMs = 0.0;
    double medianValidationMs = 0.0;
    double medianTotalMs = 0.0;
    std::uint64_t peakMemoryBytes = 0;
};

class BenchmarkRunner {
public:
    static int run(const BenchmarkOptions& options);

private:
    static BenchmarkRecord benchmarkLevel(const std::string& levelPath,
                                          solvers::SolverKind kind,
                                          solvers::OptimizationMetric metric,
                                          int repeats,
                                          std::chrono::milliseconds timeLimit,
                                          std::size_t nodeLimit);

    static void printTable(const std::vector<BenchmarkRecord>& records, std::ostream& out);
    static void printCSV(const std::vector<BenchmarkRecord>& records, std::ostream& out);
    static void printJSON(const std::vector<BenchmarkRecord>& records, std::ostream& out);
};

} // namespace sokoban::cli
