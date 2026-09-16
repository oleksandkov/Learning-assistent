#include "TestHarness.hpp"
#include "core/LevelParser.hpp"
#include "core/GameRules.hpp"

SOKOBAN_TEST(Rules, LegalStepAndWallCollision) {
    std::string xsb = 
        "######\n"
        "# @  #\n"
        "#  $ #\n"
        "#  . #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    const auto& board = parsed.board;
    auto state = parsed.initialState;

    SOKOBAN_ASSERT(!sokoban::core::GameRules::canMove(board, state, sokoban::core::Direction::Up));
    SOKOBAN_ASSERT(sokoban::core::GameRules::canMove(board, state, sokoban::core::Direction::Left));

    sokoban::core::GameState nextState;
    sokoban::core::MoveRecord rec;
    bool moved = sokoban::core::GameRules::tryMove(board, state, sokoban::core::Direction::Left, nextState, rec);
    SOKOBAN_ASSERT(moved);
    SOKOBAN_ASSERT(!rec.pushedBox());
    SOKOBAN_ASSERT_EQ(nextState.boxes, state.boxes);
}

SOKOBAN_TEST(Rules, LegalPushAndPushIntoWall) {
    std::string xsb = 
        "######\n"
        "#@$. #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    const auto& board = parsed.board;
    auto state = parsed.initialState;

    SOKOBAN_ASSERT(sokoban::core::GameRules::canMove(board, state, sokoban::core::Direction::Right));

    sokoban::core::GameState nextState;
    sokoban::core::MoveRecord rec;
    bool pushed = sokoban::core::GameRules::tryMove(board, state, sokoban::core::Direction::Right, nextState, rec);
    SOKOBAN_ASSERT(pushed);
    SOKOBAN_ASSERT(rec.pushedBox());
    SOKOBAN_ASSERT(nextState.isGoal(board));
    SOKOBAN_ASSERT(sokoban::core::GameRules::isWon(board, nextState));

    std::string wallGoalXsb = 
        "######\n"
        "#@$#.#\n"
        "######\n";
    auto parsed2 = sokoban::core::LevelParser::parseString(wallGoalXsb);
    SOKOBAN_ASSERT(!sokoban::core::GameRules::canMove(parsed2.board, parsed2.initialState, sokoban::core::Direction::Right));
}

SOKOBAN_TEST(Rules, TwoBoxesPushForbidden) {
    std::string xsb = 
        "#######\n"
        "#@$$. #\n"
        "#   . #\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT(!sokoban::core::GameRules::canMove(parsed.board, parsed.initialState, sokoban::core::Direction::Right));
}
