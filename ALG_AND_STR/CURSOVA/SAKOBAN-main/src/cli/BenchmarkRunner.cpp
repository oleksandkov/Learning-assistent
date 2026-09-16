#include "BenchmarkRunner.hpp"
#include "../core/LevelParser.hpp"
#include "../solvers/BFSSolver.hpp"
#include "../solvers/AStarSolver.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace sokoban::cli {

namespace {
double toMs(std::chrono::nanoseconds ns) {
    return static_cast<double>(ns.count()) / 1'000'000.0;
}

template <typename T>
T median(std::vector<T> v) {
    if (v.empty()) return T{};
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}
} // namespace

int BenchmarkRunner::run(const BenchmarkOptions& options) {
    std::vector<std::string> allFiles;
    for (const auto& pathStr : options.levelPaths) {
        namespace fs = std::filesystem;
        if (fs::is_directory(pathStr)) {
            for (const auto& entry : fs::directory_iterator(pathStr)) {
                if (entry.is_regular_file() && entry.path().extension() == ".xsb") {
                    allFiles.push_back(entry.path().string());
                }
            }
        } else if (fs::is_regular_file(pathStr)) {
            allFiles.push_back(pathStr);
        } else {
            std::cerr << "Warning: path not found or not regular file/dir: " << pathStr << "\n";
        }
    }

    std::sort(allFiles.begin(), allFiles.end());

    if (allFiles.empty()) {
        std::cerr << "No .xsb level files found to benchmark.\n";
        return 1;
    }

    std::vector<BenchmarkRecord> records;

    for (const auto& file : allFiles) {
        if (options.runBFS) {
            records.push_back(benchmarkLevel(file,
                                             solvers::SolverKind::BFS,
                                             solvers::OptimizationMetric::Moves,
                                             options.repeats,
                                             options.timeLimit,
                                             options.nodeLimit));
        }
        if (options.runAStarMoves) {
            records.push_back(benchmarkLevel(file,
                                             solvers::SolverKind::AStar,
                                             solvers::OptimizationMetric::Moves,
                                             options.repeats,
                                             options.timeLimit,
                                             options.nodeLimit));
        }
        if (options.runAStarPushes) {
            records.push_back(benchmarkLevel(file,
                                             solvers::SolverKind::AStar,
                                             solvers::OptimizationMetric::Pushes,
                                             options.repeats,
                                             options.timeLimit,
                                             options.nodeLimit));
        }
    }

    std::ostream* out = &std::cout;
    std::ofstream fileOut;
    if (!options.outputFile.empty()) {
        fileOut.open(options.outputFile);
        if (fileOut.is_open()) {
            out = &fileOut;
        } else {
            std::cerr << "Could not open output file: " << options.outputFile << ", writing to stdout.\n";
        }
    }

    switch (options.format) {
        case OutputFormat::Table:
            printTable(records, *out);
            break;
        case OutputFormat::CSV:
            printCSV(records, *out);
            break;
        case OutputFormat::JSON:
            printJSON(records, *out);
            break;
    }

    return 0;
}

BenchmarkRecord BenchmarkRunner::benchmarkLevel(const std::string& levelPath,
                                                solvers::SolverKind kind,
                                                solvers::OptimizationMetric metric,
                                                int repeats,
                                                std::chrono::milliseconds timeLimit,
                                                std::size_t nodeLimit) {
    BenchmarkRecord rec;
    rec.levelName = std::filesystem::path(levelPath).filename().string();
    rec.algorithm = (kind == solvers::SolverKind::BFS) ? "BFS" : "A*";
    rec.metric = (metric == solvers::OptimizationMetric::Moves) ? "Moves" : "Pushes";

    core::ParsedLevel parsed;
    try {
        parsed = core::LevelParser::parseFile(levelPath);
    } catch (const std::exception& e) {
        rec.status = "InvalidLevel";
        return rec;
    }

    solvers::SolverOptions opt{
        .algorithm = kind,
        .metric = metric,
        .timeLimit = timeLimit,
        .nodeLimit = nodeLimit,
        .memoryLimitBytes = 512 * 1024 * 1024,
        .enableDeadlockDetection = true,
        .enableSafeMode = true
    };

    auto runOnce = [&]() -> solvers::SearchStatistics {
        std::unique_ptr<solvers::ISolver> solver;
        if (kind == solvers::SolverKind::BFS) {
            solver = std::make_unique<solvers::BFSSolver>();
        } else {
            solver = std::make_unique<solvers::AStarSolver>();
        }

        solver->start(parsed.board, parsed.initialState, opt);
        while (solver->advance(10000) == solvers::SearchStatus::Running) {}
        return solver->statistics();
    };

    // Warm-up run (not included in stats)
    runOnce();

    std::vector<double> prepTimes;
    std::vector<double> searchTimes;
    std::vector<double> reconTimes;
    std::vector<double> valTimes;
    std::vector<double> totalTimes;

    solvers::SearchStatistics lastStats;

    for (int r = 0; r < repeats; ++r) {
        lastStats = runOnce();
        prepTimes.push_back(toMs(lastStats.preprocessingTime));
        searchTimes.push_back(toMs(lastStats.searchTime));
        reconTimes.push_back(toMs(lastStats.reconstructionTime));
        valTimes.push_back(toMs(lastStats.validationTime));
        totalTimes.push_back(toMs(lastStats.totalSolverTime));
    }

    rec.status = solvers::statusToString(lastStats.status);
    rec.moves = lastStats.solutionMoves;
    rec.pushes = lastStats.solutionPushes;
    rec.generated = lastStats.generatedStates;
    rec.explored = lastStats.exploredStates;
    rec.duplicatePruned = lastStats.duplicatePruned;
    rec.deadlockPruned = lastStats.deadlockPruned;
    rec.maxFrontier = lastStats.maxFrontierSize;
    rec.peakMemoryBytes = lastStats.estimatedPeakBytes;

    rec.medianPreprocessingMs = median(prepTimes);
    rec.medianSearchMs = median(searchTimes);
    rec.medianReconstructionMs = median(reconTimes);
    rec.medianValidationMs = median(valTimes);
    rec.medianTotalMs = median(totalTimes);

    return rec;
}

void BenchmarkRunner::printTable(const std::vector<BenchmarkRecord>& records, std::ostream& out) {
    out << std::left << std::setw(20) << "Level"
        << std::setw(8)  << "Algo"
        << std::setw(8)  << "Metric"
        << std::setw(14) << "Status"
        << std::right
        << std::setw(7)  << "Moves"
        << std::setw(7)  << "Push"
        << std::setw(10) << "Explored"
        << std::setw(9)  << "Deadlock"
        << std::setw(11) << "Search(ms)"
        << std::setw(11) << "Total(ms)"
        << std::setw(11) << "PeakMem(KB)"
        << "\n";

    out << std::string(106, '-') << "\n";

    for (const auto& r : records) {
        out << std::left << std::setw(20) << r.levelName
            << std::setw(8)  << r.algorithm
            << std::setw(8)  << r.metric
            << std::setw(14) << r.status
            << std::right
            << std::setw(7)  << r.moves
            << std::setw(7)  << r.pushes
            << std::setw(10) << r.explored
            << std::setw(9)  << r.deadlockPruned
            << std::fixed << std::setprecision(3)
            << std::setw(11) << r.medianSearchMs
            << std::setw(11) << r.medianTotalMs
            << std::setw(11) << (r.peakMemoryBytes / 1024)
            << "\n";
    }
}

void BenchmarkRunner::printCSV(const std::vector<BenchmarkRecord>& records, std::ostream& out) {
    out << "Level,Algorithm,Metric,Status,Moves,Pushes,Generated,Explored,DuplicatePruned,DeadlockPruned,MaxFrontier,PrepMs,SearchMs,ReconMs,ValMs,TotalMs,PeakMemBytes\n";
    for (const auto& r : records) {
        out << r.levelName << ","
            << r.algorithm << ","
            << r.metric << ","
            << r.status << ","
            << r.moves << ","
            << r.pushes << ","
            << r.generated << ","
            << r.explored << ","
            << r.duplicatePruned << ","
            << r.deadlockPruned << ","
            << r.maxFrontier << ","
            << std::fixed << std::setprecision(4)
            << r.medianPreprocessingMs << ","
            << r.medianSearchMs << ","
            << r.medianReconstructionMs << ","
            << r.medianValidationMs << ","
            << r.medianTotalMs << ","
            << r.peakMemoryBytes << "\n";
    }
}

void BenchmarkRunner::printJSON(const std::vector<BenchmarkRecord>& records, std::ostream& out) {
    out << "[\n";
    for (std::size_t i = 0; i < records.size(); ++i) {
        const auto& r = records[i];
        out << "  {\n"
            << "    \"level\": \"" << r.levelName << "\",\n"
            << "    \"algorithm\": \"" << r.algorithm << "\",\n"
            << "    \"metric\": \"" << r.metric << "\",\n"
            << "    \"status\": \"" << r.status << "\",\n"
            << "    \"moves\": " << r.moves << ",\n"
            << "    \"pushes\": " << r.pushes << ",\n"
            << "    \"generated\": " << r.generated << ",\n"
            << "    \"explored\": " << r.explored << ",\n"
            << "    \"duplicatePruned\": " << r.duplicatePruned << ",\n"
            << "    \"deadlockPruned\": " << r.deadlockPruned << ",\n"
            << "    \"maxFrontier\": " << r.maxFrontier << ",\n"
            << "    \"medianPreprocessingMs\": " << r.medianPreprocessingMs << ",\n"
            << "    \"medianSearchMs\": " << r.medianSearchMs << ",\n"
            << "    \"medianReconstructionMs\": " << r.medianReconstructionMs << ",\n"
            << "    \"medianValidationMs\": " << r.medianValidationMs << ",\n"
            << "    \"medianTotalMs\": " << r.medianTotalMs << ",\n"
            << "    \"peakMemoryBytes\": " << r.peakMemoryBytes << "\n"
            << "  }" << (i + 1 < records.size() ? "," : "") << "\n";
    }
    out << "]\n";
}

} // namespace sokoban::cli
