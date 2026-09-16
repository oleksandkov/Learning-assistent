#include "GreedySolver.hpp"
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

void GreedySolver::start(const core::Board& board,
                         const core::GameState& state,
                         const SolverOptions& options) {
    auto prepStart = std::chrono::steady_clock::now();

    board_ = &board;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::Greedy;
    stats_.metric = options.metric;
    stats_.status = SearchStatus::NotStarted;
    solution_.reset();
    debugState_.reset();
    cancelRequested_.store(false);
    sequenceCounter_ = 0;

    while (!openQueue_.empty()) {
        openQueue_.pop();
    }
    nodes_.clear();
    closed_.clear();
    heuristicCache_.clear();

    auto prepEnd = std::chrono::steady_clock::now();
    stats_.preprocessingTime = std::chrono::duration_cast<std::chrono::nanoseconds>(prepEnd - prepStart);

    if (state.isGoal(*board_)) {
        solution_ = Solution{.moves = {}, .moveCount = 0, .pushCount = 0, .optimizedFor = options_.metric};
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

    nodes_.push_back(GreedyNode{.state = state, .parentIndex = INVALID_NODE, .edge = {}, .g = 0, .h = h0});
    closed_.insert(state);
    openQueue_.push(OpenEntry{.h = h0, .seq = ++sequenceCounter_, .nodeIndex = 0});

    stats_.maxFrontierSize = 1;
    stats_.generatedStates = 1;
    stats_.status = SearchStatus::Running;

    if (options_.collectDebugState) {
        debugState_ = SolverDebugState{.state = state, .g = 0, .h = h0, .depth = 0};
    }

    updatePeakMemory();
    searchStartTime_ = std::chrono::steady_clock::now();
}

GreedySolver::ReachabilityResult GreedySolver::computeReachability(const core::GameState& state) const {
    ReachabilityResult res;
    res.reachable.assign(board_->size(), false);
    res.dist.assign(board_->size(), core::Board::INF_DISTANCE);
    res.parentCell.assign(board_->size(), core::INVALID_CELL);
    res.moveFromParent.assign(board_->size(), core::Direction::Up);

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

std::uint64_t GreedySolver::computeHeuristic(const std::vector<core::CellIndex>& boxes) {
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

SearchStatus GreedySolver::advance(std::size_t nodeBudget) {
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
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - searchStartTime_) >= options_.timeLimit) {
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
        core::GameState currState = nodes_[currIdx].state;
        std::uint64_t currG = nodes_[currIdx].g;
        stats_.exploredStates++;

        if (options_.collectDebugState) {
            const auto& node = nodes_[currIdx];
            debugState_ = SolverDebugState{.state = currState, .g = node.g, .h = node.h,
                                           .depth = node.g,
                                           .hasTransition = node.parentIndex != INVALID_NODE,
                                           .pushed = node.parentIndex != INVALID_NODE,
                                           .direction = node.edge.steps.empty() ? core::Direction::Up : node.edge.steps.back(),
                                           .boxFrom = node.edge.boxFrom,
                                           .boxTo = node.edge.boxTo};
        }

        if (currState.isGoal(*board_)) {
            auto stepEnd = std::chrono::steady_clock::now();
            stats_.searchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);
            finishSolved(currIdx);
            return stats_.status;
        }

        ReachabilityResult reach = computeReachability(currState);
        std::vector<core::CellIndex> boxesToIterate = currState.boxes;

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

                core::GameState nextState = currState;
                nextState.player = boxCell;
                nextState.moveBox(boxCell, boxDest);
                stats_.generatedStates++;

                if (options_.enableDeadlockDetection) {
                    if (DeadlockDetector::isDeadlock(*board_, nextState, boxDest)) {
                        stats_.deadlockPruned++;
                        continue;
                    }
                }

                // BAD: first-visit wins, never reopen even with smaller g.
                if (closed_.find(nextState) != closed_.end()) {
                    stats_.duplicatePruned++;
                    continue;
                }

                std::uint64_t h = computeHeuristic(nextState.boxes);
                if (h >= Hungarian::INF_COST) {
                    stats_.deadlockPruned++;
                    continue;
                }
                closed_.insert(nextState);

                std::vector<core::Direction> segment;
                core::CellIndex walkCurr = pushSpot;
                while (walkCurr != currState.player && walkCurr != core::INVALID_CELL) {
                    segment.push_back(reach.moveFromParent[walkCurr]);
                    walkCurr = reach.parentCell[walkCurr];
                }
                std::reverse(segment.begin(), segment.end());
                segment.push_back(pushDir);

                std::uint32_t nextIdx = static_cast<std::uint32_t>(nodes_.size());
                nodes_.push_back(GreedyNode{.state = std::move(nextState),
                                            .parentIndex = currIdx,
                                            .edge = MacroEdge{.steps = std::move(segment), .boxFrom = boxCell, .boxTo = boxDest},
                                            .g = currG + 1,
                                            .h = h});
                openQueue_.push(OpenEntry{.h = h, .seq = ++sequenceCounter_, .nodeIndex = nextIdx});
                stats_.maxFrontierSize = std::max(stats_.maxFrontierSize, static_cast<std::uint64_t>(openQueue_.size()));
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

void GreedySolver::finishSolved(std::uint32_t goalNodeIndex) {
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
    for (const auto& e : macroPath) {
        for (core::Direction d : e.steps) {
            fullMoves.push_back(d);
        }
        totalPushes++;
    }
    auto reconEnd = std::chrono::steady_clock::now();
    stats_.reconstructionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(reconEnd - reconStart);

    Solution sol{.moves = std::move(fullMoves), .moveCount = 0,
                 .pushCount = totalPushes, .optimizedFor = options_.metric};
    sol.moveCount = static_cast<std::uint64_t>(sol.moves.size());

    auto valStart = std::chrono::steady_clock::now();
    auto valResult = ReplayValidator::validate(*board_, nodes_[0].state, sol);
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
    stats_.totalSolverTime = stats_.preprocessingTime + stats_.searchTime +
                             stats_.reconstructionTime + stats_.validationTime;
}

void GreedySolver::updatePeakMemory() {
    std::size_t nodeBytes = nodes_.size() * sizeof(GreedyNode);
    std::size_t queueBytes = openQueue_.size() * sizeof(OpenEntry);
    std::size_t closedBytes = closed_.size() * (sizeof(core::GameState) + 32);
    std::size_t cacheBytes = heuristicCache_.size() * (sizeof(std::vector<core::CellIndex>) + sizeof(std::uint64_t) + 32);
    std::uint64_t total = static_cast<std::uint64_t>(nodeBytes + queueBytes + closedBytes + cacheBytes);
    stats_.estimatedPeakBytes = std::max(stats_.estimatedPeakBytes, total);
}

void GreedySolver::requestCancel() {
    cancelRequested_.store(true, std::memory_order_relaxed);
}

SolverProgress GreedySolver::progress() const {
    return SolverProgress{.exploredStates = stats_.exploredStates,
                          .frontierSize = static_cast<std::uint64_t>(openQueue_.size()),
                          .elapsedSearchTime = stats_.searchTime,
                          .status = stats_.status};
}

std::optional<SolverDebugState> GreedySolver::debugState() const {
    return debugState_;
}

SearchStatistics GreedySolver::statistics() const {
    return stats_;
}

std::optional<Solution> GreedySolver::solution() const {
    return solution_;
}

} // namespace sokoban::solvers
