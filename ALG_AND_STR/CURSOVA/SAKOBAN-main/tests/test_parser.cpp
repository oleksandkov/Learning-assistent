#include "TestHarness.hpp"
#include "core/LevelParser.hpp"

SOKOBAN_TEST(Parser, ValidSimpleLevel) {
    std::string xsb = 
        "#####\n"
        "#@$.#\n"
        "#####\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT_EQ(parsed.board.goalCount(), 1);
    SOKOBAN_ASSERT_EQ(parsed.initialState.boxCount(), 1);
    SOKOBAN_ASSERT(!parsed.initialState.isGoal(parsed.board));
}

SOKOBAN_TEST(Parser, BoxAndPlayerOnGoalSymbols) {
    std::string xsb = 
        "######\n"
        "# +*$#\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT_EQ(parsed.board.goalCount(), 2);
    SOKOBAN_ASSERT_EQ(parsed.initialState.boxCount(), 2);
}

SOKOBAN_TEST(Parser, MissingPlayerThrows) {
    std::string xsb = 
        "#####\n"
        "# $.#\n"
        "#####\n";
    SOKOBAN_ASSERT_THROWS(sokoban::core::LevelParser::parseString(xsb), std::invalid_argument);
}

SOKOBAN_TEST(Parser, MultiplePlayersThrows) {
    std::string xsb = 
        "######\n"
        "#@$@.#\n"
        "######\n";
    SOKOBAN_ASSERT_THROWS(sokoban::core::LevelParser::parseString(xsb), std::invalid_argument);
}

SOKOBAN_TEST(Parser, MismatchedBoxesAndGoalsThrows) {
    std::string xsb = 
        "######\n"
        "#@$$.#\n"
        "######\n";
    SOKOBAN_ASSERT_THROWS(sokoban::core::LevelParser::parseString(xsb), std::invalid_argument);
}

SOKOBAN_TEST(Parser, ZeroBoxesThrows) {
    std::string xsb = 
        "#####\n"
        "# @ #\n"
        "#####\n";
    SOKOBAN_ASSERT_THROWS(sokoban::core::LevelParser::parseString(xsb), std::invalid_argument);
}

SOKOBAN_TEST(Parser, OpenBoundaryLeakThrows) {
    std::string xsb = 
        "#####\n"
        "  @$.#\n"
        "#####\n";
    SOKOBAN_ASSERT_THROWS(sokoban::core::LevelParser::parseString(xsb), std::invalid_argument);
}

SOKOBAN_TEST(Parser, InvalidCharThrows) {
    std::string xsb = 
        "#####\n"
        "#@$X#\n"
        "#####\n";
    SOKOBAN_ASSERT_THROWS(sokoban::core::LevelParser::parseString(xsb), std::invalid_argument);
}
