#pragma once

#include "Types.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <limits>
#include <string>

namespace sokoban::core {

class Board {
public:
    static constexpr std::uint32_t INF_DISTANCE = 1'000'000'000;

    Board() = default;
    Board(int width, int height);

    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }
    [[nodiscard]] std::size_t size() const noexcept { return static_cast<std::size_t>(width_) * height_; }

    [[nodiscard]] CellIndex toIndex(int x, int y) const noexcept {
        return static_cast<CellIndex>(y * width_ + x);
    }

    [[nodiscard]] int toX(CellIndex idx) const noexcept {
        return static_cast<int>(idx % width_);
    }

    [[nodiscard]] int toY(CellIndex idx) const noexcept {
        return static_cast<int>(idx / width_);
    }

    [[nodiscard]] bool inBounds(int x, int y) const noexcept {
        return x >= 0 && x < width_ && y >= 0 && y < height_;
    }

    [[nodiscard]] bool isWall(CellIndex idx) const noexcept {
        return idx < size() && walls_[idx];
    }

    [[nodiscard]] bool isGoal(CellIndex idx) const noexcept {
        return idx < size() && goalsMask_[idx];
    }

    [[nodiscard]] bool isFloor(CellIndex idx) const noexcept {
        return idx < size() && floorMask_[idx];
    }

    [[nodiscard]] bool isDeadSquare(CellIndex idx) const noexcept {
        return idx < size() && deadSquares_[idx];
    }

    [[nodiscard]] const std::vector<CellIndex>& goals() const noexcept {
        return goals_;
    }

    [[nodiscard]] std::size_t goalCount() const noexcept {
        return goals_.size();
    }

    [[nodiscard]] CellIndex neighbor(CellIndex idx, Direction dir) const noexcept {
        if (idx >= size()) return INVALID_CELL;
        return neighbors_[idx][static_cast<std::size_t>(dir)];
    }

    [[nodiscard]] std::uint32_t reversePushDistance(std::size_t goalIdx, CellIndex cell) const noexcept {
        if (goalIdx >= reversePushDistances_.size() || cell >= size()) {
            return INF_DISTANCE;
        }
        return reversePushDistances_[goalIdx][cell];
    }

    [[nodiscard]] std::uint32_t minReversePushDistance(CellIndex cell) const noexcept {
        if (cell >= size()) return INF_DISTANCE;
        return minReversePushDistances_[cell];
    }

    // Builder methods (used by parser)
    void setWall(CellIndex idx, bool val = true);
    void setGoal(CellIndex idx, bool val = true);
    void setFloor(CellIndex idx, bool val = true);
    void finalize();

private:
    void computeNeighbors();
    void computeReversePushDistances();

    int width_ = 0;
    int height_ = 0;

    std::vector<bool> walls_;
    std::vector<bool> goalsMask_;
    std::vector<bool> floorMask_;
    std::vector<bool> deadSquares_;
    std::vector<CellIndex> goals_;

    // [idx][direction]
    std::vector<std::array<CellIndex, 4>> neighbors_;

    // [goalIndex][cellIndex]
    std::vector<std::vector<std::uint32_t>> reversePushDistances_;
    std::vector<std::uint32_t> minReversePushDistances_;
};

} // namespace sokoban::core
