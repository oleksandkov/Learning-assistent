#include "TestHarness.hpp"
#include "core/LevelParser.hpp"
#include "solvers/DeadlockDetector.hpp"

SOKOBAN_TEST(Deadlock, NonGoalCornerIsDeadlock) {
    std::string xsb = 
        "######\n"
        "#$   #\n"
        "# @ .#\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT(sokoban::solvers::DeadlockDetector::hasCornerDeadlock(parsed.board, parsed.initialState));
    SOKOBAN_ASSERT(sokoban::solvers::DeadlockDetector::isDeadlock(parsed.board, parsed.initialState));
}

SOKOBAN_TEST(Deadlock, GoalCornerIsNotDeadlock) {
    std::string xsb = 
        "######\n"
        "#*   #\n"
        "# @  #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT(!sokoban::solvers::DeadlockDetector::hasCornerDeadlock(parsed.board, parsed.initialState));
    SOKOBAN_ASSERT(!sokoban::solvers::DeadlockDetector::hasStaticDeadSquare(parsed.board, parsed.initialState));
}

SOKOBAN_TEST(Deadlock, Block2x2Deadlock) {
    std::string xsb = 
        "######\n"
        "#    #\n"
        "# #$ #\n"
        "# #$ #\n"
        "#  @.#\n"
        "#  . #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT(sokoban::solvers::DeadlockDetector::has2x2Deadlock(parsed.board, parsed.initialState));
    SOKOBAN_ASSERT(sokoban::solvers::DeadlockDetector::isDeadlock(parsed.board, parsed.initialState));
}

SOKOBAN_TEST(Deadlock, Block2x2AllGoalsNotDeadlock) {
    std::string xsb = 
        "######\n"
        "#    #\n"
        "# #* #\n"
        "# #* #\n"
        "#  @ #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT(!sokoban::solvers::DeadlockDetector::has2x2Deadlock(parsed.board, parsed.initialState));
}

SOKOBAN_TEST(Deadlock, StaticDeadSquareDetection) {
    std::string xsb = 
        "########\n"
        "#    $ #\n"
        "# ###  #\n"
        "#   # @#\n"
        "#   #. #\n"
        "########\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT(sokoban::solvers::DeadlockDetector::hasStaticDeadSquare(parsed.board, parsed.initialState));
}

SOKOBAN_TEST(Deadlock, AssignmentDeadlock) {
    std::string xsb = 
        "#########\n"
        "#   # . #\n"
        "# $ # . #\n"
        "# $   @ #\n"
        "#########\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    SOKOBAN_ASSERT(sokoban::solvers::DeadlockDetector::isDeadlock(parsed.board, parsed.initialState, sokoban::core::INVALID_CELL, true));
}
