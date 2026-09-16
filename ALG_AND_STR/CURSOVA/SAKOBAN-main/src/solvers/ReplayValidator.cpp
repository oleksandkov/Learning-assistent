#include "ReplayValidator.hpp"

namespace sokoban::solvers {

ReplayValidationResult ReplayValidator::validate(const core::Board& board,
                                                 const core::GameState& initial,
                                                 const Solution& solution) {
    core::GameState state = initial;
    std::uint64_t moves = 0;
    std::uint64_t pushes = 0;

    for (std::size_t i = 0; i < solution.moves.size(); ++i) {
        core::Direction dir = solution.moves[i];
        core::GameState nextState;
        core::MoveRecord record;

        if (!core::GameRules::tryMove(board, state, dir, nextState, record, core::CommandSource::Replay)) {
            return ReplayValidationResult{
                .valid = false,
                .errorMessage = "Illegal move at step " + std::to_string(i) + " (" + core::directionToChar(dir) + ")",
                .actualMoves = moves,
                .actualPushes = pushes
            };
        }

        moves++;
        if (record.pushedBox()) {
            pushes++;
        }
        state = std::move(nextState);
    }

    if (!core::GameRules::isWon(board, state)) {
        return ReplayValidationResult{
            .valid = false,
            .errorMessage = "Final state after solution is not a goal state",
            .actualMoves = moves,
            .actualPushes = pushes
        };
    }

    if (moves != solution.moveCount) {
        return ReplayValidationResult{
            .valid = false,
            .errorMessage = "Move count mismatch: expected " + std::to_string(solution.moveCount) +
                            " but replay took " + std::to_string(moves),
            .actualMoves = moves,
            .actualPushes = pushes
        };
    }

    if (pushes != solution.pushCount) {
        return ReplayValidationResult{
            .valid = false,
            .errorMessage = "Push count mismatch: expected " + std::to_string(solution.pushCount) +
                            " but replay took " + std::to_string(pushes),
            .actualMoves = moves,
            .actualPushes = pushes
        };
    }

    return ReplayValidationResult{
        .valid = true,
        .errorMessage = "",
        .actualMoves = moves,
        .actualPushes = pushes
    };
}

} // namespace sokoban::solvers
