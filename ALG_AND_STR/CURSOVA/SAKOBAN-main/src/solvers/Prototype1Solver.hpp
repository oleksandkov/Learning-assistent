#pragma once

#include "ISolver.hpp"
#include <optional>

namespace sokoban::solvers {

// Educational baseline: nearest free goal, then straight pushes X -> Y.
// Walls are deliberately ignored while planning; normal rules validate replay.
class Prototype1Solver : public ISolver {
public:
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
    std::optional<std::vector<core::Direction>> playerPath(
        const core::GameState& state, core::CellIndex target);
    bool appendAxis(core::GameState& state,
                    core::CellIndex& box,
                    core::CellIndex goal,
                    bool horizontal);
    core::CellIndex step(core::CellIndex cell, core::Direction direction) const;
    void finish(SearchStatus status);

    const core::Board* board_ = nullptr;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;
    core::GameState initialState_;
    core::GameState replayState_;
    std::vector<core::Direction> plannedMoves_;
    std::size_t replayIndex_ = 0;
    std::uint64_t replayPushes_ = 0;
    bool cancelRequested_ = false;
};

} // namespace sokoban::solvers
