#include "TestHarness.hpp"
#include "core/LevelParser.hpp"
#include "core/GameSession.hpp"

SOKOBAN_TEST(UndoRedo, MoveUndoRedo) {
    std::string xsb = 
        "#######\n"
        "#  @$.#\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    sokoban::core::GameSession session(std::move(parsed.board), std::move(parsed.initialState));

    SOKOBAN_ASSERT(!session.canUndo());
    SOKOBAN_ASSERT(!session.canRedo());

    session.apply({ .direction = sokoban::core::Direction::Left });
    SOKOBAN_ASSERT_EQ(session.moveCount(), 1);
    SOKOBAN_ASSERT_EQ(session.pushCount(), 0);
    SOKOBAN_ASSERT(session.canUndo());
    SOKOBAN_ASSERT(!session.canRedo());

    SOKOBAN_ASSERT(session.undo());
    SOKOBAN_ASSERT_EQ(session.moveCount(), 0);
    SOKOBAN_ASSERT_EQ(session.currentState(), session.initialState());
    SOKOBAN_ASSERT(session.canRedo());

    SOKOBAN_ASSERT(session.redo());
    SOKOBAN_ASSERT_EQ(session.moveCount(), 1);
    SOKOBAN_ASSERT(!session.canRedo());
}

SOKOBAN_TEST(UndoRedo, PushUndoRedo) {
    std::string xsb = 
        "#######\n"
        "#  @$.#\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    sokoban::core::GameSession session(std::move(parsed.board), std::move(parsed.initialState));

    auto res = session.apply({ .direction = sokoban::core::Direction::Right });
    SOKOBAN_ASSERT(res.success);
    SOKOBAN_ASSERT(res.pushedBox);
    SOKOBAN_ASSERT_EQ(session.pushCount(), 1);
    SOKOBAN_ASSERT(session.isWon());

    SOKOBAN_ASSERT(session.undo());
    SOKOBAN_ASSERT_EQ(session.pushCount(), 0);
    SOKOBAN_ASSERT_EQ(session.moveCount(), 0);
    SOKOBAN_ASSERT(!session.isWon());
    SOKOBAN_ASSERT_EQ(session.currentState(), session.initialState());

    SOKOBAN_ASSERT(session.redo());
    SOKOBAN_ASSERT_EQ(session.pushCount(), 1);
    SOKOBAN_ASSERT(session.isWon());
}

SOKOBAN_TEST(UndoRedo, HistoryTruncationOnNewManualMove) {
    std::string xsb = 
        "#######\n"
        "#  @$.#\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    sokoban::core::GameSession session(std::move(parsed.board), std::move(parsed.initialState));

    session.apply({ .direction = sokoban::core::Direction::Left });
    session.undo();
    SOKOBAN_ASSERT(session.canRedo());

    session.apply({ .direction = sokoban::core::Direction::Right, .source = sokoban::core::CommandSource::Human });
    SOKOBAN_ASSERT(!session.canRedo());
}

SOKOBAN_TEST(UndoRedo, Restart) {
    std::string xsb = 
        "#######\n"
        "#  @$.#\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    sokoban::core::GameSession session(std::move(parsed.board), std::move(parsed.initialState));

    session.apply({ .direction = sokoban::core::Direction::Left });
    session.apply({ .direction = sokoban::core::Direction::Right });
    session.restart();

    SOKOBAN_ASSERT_EQ(session.moveCount(), 0);
    SOKOBAN_ASSERT_EQ(session.pushCount(), 0);
    SOKOBAN_ASSERT_EQ(session.currentState(), session.initialState());
    SOKOBAN_ASSERT(!session.canUndo());
    SOKOBAN_ASSERT(!session.canRedo());
}
