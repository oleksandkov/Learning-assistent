#include "GeneticSolver.hpp"
#include "DeadlockDetector.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

namespace sokoban::solvers {

double GeneticSolver::distanceToGoals(const core::GameState& state) const {
    double total = 0;
    for (auto box : state.boxes) {
        int best = std::numeric_limits<int>::max();
        for (auto goal : board_->goals()) {
            best = std::min(best,
                std::abs(board_->toX(box) - board_->toX(goal)) +
                std::abs(board_->toY(box) - board_->toY(goal)));
        }
        total += best;
    }
    return total;
}

void GeneticSolver::evaluate(ChromosomeTrace& chromosome) {
    core::GameState current = initialState_;
    std::uint64_t pushes = 0;
    std::size_t invalid = 0;
    chromosome.moves.clear();
    for (auto gene : chromosome.genes) {
        core::GameState next;
        core::MoveRecord record;
        if (!core::GameRules::tryMove(*board_, current, gene, next, record,
                                      core::CommandSource::AI)) {
            ++invalid;
            continue;
        }
        chromosome.moves.push_back(gene);
        current = std::move(next);
        ++stats_.exploredStates;
        ++stats_.generatedStates;
        if (record.pushedBox()) {
            ++pushes;
            if (DeadlockDetector::isDeadlock(*board_, current) && !current.isGoal(*board_)) break;
        }
        if (current.isGoal(*board_)) break;
    }
    chromosome.won = current.isGoal(*board_);
    chromosome.fitness = chromosome.won
        ? static_cast<double>(chromosome.moves.size() + pushes * 2)
        : distanceToGoals(current) * 35.0 + chromosome.moves.size() + invalid * 3.0 + 500.0;
    if (chromosome.won && (!pending_ || chromosome.moves.size() < pending_->moves.size())) {
        pending_ = Solution{
            .moves = chromosome.moves,
            .moveCount = chromosome.moves.size(),
            .pushCount = pushes,
            .optimizedFor = OptimizationMetric::Moves
        };
    }
}

void GeneticSolver::start(const core::Board& board,
                          const core::GameState& state,
                          const SolverOptions& options) {
    const auto started = std::chrono::steady_clock::now();
    board_ = &board;
    initialState_ = state;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::Genetic;
    stats_.metric = OptimizationMetric::Moves;
    stats_.status = SearchStatus::Running;
    pending_.reset();
    solution_.reset();
    history_.clear();
    cancelRequested_ = false;
    debugState_ = options.collectDebugState
        ? std::optional<SolverDebugState>(SolverDebugState{.state = state})
        : std::nullopt;

    if (state.isGoal(board)) {
        pending_ = Solution{.moves = {}, .moveCount = 0, .pushCount = 0,
                            .optimizedFor = OptimizationMetric::Moves};
        stats_.searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - started);
        return;
    }

    constexpr std::size_t populationSize = 36;
    constexpr std::size_t eliteCount = 6;
    constexpr std::size_t generationCount = 24;
    const std::size_t geneCount = std::clamp<std::size_t>(board.size(), 28, 96);
    std::mt19937_64 random(0x6E3E71CULL ^ state.player ^ (state.boxes.size() << 14));
    std::uniform_int_distribution<int> direction(0, 3);
    std::vector<ChromosomeTrace> population(populationSize);
    std::size_t nextId = 0;
    for (std::size_t i = 0; i < population.size(); ++i) {
        population[i].id = nextId++;
        population[i].genes.resize(geneCount);
        for (auto& gene : population[i].genes)
            gene = static_cast<core::Direction>(direction(random));
        if (i < 4) population[i].genes[0] = static_cast<core::Direction>(i);
    }

    for (std::size_t generation = 0; generation < generationCount; ++generation) {
        if (std::chrono::steady_clock::now() - started >= options.timeLimit) break;
        for (auto& chromosome : population) {
            chromosome.generation = generation;
            evaluate(chromosome);
        }
        std::sort(population.begin(), population.end(), [](const auto& a, const auto& b) {
            return a.fitness < b.fitness;
        });
        history_.push_back(population);
        if (pending_ && generation >= 2) break;

        std::vector<ChromosomeTrace> next;
        next.reserve(populationSize);
        for (std::size_t elite = 0; elite < eliteCount; ++elite) {
            auto copy = population[elite];
            copy.id = nextId++;
            copy.parentA = static_cast<int>(population[elite].id);
            copy.parentB = static_cast<int>(population[elite].id);
            copy.mutationIndex = -1;
            next.push_back(std::move(copy));
        }
        auto tournament = [&]() -> const ChromosomeTrace& {
            const auto a = static_cast<std::size_t>(random() % population.size());
            const auto b = static_cast<std::size_t>(random() % population.size());
            return population[a].fitness < population[b].fitness ? population[a] : population[b];
        };
        while (next.size() < populationSize) {
            const auto& parentA = tournament();
            const auto& parentB = tournament();
            const std::size_t cut = 1 + random() % (geneCount - 1);
            ChromosomeTrace child;
            child.id = nextId++;
            child.parentA = static_cast<int>(parentA.id);
            child.parentB = static_cast<int>(parentB.id);
            child.genes = parentA.genes;
            std::copy(parentB.genes.begin() + cut, parentB.genes.end(), child.genes.begin() + cut);
            if (std::uniform_real_distribution<double>(0.0, 1.0)(random) < 0.22) {
                child.mutationIndex = static_cast<int>(random() % geneCount);
                child.genes[static_cast<std::size_t>(child.mutationIndex)] =
                    static_cast<core::Direction>(direction(random));
            }
            next.push_back(std::move(child));
        }
        population = std::move(next);
    }

    stats_.searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - started);
    stats_.maxFrontierSize = populationSize;
    stats_.estimatedPeakBytes = stats_.generatedStates * sizeof(core::Direction);
    if (!pending_) stats_.status = SearchStatus::NoSolution;
}

void GeneticSolver::finishCandidate() {
    if (!pending_) {
        stats_.status = SearchStatus::NoSolution;
        return;
    }
    const auto started = std::chrono::steady_clock::now();
    const auto validation = ReplayValidator::validate(*board_, initialState_, *pending_);
    stats_.validationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - started);
    if (!validation.valid) {
        stats_.status = SearchStatus::InternalError;
        pending_.reset();
        return;
    }
    solution_ = *pending_;
    pending_.reset();
    stats_.solutionMoves = solution_->moveCount;
    stats_.solutionPushes = solution_->pushCount;
    stats_.status = SearchStatus::Solved;
}

SearchStatus GeneticSolver::advance(std::size_t) {
    if (stats_.status != SearchStatus::Running) return stats_.status;
    if (cancelRequested_) stats_.status = SearchStatus::Cancelled;
    else finishCandidate();
    stats_.totalSolverTime = stats_.searchTime + stats_.validationTime;
    return stats_.status;
}

void GeneticSolver::requestCancel() { cancelRequested_ = true; }
SolverProgress GeneticSolver::progress() const {
    return {.exploredStates = stats_.exploredStates, .frontierSize = stats_.maxFrontierSize,
            .elapsedSearchTime = stats_.searchTime, .status = stats_.status};
}
std::optional<SolverDebugState> GeneticSolver::debugState() const { return debugState_; }
SearchStatistics GeneticSolver::statistics() const { return stats_; }
std::optional<Solution> GeneticSolver::solution() const { return solution_; }

} // namespace sokoban::solvers
