#pragma once

#include "Types.hpp"
#include "Board.hpp"
#include "GameState.hpp"
#include "GameRules.hpp"
#include <vector>
#include <chrono>

namespace sokoban::core {

class GameSession {
public:
    GameSession(Board board, GameState initialState);

    [[nodiscard]] const Board& board() const noexcept { return board_; }
    [[nodiscard]] const GameState& initialState() const noexcept { return initialState_; }
    [[nodiscard]] const GameState& currentState() const noexcept { return currentState_; }
    [[nodiscard]] const std::vector<MoveRecord>& history() const noexcept { return history_; }
    [[nodiscard]] std::size_t historySize() const noexcept { return history_.size(); }
    [[nodiscard]] std::size_t redoSize() const noexcept { return redoStack_.size(); }

    [[nodiscard]] std::uint64_t moveCount() const noexcept { return moveCount_; }
    [[nodiscard]] std::uint64_t pushCount() const noexcept { return pushCount_; }
    [[nodiscard]] std::uint64_t stateVersion() const noexcept { return stateVersion_; }

    [[nodiscard]] bool canUndo() const noexcept { return !history_.empty(); }
    [[nodiscard]] bool canRedo() const noexcept { return !redoStack_.empty(); }
    [[nodiscard]] bool isWon() const noexcept { return GameRules::isWon(board_, currentState_); }

    MoveResult apply(const MoveCommand& command);
    bool undo();
    bool redo();
    void restart();

private:
    Board board_;
    GameState initialState_;
    GameState currentState_;

    std::vector<MoveRecord> history_;
    std::vector<MoveRecord> redoStack_;

    std::uint64_t moveCount_ = 0;
    std::uint64_t pushCount_ = 0;
    std::uint64_t stateVersion_ = 0;
};

} // namespace sokoban::core
