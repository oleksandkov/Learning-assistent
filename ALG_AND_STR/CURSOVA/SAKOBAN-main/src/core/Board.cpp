#include "Board.hpp"
#include <queue>
#include <algorithm>

namespace sokoban::core {

namespace {
constexpr int DX[] = { 0, -1, 0, 1 };
constexpr int DY[] = { -1, 0, 1, 0 };
} // namespace

Board::Board(int width, int height)
    : width_(width),
      height_(height),
      walls_(static_cast<std::size_t>(width) * height, false),
      goalsMask_(static_cast<std::size_t>(width) * height, false),
      floorMask_(static_cast<std::size_t>(width) * height, false),
      deadSquares_(static_cast<std::size_t>(width) * height, false) {
}

void Board::setWall(CellIndex idx, bool val) {
    if (idx < size()) {
        walls_[idx] = val;
    }
}

void Board::setGoal(CellIndex idx, bool val) {
    if (idx < size()) {
        goalsMask_[idx] = val;
    }
}

void Board::setFloor(CellIndex idx, bool val) {
    if (idx < size()) {
        floorMask_[idx] = val;
    }
}

void Board::finalize() {
    goals_.clear();
    for (CellIndex i = 0; i < size(); ++i) {
        if (goalsMask_[i]) {
            goals_.push_back(i);
        }
    }
    std::sort(goals_.begin(), goals_.end());

    computeNeighbors();
    computeReversePushDistances();
}

void Board::computeNeighbors() {
    neighbors_.assign(size(), { INVALID_CELL, INVALID_CELL, INVALID_CELL, INVALID_CELL });

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            CellIndex idx = toIndex(x, y);
            if (!isFloor(idx)) {
                continue;
            }

            for (std::size_t d = 0; d < 4; ++d) {
                int nx = x + DX[d];
                int ny = y + DY[d];
                if (inBounds(nx, ny)) {
                    CellIndex nIdx = toIndex(nx, ny);
                    if (isFloor(nIdx)) {
                        neighbors_[idx][d] = nIdx;
                    }
                }
            }
        }
    }
}

void Board::computeReversePushDistances() {
    const std::size_t K = goals_.size();
    reversePushDistances_.assign(K, std::vector<std::uint32_t>(size(), INF_DISTANCE));
    minReversePushDistances_.assign(size(), INF_DISTANCE);
    deadSquares_.assign(size(), false);

    for (std::size_t g = 0; g < K; ++g) {
        CellIndex goalCell = goals_[g];
        std::vector<std::uint32_t>& dist = reversePushDistances_[g];
        std::queue<CellIndex> q;

        dist[goalCell] = 0;
        q.push(goalCell);

        while (!q.empty()) {
            CellIndex curr = q.front();
            q.pop();

            int cx = toX(curr);
            int cy = toY(curr);
            std::uint32_t curDist = dist[curr];

            for (int d = 0; d < 4; ++d) {
                int dx = DX[d];
                int dy = DY[d];

                // Reverse push:
                // Forward push was: player at (prevX - dx, prevY - dy) pushed box at (prevX, prevY) in direction (dx, dy) to curr (cx, cy).
                // So prevX = cx - dx, prevY = cy - dy.
                // playerX = prevX - dx = cx - 2*dx, playerY = prevY - dy = cy - 2*dy.
                int prevX = cx - dx;
                int prevY = cy - dy;
                int playerX = prevX - dx;
                int playerY = prevY - dy;

                if (inBounds(prevX, prevY) && inBounds(playerX, playerY)) {
                    CellIndex prevIdx = toIndex(prevX, prevY);
                    CellIndex playerIdx = toIndex(playerX, playerY);

                    if (isFloor(prevIdx) && isFloor(playerIdx)) {
                        if (dist[prevIdx] > curDist + 1) {
                            dist[prevIdx] = curDist + 1;
                            q.push(prevIdx);
                        }
                    }
                }
            }
        }
    }

    for (CellIndex i = 0; i < size(); ++i) {
        if (!isFloor(i)) {
            continue;
        }
        std::uint32_t minD = INF_DISTANCE;
        for (std::size_t g = 0; g < K; ++g) {
            minD = std::min(minD, reversePushDistances_[g][i]);
        }
        minReversePushDistances_[i] = minD;

        // A floor cell is a static dead square if it cannot reach any goal and is not already a goal.
        if (!isGoal(i) && minD == INF_DISTANCE) {
            deadSquares_[i] = true;
        }
    }
}

} // namespace sokoban::core
