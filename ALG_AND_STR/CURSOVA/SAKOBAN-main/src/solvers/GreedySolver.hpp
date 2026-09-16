#pragma once

#include "ISolver.hpp"
#include "Hungarian.hpp"
#include <queue>
#include <unordered_set>
#include <atomic>
#include <optional>

namespace sokoban::solvers {

// Greedy Best-First Search over macro-pushes (BAD / intentionally weak).
// Orders expansion by h only (f = h), keeps first-visit closed set without
// reopening on better g. Fast on trivial levels, often longer solutions,
// can wander and hit limits on harder levels. No optimality claim.
class GreedySolver : public ISolver {
public:
    GreedySolver() = default;

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
    struct MacroEdge {
        std::vector<core::Direction> steps;
        core::CellIndex boxFrom = core::INVALID_CELL;
        core::CellIndex boxTo = core::INVALID_CELL;
    };

    struct GreedyNode {
        core::GameState state;
        std::uint32_t parentIndex = 0;
        MacroEdge edge;
        std::uint64_t g = 0; // actual pushes cost (for reporting only)
        std::uint64_t h = 0;
    };

    struct OpenEntry {
        std::uint64_t h = 0;
        std::uint64_t seq = 0;
        std::uint32_t nodeIndex = 0;
        bool operator>(const OpenEntry& other) const noexcept {
            if (h != other.h) {
                return h > other.h;
            }
            return seq > other.seq;
        }
    };

    struct ReachabilityResult {
        std::vector<bool> reachable;
        std::vector<std::uint32_t> dist;
        std::vector<core::CellIndex> parentCell;
        std::vector<core::Direction> moveFromParent;
    };

    static constexpr std::uint32_t INVALID_NODE = std::numeric_limits<std::uint32_t>::max();

    ReachabilityResult computeReachability(const core::GameState& state) const;
    std::uint64_t computeHeuristic(const std::vector<core::CellIndex>& boxes);
    void finishSolved(std::uint32_t goalNodeIndex);
    void updatePeakMemory();

    const core::Board* board_ = nullptr;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;

    std::priority_queue<OpenEntry, std::vector<OpenEntry>, std::greater<OpenEntry>> openQueue_;
    std::vector<GreedyNode> nodes_;
    std::unordered_set<core::GameState, core::StateHasher> closed_;
    std::unordered_map<std::vector<core::CellIndex>, std::uint64_t, core::BoxConfigHasher> heuristicCache_;

    std::uint64_t sequenceCounter_ = 0;
    std::atomic<bool> cancelRequested_{false};
    std::chrono::steady_clock::time_point searchStartTime_;
};

} // namespace sokoban::solvers
