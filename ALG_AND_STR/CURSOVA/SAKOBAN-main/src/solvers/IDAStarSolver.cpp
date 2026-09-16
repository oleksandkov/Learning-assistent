#include "IDAStarSolver.hpp"
#include "DeadlockDetector.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>
#include <queue>
#include <limits>

namespace sokoban::solvers {

namespace {
constexpr core::Direction SEARCH_DIRECTIONS[] = {
    core::Direction::Up,
    core::Direction::Left,
    core::Direction::Down,
    core::Direction::Right
};
constexpr std::uint64_t INF64 = std::numeric_limits<std::uint64_t>::max() / 4;
} // namespace

void IDAStarSolver::start(const core::Board& board,
                          const core::GameState& state,
                          const SolverOptions& options) {
    auto prepStart = std::chrono::steady_clock::now();

    board_ = &board;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::IDAStar;
    stats_.metric = options.metric;
    stats_.status = SearchStatus::NotStarted;
    solution_.reset();
    debugState_.reset();
    cancelRequested_.store(false);
    heuristicCache_.clear();
    path_.clear();
    stateStack_.clear();
    started_ = false;
    finishedIterationSetup_ = false;

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

    bound_ = h0;
    nextBound_ = INF64;
    stateStack_.push_back(state);
    stats_.generatedStates = 1;
    stats_.maxFrontierSize = 1; // depth of current path
    stats_.status = SearchStatus::Running;
    started_ = true;

    if (options_.collectDebugState) {
        debugState_ = SolverDebugState{.state = state, .g = 0, .h = h0, .depth = 0};
    }

    updatePeakMemory();
    searchStartTime_ = std::chrono::steady_clock::now();
}

IDAStarSolver::ReachabilityResult IDAStarSolver::computeReachability(const core::GameState& state) const {
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

std::uint64_t IDAStarSolver::computeHeuristic(const std::vector<core::CellIndex>& boxes) {
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

std::vector<IDAStarSolver::PushAction> IDAStarSolver::legalPushes(const core::GameState& state,
                                                                 const ReachabilityResult& reach) {
    std::vector<PushAction> out;
    for (core::CellIndex boxCell : state.boxes) {
        for (core::Direction pushDir : SEARCH_DIRECTIONS) {
            core::Direction approachDir = core::opposite(pushDir);
            core::CellIndex pushSpot = board_->neighbor(boxCell, approachDir);
            core::CellIndex boxDest = board_->neighbor(boxCell, pushDir);
            if (pushSpot == core::INVALID_CELL || !reach.reachable[pushSpot]) {
                continue;
            }
            if (boxDest == core::INVALID_CELL || !board_->isFloor(boxDest) || state.hasBox(boxDest)) {
                continue;
            }
            // Walk segment for replay
            std::vector<core::Direction> segment;
            core::CellIndex walkCurr = pushSpot;
            bool ok = true;
            while (walkCurr != state.player && walkCurr != core::INVALID_CELL) {
                segment.push_back(reach.moveFromParent[walkCurr]);
                walkCurr = reach.parentCell[walkCurr];
                if (segment.size() > board_->size() + 4) {
                    ok = false;
                    break;
                }
            }
            if (!ok) {
                continue;
            }
            std::reverse(segment.begin(), segment.end());
            segment.push_back(pushDir);
            std::uint64_t edgeCost = (options_.metric == OptimizationMetric::Pushes)
                ? 1
                : static_cast<std::uint64_t>(reach.dist[pushSpot]) + 1;
            out.push_back(PushAction{.boxFrom = boxCell, .boxTo = boxDest,
                                     .pushDir = pushDir, .steps = std::move(segment),
                                     .edgeCost = edgeCost});
        }
    }
    return out;
}

bool IDAStarSolver::dfs(core::GameState& state, std::uint64_t g, std::uint64_t bound,
                        std::uint64_t& nextBound, std::size_t& budget) {
    if (stats_.status != SearchStatus::Running) {
        return false;
    }
    if (cancelRequested_.load(std::memory_order_relaxed)) {
        stats_.status = SearchStatus::Cancelled;
        return false;
    }
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - searchStartTime_) >= options_.timeLimit) {
        stats_.status = SearchStatus::LimitReached;
        return false;
    }
    if (stats_.exploredStates >= options_.nodeLimit) {
        stats_.status = SearchStatus::LimitReached;
        return false;
    }
    if (stats_.estimatedPeakBytes >= options_.memoryLimitBytes) {
        stats_.status = SearchStatus::LimitReached;
        return false;
    }
    if (budget == 0) {
        return false; // cooperative pause: caller restarts this bound
    }

    std::uint64_t h = computeHeuristic(state.boxes);
    if (h >= Hungarian::INF_COST) {
        stats_.deadlockPruned++;
        return false;
    }
    std::uint64_t f = g + h;
    if (f > bound) {
        nextBound = std::min(nextBound, f);
        return false;
    }

    stats_.exploredStates++;
    budget--;
    stats_.maxFrontierSize = std::max(stats_.maxFrontierSize, static_cast<std::uint64_t>(path_.size() + 1));

    if (options_.collectDebugState) {
        debugState_ = SolverDebugState{.state = state, .g = g, .h = h,
                                       .depth = static_cast<std::uint64_t>(path_.size())};
        if (!path_.empty()) {
            debugState_->hasTransition = true;
            debugState_->pushed = true;
            debugState_->direction = path_.back().pushDir;
            debugState_->boxFrom = path_.back().boxFrom;
            debugState_->boxTo = path_.back().boxTo;
        }
    }

    if (state.isGoal(*board_)) {
        return true;
    }

    ReachabilityResult reach = computeReachability(state);
    std::vector<PushAction> pushes = legalPushes(state, reach);
    // Order by heuristic of child (best-first within DFS) for speed
    struct Scored {
        std::size_t idx;
        std::uint64_t h;
    };
    std::vector<Scored> order;
    order.reserve(pushes.size());
    for (std::size_t i = 0; i < pushes.size(); ++i) {
        core::GameState child = state;
        child.player = pushes[i].boxFrom;
        child.moveBox(pushes[i].boxFrom, pushes[i].boxTo);
        std::uint64_t ch = computeHeuristic(child.boxes);
        order.push_back({i, ch});
    }
    std::sort(order.begin(), order.end(), [](const Scored& a, const Scored& b) { return a.h < b.h; });

    for (const auto& s : order) {
        const PushAction& action = pushes[s.idx];
        core::GameState child = state;
        child.player = action.boxFrom;
        child.moveBox(action.boxFrom, action.boxTo);
        stats_.generatedStates++;

        if (options_.enableDeadlockDetection) {
            if (DeadlockDetector::isDeadlock(*board_, child, action.boxTo)) {
                stats_.deadlockPruned++;
                continue;
            }
        }
        if (s.h >= Hungarian::INF_COST) {
            stats_.deadlockPruned++;
            continue;
        }
        // Cycle check on current path (physical states)
        bool onPath = false;
        for (const auto& st : stateStack_) {
            if (st == child) {
                onPath = true;
                break;
            }
        }
        if (onPath) {
            stats_.duplicatePruned++;
            continue;
        }

        path_.push_back(action);
        stateStack_.push_back(child);
        updatePeakMemory();
        bool solved = dfs(stateStack_.back(), g + action.edgeCost, bound, nextBound, budget);
        if (solved) {
            return true;
        }
        stateStack_.pop_back();
        path_.pop_back();
        if (stats_.status != SearchStatus::Running) {
            return false;
        }
        if (budget == 0) {
            return false; // pause; bound iteration restarts next advance()
        }
    }
    return false;
}

SearchStatus IDAStarSolver::advance(std::size_t nodeBudget) {
    if (stats_.status != SearchStatus::Running) {
        return stats_.status;
    }
    if (!started_) {
        return stats_.status;
    }
    auto stepStart = std::chrono::steady_clock::now();

    std::size_t budget = nodeBudget;
    // Outer iterative-deepening loop. A zero budget just returns.
    while (stats_.status == SearchStatus::Running && budget > 0) {
        nextBound_ = INF64;
        path_.clear();
        stateStack_.resize(1); // keep start state
        std::size_t iterBudget = budget;
        bool solved = dfs(stateStack_.front(), 0, bound_, nextBound_, iterBudget);
        budget = iterBudget;
        if (solved) {
            auto stepEnd = std::chrono::steady_clock::now();
            stats_.searchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);
            finishSolved();
            return stats_.status;
        }
        if (stats_.status != SearchStatus::Running) {
            break;
        }
        if (budget == 0) {
            break; // cooperative pause, same bound retried next advance()
        }
        if (nextBound_ == INF64) {
            stats_.status = SearchStatus::NoSolution;
            break;
        }
        bound_ = nextBound_;
        // Guard against pathological bound growth loops: if bound exceeds
        // a sane cap relative to heuristic, keep going — completeness first.
    }

    auto stepEnd = std::chrono::steady_clock::now();
    stats_.searchTime += std::chrono::duration_cast<std::chrono::nanoseconds>(stepEnd - stepStart);
    updatePeakMemory();
    if (stats_.status == SearchStatus::Running && stats_.estimatedPeakBytes >= options_.memoryLimitBytes) {
        stats_.status = SearchStatus::LimitReached;
    }
    if (stats_.status == SearchStatus::Running) {
        stats_.totalSolverTime = stats_.preprocessingTime + stats_.searchTime;
    } else if (stats_.status == SearchStatus::NoSolution ||
               stats_.status == SearchStatus::LimitReached ||
               stats_.status == SearchStatus::Cancelled) {
        stats_.totalSolverTime = stats_.preprocessingTime + stats_.searchTime;
    }
    return stats_.status;
}

void IDAStarSolver::finishSolved() {
    auto reconStart = std::chrono::steady_clock::now();
    std::vector<core::Direction> fullMoves;
    std::uint64_t totalPushes = 0;
    for (const auto& action : path_) {
        for (core::Direction d : action.steps) {
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
    auto valResult = ReplayValidator::validate(*board_, stateStack_.front(), sol);
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

void IDAStarSolver::updatePeakMemory() {
    std::size_t pathBytes = path_.size() * sizeof(PushAction);
    for (const auto& a : path_) {
        pathBytes += a.steps.size() * sizeof(core::Direction);
    }
    std::size_t stackBytes = stateStack_.size() * sizeof(core::GameState);
    std::size_t cacheBytes = heuristicCache_.size() * (sizeof(std::vector<core::CellIndex>) + sizeof(std::uint64_t) + 32);
    std::uint64_t total = static_cast<std::uint64_t>(pathBytes + stackBytes + cacheBytes + sizeof(*this));
    stats_.estimatedPeakBytes = std::max(stats_.estimatedPeakBytes, total);
}

void IDAStarSolver::requestCancel() {
    cancelRequested_.store(true, std::memory_order_relaxed);
}

SolverProgress IDAStarSolver::progress() const {
    return SolverProgress{.exploredStates = stats_.exploredStates,
                          .frontierSize = static_cast<std::uint64_t>(path_.size() + 1),
                          .elapsedSearchTime = stats_.searchTime,
                          .status = stats_.status};
}

std::optional<SolverDebugState> IDAStarSolver::debugState() const {
    return debugState_;
}

SearchStatistics IDAStarSolver::statistics() const {
    return stats_;
}

std::optional<Solution> IDAStarSolver::solution() const {
    return solution_;
}

} // namespace sokoban::solvers
