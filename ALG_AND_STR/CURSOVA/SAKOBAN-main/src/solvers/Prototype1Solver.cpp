#include "Prototype1Solver.hpp"
#include "ReplayValidator.hpp"
#include "../core/GameRules.hpp"
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <queue>

namespace sokoban::solvers {

namespace {
constexpr core::Direction DIRECTIONS[] = {
    core::Direction::Up,
    core::Direction::Left,
    core::Direction::Down,
    core::Direction::Right
};
}

core::CellIndex Prototype1Solver::step(core::CellIndex cell,
                                       core::Direction direction) const {
    const int x = board_->toX(cell);
    const int y = board_->toY(cell);
    const int dx[] = {0, -1, 0, 1};
    const int dy[] = {-1, 0, 1, 0};
    const auto d = static_cast<std::size_t>(direction);
    const int nextX = x + dx[d];
    const int nextY = y + dy[d];
    return board_->inBounds(nextX, nextY)
        ? board_->toIndex(nextX, nextY)
        : core::INVALID_CELL;
}

std::optional<std::vector<core::Direction>> Prototype1Solver::playerPath(
    const core::GameState& state, core::CellIndex target) {
    if (target == core::INVALID_CELL || state.hasBox(target)) return std::nullopt;
    if (state.player == target) return std::vector<core::Direction>{};

    std::vector<core::CellIndex> parent(board_->size(), core::INVALID_CELL);
    std::vector<core::Direction> parentDirection(board_->size(), core::Direction::Up);
    std::queue<core::CellIndex> queue;
    parent[state.player] = state.player;
    queue.push(state.player);

    while (!queue.empty() && parent[target] == core::INVALID_CELL) {
        const auto current = queue.front();
        queue.pop();
        for (auto direction : DIRECTIONS) {
            const auto next = step(current, direction);
            if (next == core::INVALID_CELL || parent[next] != core::INVALID_CELL || state.hasBox(next))
                continue;
            // Prototype 1 intentionally checks only bounds and boxes, never walls.
            parent[next] = current;
            parentDirection[next] = direction;
            queue.push(next);
        }
    }
    if (parent[target] == core::INVALID_CELL) return std::nullopt;

    std::vector<core::Direction> path;
    for (auto current = target; current != state.player; current = parent[current])
        path.push_back(parentDirection[current]);
    std::reverse(path.begin(), path.end());
    return path;
}

bool Prototype1Solver::appendAxis(core::GameState& state,
                                  core::CellIndex& box,
                                  core::CellIndex goal,
                                  bool horizontal) {
    const int boxCoordinate = horizontal ? board_->toX(box) : board_->toY(box);
    const int goalCoordinate = horizontal ? board_->toX(goal) : board_->toY(goal);
    if (boxCoordinate == goalCoordinate) return true;

    const core::Direction push = horizontal
        ? (goalCoordinate > boxCoordinate ? core::Direction::Right : core::Direction::Left)
        : (goalCoordinate > boxCoordinate ? core::Direction::Down : core::Direction::Up);
    const auto support = step(box, core::opposite(push));
    auto approach = playerPath(state, support);
    if (!approach) return false;
    plannedMoves_.insert(plannedMoves_.end(), approach->begin(), approach->end());
    state.player = support;

    while ((horizontal ? board_->toX(box) : board_->toY(box)) != goalCoordinate) {
        const auto destination = step(box, push);
        if (destination == core::INVALID_CELL || state.hasBox(destination)) return false;
        plannedMoves_.push_back(push);
        state.player = box;
        state.moveBox(box, destination);
        box = destination;
    }
    return true;
}

void Prototype1Solver::start(const core::Board& board,
                             const core::GameState& state,
                             const SolverOptions& options) {
    const auto started = std::chrono::steady_clock::now();
    board_ = &board;
    options_ = options;
    stats_ = SearchStatistics{};
    stats_.algorithm = SolverKind::Prototype1;
    stats_.metric = OptimizationMetric::Pushes;
    solution_.reset();
    initialState_ = state;
    replayState_ = state;
    plannedMoves_.clear();
    replayIndex_ = 0;
    replayPushes_ = 0;
    cancelRequested_ = false;
    debugState_ = options.collectDebugState
        ? std::optional<SolverDebugState>(SolverDebugState{.state = state})
        : std::nullopt;

    if (state.boxes.size() != board.goals().size()) {
        finish(SearchStatus::InvalidLevel);
        return;
    }
    if (state.isGoal(board)) {
        solution_ = Solution{
            .moves = {},
            .moveCount = 0,
            .pushCount = 0,
            .optimizedFor = OptimizationMetric::Pushes
        };
        finish(SearchStatus::Solved);
        return;
    }

    core::GameState planned = state;
    std::vector<core::CellIndex> boxes = state.boxes;
    std::vector<bool> goalUsed(board.goals().size(), false);
    for (std::size_t box = 0; box < boxes.size(); ++box) {
        for (std::size_t goal = 0; goal < board.goals().size(); ++goal) {
            if (!goalUsed[goal] && boxes[box] == board.goals()[goal]) {
                goalUsed[goal] = true;
                break;
            }
        }
    }

    for (std::size_t boxIndex = 0; boxIndex < boxes.size(); ++boxIndex) {
        auto& box = boxes[boxIndex];
        if (board.isGoal(box)) continue;
        std::size_t bestGoal = board.goals().size();
        int bestDistance = std::numeric_limits<int>::max();
        for (std::size_t goal = 0; goal < board.goals().size(); ++goal) {
            if (goalUsed[goal]) continue;
            const int distance = std::abs(board.toX(box) - board.toX(board.goals()[goal])) +
                                 std::abs(board.toY(box) - board.toY(board.goals()[goal]));
            if (distance < bestDistance) {
                bestDistance = distance;
                bestGoal = goal;
            }
        }
        if (bestGoal == board.goals().size() ||
            !appendAxis(planned, box, board.goals()[bestGoal], true) ||
            !appendAxis(planned, box, board.goals()[bestGoal], false)) {
            finish(SearchStatus::NoSolution);
            return;
        }
        goalUsed[bestGoal] = true;
    }

    stats_.searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now() - started);
    stats_.generatedStates = plannedMoves_.size() + 1;
    stats_.maxFrontierSize = plannedMoves_.size();
    stats_.estimatedPeakBytes = plannedMoves_.capacity() * sizeof(core::Direction);
    stats_.status = SearchStatus::Running;
}

SearchStatus Prototype1Solver::advance(std::size_t nodeBudget) {
    if (stats_.status != SearchStatus::Running) return stats_.status;
    if (cancelRequested_) {
        finish(SearchStatus::Cancelled);
        return stats_.status;
    }
    if (plannedMoves_.size() > options_.nodeLimit) {
        finish(SearchStatus::LimitReached);
        return stats_.status;
    }

    while (nodeBudget-- > 0 && replayIndex_ < plannedMoves_.size()) {
        core::GameState next;
        core::MoveRecord record;
        const auto direction = plannedMoves_[replayIndex_];
        if (!core::GameRules::tryMove(*board_, replayState_, direction, next, record,
                                      core::CommandSource::AI)) {
            finish(SearchStatus::NoSolution);
            return stats_.status;
        }
        replayState_ = std::move(next);
        ++replayIndex_;
        ++stats_.exploredStates;
        if (record.pushedBox()) ++replayPushes_;
        if (options_.collectDebugState) {
            debugState_ = SolverDebugState{
                .state = replayState_,
                .g = replayIndex_,
                .h = plannedMoves_.size() - replayIndex_,
                .depth = replayIndex_,
                .hasTransition = true,
                .pushed = record.pushedBox(),
                .direction = direction,
                .boxFrom = record.boxFrom,
                .boxTo = record.boxTo
            };
        }
    }

    if (replayIndex_ == plannedMoves_.size()) {
        if (!replayState_.isGoal(*board_)) {
            finish(SearchStatus::NoSolution);
            return stats_.status;
        }
        Solution candidate{
            .moves = plannedMoves_,
            .moveCount = plannedMoves_.size(),
            .pushCount = replayPushes_,
            .optimizedFor = OptimizationMetric::Pushes
        };
        const auto validationStart = std::chrono::steady_clock::now();
        const auto validation = ReplayValidator::validate(*board_, initialState_, candidate);
        stats_.validationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now() - validationStart);
        if (!validation.valid) {
            finish(SearchStatus::InternalError);
            return stats_.status;
        }
        solution_ = std::move(candidate);
        stats_.solutionMoves = solution_->moveCount;
        stats_.solutionPushes = solution_->pushCount;
        finish(SearchStatus::Solved);
    }
    return stats_.status;
}

void Prototype1Solver::finish(SearchStatus status) {
    stats_.status = status;
    stats_.totalSolverTime = stats_.preprocessingTime + stats_.searchTime +
                             stats_.reconstructionTime + stats_.validationTime;
}

void Prototype1Solver::requestCancel() { cancelRequested_ = true; }

SolverProgress Prototype1Solver::progress() const {
    return SolverProgress{
        .exploredStates = stats_.exploredStates,
        .frontierSize = plannedMoves_.size() - replayIndex_,
        .elapsedSearchTime = stats_.searchTime,
        .status = stats_.status
    };
}

std::optional<SolverDebugState> Prototype1Solver::debugState() const { return debugState_; }
SearchStatistics Prototype1Solver::statistics() const { return stats_; }
std::optional<Solution> Prototype1Solver::solution() const { return solution_; }

} // namespace sokoban::solvers
