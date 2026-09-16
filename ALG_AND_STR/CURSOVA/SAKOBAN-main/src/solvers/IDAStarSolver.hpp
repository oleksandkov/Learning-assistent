#pragma once

#include "ISolver.hpp"
#include "Hungarian.hpp"
#include <vector>
#include <unordered_set>
#include <atomic>
#include <optional>

namespace sokoban::solvers {

// IDA* (Iterative Deepening A*) over macro-pushes.
// Good algorithm: same admissible heuristic as A* (Hungarian assignment over
// reverse-push distances) + safe Deadlock Detection, but memory O(depth)
// instead of O(V). Slower than A* due to re-expansions, optimal for Pushes.
class IDAStarSolver : public ISolver {
public:
    IDAStarSolver() = default;

    void start(const core::Board& board,
               const core::GameState& state,
               const SolverOptions& options) override;

    SearchStatus advance(std::size_t nodeBudget) override;
    void requestCancel() override;
    SolverProgress progress() const override;
    std::optional<SolverDebugState> debugState() const override;
    SearchStatistics statistics() const override;
    std::optional<Solution> solution() const override;

private:
    struct PushAction {
        core::CellIndex boxFrom = core::INVALID_CELL;
        core::CellIndex boxTo = core::INVALID_CELL;
        core::Direction pushDir = core::Direction::Up;
        std::vector<core::Direction> steps; // walk path + push
        std::uint64_t edgeCost = 1;         // 1 for Pushes, walk+1 for Moves
    };

    struct ReachabilityResult {
        std::vector<bool> reachable;
        std::vector<std::uint32_t> dist;
        std::vector<core::CellIndex> parentCell;
        std::vector<core::Direction> moveFromParent;
    };

    ReachabilityResult computeReachability(const core::GameState& state) const;
    std::uint64_t computeHeuristic(const std::vector<core::CellIndex>& boxes);
    std::vector<PushAction> legalPushes(const core::GameState& state,
                                        const ReachabilityResult& reach);
    // Depth-first search limited by bound (on g). Returns true when solved.
    // nextBound is updated with minimal f that exceeded bound.
    bool dfs(core::GameState& state, std::uint64_t g, std::uint64_t bound,
             std::uint64_t& nextBound, std::size_t& budget);
    void finishSolved();
    void updatePeakMemory();

    const core::Board* board_ = nullptr;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;

    std::unordered_map<std::vector<core::CellIndex>, std::uint64_t, core::BoxConfigHasher> heuristicCache_;

    // Current iterative-deepening state
    std::uint64_t bound_ = 0;
    std::uint64_t nextBound_ = 0;
    std::vector<PushAction> path_;          // stack of taken pushes
    std::vector<core::GameState> stateStack_; // stateStack_[0] = start
    bool started_ = false;
    bool finishedIterationSetup_ = false;

    std::atomic<bool> cancelRequested_{false};
    std::chrono::steady_clock::time_point searchStartTime_;
};

} // namespace sokoban::solvers
