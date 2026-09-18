#include "AntColonySolver.hpp"
#include "DeadlockDetector.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <unordered_set>

namespace sokoban::solvers {

namespace {
constexpr core::Direction DIRECTIONS[] = {
    core::Direction::Up, core::Direction::Left,
    core::Direction::Down, core::Direction::Right
};
}

double AntColonySolver::distanceToGoals(const core::GameState& state) const {
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

void AntColonySolver::start(const core::Board& board,
                            const core::GameState& state,
                            const SolverOptions& options) {
    const auto started = std::chrono::steady_clock::now();
    board_ = &board;
    initialState_ = state;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::AntColony;
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

    constexpr std::size_t iterationCount = 14;
    constexpr std::size_t antCount = 28;
    const std::size_t maxSteps = std::clamp<std::size_t>(board.size() * 2, 24, 120);
    std::vector<std::array<double, 4>> pheromone(board.size(), {1.0, 1.0, 1.0, 1.0});
    std::mt19937_64 random(0xA17C010ULL ^ state.player ^ (state.boxes.size() << 12));
    double bestCost = std::numeric_limits<double>::infinity();

    for (std::size_t iteration = 0; iteration < iterationCount; ++iteration) {
        if (std::chrono::steady_clock::now() - started >= options.timeLimit) break;
        std::vector<AntTrace> traces;
        traces.reserve(antCount);
        for (std::size_t ant = 0; ant < antCount; ++ant) {
            core::GameState current = state;
            std::unordered_set<core::GameState, core::StateHasher> seen;
            seen.insert(current);
            AntTrace trace;
            trace.antId = ant;
            trace.iteration = iteration;
            bool deadlocked = false;

            for (std::size_t step = 0; step < maxSteps && !current.isGoal(board); ++step) {
                struct Candidate {
                    core::Direction direction;
                    core::GameState state;
                    core::MoveRecord record;
                    double weight;
                };
                std::vector<Candidate> candidates;
                for (auto direction : DIRECTIONS) {
                    core::GameState next;
                    core::MoveRecord record;
                    if (!core::GameRules::tryMove(board, current, direction, next, record,
                                                  core::CommandSource::AI)) continue;
                    const bool repeated = seen.find(next) != seen.end();
                    double weight = pheromone[current.player][static_cast<std::size_t>(direction)] *
                                    (1.0 / (1.0 + distanceToGoals(next)));
                    if (record.pushedBox()) weight *= 2.2;
                    if (repeated) weight *= 0.08;
                    candidates.push_back({direction, std::move(next), record, weight});
                }
                if (candidates.empty()) break;
                std::vector<double> weights;
                for (const auto& candidate : candidates) weights.push_back(candidate.weight);
                const auto chosen = std::discrete_distribution<std::size_t>(
                    weights.begin(), weights.end())(random);
                auto candidate = std::move(candidates[chosen]);
                trace.moves.push_back(candidate.direction);
                if (candidate.record.pushedBox()) ++trace.pushes;
                current = std::move(candidate.state);
                seen.insert(current);
                ++stats_.exploredStates;
                ++stats_.generatedStates;
                if (candidate.record.pushedBox() && DeadlockDetector::isDeadlock(board, current) &&
                    !current.isGoal(board)) {
                    deadlocked = true;
                    break;
                }
            }

            trace.won = current.isGoal(board);
            trace.cost = trace.won
                ? static_cast<double>(trace.moves.size() + trace.pushes * 2)
                : 1000.0 + distanceToGoals(current) * 30.0 + trace.moves.size() +
                  (deadlocked ? 500.0 : 0.0);
            if (trace.won && trace.cost < bestCost) {
                bestCost = trace.cost;
                pending_ = Solution{
                    .moves = trace.moves,
                    .moveCount = trace.moves.size(),
                    .pushCount = trace.pushes,
                    .optimizedFor = OptimizationMetric::Moves
                };
            }
            traces.push_back(std::move(trace));
        }

        for (auto& cell : pheromone)
            for (double& value : cell) value = std::max(0.1, value * 0.72);
        std::sort(traces.begin(), traces.end(), [](const AntTrace& a, const AntTrace& b) {
            return a.cost < b.cost;
        });
        for (std::size_t rank = 0; rank < std::min<std::size_t>(4, traces.size()); ++rank) {
            core::GameState current = state;
            const double deposit = 45.0 / (1.0 + traces[rank].cost);
            for (auto direction : traces[rank].moves) {
                pheromone[current.player][static_cast<std::size_t>(direction)] += deposit;
                core::GameState next;
                core::MoveRecord record;
                if (!core::GameRules::tryMove(board, current, direction, next, record,
                                              core::CommandSource::AI)) break;
                current = std::move(next);
            }
        }
        history_.push_back(std::move(traces));
        if (pending_ && iteration >= 3) break;
    }

    stats_.searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - started);
    stats_.maxFrontierSize = antCount;
    stats_.estimatedPeakBytes = stats_.generatedStates * sizeof(core::Direction);
    if (!pending_) stats_.status = SearchStatus::NoSolution;
}

void AntColonySolver::finishCandidate() {
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
    if (options_.collectDebugState) {
        core::GameState current = initialState_;
        for (auto direction : solution_->moves) {
            core::GameState next;
            core::MoveRecord record;
            core::GameRules::tryMove(*board_, current, direction, next, record, core::CommandSource::AI);
            current = std::move(next);
        }
        debugState_ = SolverDebugState{.state = current, .g = solution_->moveCount,
                                       .depth = solution_->moveCount};
    }
}

SearchStatus AntColonySolver::advance(std::size_t) {
    if (stats_.status != SearchStatus::Running) return stats_.status;
    if (cancelRequested_) stats_.status = SearchStatus::Cancelled;
    else finishCandidate();
    stats_.totalSolverTime = stats_.searchTime + stats_.validationTime;
    return stats_.status;
}

void AntColonySolver::requestCancel() { cancelRequested_ = true; }
SolverProgress AntColonySolver::progress() const {
    return {.exploredStates = stats_.exploredStates, .frontierSize = stats_.maxFrontierSize,
            .elapsedSearchTime = stats_.searchTime, .status = stats_.status};
}
std::optional<SolverDebugState> AntColonySolver::debugState() const { return debugState_; }
SearchStatistics AntColonySolver::statistics() const { return stats_; }
std::optional<Solution> AntColonySolver::solution() const { return solution_; }

} // namespace sokoban::solvers
