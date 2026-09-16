#include "GameSession.hpp"

namespace sokoban::core {

GameSession::GameSession(Board board, GameState initialState)
    : board_(std::move(board)),
      initialState_(std::move(initialState)),
      currentState_(initialState_) {
}

MoveResult GameSession::apply(const MoveCommand& command) {
    auto startTime = std::chrono::steady_clock::now();

    GameState nextState;
    MoveRecord record;
    bool success = GameRules::tryMove(board_, currentState_, command.direction, nextState, record, command.source);

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

    if (!success) {
        return MoveResult{
            .success = false,
            .pushedBox = false,
            .playerFrom = currentState_.player,
            .playerTo = currentState_.player,
            .boxFrom = INVALID_CELL,
            .boxTo = INVALID_CELL,
            .levelCompleted = isWon(),
            .logicalMoveDuration = duration
        };
    }

    currentState_ = std::move(nextState);
    history_.push_back(record);

    if (command.source == CommandSource::Human) {
        redoStack_.clear();
    }

    moveCount_++;
    if (record.pushedBox()) {
        pushCount_++;
    }
    stateVersion_++;

    bool completed = isWon();

    return MoveResult{
        .success = true,
        .pushedBox = record.pushedBox(),
        .playerFrom = record.playerFrom,
        .playerTo = record.playerTo,
        .boxFrom = record.boxFrom,
        .boxTo = record.boxTo,
        .levelCompleted = completed,
        .logicalMoveDuration = duration
    };
}

bool GameSession::undo() {
    if (history_.empty()) {
        return false;
    }

    MoveRecord record = history_.back();
    history_.pop_back();

    if (!GameRules::undoRecord(board_, currentState_, record)) {
        return false;
    }

    redoStack_.push_back(record);
    if (moveCount_ > 0) {
        moveCount_--;
    }
    if (record.pushedBox() && pushCount_ > 0) {
        pushCount_--;
    }
    stateVersion_++;
    return true;
}

bool GameSession::redo() {
    if (redoStack_.empty()) {
        return false;
    }

    MoveRecord record = redoStack_.back();
    redoStack_.pop_back();

    MoveCommand cmd{
        .direction = record.direction,
        .source = record.source
    };

    MoveResult res = apply(cmd);
    return res.success;
}

void GameSession::restart() {
    currentState_ = initialState_;
    history_.clear();
    redoStack_.clear();
    moveCount_ = 0;
    pushCount_ = 0;
    stateVersion_++;
}

} // namespace sokoban::core
