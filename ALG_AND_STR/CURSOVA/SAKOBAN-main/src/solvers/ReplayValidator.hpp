#pragma once

#include "../core/Types.hpp"
#include "../core/Board.hpp"
#include "../core/GameState.hpp"
#include "../core/GameRules.hpp"
#include "ISolver.hpp"
#include <string>

namespace sokoban::solvers {

struct ReplayValidationResult {
    bool valid = false;
    std::string errorMessage;
    std::uint64_t actualMoves = 0;
    std::uint64_t actualPushes = 0;
};

class ReplayValidator {
public:
    static ReplayValidationResult validate(const core::Board& board,
                                           const core::GameState& initial,
                                           const Solution& solution);
};

} // namespace sokoban::solvers
