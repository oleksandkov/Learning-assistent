#pragma once

#include "ISolver.hpp"

namespace sokoban::solvers {

struct DrainTrace {
    std::size_t id = 0;
    std::size_t cycle = 0;
    std::vector<core::Direction> moves;
    std::uint64_t pushes = 0;
    std::uint64_t shakes = 0;
    double cost = 0;
    bool won = false;
};

class GravityDrainSolver : public ISolver {
public:
    void start(const core::Board&, const core::GameState&, const SolverOptions&) override;
    SearchStatus advance(std::size_t) override;
    void requestCancel() override;
    SolverProgress progress() const override;
    std::optional<SolverDebugState> debugState() const override;
    SearchStatistics statistics() const override;
    std::optional<Solution> solution() const override;
    [[nodiscard]] const std::vector<std::vector<DrainTrace>>& history() const noexcept { return history_; }

private:
    double height(const core::GameState&) const;
    void finishCandidate();

    const core::Board* board_ = nullptr;
    core::GameState initialState_;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> pending_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;
    std::vector<std::vector<DrainTrace>> history_;
    bool cancelRequested_ = false;
};

} // namespace sokoban::solvers
