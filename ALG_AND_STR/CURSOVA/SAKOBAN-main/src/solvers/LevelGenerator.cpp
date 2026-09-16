#include "LevelGenerator.hpp"
#include "AStarSolver.hpp"
#include "ReplayValidator.hpp"
#include "../core/LevelParser.hpp"
#include <random>
#include <algorithm>
#include <queue>
#include <sstream>

namespace sokoban::solvers {

namespace {

struct Point {
    int x = 0;
    int y = 0;
    bool operator==(const Point& other) const noexcept {
        return x == other.x && y == other.y;
    }
};

const Point DIRS[4] = {
    {0, -1}, // Up
    {0, 1},  // Down
    {-1, 0}, // Left
    {1, 0}   // Right
};

} // namespace

std::string LevelGenerator::serializeToXsb(const core::Board& board,
                                          const core::GameState& state,
                                          const std::string& levelName) {
    std::string out;
    if (!levelName.empty()) {
        out += "; " + levelName + "\n";
    }

    int minX = board.width(), maxX = -1;
    int minY = board.height(), maxY = -1;
    for (int y = 0; y < board.height(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
            core::CellIndex idx = board.toIndex(x, y);
            bool isPlayer = (state.player == idx);
            bool hasBox = state.hasBox(idx);
            bool isGoal = board.isGoal(idx);
            bool isWall = board.isWall(idx);
            if (isPlayer || hasBox || isGoal || isWall) {
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
                minY = std::min(minY, y);
                maxY = std::max(maxY, y);
            }
        }
    }

    if (maxX < minX || maxY < minY) {
        return out;
    }

    for (int y = minY; y <= maxY; ++y) {
        std::string row;
        for (int x = minX; x <= maxX; ++x) {
            core::CellIndex idx = board.toIndex(x, y);
            bool isPlayer = (state.player == idx);
            bool hasBox = state.hasBox(idx);
            bool isGoal = board.isGoal(idx);
            bool isWall = board.isWall(idx);

            if (isPlayer) {
                row += isGoal ? '+' : '@';
            } else if (hasBox) {
                row += isGoal ? '*' : '$';
            } else if (isGoal) {
                row += '.';
            } else if (isWall) {
                row += '#';
            } else {
                row += ' ';
            }
        }
        out += row + "\n";
    }
    return out;
}

GeneratedLevel LevelGenerator::generate(const GeneratorOptions& options) {
    std::uint64_t seedUsed = options.seed.value_or(
        static_cast<std::uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
    );
    std::mt19937_64 rng(seedUsed);

    int w = std::clamp(options.width, 5, 100);
    int h = std::clamp(options.height, 5, 100);
    int innerCells = (w - 2) * (h - 2);
    int boxes = std::clamp(options.boxCount, 1, std::max(1, (innerCells - 3) / 4));

    for (std::size_t attempt = 1; attempt <= options.maxAttempts; ++attempt) {
        // 1. Initialize grid with outer walls
        std::vector<std::string> grid(h, std::string(w, ' '));
        for (int y = 0; y < h; ++y) {
            grid[y][0] = '#';
            grid[y][w - 1] = '#';
        }
        for (int x = 0; x < w; ++x) {
            grid[0][x] = '#';
            grid[h - 1][x] = '#';
        }

        // 2. Optionally place random internal wall blocks
        int maxWalls = std::min(20, std::max(0, (innerCells - (boxes * 3 + 4)) / 6));
        int numWalls = (maxWalls > 0) ? std::uniform_int_distribution<int>(0, maxWalls)(rng) : 0;

        for (int i = 0; i < numWalls; ++i) {
            int wx = std::uniform_int_distribution<int>(2, w - 3)(rng);
            int wy = std::uniform_int_distribution<int>(2, h - 3)(rng);
            grid[wy][wx] = '#';
        }

        // 3. Flood-fill check connectivity of all floor cells
        std::vector<Point> floorCells;
        for (int y = 1; y < h - 1; ++y) {
            for (int x = 1; x < w - 1; ++x) {
                if (grid[y][x] == ' ') {
                    floorCells.push_back({x, y});
                }
            }
        }

        if (floorCells.size() < static_cast<std::size_t>(boxes * 2 + 2)) {
            continue;
        }

        std::vector<std::vector<bool>> visited(h, std::vector<bool>(w, false));
        std::queue<Point> q;
        q.push(floorCells[0]);
        visited[floorCells[0].y][floorCells[0].x] = true;
        std::size_t reached = 0;

        while (!q.empty()) {
            Point curr = q.front();
            q.pop();
            reached++;

            for (const auto& d : DIRS) {
                int nx = curr.x + d.x;
                int ny = curr.y + d.y;
                if (nx > 0 && nx < w - 1 && ny > 0 && ny < h - 1 && grid[ny][nx] == ' ' && !visited[ny][nx]) {
                    visited[ny][nx] = true;
                    q.push({nx, ny});
                }
            }
        }

        if (reached != floorCells.size()) {
            continue;
        }

        // 4. Choose goals (for large boards, cluster goals around a localized center to ensure compact, fast solvable puzzles)
        if (w > 12 || h > 12) {
            int minXBound = std::max(6, w / 3);
            int maxXBound = std::min(w - 7, 2 * w / 3);
            int minYBound = std::max(6, h / 3);
            int maxYBound = std::min(h - 7, 2 * h / 3);
            if (minXBound > maxXBound) std::swap(minXBound, maxXBound);
            if (minYBound > maxYBound) std::swap(minYBound, maxYBound);
            int cx = std::uniform_int_distribution<int>(minXBound, maxXBound)(rng);
            int cy = std::uniform_int_distribution<int>(minYBound, maxYBound)(rng);
            std::sort(floorCells.begin(), floorCells.end(), [cx, cy](const Point& a, const Point& b) {
                return (std::abs(a.x - cx) + std::abs(a.y - cy)) < (std::abs(b.x - cx) + std::abs(b.y - cy));
            });
            std::size_t poolSize = std::min(floorCells.size(), static_cast<std::size_t>(boxes * 16 + 20));
            std::shuffle(floorCells.begin(), floorCells.begin() + poolSize, rng);
        } else {
            std::shuffle(floorCells.begin(), floorCells.end(), rng);
        }
        std::vector<Point> goals(floorCells.begin(), floorCells.begin() + boxes);
        std::vector<Point> currentBoxes = goals;

        // 5. Choose player starting adjacent to a box
        Point player{-1, -1};
        for (std::size_t fi = boxes; fi < floorCells.size(); ++fi) {
            Point cand = floorCells[fi];
            bool adj = false;
            for (const auto& d : DIRS) {
                Point nb{cand.x + d.x, cand.y + d.y};
                if (std::find(currentBoxes.begin(), currentBoxes.end(), nb) != currentBoxes.end()) {
                    adj = true;
                    break;
                }
            }
            if (adj) {
                player = cand;
                break;
            }
        }
        if (player.x == -1) {
            player = floorCells[boxes];
        }

        // 6. Reverse walk with reachable-component box pulling
        struct PullAction {
            std::size_t bIdx = 0;
            Point newBox{0, 0};
            Point newPlayer{0, 0};
        };

        auto countOnGoals = [&]() {
            int c = 0;
            for (const auto& b : currentBoxes) {
                if (std::find(goals.begin(), goals.end(), b) != goals.end()) {
                    c++;
                }
            }
            return c;
        };

        int maxSteps = std::max(options.reverseSteps, static_cast<int>(boxes * 10 + 20));
        for (int step = 0; step < maxSteps; ++step) {
            // BFS to find all floor cells reachable by the player without passing through boxes
            std::vector<std::vector<bool>> reach(h, std::vector<bool>(w, false));
            std::queue<Point> q;
            reach[player.y][player.x] = true;
            q.push(player);

            while (!q.empty()) {
                Point curr = q.front();
                q.pop();

                for (const auto& d : DIRS) {
                    int nx = curr.x + d.x;
                    int ny = curr.y + d.y;
                    if (nx > 0 && nx < w - 1 && ny > 0 && ny < h - 1 && grid[ny][nx] == ' ') {
                        Point np{nx, ny};
                        bool hasBox = (std::find(currentBoxes.begin(), currentBoxes.end(), np) != currentBoxes.end());
                        if (!hasBox && !reach[ny][nx]) {
                            reach[ny][nx] = true;
                            q.push(np);
                        }
                    }
                }
            }

            // Find all valid reverse pulls from any reachable position
            // To pull box at B in direction d:
            //   - player stands at pPos = B + d (must be reachable by player)
            //   - player steps back to pDest = B + 2*d (must be empty floor, not a box)
            //   - box moves to pPos
            std::vector<PullAction> catOffGoal;   // Pulling a box off a goal to non-goal floor
            std::vector<PullAction> catScramble;  // Pulling a box already off goal to non-goal floor
            std::vector<PullAction> catGoalToGoal;// Pulling a box from goal to another goal (fallback)

            for (std::size_t bi = 0; bi < currentBoxes.size(); ++bi) {
                Point B = currentBoxes[bi];
                bool wasOnGoal = (std::find(goals.begin(), goals.end(), B) != goals.end());

                for (const auto& d : DIRS) {
                    Point pPos{B.x + d.x, B.y + d.y};
                    Point pDest{B.x + 2 * d.x, B.y + 2 * d.y};

                    if (pPos.x > 0 && pPos.x < w - 1 && pPos.y > 0 && pPos.y < h - 1 &&
                        pDest.x > 0 && pDest.x < w - 1 && pDest.y > 0 && pDest.y < h - 1) {
                        if (reach[pPos.y][pPos.x] && grid[pDest.y][pDest.x] == ' ') {
                            bool boxAtDest = (std::find(currentBoxes.begin(), currentBoxes.end(), pDest) != currentBoxes.end());
                            if (!boxAtDest) {
                                bool landsOnGoal = (std::find(goals.begin(), goals.end(), pPos) != goals.end());
                                if (wasOnGoal && !landsOnGoal) {
                                    catOffGoal.push_back({bi, pPos, pDest});
                                } else if (!wasOnGoal && !landsOnGoal) {
                                    catScramble.push_back({bi, pPos, pDest});
                                } else if (wasOnGoal && landsOnGoal) {
                                    catGoalToGoal.push_back({bi, pPos, pDest});
                                }
                            }
                        }
                    }
                }
            }

            int onG = countOnGoals();
            if (onG > 0) {
                if (!catOffGoal.empty()) {
                    std::uniform_int_distribution<std::size_t> dist(0, catOffGoal.size() - 1);
                    auto chosen = catOffGoal[dist(rng)];
                    currentBoxes[chosen.bIdx] = chosen.newBox;
                    player = chosen.newPlayer;
                } else if (!catScramble.empty()) {
                    std::uniform_int_distribution<std::size_t> dist(0, catScramble.size() - 1);
                    auto chosen = catScramble[dist(rng)];
                    currentBoxes[chosen.bIdx] = chosen.newBox;
                    player = chosen.newPlayer;
                } else if (!catGoalToGoal.empty()) {
                    std::uniform_int_distribution<std::size_t> dist(0, catGoalToGoal.size() - 1);
                    auto chosen = catGoalToGoal[dist(rng)];
                    currentBoxes[chosen.bIdx] = chosen.newBox;
                    player = chosen.newPlayer;
                } else {
                    break;
                }
            } else {
                // All boxes already moved off goals (0 on goals!)
                // Continue scrambling for up to 40 additional steps to disperse boxes further
                if (step > 40) {
                    break;
                }
                if (!catScramble.empty()) {
                    std::uniform_int_distribution<std::size_t> dist(0, catScramble.size() - 1);
                    auto chosen = catScramble[dist(rng)];
                    currentBoxes[chosen.bIdx] = chosen.newBox;
                    player = chosen.newPlayer;
                } else {
                    break;
                }
            }
        }

        // 7. Check if puzzle is non-trivial: ALL boxes must be moved off goals (0 boxes starting on goals)
        int onGoalCount = countOnGoals();
        if (onGoalCount > 0) {
            continue;
        }

        // 8. Build XSB representation
        std::string xsbStr;
        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (grid[y][x] == '#') {
                    xsbStr += '#';
                    continue;
                }
                Point p{x, y};
                bool isGoal = (std::find(goals.begin(), goals.end(), p) != goals.end());
                bool isBox = (std::find(currentBoxes.begin(), currentBoxes.end(), p) != currentBoxes.end());
                bool isPl = (player == p);

                if (isPl) {
                    xsbStr += isGoal ? '+' : '@';
                } else if (isBox) {
                    xsbStr += isGoal ? '*' : '$';
                } else if (isGoal) {
                    xsbStr += '.';
                } else {
                    xsbStr += ' ';
                }
            }
            xsbStr += '\n';
        }

        // 9. Parse candidate level
        core::ParsedLevel parsed;
        try {
            parsed = core::LevelParser::parseString(xsbStr, "Random Level");
        } catch (...) {
            continue;
        }

        // 10. Solve candidate level
        AStarSolver solver;
        SolverOptions opt{
            .algorithm = SolverKind::AStar,
            .metric = OptimizationMetric::Pushes,
            .timeLimit = options.solveTimeout,
            .nodeLimit = options.nodeLimit,
            .memoryLimitBytes = 256 * 1024 * 1024,
            .enableDeadlockDetection = true,
            .enableSafeMode = true
        };

        solver.start(parsed.board, parsed.initialState, opt);
        while (solver.advance(2000) == SearchStatus::Running) {}

        auto stats = solver.statistics();
        if (stats.status != SearchStatus::Solved) {
            continue;
        }

        auto sol = solver.solution();
        if (!sol.has_value() || sol->moveCount == 0 || sol->pushCount == 0) {
            continue;
        }

        // 11. Validate candidate solution with ReplayValidator
        auto valResult = ReplayValidator::validate(parsed.board, parsed.initialState, *sol);
        if (!valResult.valid) {
            continue;
        }

        GeneratedLevel result;
        result.success = true;
        result.board = std::move(parsed.board);
        result.initialState = std::move(parsed.initialState);
        result.solution = std::move(*sol);
        result.xsb = std::move(xsbStr);
        result.attempts = attempt;
        result.seedUsed = seedUsed;
        return result;
    }

    GeneratedLevel fail;
    fail.success = false;
    fail.attempts = options.maxAttempts;
    fail.seedUsed = seedUsed;
    fail.errorMessage = "Ліміт спроб вичерпано.";
    return fail;
}

} // namespace sokoban::solvers
