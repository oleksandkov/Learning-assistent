#include "LevelParser.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <queue>
#include <iostream>

namespace sokoban::core {

namespace {
bool isXsbChar(char c) {
    return c == '#' || c == ' ' || c == '.' || c == '$' || c == '*' || c == '@' || c == '+';
}
} // namespace

ParsedLevel LevelParser::parseFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open level file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseString(buffer.str(), filePath);
}

ParsedLevel LevelParser::parseString(const std::string& content, const std::string& levelName) {
    std::istringstream stream(content);
    std::string line;
    std::vector<std::string> mapLines;
    std::string title = levelName;

    while (std::getline(stream, line)) {
        // Strip trailing \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Comments or metadata
        if (!line.empty() && (line[0] == ';' || line[0] == '\'')) {
            if (title.empty() || title == levelName) {
                // Extract possible title
                auto pos = line.find_first_not_of(";'\t ");
                if (pos != std::string::npos) {
                    title = line.substr(pos);
                }
            }
            continue;
        }

        // Empty line before level starts
        if (line.empty() && mapLines.empty()) {
            continue;
        }

        // Check if line contains XSB characters
        bool hasXsb = false;
        for (char c : line) {
            if (c == '#' || c == '.' || c == '$' || c == '*' || c == '@' || c == '+') {
                hasXsb = true;
                break;
            }
        }

        if (hasXsb) {
            // Validate characters
            for (std::size_t col = 0; col < line.size(); ++col) {
                char c = line[col];
                if (!isXsbChar(c)) {
                    throw std::invalid_argument(std::string("Invalid character '") + c + "' in level map.");
                }
            }
            mapLines.push_back(line);
        } else if (!mapLines.empty()) {
            // Empty line after map began might mean end of level or separator
            // If the rest has more levels or comments, we stop here for single level
            break;
        }
    }

    if (mapLines.empty()) {
        throw std::invalid_argument("Empty level: no valid Sokoban map found.");
    }

    int rawH = static_cast<int>(mapLines.size());
    int rawW = 0;
    for (const auto& l : mapLines) {
        rawW = std::max(rawW, static_cast<int>(l.size()));
    }

    // Pad with 1-cell border around the map to safely identify exterior space
    int W = rawW + 2;
    int H = rawH + 2;

    std::vector<std::string> grid(H, std::string(W, ' '));
    for (int y = 0; y < rawH; ++y) {
        for (int x = 0; x < static_cast<int>(mapLines[y].size()); ++x) {
            grid[y + 1][x + 1] = mapLines[y][x];
        }
    }

    // Flood fill from (0, 0) to mark all reachable empty cells as exterior
    std::vector<std::vector<bool>> isExterior(H, std::vector<bool>(W, false));
    std::queue<std::pair<int, int>> q;
    isExterior[0][0] = true;
    q.push({0, 0});

    const int dx[] = { 0, -1, 0, 1 };
    const int dy[] = { -1, 0, 1, 0 };

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();

        for (int d = 0; d < 4; ++d) {
            int nx = cx + dx[d];
            int ny = cy + dy[d];
            if (nx >= 0 && nx < W && ny >= 0 && ny < H) {
                // Exterior can expand into any cell that is NOT a wall '#'
                if (!isExterior[ny][nx] && grid[ny][nx] != '#') {
                    isExterior[ny][nx] = true;
                    q.push({nx, ny});
                }
            }
        }
    }

    // Counters and positions
    int playerCount = 0;
    std::pair<int, int> playerCoord = {-1, -1};
    std::vector<std::pair<int, int>> boxCoords;
    std::vector<std::pair<int, int>> goalCoords;

    for (int y = 1; y <= rawH; ++y) {
        for (int x = 1; x <= rawW; ++x) {
            char c = grid[y][x];
            bool hasPlayer = (c == '@' || c == '+');
            bool hasBox = (c == '$' || c == '*');
            bool hasGoal = (c == '.' || c == '*' || c == '+');

            if (hasPlayer) {
                if (isExterior[y][x]) {
                    throw std::invalid_argument("Level validation error: player is in exterior unbounded space.");
                }
                playerCount++;
                playerCoord = {x, y};
            }

            if (hasBox) {
                if (isExterior[y][x]) {
                    throw std::invalid_argument("Level validation error: box is in exterior unbounded space.");
                }
                boxCoords.push_back({x, y});
            }

            if (hasGoal) {
                if (isExterior[y][x]) {
                    throw std::invalid_argument("Level validation error: goal is in exterior unbounded space.");
                }
                goalCoords.push_back({x, y});
            }
        }
    }

    if (playerCount != 1) {
        throw std::invalid_argument("Level validation error: must have exactly 1 player, found: " + std::to_string(playerCount));
    }

    if (boxCoords.empty()) {
        throw std::invalid_argument("Level validation error: level must have at least 1 box.");
    }

    if (boxCoords.size() != goalCoords.size()) {
        throw std::invalid_argument("Level validation error: box count (" + std::to_string(boxCoords.size()) +
                                    ") does not match goal count (" + std::to_string(goalCoords.size()) + ").");
    }

    // Build Board
    Board board(W, H);
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            CellIndex idx = board.toIndex(x, y);
            char c = grid[y][x];
            if (c == '#') {
                board.setWall(idx, true);
            } else if (!isExterior[y][x]) {
                board.setFloor(idx, true);
                if (c == '.' || c == '*' || c == '+') {
                    board.setGoal(idx, true);
                }
            }
        }
    }

    board.finalize();

    // Build initial GameState
    CellIndex playerIdx = board.toIndex(playerCoord.first, playerCoord.second);
    std::vector<CellIndex> boxIndices;
    boxIndices.reserve(boxCoords.size());
    for (const auto& [bx, by] : boxCoords) {
        boxIndices.push_back(board.toIndex(bx, by));
    }

    GameState initialState(playerIdx, boxIndices);

    return ParsedLevel{
        .board = std::move(board),
        .initialState = std::move(initialState),
        .name = std::move(title)
    };
}

} // namespace sokoban::core
