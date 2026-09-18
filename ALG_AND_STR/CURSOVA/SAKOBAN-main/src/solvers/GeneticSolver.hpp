#pragma once

#include "ISolver.hpp"

namespace sokoban::solvers {

struct ChromosomeTrace {
    std::size_t id = 0;
    std::size_t generation = 0;
    std::vector<core::Direction> genes;
    std::vector<core::Direction> moves;
    double fitness = 0;
    int parentA = -1;
    int parentB = -1;
    bool won = false;
    int mutationIndex = -1;
};

class GeneticSolver : public ISolver {
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

    [[nodiscard]] const std::vector<std::vector<ChromosomeTrace>>& history() const noexcept {
        return history_;
    }

private:
    double distanceToGoals(const core::GameState& state) const;
    void evaluate(ChromosomeTrace& chromosome);
    void finishCandidate();

    const core::Board* board_ = nullptr;
    core::GameState initialState_;
    SolverOptions options_;
    SearchStatistics stats_;
    std::optional<Solution> pending_;
    std::optional<Solution> solution_;
    std::optional<SolverDebugState> debugState_;
    std::vector<std::vector<ChromosomeTrace>> history_;
    bool cancelRequested_ = false;
};

} // namespace sokoban::solvers
