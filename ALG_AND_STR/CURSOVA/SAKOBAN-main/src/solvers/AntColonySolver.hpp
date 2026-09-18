#pragma once

#include "ISolver.hpp"
#include <array>

namespace sokoban::solvers {

struct AntTrace {
    std::size_t antId = 0;
    std::size_t iteration = 0;
    std::vector<core::Direction> moves;
    std::uint64_t pushes = 0;
    double cost = 0;
    bool won = false;
};

class AntColonySolver : public ISolver {
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

    [[nodiscard]] const std::vector<std::vector<AntTrace>>& history() const noexcept {
        return history_;
    }

private:
    double distanceToGoals(const core::GameState& state) const;
    void finishCandidate();

    const core::Board* board_ = nullptr;
    core::GameState initialState_;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> pending_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;
    std::vector<std::vector<AntTrace>> history_;
    bool cancelRequested_ = false;
};

} // namespace sokoban::solvers
