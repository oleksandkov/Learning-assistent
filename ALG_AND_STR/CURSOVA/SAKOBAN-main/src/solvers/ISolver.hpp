#pragma once

#include "../core/Types.hpp"
#include "../core/Board.hpp"
#include "../core/GameState.hpp"
#include <vector>
#include <chrono>
#include <optional>
#include <cstddef>
#include <cstdint>
#include <string>

namespace sokoban::solvers {

enum class SolverKind {
    BFS,
    AStar,
    IDAStar,
    Greedy,
    Prototype1,
    AntColony,
    Genetic
};

enum class OptimizationMetric {
    Moves,
    Pushes
};

enum class SearchStatus {
    NotStarted,
    Running,
    Solved,
    NoSolution,
    Cancelled,
    LimitReached,
    InvalidLevel,
    InternalError
};

inline std::string statusToString(SearchStatus s) {
    switch (s) {
        case SearchStatus::NotStarted: return "NotStarted";
        case SearchStatus::Running: return "Running";
        case SearchStatus::Solved: return "Solved";
        case SearchStatus::NoSolution: return "NoSolution";
        case SearchStatus::Cancelled: return "Cancelled";
        case SearchStatus::LimitReached: return "LimitReached";
        case SearchStatus::InvalidLevel: return "InvalidLevel";
        case SearchStatus::InternalError: return "InternalError";
    }
    return "Unknown";
}

struct SolverOptions {
    SolverKind algorithm = SolverKind::AStar;
    OptimizationMetric metric = OptimizationMetric::Pushes;
    std::chrono::milliseconds timeLimit{60000};
    std::size_t nodeLimit{1000000};
    std::size_t memoryLimitBytes{512 * 1024 * 1024};
    bool enableDeadlockDetection = true;
    bool enableSafeMode = true;
    bool collectDebugState = false;
};

struct Solution {
    std::vector<core::Direction> moves;
    std::uint64_t moveCount = 0;
    std::uint64_t pushCount = 0;
    OptimizationMetric optimizedFor = OptimizationMetric::Moves;
};

struct SearchStatistics {
    SolverKind algorithm = SolverKind::BFS;
    OptimizationMetric metric = OptimizationMetric::Moves;
    SearchStatus status = SearchStatus::NotStarted;

    std::uint64_t generatedStates = 0;
    std::uint64_t exploredStates = 0;
    std::uint64_t duplicatePruned = 0;
    std::uint64_t deadlockPruned = 0;
    std::uint64_t staleQueueEntries = 0;
    std::uint64_t reopenedStates = 0;
    std::uint64_t maxFrontierSize = 0;

    std::uint64_t solutionMoves = 0;
    std::uint64_t solutionPushes = 0;

    std::chrono::nanoseconds preprocessingTime{0};
    std::chrono::nanoseconds searchTime{0};
    std::chrono::nanoseconds reconstructionTime{0};
    std::chrono::nanoseconds validationTime{0};
    std::chrono::nanoseconds totalSolverTime{0};

    std::uint64_t estimatedPeakBytes = 0;
};

struct SolverProgress {
    std::uint64_t exploredStates = 0;
    std::uint64_t frontierSize = 0;
    std::chrono::nanoseconds elapsedSearchTime{0};
    SearchStatus status = SearchStatus::NotStarted;
};

// Optional, bounded UI/teaching telemetry. A frame is the real state selected
// from the solver frontier; normal CLI and benchmark runs leave collection off.
struct SolverDebugState {
    core::GameState state;
    std::uint64_t g = 0;
    std::uint64_t h = 0;
    std::uint64_t depth = 0;
    bool hasTransition = false;
    bool pushed = false;
    core::Direction direction = core::Direction::Up;
    core::CellIndex boxFrom = core::INVALID_CELL;
    core::CellIndex boxTo = core::INVALID_CELL;
};

class ISolver {
public:
    virtual ~ISolver() = default;

    virtual void start(
        const core::Board& board,
        const core::GameState& state,
        const SolverOptions& options) = 0;

    virtual SearchStatus advance(std::size_t nodeBudget) = 0;
    virtual void requestCancel() = 0;
    virtual SolverProgress progress() const = 0;
    virtual std::optional<SolverDebugState> debugState() const = 0;
    virtual SearchStatistics statistics() const = 0;
    virtual std::optional<Solution> solution() const = 0;
};

} // namespace sokoban::solvers
