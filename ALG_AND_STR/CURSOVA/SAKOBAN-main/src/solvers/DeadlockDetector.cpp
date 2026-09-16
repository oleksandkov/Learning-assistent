#include "DeadlockDetector.hpp"
#include <unordered_set>

namespace sokoban::solvers {

bool DeadlockDetector::hasStaticDeadSquare(const core::Board& board, const core::GameState& state) noexcept {
    for (core::CellIndex b : state.boxes) {
        if (board.isDeadSquare(b)) {
            return true;
        }
    }
    return false;
}

bool DeadlockDetector::hasCornerDeadlock(const core::Board& board,
                                        const core::GameState& state,
                                        core::CellIndex movedBox) noexcept {
    auto checkCell = [&](core::CellIndex b) -> bool {
        if (board.isGoal(b)) {
            return false;
        }
        int x = board.toX(b);
        int y = board.toY(b);

        bool wallLeft = (x == 0) || board.isWall(board.toIndex(x - 1, y));
        bool wallRight = (x + 1 >= board.width()) || board.isWall(board.toIndex(x + 1, y));
        bool wallUp = (y == 0) || board.isWall(board.toIndex(x, y - 1));
        bool wallDown = (y + 1 >= board.height()) || board.isWall(board.toIndex(x, y + 1));

        return (wallLeft || wallRight) && (wallUp || wallDown);
    };

    if (movedBox != core::INVALID_CELL) {
        return checkCell(movedBox);
    }

    for (core::CellIndex b : state.boxes) {
        if (checkCell(b)) {
            return true;
        }
    }
    return false;
}

bool DeadlockDetector::has2x2Deadlock(const core::Board& board,
                                     const core::GameState& state,
                                     core::CellIndex movedBox) noexcept {
    auto check2x2At = [&](int x, int y) -> bool {
        if (x < 0 || x + 1 >= board.width() || y < 0 || y + 1 >= board.height()) {
            return false;
        }

        core::CellIndex c00 = board.toIndex(x, y);
        core::CellIndex c10 = board.toIndex(x + 1, y);
        core::CellIndex c01 = board.toIndex(x, y + 1);
        core::CellIndex c11 = board.toIndex(x + 1, y + 1);

        core::CellIndex cells[4] = {c00, c10, c01, c11};
        int boxCount = 0;
        bool allBoxesOnGoals = true;

        for (core::CellIndex c : cells) {
            bool isW = board.isWall(c);
            bool isB = state.hasBox(c);
            if (!isW && !isB) {
                return false; // Cell is open space, not a full 2x2 block
            }
            if (isB) {
                boxCount++;
                if (!board.isGoal(c)) {
                    allBoxesOnGoals = false;
                }
            }
        }

        // Deadlock if 2x2 is completely blocked and contains at least one box not on a goal
        return (boxCount > 0 && !allBoxesOnGoals);
    };

    if (movedBox != core::INVALID_CELL) {
        int bx = board.toX(movedBox);
        int by = board.toY(movedBox);
        return check2x2At(bx, by) ||
               check2x2At(bx - 1, by) ||
               check2x2At(bx, by - 1) ||
               check2x2At(bx - 1, by - 1);
    }

    for (int y = 0; y < board.height() - 1; ++y) {
        for (int x = 0; x < board.width() - 1; ++x) {
            if (check2x2At(x, y)) {
                return true;
            }
        }
    }
    return false;
}

bool DeadlockDetector::hasFreezeDeadlock(const core::Board& board, const core::GameState& state) noexcept {
    std::unordered_set<core::CellIndex> frozen;

    bool changed = true;
    while (changed) {
        changed = false;
        for (core::CellIndex b : state.boxes) {
            if (frozen.count(b)) {
                continue;
            }

            int x = board.toX(b);
            int y = board.toY(b);

            auto isBlocked = [&](int nx, int ny) -> bool {
                if (nx < 0 || nx >= board.width() || ny < 0 || ny >= board.height()) {
                    return true;
                }
                core::CellIndex idx = board.toIndex(nx, ny);
                if (board.isWall(idx)) {
                    return true;
                }
                if (frozen.count(idx)) {
                    return true;
                }
                return false;
            };

            bool blockedH = isBlocked(x - 1, y) || isBlocked(x + 1, y);
            bool blockedV = isBlocked(x, y - 1) || isBlocked(x, y + 1);

            if (blockedH && blockedV) {
                if (!board.isGoal(b)) {
                    return true; // Frozen non-goal box
                }
                frozen.insert(b);
                changed = true;
            }
        }
    }
    return false;
}

bool DeadlockDetector::hasAssignmentDeadlock(const core::Board& board, const core::GameState& state) {
    std::size_t K = state.boxes.size();
    if (K == 0 || K != board.goalCount()) {
        return false;
    }

    std::vector<std::vector<std::int64_t>> cost(K, std::vector<std::int64_t>(K));
    for (std::size_t i = 0; i < K; ++i) {
        for (std::size_t j = 0; j < K; ++j) {
            std::uint32_t d = board.reversePushDistance(j, state.boxes[i]);
            cost[i][j] = (d == core::Board::INF_DISTANCE) ? Hungarian::INF_COST : static_cast<std::int64_t>(d);
        }
    }

    auto [totalCost, _] = Hungarian::solve(cost);
    return totalCost >= Hungarian::INF_COST;
}

bool DeadlockDetector::isDeadlock(const core::Board& board,
                                  const core::GameState& state,
                                  core::CellIndex movedBox,
                                  bool checkAssignment) {
    if (hasStaticDeadSquare(board, state)) {
        return true;
    }
    if (hasCornerDeadlock(board, state, movedBox)) {
        return true;
    }
    if (has2x2Deadlock(board, state, movedBox)) {
        return true;
    }
    if (hasFreezeDeadlock(board, state)) {
        return true;
    }
    if (checkAssignment && hasAssignmentDeadlock(board, state)) {
        return true;
    }
    return false;
}

} // namespace sokoban::solvers
