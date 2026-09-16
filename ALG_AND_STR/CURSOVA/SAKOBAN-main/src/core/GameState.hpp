#pragma once

#include "Types.hpp"
#include "Board.hpp"
#include <vector>
#include <algorithm>
#include <cstddef>
#include <functional>

namespace sokoban::core {

class GameState {
public:
    CellIndex player = INVALID_CELL;
    std::vector<CellIndex> boxes;

    GameState() = default;
    GameState(CellIndex playerPos, std::vector<CellIndex> boxPositions);

    [[nodiscard]] bool hasBox(CellIndex c) const noexcept {
        return std::binary_search(boxes.begin(), boxes.end(), c);
    }

    [[nodiscard]] std::size_t boxCount() const noexcept {
        return boxes.size();
    }

    [[nodiscard]] bool isGoal(const Board& board) const noexcept {
        for (CellIndex b : boxes) {
            if (!board.isGoal(b)) {
                return false;
            }
        }
        return true;
    }

    void moveBox(CellIndex from, CellIndex to);

    bool operator==(const GameState& other) const noexcept {
        return player == other.player && boxes == other.boxes;
    }

    bool operator!=(const GameState& other) const noexcept {
        return !(*this == other);
    }

    bool operator<(const GameState& other) const noexcept {
        if (player != other.player) {
            return player < other.player;
        }
        return boxes < other.boxes;
    }
};

struct StateHasher {
    std::size_t operator()(const GameState& s) const noexcept {
        // 64-bit FNV-1a / SplitMix hash
        std::uint64_t h = 0xcbf29ce484222325ULL;
        auto mix = [&h](std::uint64_t v) {
            h ^= v;
            h *= 0x100000001b3ULL;
        };

        mix(static_cast<std::uint64_t>(s.player));
        for (CellIndex b : s.boxes) {
            mix(static_cast<std::uint64_t>(b));
        }
        return static_cast<std::size_t>(h);
    }
};

struct BoxConfigHasher {
    std::size_t operator()(const std::vector<CellIndex>& bxs) const noexcept {
        std::uint64_t h = 0xcbf29ce484222325ULL;
        for (CellIndex b : bxs) {
            h ^= static_cast<std::uint64_t>(b);
            h *= 0x100000001b3ULL;
        }
        return static_cast<std::size_t>(h);
    }
};

} // namespace sokoban::core
