#include "TestHarness.hpp"
#include "cli/AlgorithmComparator.hpp"
#include "core/LevelParser.hpp"
#include <sstream>
#include <filesystem>

namespace {
std::string resolveLevelPath(const std::string& rel) {
    if (std::filesystem::exists(rel)) return rel;
    if (std::filesystem::exists("../" + rel)) return "../" + rel;
    return rel;
}
} // namespace

SOKOBAN_TEST(Comparator, CompareMetricsOnSimpleLevel) {
    auto report = sokoban::cli::AlgorithmComparator::compareFile(resolveLevelPath("levels/01_simple.xsb"));

    SOKOBAN_ASSERT_EQ(report.results.size(), 5ULL);

    const auto& bfsRes = report.results[0];
    const auto& astarMovesRes = report.results[1];
    const auto& astarPushesRes = report.results[2];
    const auto& idastarRes = report.results[3];
    const auto& greedyRes = report.results[4];

    // Check algorithms and metrics
    SOKOBAN_ASSERT_EQ(bfsRes.algorithmName, "BFS");
    SOKOBAN_ASSERT(bfsRes.metric == sokoban::solvers::OptimizationMetric::Moves);
    SOKOBAN_ASSERT(bfsRes.status == sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(bfsRes.replayValid);
    SOKOBAN_ASSERT(bfsRes.isOptimal);
    SOKOBAN_ASSERT_EQ(bfsRes.optimalityDescription, "Yes (Moves)");

    SOKOBAN_ASSERT_EQ(astarMovesRes.algorithmName, "A* (Moves)");
    SOKOBAN_ASSERT(astarMovesRes.metric == sokoban::solvers::OptimizationMetric::Moves);
    SOKOBAN_ASSERT(astarMovesRes.status == sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(astarMovesRes.replayValid);
    SOKOBAN_ASSERT(astarMovesRes.isOptimal);
    SOKOBAN_ASSERT_EQ(astarMovesRes.optimalityDescription, "Yes (Moves)");

    SOKOBAN_ASSERT_EQ(astarPushesRes.algorithmName, "A* (Pushes)");
    SOKOBAN_ASSERT(astarPushesRes.metric == sokoban::solvers::OptimizationMetric::Pushes);
    SOKOBAN_ASSERT(astarPushesRes.status == sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(astarPushesRes.replayValid);
    SOKOBAN_ASSERT(astarPushesRes.isOptimal);
    SOKOBAN_ASSERT_EQ(astarPushesRes.optimalityDescription, "Yes (Pushes)");

    SOKOBAN_ASSERT_EQ(idastarRes.algorithmName, "IDA* (Pushes)");
    SOKOBAN_ASSERT(idastarRes.metric == sokoban::solvers::OptimizationMetric::Pushes);
    SOKOBAN_ASSERT(idastarRes.status == sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(idastarRes.replayValid);
    SOKOBAN_ASSERT(idastarRes.isOptimal);
    SOKOBAN_ASSERT_EQ(idastarRes.optimalityDescription, "Yes (Pushes)");
    // IDA* optimal pushes must match A* Pushes on tiny level
    SOKOBAN_ASSERT_EQ(idastarRes.pushes, astarPushesRes.pushes);

    SOKOBAN_ASSERT_EQ(greedyRes.algorithmName, "Greedy");
    SOKOBAN_ASSERT(greedyRes.metric == sokoban::solvers::OptimizationMetric::Pushes);
    SOKOBAN_ASSERT(greedyRes.status == sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(greedyRes.replayValid);
    SOKOBAN_ASSERT(!greedyRes.isOptimal);
    SOKOBAN_ASSERT_EQ(greedyRes.optimalityDescription, "No");

    // BFS and A* (Moves) must agree on the minimum number of moves
    SOKOBAN_ASSERT_EQ(bfsRes.moves, astarMovesRes.moves);

    // Explored states must be recorded
    SOKOBAN_ASSERT(bfsRes.exploredStates >= 1);
    SOKOBAN_ASSERT(astarMovesRes.exploredStates >= 1);
    SOKOBAN_ASSERT(astarPushesRes.exploredStates >= 1);
    SOKOBAN_ASSERT(idastarRes.exploredStates >= 1);
    SOKOBAN_ASSERT(greedyRes.exploredStates >= 1);

    // Pure search time must be non-negative
    SOKOBAN_ASSERT(bfsRes.searchTime.count() >= 0);
    SOKOBAN_ASSERT(astarMovesRes.searchTime.count() >= 0);
    SOKOBAN_ASSERT(astarPushesRes.searchTime.count() >= 0);
    SOKOBAN_ASSERT(idastarRes.searchTime.count() >= 0);
    SOKOBAN_ASSERT(greedyRes.searchTime.count() >= 0);
}

SOKOBAN_TEST(Comparator, CompareOnUnsolvableDeadlockLevel) {
    std::string xsb = 
        "######\n"
        "#$   #\n"
        "# @ .#\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb, "Unsolvable");
    auto report = sokoban::cli::AlgorithmComparator::compare(parsed.board, parsed.initialState, "Unsolvable");

    SOKOBAN_ASSERT_EQ(report.results.size(), 5ULL);
    for (const auto& r : report.results) {
        SOKOBAN_ASSERT(r.status == sokoban::solvers::SearchStatus::NoSolution);
        SOKOBAN_ASSERT(!r.replayValid);
        SOKOBAN_ASSERT(!r.isOptimal);
        SOKOBAN_ASSERT_EQ(r.optimalityDescription, "N/A");
    }
}

SOKOBAN_TEST(Comparator, TablePrintFormattedOutput) {
    auto report = sokoban::cli::AlgorithmComparator::compareFile(resolveLevelPath("levels/01_simple.xsb"));

    std::ostringstream ss;
    sokoban::cli::AlgorithmComparator::printTable(report, ss, false);
    std::string text = ss.str();

    SOKOBAN_ASSERT(!text.empty());
    SOKOBAN_ASSERT(text.find("BFS") != std::string::npos);
    SOKOBAN_ASSERT(text.find("A* (Moves)") != std::string::npos);
    SOKOBAN_ASSERT(text.find("A* (Pushes)") != std::string::npos);
    SOKOBAN_ASSERT(text.find("IDA*") != std::string::npos);
    SOKOBAN_ASSERT(text.find("Greedy") != std::string::npos);
    SOKOBAN_ASSERT(text.find("Yes (Moves)") != std::string::npos);
    SOKOBAN_ASSERT(text.find("Yes (Pushes)") != std::string::npos);
    SOKOBAN_ASSERT(text.find("Passed") != std::string::npos);
}
