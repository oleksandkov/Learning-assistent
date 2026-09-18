#include "TestHarness.hpp"
#include "core/LevelParser.hpp"
#include "solvers/BFSSolver.hpp"
#include "solvers/AStarSolver.hpp"
#include "solvers/IDAStarSolver.hpp"
#include "solvers/GreedySolver.hpp"
#include "solvers/Prototype1Solver.hpp"
#include "solvers/AntColonySolver.hpp"
#include "solvers/GeneticSolver.hpp"
#include "solvers/SolverRegistry.hpp"
#include "solvers/ReplayValidator.hpp"

SOKOBAN_TEST(Solvers, BFSOptimalityOnTinyLevel) {
    std::string xsb = 
        "######\n"
        "#  . #\n"
        "#  $ #\n"
        "#  @ #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::BFSSolver solver;
    sokoban::solvers::SolverOptions opt{
        .algorithm = sokoban::solvers::SolverKind::BFS,
        .metric = sokoban::solvers::OptimizationMetric::Moves
    };

    solver.start(parsed.board, parsed.initialState, opt);
    while (solver.advance(1000) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = solver.statistics();
    SOKOBAN_ASSERT_EQ(stats.status, sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT_EQ(stats.solutionMoves, 1);
    SOKOBAN_ASSERT_EQ(stats.solutionPushes, 1);

    auto sol = solver.solution();
    SOKOBAN_ASSERT(sol.has_value());
    auto valResult = sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, *sol);
    SOKOBAN_ASSERT(valResult.valid);
}

SOKOBAN_TEST(Solvers, AStarMovesMatchesBFSShortestMoves) {
    std::string xsb = 
        "#######\n"
        "#  .  #\n"
        "#  $  #\n"
        "#     #\n"
        "#  @  #\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    // Run BFS
    sokoban::solvers::BFSSolver bfs;
    sokoban::solvers::SolverOptions optBFS{
        .algorithm = sokoban::solvers::SolverKind::BFS,
        .metric = sokoban::solvers::OptimizationMetric::Moves
    };
    bfs.start(parsed.board, parsed.initialState, optBFS);
    while (bfs.advance(1000) == sokoban::solvers::SearchStatus::Running) {}
    auto bfsStats = bfs.statistics();
    SOKOBAN_ASSERT_EQ(bfsStats.status, sokoban::solvers::SearchStatus::Solved);

    // Run A* (Moves)
    sokoban::solvers::AStarSolver astar;
    sokoban::solvers::SolverOptions optAStar{
        .algorithm = sokoban::solvers::SolverKind::AStar,
        .metric = sokoban::solvers::OptimizationMetric::Moves
    };
    astar.start(parsed.board, parsed.initialState, optAStar);
    while (astar.advance(1000) == sokoban::solvers::SearchStatus::Running) {}
    auto astarStats = astar.statistics();
    SOKOBAN_ASSERT_EQ(astarStats.status, sokoban::solvers::SearchStatus::Solved);

    SOKOBAN_ASSERT_EQ(bfsStats.solutionMoves, astarStats.solutionMoves);

    auto solAStar = astar.solution();
    SOKOBAN_ASSERT(solAStar.has_value());
    auto valResult = sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, *solAStar);
    SOKOBAN_ASSERT(valResult.valid);
}

SOKOBAN_TEST(Solvers, AStarPushesMetric) {
    std::string xsb = 
        "#######\n"
        "#     #\n"
        "# $ # #\n"
        "# . # #\n"
        "#    @#\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::AStarSolver astar;
    sokoban::solvers::SolverOptions opt{
        .algorithm = sokoban::solvers::SolverKind::AStar,
        .metric = sokoban::solvers::OptimizationMetric::Pushes
    };
    astar.start(parsed.board, parsed.initialState, opt);
    while (astar.advance(1000) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = astar.statistics();
    SOKOBAN_ASSERT_EQ(stats.status, sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(stats.solutionPushes > 0);

    auto sol = astar.solution();
    SOKOBAN_ASSERT(sol.has_value());
    auto valResult = sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, *sol);
    SOKOBAN_ASSERT(valResult.valid);
}

SOKOBAN_TEST(Solvers, StatisticsInvariants) {
    std::string xsb = 
        "######\n"
        "#  . #\n"
        "#  $ #\n"
        "#  @ #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::AStarSolver astar;
    sokoban::solvers::SolverOptions opt;
    astar.start(parsed.board, parsed.initialState, opt);
    while (astar.advance(1000) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = astar.statistics();
    SOKOBAN_ASSERT_EQ(stats.status, sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(stats.generatedStates >= stats.exploredStates);
    SOKOBAN_ASSERT(stats.maxFrontierSize >= 1);
    SOKOBAN_ASSERT(stats.totalSolverTime.count() > 0);
}

SOKOBAN_TEST(Solvers, NodeLimitHandling) {
    std::string xsb = 
        "#######\n"
        "#     #\n"
        "# $ # #\n"
        "# . # #\n"
        "#    @#\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::BFSSolver bfs;
    sokoban::solvers::SolverOptions opt{
        .nodeLimit = 2
    };
    bfs.start(parsed.board, parsed.initialState, opt);
    while (bfs.advance(1) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = bfs.statistics();
    SOKOBAN_ASSERT_EQ(stats.status, sokoban::solvers::SearchStatus::LimitReached);
}

SOKOBAN_TEST(Solvers, UnsolvableReturnsNoSolution) {
    std::string xsb = 
        "######\n"
        "#$   #\n"
        "# @ .#\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::AStarSolver astar;
    sokoban::solvers::SolverOptions opt;
    astar.start(parsed.board, parsed.initialState, opt);
    while (astar.advance(1000) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = astar.statistics();
    SOKOBAN_ASSERT_EQ(stats.status, sokoban::solvers::SearchStatus::NoSolution);
}

SOKOBAN_TEST(Solvers, IDAStarOptimalPushesOnTinyLevel) {
    std::string xsb =
        "######\n"
        "#  . #\n"
        "#  $ #\n"
        "#  @ #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::IDAStarSolver solver;
    sokoban::solvers::SolverOptions opt{
        .algorithm = sokoban::solvers::SolverKind::IDAStar,
        .metric = sokoban::solvers::OptimizationMetric::Pushes
    };
    solver.start(parsed.board, parsed.initialState, opt);
    while (solver.advance(1000) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = solver.statistics();
    SOKOBAN_ASSERT_EQ(stats.status, sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT_EQ(stats.solutionPushes, 1);

    auto sol = solver.solution();
    SOKOBAN_ASSERT(sol.has_value());
    auto valResult = sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, *sol);
    SOKOBAN_ASSERT(valResult.valid);
}

SOKOBAN_TEST(Solvers, IDAStarMatchesAStarPushes) {
    std::string xsb =
        "#######\n"
        "#     #\n"
        "# $ # #\n"
        "# . # #\n"
        "#    @#\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::AStarSolver astar;
    sokoban::solvers::SolverOptions optA{
        .algorithm = sokoban::solvers::SolverKind::AStar,
        .metric = sokoban::solvers::OptimizationMetric::Pushes
    };
    astar.start(parsed.board, parsed.initialState, optA);
    while (astar.advance(1000) == sokoban::solvers::SearchStatus::Running) {}
    SOKOBAN_ASSERT_EQ(astar.statistics().status, sokoban::solvers::SearchStatus::Solved);

    sokoban::solvers::IDAStarSolver ida;
    sokoban::solvers::SolverOptions optI{
        .algorithm = sokoban::solvers::SolverKind::IDAStar,
        .metric = sokoban::solvers::OptimizationMetric::Pushes
    };
    ida.start(parsed.board, parsed.initialState, optI);
    while (ida.advance(1000) == sokoban::solvers::SearchStatus::Running) {}
    auto idaStats = ida.statistics();
    SOKOBAN_ASSERT_EQ(idaStats.status, sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT_EQ(idaStats.solutionPushes, astar.statistics().solutionPushes);

    auto sol = ida.solution();
    SOKOBAN_ASSERT(sol.has_value());
    SOKOBAN_ASSERT(sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, *sol).valid);
}

SOKOBAN_TEST(Solvers, GreedySolvesTinyLevelButNotOptimalClaim) {
    std::string xsb =
        "######\n"
        "#  . #\n"
        "#  $ #\n"
        "#  @ #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);

    sokoban::solvers::GreedySolver solver;
    sokoban::solvers::SolverOptions opt{
        .algorithm = sokoban::solvers::SolverKind::Greedy,
        .metric = sokoban::solvers::OptimizationMetric::Pushes
    };
    solver.start(parsed.board, parsed.initialState, opt);
    while (solver.advance(1000) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = solver.statistics();
    SOKOBAN_ASSERT_EQ(stats.status, sokoban::solvers::SearchStatus::Solved);
    auto sol = solver.solution();
    SOKOBAN_ASSERT(sol.has_value());
    SOKOBAN_ASSERT(sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, *sol).valid);
}

SOKOBAN_TEST(Solvers, RegistryCreatesIDAStarAndGreedy) {
    auto ida = sokoban::solvers::SolverRegistry::create("idastar");
    SOKOBAN_ASSERT(ida != nullptr);
    auto greedy = sokoban::solvers::SolverRegistry::create("greedy");
    SOKOBAN_ASSERT(greedy != nullptr);
    auto prototype = sokoban::solvers::SolverRegistry::create("prototype1");
    SOKOBAN_ASSERT(prototype != nullptr);
    SOKOBAN_ASSERT_EQ(sokoban::solvers::SolverRegistry::parseKind("IDA*"), sokoban::solvers::SolverKind::IDAStar);
    SOKOBAN_ASSERT_EQ(sokoban::solvers::SolverRegistry::parseKind("gbfs"), sokoban::solvers::SolverKind::Greedy);
    SOKOBAN_ASSERT_EQ(sokoban::solvers::SolverRegistry::toString(sokoban::solvers::SolverKind::IDAStar), "IDA*");
    SOKOBAN_ASSERT_EQ(sokoban::solvers::SolverRegistry::toString(sokoban::solvers::SolverKind::Greedy), "Greedy");
    SOKOBAN_ASSERT_EQ(sokoban::solvers::SolverRegistry::toString(sokoban::solvers::SolverKind::Prototype1), "Prototype 1");
}

SOKOBAN_TEST(Solvers, Prototype1SolvesOpenFieldWithTwoBoxes) {
    const std::string xsb =
        "#######\n"
        "#     #\n"
        "# $ $ #\n"
        "# . . #\n"
        "#  @  #\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    sokoban::solvers::Prototype1Solver solver;
    sokoban::solvers::SolverOptions options{
        .algorithm = sokoban::solvers::SolverKind::Prototype1,
        .metric = sokoban::solvers::OptimizationMetric::Pushes
    };
    solver.start(parsed.board, parsed.initialState, options);
    while (solver.advance(1) == sokoban::solvers::SearchStatus::Running) {}

    SOKOBAN_ASSERT_EQ(solver.statistics().status, sokoban::solvers::SearchStatus::Solved);
    auto result = solver.solution();
    SOKOBAN_ASSERT(result.has_value());
    SOKOBAN_ASSERT(sokoban::solvers::ReplayValidator::validate(
        parsed.board, parsed.initialState, *result).valid);
}

SOKOBAN_TEST(Solvers, Prototype1RejectsItsPlanThroughAWall) {
    const std::string xsb =
        "#######\n"
        "#@    #\n"
        "# $#. #\n"
        "#     #\n"
        "#######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    sokoban::solvers::Prototype1Solver solver;
    sokoban::solvers::SolverOptions options{
        .algorithm = sokoban::solvers::SolverKind::Prototype1,
        .metric = sokoban::solvers::OptimizationMetric::Pushes
    };
    solver.start(parsed.board, parsed.initialState, options);
    while (solver.advance(1) == sokoban::solvers::SearchStatus::Running) {}

    SOKOBAN_ASSERT_EQ(solver.statistics().status, sokoban::solvers::SearchStatus::NoSolution);
    SOKOBAN_ASSERT(!solver.solution().has_value());
}

SOKOBAN_TEST(Solvers, EvolutionaryPrototypesSolveTinyLevelAndKeepHistory) {
    const std::string xsb =
        "######\n"
        "#  . #\n"
        "#  $ #\n"
        "#  @ #\n"
        "######\n";
    auto parsed = sokoban::core::LevelParser::parseString(xsb);
    sokoban::solvers::SolverOptions options{
        .algorithm = sokoban::solvers::SolverKind::AntColony,
        .metric = sokoban::solvers::OptimizationMetric::Moves
    };

    sokoban::solvers::AntColonySolver aco;
    aco.start(parsed.board, parsed.initialState, options);
    while (aco.advance(1) == sokoban::solvers::SearchStatus::Running) {}
    SOKOBAN_ASSERT_EQ(aco.statistics().status, sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(!aco.history().empty());
    SOKOBAN_ASSERT(aco.solution().has_value());
    SOKOBAN_ASSERT(sokoban::solvers::ReplayValidator::validate(
        parsed.board, parsed.initialState, *aco.solution()).valid);

    options.algorithm = sokoban::solvers::SolverKind::Genetic;
    sokoban::solvers::GeneticSolver genetic;
    genetic.start(parsed.board, parsed.initialState, options);
    while (genetic.advance(1) == sokoban::solvers::SearchStatus::Running) {}
    SOKOBAN_ASSERT_EQ(genetic.statistics().status, sokoban::solvers::SearchStatus::Solved);
    SOKOBAN_ASSERT(!genetic.history().empty());
    SOKOBAN_ASSERT(genetic.solution().has_value());
    SOKOBAN_ASSERT(sokoban::solvers::ReplayValidator::validate(
        parsed.board, parsed.initialState, *genetic.solution()).valid);
}
