#include "CannibalGeneticSolver.hpp"
#include "DeadlockDetector.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <random>

namespace sokoban::solvers {

double CannibalGeneticSolver::distanceToGoals(const core::GameState& state) const {
    double total = 0;
    for (auto box : state.boxes) {
        int best = std::numeric_limits<int>::max();
        for (auto goal : board_->goals()) best = std::min(best,
            std::abs(board_->toX(box) - board_->toX(goal)) + std::abs(board_->toY(box) - board_->toY(goal)));
        total += best;
    }
    return total;
}

void CannibalGeneticSolver::evaluate(CannibalTrace& bot) {
    core::GameState current = initialState_;
    std::uint64_t pushes = 0;
    std::size_t invalid = 0;
    bot.moves.clear();
    bot.deadlocked = false;
    bot.fatalGene = -1;
    bot.validPrefix = 0;
    for (std::size_t index = 0; index < bot.genes.size(); ++index) {
        core::GameState next;
        core::MoveRecord record;
        if (!core::GameRules::tryMove(*board_, current, bot.genes[index], next, record, core::CommandSource::AI)) {
            ++invalid;
            continue;
        }
        bot.moves.push_back(bot.genes[index]);
        current = std::move(next);
        bot.validPrefix = static_cast<int>(index + 1);
        ++stats_.exploredStates;
        ++stats_.generatedStates;
        if (record.pushedBox()) {
            ++pushes;
            if (DeadlockDetector::isDeadlock(*board_, current) && !current.isGoal(*board_)) {
                bot.deadlocked = true;
                bot.fatalGene = static_cast<int>(index);
                bot.validPrefix = static_cast<int>(index);
                ++stats_.deadlockPruned;
                break;
            }
        }
        if (current.isGoal(*board_)) break;
    }
    bot.won = current.isGoal(*board_);
    bot.fitness = bot.won
        ? static_cast<double>(bot.moves.size() + pushes * 2)
        : 500.0 + distanceToGoals(current) * 35.0 + bot.moves.size() + invalid * 3.0 + (bot.deadlocked ? 1000.0 : 0.0);
    if (bot.won && (!pending_ || bot.moves.size() < pending_->moves.size())) {
        pending_ = Solution{.moves = bot.moves, .moveCount = bot.moves.size(), .pushCount = pushes,
                            .optimizedFor = OptimizationMetric::Moves};
    }
}

void CannibalGeneticSolver::start(const core::Board& board, const core::GameState& state,
                                  const SolverOptions& options) {
    const auto started = std::chrono::steady_clock::now();
    board_ = &board;
    initialState_ = state;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::CannibalGenetic;
    stats_.metric = OptimizationMetric::Moves;
    stats_.status = SearchStatus::Running;
    pending_.reset();
    solution_.reset();
    history_.clear();
    cancelRequested_ = false;
    debugState_ = options.collectDebugState ? std::optional(SolverDebugState{.state = state}) : std::nullopt;

    if (state.isGoal(board)) {
        pending_ = Solution{.moves = {}, .moveCount = 0, .pushCount = 0,
                            .optimizedFor = OptimizationMetric::Moves};
        stats_.searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - started);
        return;
    }

    constexpr std::size_t populationSize = 64;
    constexpr std::size_t eliteCount = 6;
    constexpr std::size_t generationCount = 24;
    const std::size_t geneCount = std::clamp<std::size_t>(board.size(), 28, 96);
    std::mt19937_64 random(0xCA661BA1ULL ^ state.player ^ (state.boxes.size() << 13));
    std::uniform_int_distribution<int> direction(0, 3);
    std::vector<CannibalTrace> population(populationSize);
    std::size_t nextId = 0;
    for (std::size_t i = 0; i < population.size(); ++i) {
        population[i].id = nextId++;
        population[i].genes.resize(geneCount);
        for (auto& gene : population[i].genes) gene = static_cast<core::Direction>(direction(random));
        if (i < 4) population[i].genes[0] = static_cast<core::Direction>(i);
    }

    for (std::size_t generation = 0; generation < generationCount; ++generation) {
        if (std::chrono::steady_clock::now() - started >= options.timeLimit) break;
        for (auto& bot : population) { bot.generation = generation; evaluate(bot); }
        std::sort(population.begin(), population.end(), [](const auto& a, const auto& b) { return a.fitness < b.fitness; });
        history_.push_back(population);
        if (pending_ && generation >= 2) break;

        std::vector<CannibalTrace> next;
        next.reserve(populationSize);
        for (std::size_t elite = 0; elite < eliteCount; ++elite) {
            auto copy = population[elite];
            copy.id = nextId++;
            copy.predatorId = static_cast<int>(population[elite].id);
            copy.consumedId = -1;
            copy.mutationIndex = -1;
            next.push_back(std::move(copy));
        }
        while (next.size() < populationSize) {
            std::array<std::size_t, 4> group{};
            for (auto& index : group) index = static_cast<std::size_t>(random() % population.size());
            const auto predatorIndex = *std::min_element(group.begin(), group.end(), [&](auto a, auto b) {
                return population[a].fitness < population[b].fitness;
            });
            const auto victimIndex = *std::max_element(group.begin(), group.end(), [&](auto a, auto b) {
                const double aPenalty = population[a].fitness + (population[a].deadlocked ? 1000.0 : 0.0);
                const double bPenalty = population[b].fitness + (population[b].deadlocked ? 1000.0 : 0.0);
                return aPenalty < bPenalty;
            });
            const auto& predator = population[predatorIndex];
            const auto& victim = population[victimIndex];
            CannibalTrace child;
            child.id = nextId++;
            child.predatorId = static_cast<int>(predator.id);
            child.consumedId = static_cast<int>(victim.id);
            child.genes = predator.genes;
            const std::size_t join = std::min<std::size_t>(
                victim.fatalGene >= 0 ? static_cast<std::size_t>(victim.fatalGene) : static_cast<std::size_t>(victim.validPrefix),
                geneCount - 1);
            std::copy(victim.genes.begin(), victim.genes.begin() + join, child.genes.begin());
            const auto rejected = victim.genes[join];
            auto alternative = static_cast<core::Direction>(direction(random));
            if (alternative == rejected) alternative = static_cast<core::Direction>((static_cast<int>(alternative) + 1) % 4);
            child.genes[join] = alternative;
            child.mutationIndex = static_cast<int>(join);
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

void CannibalGeneticSolver::finishCandidate() {
    if (!pending_) { stats_.status = SearchStatus::NoSolution; return; }
    const auto started = std::chrono::steady_clock::now();
    const auto validation = ReplayValidator::validate(*board_, initialState_, *pending_);
    stats_.validationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - started);
    if (!validation.valid) { stats_.status = SearchStatus::InternalError; pending_.reset(); return; }
    solution_ = *pending_;
    pending_.reset();
    stats_.solutionMoves = solution_->moveCount;
    stats_.solutionPushes = solution_->pushCount;
    stats_.status = SearchStatus::Solved;
}

SearchStatus CannibalGeneticSolver::advance(std::size_t) {
    if (stats_.status != SearchStatus::Running) return stats_.status;
    if (cancelRequested_) stats_.status = SearchStatus::Cancelled; else finishCandidate();
    stats_.totalSolverTime = stats_.searchTime + stats_.validationTime;
    return stats_.status;
}
void CannibalGeneticSolver::requestCancel() { cancelRequested_ = true; }
SolverProgress CannibalGeneticSolver::progress() const { return {.exploredStates = stats_.exploredStates, .frontierSize = stats_.maxFrontierSize, .elapsedSearchTime = stats_.searchTime, .status = stats_.status}; }
std::optional<SolverDebugState> CannibalGeneticSolver::debugState() const { return debugState_; }
SearchStatistics CannibalGeneticSolver::statistics() const { return stats_; }
std::optional<Solution> CannibalGeneticSolver::solution() const { return solution_; }

} // namespace sokoban::solvers
