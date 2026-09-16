#pragma once

#include "../core/Types.hpp"
#include "../core/Board.hpp"
#include "../core/GameState.hpp"
#include "Hungarian.hpp"

namespace sokoban::solvers {

class DeadlockDetector {
public:
    // Returns true if the state is a proven deadlock
    static bool isDeadlock(const core::Board& board,
                           const core::GameState& state,
                           core::CellIndex movedBox = core::INVALID_CELL,
                           bool checkAssignment = false);

    // Individual checks for fine-grained testing and verification
    static bool hasStaticDeadSquare(const core::Board& board, const core::GameState& state) noexcept;
    static bool hasCornerDeadlock(const core::Board& board, const core::GameState& state, core::CellIndex movedBox = core::INVALID_CELL) noexcept;
    static bool has2x2Deadlock(const core::Board& board, const core::GameState& state, core::CellIndex movedBox = core::INVALID_CELL) noexcept;
    static bool hasFreezeDeadlock(const core::Board& board, const core::GameState& state) noexcept;
    static bool hasAssignmentDeadlock(const core::Board& board, const core::GameState& state);
};

} // namespace sokoban::solvers
