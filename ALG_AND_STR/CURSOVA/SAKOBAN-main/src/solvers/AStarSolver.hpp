#pragma once

#include "ISolver.hpp"
#include "Hungarian.hpp"
#include "Zobrist.hpp"
#include <queue>
#include <unordered_map>
#include <atomic>
#include <optional>

namespace sokoban::solvers {

class AStarSolver : public ISolver {
public:
    AStarSolver() = default;

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
        std::vector<core::Direction> steps; // walk steps + 1 push step
        core::CellIndex boxFrom = core::INVALID_CELL;
        core::CellIndex boxTo = core::INVALID_CELL;
    };

    struct AStarNode {
        core::GameState state;
        std::uint32_t parentIndex = 0;
        MacroEdge edge;
        std::uint64_t g = 0;
        std::uint64_t h = 0;
        std::uint64_t pushCount = 0;
    };

    struct OpenEntry {
        std::uint64_t f = 0;
        std::uint64_t g = 0;
        std::uint64_t h = 0;
        std::uint64_t seq = 0;
        std::uint32_t nodeIndex = 0;

        bool operator>(const OpenEntry& other) const noexcept {
            if (f != other.f) {
                return f > other.f;
            }
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
        core::CellIndex minCell = core::INVALID_CELL;
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
    std::vector<AStarNode> nodes_;
    std::unordered_map<core::GameState, std::uint64_t, core::StateHasher> bestG_;
    std::unordered_map<std::vector<core::CellIndex>, std::uint64_t, core::BoxConfigHasher> heuristicCache_;

    std::unique_ptr<Zobrist> zobrist_;
    std::uint64_t sequenceCounter_ = 0;
    std::atomic<bool> cancelRequested_{false};
    std::chrono::steady_clock::time_point searchStartTime_;
};

} // namespace sokoban::solvers
