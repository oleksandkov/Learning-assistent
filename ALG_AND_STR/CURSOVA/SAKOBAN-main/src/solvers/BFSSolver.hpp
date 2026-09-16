#pragma once

#include "ISolver.hpp"
#include <deque>
#include <unordered_map>
#include <atomic>
#include <memory>

namespace sokoban::solvers {

class BFSSolver : public ISolver {
public:
    BFSSolver() = default;

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
    struct BFSNode {
        core::GameState state;
        std::uint32_t parentIndex = 0;
        core::Direction move = core::Direction::Up;
        std::uint32_t depth = 0;
        bool pushed = false;
    };

    static constexpr std::uint32_t INVALID_NODE = std::numeric_limits<std::uint32_t>::max();

    void finishSolved(std::uint32_t goalNodeIndex);
    void updatePeakMemory();

    const core::Board* board_ = nullptr;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;

    std::deque<std::uint32_t> queue_;
    std::vector<BFSNode> nodes_;
    std::unordered_map<core::GameState, std::uint32_t, core::StateHasher> visited_;

    std::atomic<bool> cancelRequested_{false};
    std::chrono::steady_clock::time_point searchStartTime_;
    std::chrono::steady_clock::time_point lastClockTime_;
};

} // namespace sokoban::solvers
