#pragma once

#include "Board.hpp"
#include "GameState.hpp"
#include <string>
#include <vector>
#include <optional>

namespace sokoban::core {

struct ParsedLevel {
    Board board;
    GameState initialState;
    std::string name;
    std::string xsbSource;
    bool isCustom = false;
};

class LevelParser {
public:
    static ParsedLevel parseString(const std::string& content, const std::string& levelName = "");
    static ParsedLevel parseFile(const std::string& filePath);
};

} // namespace sokoban::core
