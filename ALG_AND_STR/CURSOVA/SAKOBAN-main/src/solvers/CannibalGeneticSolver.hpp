#pragma once

#include "ISolver.hpp"

namespace sokoban::solvers {

struct CannibalTrace {
    std::size_t id = 0;
    std::size_t generation = 0;
    std::vector<core::Direction> genes;
    std::vector<core::Direction> moves;
    double fitness = 0;
    int predatorId = -1;
    int consumedId = -1;
    int fatalGene = -1;
    int validPrefix = 0;
    int mutationIndex = -1;
    bool won = false;
    bool deadlocked = false;
};

class CannibalGeneticSolver : public ISolver {
public:
    void start(const core::Board&, const core::GameState&, const SolverOptions&) override;
    SearchStatus advance(std::size_t) override;
    void requestCancel() override;
    SolverProgress progress() const override;
    std::optional<SolverDebugState> debugState() const override;
    SearchStatistics statistics() const override;
    std::optional<Solution> solution() const override;
    [[nodiscard]] const std::vector<std::vector<CannibalTrace>>& history() const noexcept { return history_; }

private:
    double distanceToGoals(const core::GameState&) const;
    void evaluate(CannibalTrace&);
    void finishCandidate();

    const core::Board* board_ = nullptr;
    core::GameState initialState_;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> pending_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;
    std::vector<std::vector<CannibalTrace>> history_;
    bool cancelRequested_ = false;
};

} // namespace sokoban::solvers
