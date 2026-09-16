#include "GameState.hpp"
#include <algorithm>

namespace sokoban::core {

GameState::GameState(CellIndex playerPos, std::vector<CellIndex> boxPositions)
    : player(playerPos), boxes(std::move(boxPositions)) {
    std::sort(boxes.begin(), boxes.end());
}

void GameState::moveBox(CellIndex from, CellIndex to) {
    auto it = std::lower_bound(boxes.begin(), boxes.end(), from);
    if (it != boxes.end() && *it == from) {
        *it = to;
        // Keep sorted
        std::sort(boxes.begin(), boxes.end());
    }
}

} // namespace sokoban::core
