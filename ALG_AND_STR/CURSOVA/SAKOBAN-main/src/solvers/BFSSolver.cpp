#include "BFSSolver.hpp"
#include "DeadlockDetector.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>

namespace sokoban::solvers {

namespace {
constexpr core::Direction SEARCH_DIRECTIONS[] = {
    core::Direction::Up,
    core::Direction::Left,
    core::Direction::Down,
    core::Direction::Right
};
} // namespace

void BFSSolver::start(const core::Board& board,
                      const core::GameState& state,
                      const SolverOptions& options) {
    auto prepStart = std::chrono::steady_clock::now();

    board_ = &board;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::BFS;
    stats_.metric = OptimizationMetric::Moves;
    stats_.status = SearchStatus::NotStarted;
    solution_.reset();
    debugState_.reset();
    cancelRequested_.store(false);

    queue_.clear();
    nodes_.clear();
    visited_.clear();

    auto prepEnd = std::chrono::steady_clock::now();
    stats_.preprocessingTime = std::chrono::duration_cast<std::chrono::nanoseconds>(prepEnd - prepStart);

    // Initial check
    if (state.isGoal(*board_)) {
        solution_ = Solution{
            .moves = {},
            .moveCount = 0,
            .pushCount = 0,
            .optimizedFor = OptimizationMetric::Moves
        };
        stats_.status = SearchStatus::Solved;
        stats_.totalSolverTime = stats_.preprocessingTime;
        return;
    }

    if (options_.enableDeadlockDetection && DeadlockDetector::isDeadlock(*board_, state)) {
        stats_.status = SearchStatus::NoSolution;
        stats_.totalSolverTime = stats_.preprocessingTime;
        return;
    }

    // Root node
    BFSNode root{
        .state = state,
        .parentIndex = INVALID_NODE,
        .move = core::Direction::Up,
        .depth = 0,
        .pushed = false
    };

    nodes_.push_back(std::move(root));
    queue_.push_back(0);
    visited_.emplace(state, 0);

    stats_.maxFrontierSize = 1;
    stats_.generatedStates = 1;
    stats_.status = SearchStatus::Running;

    if (options_.collectDebugState) {
        debugState_ = SolverDebugState{.state = state};
    }

    updatePeakMemory();
    searchStartTime_ = std::chrono::steady_clock::now();
}

SearchStatus BFSSolver::advance(std::size_t nodeBudget) {
    if (stats_.status != SearchStatus::Running) {
        return stats_.status;
    }

    auto stepStart = std::chrono::steady_clock::now();

    while (nodeBudget > 0 && !queue_.empty()) {
        if (cancelRequested_.load(std::memory_order_relaxed)) {
            stats_.status = SearchStatus::Cancelled;
            break;
        }

        auto now = std::chrono::steady_clock::now();
        auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - searchStartTime_);
        if (elapsedMs >= options_.timeLimit) {
            stats_.status = SearchStatus::LimitReached;
            break;
        }

        if (stats_.exploredStates >= options_.nodeLimit) {
            stats_.status = SearchStatus::LimitReached;
            break;
        }

        if (stats_.estimatedPeakBytes >= options_.memoryLimitBytes) {
            stats_.status = SearchStatus::LimitReached;
            break;
        }

        std::uint32_t currIdx = queue_.front();
        queue_.pop_front();
        stats_.exploredStates++;

        core::GameState currState = nodes_[currIdx].state;
        std::uint32_t currDepth = nodes_[currIdx].depth;

        if (options_.collectDebugState) {
            const auto& currentNode = nodes_[currIdx];
            SolverDebugState frame{
                .state = currState,
                .g = currDepth,
                .h = 0,
                .depth = currDepth,
                .hasTransition = currentNode.parentIndex != INVALID_NODE,
                .pushed = currentNode.pushed,
                .direction = currentNode.move
            };
            if (currentNode.pushed && currentNode.parentIndex != INVALID_NODE) {
                const auto& parentBoxes = nodes_[currentNode.parentIndex].state.boxes;
                for (core::CellIndex box : parentBoxes) {
                    if (!std::binary_search(currState.boxes.begin(), currState.boxes.end(), box)) {
                        frame.boxFrom = box;
                        break;
                    }
                }
                for (core::CellIndex box : currState.boxes) {
                    if (!std::binary_search(parentBoxes.begin(), parentBoxes.end(), box)) {
                        frame.boxTo = box;
                        break;
                    }
                }
            }
            debugState_ = std::move(frame);
        }

        if (currState.isGoal(*board_)) {
            auto stepEnd = std::chrono::steady_clock::now();
            stats_.searchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);
            finishSolved(currIdx);
            return stats_.status;
        }

        for (core::Direction dir : SEARCH_DIRECTIONS) {
            core::GameState nextState;
            core::MoveRecord record;
            if (!core::GameRules::tryMove(*board_, currState, dir, nextState, record, core::CommandSource::AI)) {
                continue;
            }

            stats_.generatedStates++;

            if (record.pushedBox() && options_.enableDeadlockDetection) {
                if (DeadlockDetector::isDeadlock(*board_, nextState, record.boxTo)) {
                    stats_.deadlockPruned++;
                    continue;
                }
            }

            std::uint32_t nextIdx = static_cast<std::uint32_t>(nodes_.size());
            auto [it, inserted] = visited_.emplace(nextState, nextIdx);
            if (!inserted) {
                stats_.duplicatePruned++;
                continue;
            }

            BFSNode nextNode{
                .state = std::move(nextState),
                .parentIndex = currIdx,
                .move = dir,
                .depth = currDepth + 1,
                .pushed = record.pushedBox()
            };

            nodes_.push_back(std::move(nextNode));
            queue_.push_back(nextIdx);

            stats_.maxFrontierSize = std::max(stats_.maxFrontierSize, static_cast<std::uint64_t>(queue_.size()));
        }

        nodeBudget--;
    }

    auto stepEnd = std::chrono::steady_clock::now();
    stats_.searchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);

    updatePeakMemory();

    if (stats_.status == SearchStatus::Running && queue_.empty()) {
        stats_.status = SearchStatus::NoSolution;
        stats_.totalSolverTime = stats_.preprocessingTime + stats_.searchTime;
    }

    return stats_.status;
}

void BFSSolver::finishSolved(std::uint32_t goalNodeIndex) {
    auto reconStart = std::chrono::steady_clock::now();

    std::vector<core::Direction> path;
    std::uint64_t pushCount = 0;
    std::uint32_t currIdx = goalNodeIndex;

    while (currIdx != INVALID_NODE && nodes_[currIdx].parentIndex != INVALID_NODE) {
        path.push_back(nodes_[currIdx].move);
        if (nodes_[currIdx].pushed) {
            pushCount++;
        }
        currIdx = nodes_[currIdx].parentIndex;
    }

    std::reverse(path.begin(), path.end());

    auto reconEnd = std::chrono::steady_clock::now();
    stats_.reconstructionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(reconEnd - reconStart);

    Solution sol{
        .moves = std::move(path),
        .moveCount = static_cast<std::uint64_t>(sol.moves.size()),
        .pushCount = pushCount,
        .optimizedFor = OptimizationMetric::Moves
    };
    sol.moveCount = static_cast<std::uint64_t>(sol.moves.size());

    // Validation
    auto valStart = std::chrono::steady_clock::now();
    const core::GameState& initial = nodes_[0].state;
    auto valResult = ReplayValidator::validate(*board_, initial, sol);
    auto valEnd = std::chrono::steady_clock::now();
    stats_.validationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(valEnd - valStart);

    if (valResult.valid) {
        stats_.status = SearchStatus::Solved;
        stats_.solutionMoves = sol.moveCount;
        stats_.solutionPushes = sol.pushCount;
        solution_ = std::move(sol);
    } else {
        stats_.status = SearchStatus::InternalError;
    }

    stats_.totalSolverTime = stats_.preprocessingTime + stats_.searchTime + stats_.reconstructionTime + stats_.validationTime;
}

void BFSSolver::updatePeakMemory() {
    std::size_t nodeBytes = nodes_.size() * sizeof(BFSNode);
    std::size_t queueBytes = queue_.size() * sizeof(std::uint32_t);
    std::size_t visitedBytes = visited_.size() * (sizeof(core::GameState) + sizeof(std::uint32_t) + 32);
    std::uint64_t total = static_cast<std::uint64_t>(nodeBytes + queueBytes + visitedBytes);
    stats_.estimatedPeakBytes = std::max(stats_.estimatedPeakBytes, total);
}

void BFSSolver::requestCancel() {
    cancelRequested_.store(true, std::memory_order_relaxed);
}

SolverProgress BFSSolver::progress() const {
    return SolverProgress{
        .exploredStates = stats_.exploredStates,
        .frontierSize = static_cast<std::uint64_t>(queue_.size()),
        .elapsedSearchTime = stats_.searchTime,
        .status = stats_.status
    };
}

std::optional<SolverDebugState> BFSSolver::debugState() const {
    return debugState_;
}

SearchStatistics BFSSolver::statistics() const {
    return stats_;
}

std::optional<Solution> BFSSolver::solution() const {
    return solution_;
}

} // namespace sokoban::solvers
