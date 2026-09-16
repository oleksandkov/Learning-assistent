#include "TestHarness.hpp"
#include "solvers/LevelGenerator.hpp"
#include "solvers/ReplayValidator.hpp"
#include "core/LevelParser.hpp"

SOKOBAN_TEST(Generator, SolvabilityAndReplayValidation) {
    sokoban::solvers::GeneratorOptions opt{
        .width = 7,
        .height = 7,
        .boxCount = 2,
        .maxAttempts = 50,
        .seed = 42
    };

    auto res = sokoban::solvers::LevelGenerator::generate(opt);
    SOKOBAN_ASSERT(res.success);
    SOKOBAN_ASSERT(res.attempts >= 1);
    SOKOBAN_ASSERT(res.solution.moveCount > 0);
    SOKOBAN_ASSERT(res.solution.pushCount > 0);

    // Verify ReplayValidator independently confirms this solution on the generated board
    auto val = sokoban::solvers::ReplayValidator::validate(res.board, res.initialState, res.solution);
    SOKOBAN_ASSERT(val.valid);
    SOKOBAN_ASSERT_EQ(val.actualMoves, res.solution.moveCount);
    SOKOBAN_ASSERT_EQ(val.actualPushes, res.solution.pushCount);
}

SOKOBAN_TEST(Generator, SeedDeterminism) {
    sokoban::solvers::GeneratorOptions opt1{
        .width = 6,
        .height = 6,
        .boxCount = 1,
        .maxAttempts = 30,
        .seed = 98765
    };

    sokoban::solvers::GeneratorOptions opt2 = opt1;

    auto res1 = sokoban::solvers::LevelGenerator::generate(opt1);
    auto res2 = sokoban::solvers::LevelGenerator::generate(opt2);

    SOKOBAN_ASSERT(res1.success);
    SOKOBAN_ASSERT(res2.success);
    SOKOBAN_ASSERT_EQ(res1.seedUsed, res2.seedUsed);
    SOKOBAN_ASSERT_EQ(res1.attempts, res2.attempts);
    SOKOBAN_ASSERT_EQ(res1.xsb, res2.xsb);
    SOKOBAN_ASSERT_EQ(res1.solution.moveCount, res2.solution.moveCount);
    SOKOBAN_ASSERT_EQ(res1.solution.pushCount, res2.solution.pushCount);
    SOKOBAN_ASSERT(res1.solution.moves == res2.solution.moves);
}

SOKOBAN_TEST(Generator, DifferentSeedsProduceDifferentLevels) {
    sokoban::solvers::GeneratorOptions opt1{
        .width = 7,
        .height = 7,
        .boxCount = 2,
        .maxAttempts = 50,
        .seed = 101
    };

    sokoban::solvers::GeneratorOptions opt2{
        .width = 7,
        .height = 7,
        .boxCount = 2,
        .maxAttempts = 50,
        .seed = 202
    };

    auto res1 = sokoban::solvers::LevelGenerator::generate(opt1);
    auto res2 = sokoban::solvers::LevelGenerator::generate(opt2);

    SOKOBAN_ASSERT(res1.success);
    SOKOBAN_ASSERT(res2.success);
    SOKOBAN_ASSERT(res1.seedUsed != res2.seedUsed);
    // The maps should not be identical for completely different seeds
    SOKOBAN_ASSERT(res1.xsb != res2.xsb || res1.solution.moves != res2.solution.moves);
}

SOKOBAN_TEST(Generator, SerializationAndReparseRoundtrip) {
    sokoban::solvers::GeneratorOptions opt{
        .width = 7,
        .height = 7,
        .boxCount = 2,
        .maxAttempts = 50,
        .seed = 303
    };

    auto res = sokoban::solvers::LevelGenerator::generate(opt);
    SOKOBAN_ASSERT(res.success);

    std::string xsb = sokoban::solvers::LevelGenerator::serializeToXsb(res.board, res.initialState, "Roundtrip Test");
    SOKOBAN_ASSERT(!xsb.empty());

    auto parsed = sokoban::core::LevelParser::parseString(xsb, "Roundtrip Test");
    SOKOBAN_ASSERT_EQ(parsed.board.width(), res.board.width());
    SOKOBAN_ASSERT_EQ(parsed.board.height(), res.board.height());
    SOKOBAN_ASSERT_EQ(parsed.initialState.player, res.initialState.player);
    SOKOBAN_ASSERT_EQ(parsed.initialState.boxes.size(), res.initialState.boxes.size());
    for (std::size_t i = 0; i < parsed.initialState.boxes.size(); ++i) {
        SOKOBAN_ASSERT_EQ(parsed.initialState.boxes[i], res.initialState.boxes[i]);
    }

    // Solution should also validate on re-parsed level
    auto val = sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, res.solution);
    SOKOBAN_ASSERT(val.valid);
}

SOKOBAN_TEST(Generator, MaxAttemptsExceededReturnsFailureSilently) {
    // Impossible options: 0 max attempts
    sokoban::solvers::GeneratorOptions opt{
        .width = 7,
        .height = 7,
        .boxCount = 2,
        .maxAttempts = 0,
        .seed = 999
    };

    auto res = sokoban::solvers::LevelGenerator::generate(opt);
    SOKOBAN_ASSERT(!res.success);
    SOKOBAN_ASSERT(!res.errorMessage.empty());
}

SOKOBAN_TEST(Generator, AllBoxesStartOffGoals) {
    sokoban::solvers::GeneratorOptions opt{
        .width = 12,
        .height = 12,
        .boxCount = 3,
        .maxAttempts = 50,
        .seed = 777
    };

    auto res = sokoban::solvers::LevelGenerator::generate(opt);
    SOKOBAN_ASSERT(res.success);
    SOKOBAN_ASSERT_EQ(res.initialState.boxes.size(), 3);
    for (auto bIdx : res.initialState.boxes) {
        SOKOBAN_ASSERT(!res.board.isGoal(bIdx));
    }
    SOKOBAN_ASSERT(res.solution.moveCount > 0);
    auto val = sokoban::solvers::ReplayValidator::validate(res.board, res.initialState, res.solution);
    SOKOBAN_ASSERT(val.valid);
}

