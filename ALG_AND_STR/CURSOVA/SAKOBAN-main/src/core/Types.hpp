#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <chrono>
#include <stdexcept>

namespace sokoban::core {

using CellIndex = std::uint32_t;
constexpr CellIndex INVALID_CELL = std::numeric_limits<CellIndex>::max();

enum class Direction : std::uint8_t {
    Up = 0,
    Left = 1,
    Down = 2,
    Right = 3
};

inline constexpr Direction OPPOSITE_DIRECTIONS[] = {
    Direction::Down,
    Direction::Right,
    Direction::Up,
    Direction::Left
};

constexpr Direction opposite(Direction dir) noexcept {
    return OPPOSITE_DIRECTIONS[static_cast<std::size_t>(dir)];
}

constexpr char directionToChar(Direction dir) noexcept {
    switch (dir) {
        case Direction::Up: return 'U';
        case Direction::Left: return 'L';
        case Direction::Down: return 'D';
        case Direction::Right: return 'R';
    }
    return '?';
}

constexpr Direction charToDirection(char c) {
    switch (c) {
        case 'U': case 'u': return Direction::Up;
        case 'L': case 'l': return Direction::Left;
        case 'D': case 'd': return Direction::Down;
        case 'R': case 'r': return Direction::Right;
        default: throw std::invalid_argument(std::string("Invalid direction char: ") + c);
    }
}

enum class CommandSource {
    Human,
    AI,
    Replay,
    Test
};

struct MoveCommand {
    Direction direction;
    CommandSource source = CommandSource::Human;
};

struct MoveRecord {
    CellIndex playerFrom = INVALID_CELL;
    CellIndex playerTo = INVALID_CELL;
    CellIndex boxFrom = INVALID_CELL; // INVALID_CELL if no box was pushed
    CellIndex boxTo = INVALID_CELL;
    CommandSource source = CommandSource::Human;
    Direction direction = Direction::Up;

    [[nodiscard]] bool pushedBox() const noexcept {
        return boxFrom != INVALID_CELL && boxTo != INVALID_CELL;
    }
};

struct MoveResult {
    bool success = false;
    bool pushedBox = false;
    CellIndex playerFrom = INVALID_CELL;
    CellIndex playerTo = INVALID_CELL;
    CellIndex boxFrom = INVALID_CELL;
    CellIndex boxTo = INVALID_CELL;
    bool levelCompleted = false;
    std::chrono::nanoseconds logicalMoveDuration{0};
};

} // namespace sokoban::core
