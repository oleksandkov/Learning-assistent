cmake_minimum_required(VERSION 3.20)
project(sokoban
    VERSION 1.0.0
    DESCRIPTION "High-performance C++20 Sokoban engine, solvers, and CLI"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings and options
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(MINGW)
    add_link_options(-static -static-libgcc -static-libstdc++)
endif()

# Core library (pure gameplay rules, boards, states, parser)
add_library(sokoban_core STATIC
    src/core/Types.hpp
    src/core/Board.hpp
    src/core/Board.cpp
    src/core/GameState.hpp
    src/core/GameState.cpp
    src/core/LevelParser.hpp
    src/core/LevelParser.cpp
    src/core/GameRules.hpp
    src/core/GameRules.cpp
    src/core/GameSession.hpp
    src/core/GameSession.cpp
)

target_include_directories(sokoban_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

# Solvers library (BFS, A*, IDA*, Greedy, deadlocks, Hungarian heuristic, Zobrist)
add_library(sokoban_solvers STATIC
    src/solvers/ISolver.hpp
    src/solvers/Zobrist.hpp
    src/solvers/Zobrist.cpp
    src/solvers/Hungarian.hpp
    src/solvers/Hungarian.cpp
    src/solvers/DeadlockDetector.hpp
    src/solvers/DeadlockDetector.cpp
    src/solvers/ReplayValidator.hpp
    src/solvers/ReplayValidator.cpp
    src/solvers/BFSSolver.hpp
    src/solvers/BFSSolver.cpp
    src/solvers/AStarSolver.hpp
    src/solvers/AStarSolver.cpp
    src/solvers/IDAStarSolver.hpp
    src/solvers/IDAStarSolver.cpp
    src/solvers/GreedySolver.hpp
    src/solvers/GreedySolver.cpp
    src/solvers/Prototype1Solver.hpp
    src/solvers/Prototype1Solver.cpp
    src/solvers/AntColonySolver.hpp
    src/solvers/AntColonySolver.cpp
    src/solvers/GeneticSolver.hpp
    src/solvers/GeneticSolver.cpp
    src/solvers/GravityDrainSolver.hpp
    src/solvers/GravityDrainSolver.cpp
    src/solvers/CannibalGeneticSolver.hpp
    src/solvers/CannibalGeneticSolver.cpp
    src/solvers/SolverRegistry.hpp
    src/solvers/SolverRegistry.cpp
    src/solvers/LevelGenerator.hpp
    src/solvers/LevelGenerator.cpp
)

target_include_directories(sokoban_solvers PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(sokoban_solvers PUBLIC sokoban_core)

# CLI executable
add_executable(sokoban_cli
    src/cli/TerminalUI.hpp
    src/cli/TerminalUI.cpp
    src/cli/AiClient.hpp
    src/cli/AiClient.cpp
    src/cli/BenchmarkRunner.hpp
    src/cli/BenchmarkRunner.cpp
    src/cli/AlgorithmComparator.hpp
    src/cli/AlgorithmComparator.cpp
    src/cli/Menu.hpp
    src/cli/Menu.cpp
    src/cli/main.cpp
)

target_include_directories(sokoban_cli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sokoban_cli PRIVATE sokoban_solvers sokoban_core)

# Testing
enable_testing()

add_executable(sokoban_tests
    src/cli/TerminalUI.hpp
    src/cli/TerminalUI.cpp
    src/cli/AiClient.hpp
    src/cli/AiClient.cpp
    src/cli/BenchmarkRunner.hpp
    src/cli/BenchmarkRunner.cpp
    src/cli/AlgorithmComparator.hpp
    src/cli/AlgorithmComparator.cpp
    src/cli/Menu.hpp
    src/cli/Menu.cpp
    tests/TestHarness.hpp
    tests/test_parser.cpp
    tests/test_rules.cpp
    tests/test_undo_redo.cpp
    tests/test_deadlocks.cpp
    tests/test_solvers.cpp
    tests/test_generator.cpp
    tests/test_comparator.cpp
    tests/test_ai.cpp
    tests/test_menu.cpp
    tests/main.cpp
)

target_include_directories(sokoban_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

target_link_libraries(sokoban_tests PRIVATE sokoban_solvers sokoban_core)

add_test(NAME sokoban_unit_tests COMMAND sokoban_tests)
set_tests_properties(sokoban_unit_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
