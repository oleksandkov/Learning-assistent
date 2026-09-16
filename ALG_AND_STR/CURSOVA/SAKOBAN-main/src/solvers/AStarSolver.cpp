#include "AStarSolver.hpp"
#include "DeadlockDetector.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>
#include <queue>

namespace sokoban::solvers {

namespace {
constexpr core::Direction SEARCH_DIRECTIONS[] = {
    core::Direction::Up,
    core::Direction::Left,
    core::Direction::Down,
    core::Direction::Right
};
} // namespace

void AStarSolver::start(const core::Board& board,
                        const core::GameState& state,
                        const SolverOptions& options) {
    auto prepStart = std::chrono::steady_clock::now();

    board_ = &board;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::AStar;
    stats_.metric = options.metric;
    stats_.status = SearchStatus::NotStarted;
    solution_.reset();
    debugState_.reset();
    cancelRequested_.store(false);
    sequenceCounter_ = 0;

    // Clear data structures
    while (!openQueue_.empty()) {
        openQueue_.pop();
    }
    nodes_.clear();
    bestG_.clear();
    heuristicCache_.clear();

    zobrist_ = std::make_unique<Zobrist>(board_->size());

    auto prepEnd = std::chrono::steady_clock::now();
    stats_.preprocessingTime = std::chrono::duration_cast<std::chrono::nanoseconds>(prepEnd - prepStart);

    // Initial check
    if (state.isGoal(*board_)) {
        solution_ = Solution{
            .moves = {},
            .moveCount = 0,
            .pushCount = 0,
            .optimizedFor = options_.metric
        };
        stats_.status = SearchStatus::Solved;
        stats_.totalSolverTime = stats_.preprocessingTime;
        return;
    }

    if (options_.enableDeadlockDetection && DeadlockDetector::isDeadlock(*board_, state, core::INVALID_CELL, true)) {
        stats_.status = SearchStatus::NoSolution;
        stats_.totalSolverTime = stats_.preprocessingTime;
        return;
    }

    std::uint64_t h0 = computeHeuristic(state.boxes);
    if (h0 >= Hungarian::INF_COST) {
        stats_.status = SearchStatus::NoSolution;
        stats_.totalSolverTime = stats_.preprocessingTime;
        return;
    }

    core::GameState canonicalStart = state;
    if (options_.metric == OptimizationMetric::Pushes) {
        ReachabilityResult reach = computeReachability(state);
        canonicalStart.player = reach.minCell;
    }

    AStarNode root{
        .state = state, // exact physical initial position
        .parentIndex = INVALID_NODE,
        .edge = {},
        .g = 0,
        .h = h0,
        .pushCount = 0
    };

    nodes_.push_back(std::move(root));
    bestG_.emplace(canonicalStart, 0);

    openQueue_.push(OpenEntry{
        .f = h0,
        .g = 0,
        .h = h0,
        .seq = ++sequenceCounter_,
        .nodeIndex = 0
    });

    stats_.maxFrontierSize = 1;
    stats_.generatedStates = 1;
    stats_.status = SearchStatus::Running;

    if (options_.collectDebugState) {
        debugState_ = SolverDebugState{
            .state = state,
            .g = 0,
            .h = h0,
            .depth = 0
        };
    }

    updatePeakMemory();
    searchStartTime_ = std::chrono::steady_clock::now();
}

AStarSolver::ReachabilityResult AStarSolver::computeReachability(const core::GameState& state) const {
    ReachabilityResult res;
    res.reachable.assign(board_->size(), false);
    res.dist.assign(board_->size(), core::Board::INF_DISTANCE);
    res.parentCell.assign(board_->size(), core::INVALID_CELL);
    res.moveFromParent.assign(board_->size(), core::Direction::Up);
    res.minCell = state.player;

    if (state.player >= board_->size() || !board_->isFloor(state.player)) {
        return res;
    }

    std::queue<core::CellIndex> q;
    res.reachable[state.player] = true;
    res.dist[state.player] = 0;
    res.parentCell[state.player] = state.player;
    q.push(state.player);

    while (!q.empty()) {
        core::CellIndex curr = q.front();
        q.pop();

        res.minCell = std::min(res.minCell, curr);

        for (core::Direction dir : SEARCH_DIRECTIONS) {
            core::CellIndex next = board_->neighbor(curr, dir);
            if (next != core::INVALID_CELL && board_->isFloor(next) && !state.hasBox(next)) {
                if (!res.reachable[next]) {
                    res.reachable[next] = true;
                    res.dist[next] = res.dist[curr] + 1;
                    res.parentCell[next] = curr;
                    res.moveFromParent[next] = dir;
                    q.push(next);
                }
            }
        }
    }

    return res;
}

std::uint64_t AStarSolver::computeHeuristic(const std::vector<core::CellIndex>& boxes) {
    auto it = heuristicCache_.find(boxes);
    if (it != heuristicCache_.end()) {
        return it->second;
    }

    std::size_t K = boxes.size();
    if (K == 0) {
        return 0;
    }

    std::vector<std::vector<std::int64_t>> costMatrix(K, std::vector<std::int64_t>(K));
    for (std::size_t i = 0; i < K; ++i) {
        for (std::size_t j = 0; j < K; ++j) {
            std::uint32_t d = board_->reversePushDistance(j, boxes[i]);
            costMatrix[i][j] = (d == core::Board::INF_DISTANCE) ? Hungarian::INF_COST : static_cast<std::int64_t>(d);
        }
    }

    auto [totalCost, _] = Hungarian::solve(costMatrix);
    std::uint64_t hVal = (totalCost >= Hungarian::INF_COST) ? Hungarian::INF_COST : static_cast<std::uint64_t>(totalCost);

    heuristicCache_.emplace(boxes, hVal);
    return hVal;
}

SearchStatus AStarSolver::advance(std::size_t nodeBudget) {
    if (stats_.status != SearchStatus::Running) {
        return stats_.status;
    }

    auto stepStart = std::chrono::steady_clock::now();

    while (nodeBudget > 0 && !openQueue_.empty()) {
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

        OpenEntry entry = openQueue_.top();
        openQueue_.pop();

        std::uint32_t currIdx = entry.nodeIndex;
        // Make copies to prevent reference invalidation when nodes_ reallocates!
        core::GameState currState = nodes_[currIdx].state;
        std::uint64_t currG = nodes_[currIdx].g;
        std::uint64_t currPushCount = nodes_[currIdx].pushCount;

        // Check if stale
        core::GameState canonicalCurr = currState;
        if (options_.metric == OptimizationMetric::Pushes) {
            ReachabilityResult reach = computeReachability(currState);
            canonicalCurr.player = reach.minCell;
        }

        auto gIt = bestG_.find(canonicalCurr);
        if (gIt != bestG_.end() && entry.g > gIt->second) {
            stats_.staleQueueEntries++;
            continue;
        }

        stats_.exploredStates++;

        if (options_.collectDebugState) {
            const auto& currentNode = nodes_[currIdx];
            debugState_ = SolverDebugState{
                .state = currState,
                .g = currentNode.g,
                .h = currentNode.h,
                .depth = currentNode.pushCount,
                .hasTransition = currentNode.parentIndex != INVALID_NODE,
                .pushed = currentNode.parentIndex != INVALID_NODE,
                .direction = currentNode.edge.steps.empty()
                    ? core::Direction::Up
                    : currentNode.edge.steps.back(),
                .boxFrom = currentNode.edge.boxFrom,
                .boxTo = currentNode.edge.boxTo
            };
        }

        if (currState.isGoal(*board_)) {
            auto stepEnd = std::chrono::steady_clock::now();
            stats_.searchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);
            finishSolved(currIdx);
            return stats_.status;
        }

        // Compute reachability for current physical state
        ReachabilityResult reach = computeReachability(currState);

        // Copy boxes to iterate safely
        std::vector<core::CellIndex> boxesToIterate = currState.boxes;

        // Generate macro-pushes
        for (core::CellIndex boxCell : boxesToIterate) {
            for (core::Direction pushDir : SEARCH_DIRECTIONS) {
                core::Direction approachDir = core::opposite(pushDir);
                core::CellIndex pushSpot = board_->neighbor(boxCell, approachDir);
                core::CellIndex boxDest = board_->neighbor(boxCell, pushDir);

                if (pushSpot == core::INVALID_CELL || !reach.reachable[pushSpot]) {
                    continue;
                }

                if (boxDest == core::INVALID_CELL || !board_->isFloor(boxDest) || currState.hasBox(boxDest)) {
                    continue;
                }

                // Construct next physical state: player lands on boxCell
                core::GameState nextState = currState;
                nextState.player = boxCell;
                nextState.moveBox(boxCell, boxDest);

                stats_.generatedStates++;

                // Deadlock check
                if (options_.enableDeadlockDetection) {
                    if (DeadlockDetector::isDeadlock(*board_, nextState, boxDest)) {
                        stats_.deadlockPruned++;
                        continue;
                    }
                }

                // Edge cost
                std::uint64_t edgeCost = 0;
                if (options_.metric == OptimizationMetric::Pushes) {
                    edgeCost = 1;
                } else {
                    edgeCost = static_cast<std::uint64_t>(reach.dist[pushSpot]) + 1;
                }

                std::uint64_t tentative_g = currG + edgeCost;

                // Canonical state for closed set lookup
                core::GameState canonicalNext = nextState;
                if (options_.metric == OptimizationMetric::Pushes) {
                    ReachabilityResult nextReach = computeReachability(nextState);
                    canonicalNext.player = nextReach.minCell;
                }

                auto bestIt = bestG_.find(canonicalNext);
                if (bestIt == bestG_.end() || tentative_g < bestIt->second) {
                    if (bestIt != bestG_.end()) {
                        stats_.reopenedStates++;
                    }

                    std::uint64_t h = computeHeuristic(nextState.boxes);
                    if (h >= Hungarian::INF_COST) {
                        stats_.deadlockPruned++;
                        continue;
                    }

                    bestG_[canonicalNext] = tentative_g;

                    // Reconstruct path segment from physical currState.player to pushSpot, plus pushDir
                    std::vector<core::Direction> segment;
                    core::CellIndex walkCurr = pushSpot;
                    while (walkCurr != currState.player && walkCurr != core::INVALID_CELL) {
                        segment.push_back(reach.moveFromParent[walkCurr]);
                        walkCurr = reach.parentCell[walkCurr];
                    }
                    std::reverse(segment.begin(), segment.end());
                    segment.push_back(pushDir); // the push itself

                    MacroEdge edge{
                        .steps = std::move(segment),
                        .boxFrom = boxCell,
                        .boxTo = boxDest
                    };

                    std::uint32_t nextNodeIdx = static_cast<std::uint32_t>(nodes_.size());
                    AStarNode nextNode{
                        .state = std::move(nextState), // stores true physical player position
                        .parentIndex = currIdx,
                        .edge = std::move(edge),
                        .g = tentative_g,
                        .h = h,
                        .pushCount = currPushCount + 1
                    };

                    nodes_.push_back(std::move(nextNode));

                    openQueue_.push(OpenEntry{
                        .f = tentative_g + h,
                        .g = tentative_g,
                        .h = h,
                        .seq = ++sequenceCounter_,
                        .nodeIndex = nextNodeIdx
                    });

                    stats_.maxFrontierSize = std::max(stats_.maxFrontierSize, static_cast<std::uint64_t>(openQueue_.size()));
                } else {
                    stats_.duplicatePruned++;
                }
            }
        }

        nodeBudget--;
    }

    auto stepEnd = std::chrono::steady_clock::now();
    stats_.searchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);

    updatePeakMemory();

    if (stats_.status == SearchStatus::Running && openQueue_.empty()) {
        stats_.status = SearchStatus::NoSolution;
        stats_.totalSolverTime = stats_.preprocessingTime + stats_.searchTime;
    }

    return stats_.status;
}

void AStarSolver::finishSolved(std::uint32_t goalNodeIndex) {
    auto reconStart = std::chrono::steady_clock::now();

    std::vector<MacroEdge> macroPath;
    std::uint32_t currIdx = goalNodeIndex;

    while (currIdx != INVALID_NODE && nodes_[currIdx].parentIndex != INVALID_NODE) {
        macroPath.push_back(nodes_[currIdx].edge);
        currIdx = nodes_[currIdx].parentIndex;
    }

    std::reverse(macroPath.begin(), macroPath.end());

    std::vector<core::Direction> fullMoves;
    std::uint64_t totalPushes = 0;

    for (const auto& mEdge : macroPath) {
        for (core::Direction d : mEdge.steps) {
            fullMoves.push_back(d);
        }
        totalPushes++;
    }

    auto reconEnd = std::chrono::steady_clock::now();
    stats_.reconstructionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(reconEnd - reconStart);

    Solution sol{
        .moves = std::move(fullMoves),
        .moveCount = 0,
        .pushCount = totalPushes,
        .optimizedFor = options_.metric
    };
    sol.moveCount = static_cast<std::uint64_t>(sol.moves.size());

    // Replay validation against initial state
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

void AStarSolver::updatePeakMemory() {
    std::size_t nodeBytes = nodes_.size() * sizeof(AStarNode);
    std::size_t queueBytes = openQueue_.size() * sizeof(OpenEntry);
    std::size_t bestGBytes = bestG_.size() * (sizeof(core::GameState) + sizeof(std::uint64_t) + 32);
    std::size_t cacheBytes = heuristicCache_.size() * (sizeof(std::vector<core::CellIndex>) + sizeof(std::uint64_t) + 32);
    std::uint64_t total = static_cast<std::uint64_t>(nodeBytes + queueBytes + bestGBytes + cacheBytes);
    stats_.estimatedPeakBytes = std::max(stats_.estimatedPeakBytes, total);
}

void AStarSolver::requestCancel() {
    cancelRequested_.store(true, std::memory_order_relaxed);
}

SolverProgress AStarSolver::progress() const {
    return SolverProgress{
        .exploredStates = stats_.exploredStates,
        .frontierSize = static_cast<std::uint64_t>(openQueue_.size()),
        .elapsedSearchTime = stats_.searchTime,
        .status = stats_.status
    };
}

std::optional<SolverDebugState> AStarSolver::debugState() const {
    return debugState_;
}

SearchStatistics AStarSolver::statistics() const {
    return stats_;
}

std::optional<Solution> AStarSolver::solution() const {
    return solution_;
}

} // namespace sokoban::solvers
