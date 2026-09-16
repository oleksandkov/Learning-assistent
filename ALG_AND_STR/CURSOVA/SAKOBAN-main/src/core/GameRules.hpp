#pragma once

#include "Types.hpp"
#include "Board.hpp"
#include "GameState.hpp"

namespace sokoban::core {

class GameRules {
public:
    static bool canMove(const Board& board, const GameState& state, Direction dir) noexcept;

    static bool tryMove(const Board& board,
                        const GameState& state,
                        Direction dir,
                        GameState& nextState,
                        MoveRecord& record,
                        CommandSource source = CommandSource::Human);

    static bool undoRecord(const Board& board, GameState& state, const MoveRecord& record);

    static bool isWon(const Board& board, const GameState& state) noexcept {
        return state.isGoal(board);
    }
};

} // namespace sokoban::core
