#include "GravityDrainSolver.hpp"
#include "DeadlockDetector.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>
#include <array>
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

double GravityDrainSolver::height(const core::GameState& state) const {
    double total = 0;
    for (auto box : state.boxes) {
        int best = std::numeric_limits<int>::max();
        for (auto goal : board_->goals()) {
            best = std::min(best, std::abs(board_->toX(box) - board_->toX(goal)) +
                                  std::abs(board_->toY(box) - board_->toY(goal)));
        }
        total += best;
    }
    return total;
}

void GravityDrainSolver::start(const core::Board& board, const core::GameState& state,
                               const SolverOptions& options) {
    const auto started = std::chrono::steady_clock::now();
    board_ = &board;
    initialState_ = state;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::GravityDrain;
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

    constexpr std::size_t cycleCount = 14;
    constexpr std::size_t streamCount = 28;
    const std::size_t maxSteps = std::clamp<std::size_t>(board.size() * 2, 24, 120);
    std::vector<std::array<double, 4>> pressure(board.size(), {0.0, 0.0, 0.0, 0.0});
    std::mt19937_64 random(0xD2A1AULL ^ state.player ^ (state.boxes.size() << 11));
    double bestCost = std::numeric_limits<double>::infinity();

    for (std::size_t cycle = 0; cycle < cycleCount; ++cycle) {
        if (std::chrono::steady_clock::now() - started >= options.timeLimit) break;
        std::vector<DrainTrace> traces;
        traces.reserve(streamCount);
        for (std::size_t stream = 0; stream < streamCount; ++stream) {
            core::GameState current = state;
            std::unordered_set<core::GameState, core::StateHasher> seen{current};
            DrainTrace trace;
            trace.id = stream;
            trace.cycle = cycle;
            double previousHeight = height(current);
            std::size_t stagnant = 0;
            bool deadlocked = false;
            std::optional<core::Direction> previousDirection;

            for (std::size_t tick = 0; tick < maxSteps && !current.isGoal(board); ++tick) {
                struct Candidate { core::Direction direction; core::GameState state; core::MoveRecord record; double force; };
                std::vector<Candidate> candidates;
                const bool shaking = stagnant >= 9;
                if (shaking) { ++trace.shakes; stagnant = 0; }
                for (auto direction : DIRECTIONS) {
                    core::GameState next;
                    core::MoveRecord record;
                    if (!core::GameRules::tryMove(board, current, direction, next, record, core::CommandSource::AI)) continue;
                    const double nextHeight = height(next);
                    double force = 1.0 + pressure[current.player][static_cast<std::size_t>(direction)];
                    if (record.pushedBox()) force += 5.0 + (previousHeight - nextHeight) * 8.0;
                    if (previousDirection == direction) force += 1.2;
                    if (seen.contains(next)) force *= 0.08;
                    if (shaking) force += std::uniform_real_distribution<double>(0.0, 8.0)(random);
                    candidates.push_back({direction, std::move(next), record, std::max(0.02, force)});
                }
                if (candidates.empty()) break;
                std::vector<double> weights;
                for (const auto& candidate : candidates) weights.push_back(candidate.force);
                auto chosen = std::discrete_distribution<std::size_t>(weights.begin(), weights.end())(random);
                auto candidate = std::move(candidates[chosen]);
                trace.moves.push_back(candidate.direction);
                previousDirection = candidate.direction;
                const double nextHeight = height(candidate.state);
                if (candidate.record.pushedBox()) {
                    ++trace.pushes;
                    stagnant = nextHeight < previousHeight ? 0 : stagnant + 1;
                } else {
                    ++stagnant;
                }
                previousHeight = nextHeight;
                current = std::move(candidate.state);
                seen.insert(current);
                ++stats_.exploredStates;
                ++stats_.generatedStates;
                if (candidate.record.pushedBox() && DeadlockDetector::isDeadlock(board, current) && !current.isGoal(board)) {
                    deadlocked = true;
                    ++stats_.deadlockPruned;
                    break;
                }
            }

            trace.won = current.isGoal(board);
            trace.cost = trace.won
                ? static_cast<double>(trace.moves.size() + trace.pushes * 2 + trace.shakes * 3)
                : 1000.0 + height(current) * 30.0 + trace.moves.size() + trace.shakes * 8.0 + (deadlocked ? 500.0 : 0.0);
            if (trace.won && trace.cost < bestCost) {
                bestCost = trace.cost;
                pending_ = Solution{.moves = trace.moves, .moveCount = trace.moves.size(),
                                    .pushCount = trace.pushes, .optimizedFor = OptimizationMetric::Moves};
            }
            traces.push_back(std::move(trace));
        }

        for (auto& cell : pressure) for (double& value : cell) value *= 0.68;
        std::sort(traces.begin(), traces.end(), [](const auto& a, const auto& b) { return a.cost < b.cost; });
        for (std::size_t rank = 0; rank < std::min<std::size_t>(4, traces.size()); ++rank) {
            core::GameState current = state;
            const double deposit = 30.0 / (1.0 + traces[rank].cost);
            for (auto direction : traces[rank].moves) {
                pressure[current.player][static_cast<std::size_t>(direction)] += deposit;
                core::GameState next;
                core::MoveRecord record;
                if (!core::GameRules::tryMove(board, current, direction, next, record, core::CommandSource::AI)) break;
                current = std::move(next);
            }
        }
        history_.push_back(std::move(traces));
        if (pending_ && cycle >= 3) break;
    }

    stats_.searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - started);
    stats_.maxFrontierSize = streamCount;
    stats_.estimatedPeakBytes = stats_.generatedStates * sizeof(core::Direction);
    if (!pending_) stats_.status = SearchStatus::NoSolution;
}

void GravityDrainSolver::finishCandidate() {
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

SearchStatus GravityDrainSolver::advance(std::size_t) {
    if (stats_.status != SearchStatus::Running) return stats_.status;
    if (cancelRequested_) stats_.status = SearchStatus::Cancelled; else finishCandidate();
    stats_.totalSolverTime = stats_.searchTime + stats_.validationTime;
    return stats_.status;
}
void GravityDrainSolver::requestCancel() { cancelRequested_ = true; }
SolverProgress GravityDrainSolver::progress() const { return {.exploredStates = stats_.exploredStates, .frontierSize = stats_.maxFrontierSize, .elapsedSearchTime = stats_.searchTime, .status = stats_.status}; }
std::optional<SolverDebugState> GravityDrainSolver::debugState() const { return debugState_; }
SearchStatistics GravityDrainSolver::statistics() const { return stats_; }
std::optional<Solution> GravityDrainSolver::solution() const { return solution_; }

} // namespace sokoban::solvers
