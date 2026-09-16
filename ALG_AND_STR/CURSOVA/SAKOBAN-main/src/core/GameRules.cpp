#include "GameRules.hpp"

namespace sokoban::core {

bool GameRules::canMove(const Board& board, const GameState& state, Direction dir) noexcept {
    CellIndex p = state.player;
    CellIndex target = board.neighbor(p, dir);
    if (target == INVALID_CELL || !board.isFloor(target)) {
        return false;
    }

    if (!state.hasBox(target)) {
        return true;
    }

    // Target has a box -> check if it can be pushed
    CellIndex beyond = board.neighbor(target, dir);
    if (beyond == INVALID_CELL || !board.isFloor(beyond)) {
        return false;
    }
    if (state.hasBox(beyond)) {
        return false;
    }

    return true;
}

bool GameRules::tryMove(const Board& board,
                        const GameState& state,
                        Direction dir,
                        GameState& nextState,
                        MoveRecord& record,
                        CommandSource source) {
    CellIndex p = state.player;
    CellIndex target = board.neighbor(p, dir);
    if (target == INVALID_CELL || !board.isFloor(target)) {
        return false;
    }

    if (!state.hasBox(target)) {
        // Simple step
        nextState = state;
        nextState.player = target;

        record.playerFrom = p;
        record.playerTo = target;
        record.boxFrom = INVALID_CELL;
        record.boxTo = INVALID_CELL;
        record.direction = dir;
        record.source = source;
        return true;
    }

    // Target has a box
    CellIndex beyond = board.neighbor(target, dir);
    if (beyond == INVALID_CELL || !board.isFloor(beyond)) {
        return false;
    }
    if (state.hasBox(beyond)) {
        return false;
    }

    // Valid push
    nextState = state;
    nextState.player = target;
    nextState.moveBox(target, beyond);

    record.playerFrom = p;
    record.playerTo = target;
    record.boxFrom = target;
    record.boxTo = beyond;
    record.direction = dir;
    record.source = source;
    return true;
}

bool GameRules::undoRecord(const Board&, GameState& state, const MoveRecord& record) {
    if (state.player != record.playerTo) {
        return false;
    }

    state.player = record.playerFrom;

    if (record.pushedBox()) {
        if (!state.hasBox(record.boxTo)) {
            return false;
        }
        state.moveBox(record.boxTo, record.boxFrom);
    }

    return true;
}

} // namespace sokoban::core
