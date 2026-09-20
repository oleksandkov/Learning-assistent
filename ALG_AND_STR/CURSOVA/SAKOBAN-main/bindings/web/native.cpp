#include "core/LevelParser.hpp"
#include "core/GameSession.hpp"
#include "solvers/SolverRegistry.hpp"
#include "solvers/ReplayValidator.hpp"
#include "solvers/LevelGenerator.hpp"
#include "solvers/AntColonySolver.hpp"
#include "solvers/GeneticSolver.hpp"
#include "solvers/GravityDrainSolver.hpp"
#include "solvers/CannibalGeneticSolver.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

namespace {

struct WebTracePoint {
    sokoban::solvers::SolverProgress progress;
    std::optional<sokoban::solvers::SolverDebugState> debug;
};

std::string jsonString(const std::string& value) {
    std::ostringstream out;
    out << '"';
    for (unsigned char ch : value) {
        switch (ch) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (ch < 0x20) {
                    out << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                        << static_cast<int>(ch) << std::dec << std::setfill(' ');
                } else {
                    out << ch;
                }
        }
    }
    out << '"';
    return out.str();
}

int internalWallLimit(int width, int height, int boxes) {
    const int innerCells = (width - 2) * (height - 2);
    return std::min(20, std::max(0, (innerCells - (boxes * 3 + 4)) / 6));
}

int countInternalWalls(const std::string& xsb, int width, int height) {
    int total = static_cast<int>(std::count(xsb.begin(), xsb.end(), '#'));
    return total - (2 * width + 2 * (height - 2));
}

int generateLevel(int argc, char** argv) {
    using namespace sokoban::solvers;
    if (argc < 6) return 2;
    const int width = std::stoi(argv[2]);
    const int height = std::stoi(argv[3]);
    const int boxes = std::stoi(argv[4]);
    const int requestedWalls = std::stoi(argv[5]);
    const int maxWalls = internalWallLimit(width, height, boxes);
    if (width < 5 || width > 100 || height < 5 || height > 100 || boxes < 1 ||
        boxes > 50 || requestedWalls < 0 || requestedWalls > maxWalls) {
        std::cout << "{\"success\":false,\"error\":\"InvalidGeneratorOptions\"}";
        return 0;
    }

    const std::uint64_t startSeed = argc > 6 && std::string(argv[6]).size() > 0
        ? std::stoull(argv[6])
        : static_cast<std::uint64_t>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::size_t testedCandidates = 0;
    constexpr std::size_t generationAttempts = 120;

    // LevelGenerator draws the internal-wall count first. Pick reproducible seeds
    // that draw the requested value, then let the shared generator construct,
    // solve and ReplayValidator-check the candidate without copying its rules.
    for (std::uint64_t offset = 0; offset < 10000 && testedCandidates < generationAttempts; ++offset) {
        const std::uint64_t candidateSeed = startSeed + offset;
        std::mt19937_64 probe(candidateSeed);
        const int sampledWalls = maxWalls > 0
            ? std::uniform_int_distribution<int>(0, maxWalls)(probe)
            : 0;
        if (sampledWalls != requestedWalls) continue;
        ++testedCandidates;

        GeneratorOptions options{
            .width = width,
            .height = height,
            .boxCount = boxes,
            .maxAttempts = 1,
            .seed = candidateSeed,
            .solveTimeout = std::chrono::milliseconds(1800),
            .nodeLimit = 40000,
            .reverseSteps = 25 + boxes * 6
        };
        auto generated = LevelGenerator::generate(options);
        if (!generated.success || countInternalWalls(generated.xsb, width, height) != requestedWalls)
            continue;

        std::cout << "{\"success\":true,\"xsb\":" << jsonString(generated.xsb)
            << ",\"width\":" << width
            << ",\"height\":" << height
            << ",\"boxes\":" << boxes
            << ",\"walls\":" << requestedWalls
            << ",\"attempts\":" << testedCandidates
            << ",\"seed\":\"" << generated.seedUsed << "\""
            << ",\"moves\":" << generated.solution.moveCount
            << ",\"pushes\":" << generated.solution.pushCount
            << ",\"solution\":\"";
        for (auto direction : generated.solution.moves)
            std::cout << directionToChar(direction);
        std::cout << "\"}";
        return 0;
    }

    std::cout << "{\"success\":false,\"error\":\"GenerationLimit\",\"attempts\":"
        << testedCandidates << ",\"seed\":\"" << startSeed << "\"}";
    return 0;
}

} // namespace

// stdin: XSB. argv: mode, algorithm, history, optional direction.
// No independent rules: all transitions and validation use the shared core.
int main(int argc, char** argv) {
    using namespace sokoban::core;
    using namespace sokoban::solvers;
    try {
        if (argc > 1 && std::string(argv[1]) == "generate") return generateLevel(argc, argv);
        if (argc < 4) return 2;
        const std::string mode = argv[1], algorithm = argv[2], history = argv[3];
        const std::string xsb((std::istreambuf_iterator<char>(std::cin)), {});
        auto parsed = LevelParser::parseString(xsb);
        GameSession game(parsed.board, parsed.initialState);
        for (char d : history) {
            if (!game.apply({charToDirection(d), CommandSource::Replay}).success) {
                std::cout << "{\"error\":\"InvalidHistory\"}";
                return 0;
            }
        }
        std::cout << std::boolalpha << std::setprecision(12);
        if (mode == "solve") {
            SolverOptions options;
            if (algorithm == "bfs") {
                options.algorithm = SolverKind::BFS;
                options.metric = OptimizationMetric::Moves;
            } else if (algorithm == "astar-pushes") {
                options.algorithm = SolverKind::AStar;
                options.metric = OptimizationMetric::Pushes;
            } else if (algorithm == "astar-moves") {
                options.algorithm = SolverKind::AStar;
                options.metric = OptimizationMetric::Moves;
            } else if (algorithm == "idastar" || algorithm == "idastar-pushes") {
                options.algorithm = SolverKind::IDAStar;
                options.metric = OptimizationMetric::Pushes;
            } else if (algorithm == "idastar-moves") {
                options.algorithm = SolverKind::IDAStar;
                options.metric = OptimizationMetric::Moves;
            } else if (algorithm == "greedy" || algorithm == "greedy-pushes") {
                options.algorithm = SolverKind::Greedy;
                options.metric = OptimizationMetric::Pushes;
            } else if (algorithm == "prototype1") {
                options.algorithm = SolverKind::Prototype1;
                options.metric = OptimizationMetric::Pushes;
            } else if (algorithm == "aco") {
                options.algorithm = SolverKind::AntColony;
                options.metric = OptimizationMetric::Moves;
            } else if (algorithm == "genetic") {
                options.algorithm = SolverKind::Genetic;
                options.metric = OptimizationMetric::Moves;
            } else if (algorithm == "gravity") {
                options.algorithm = SolverKind::GravityDrain;
                options.metric = OptimizationMetric::Moves;
            } else if (algorithm == "cannibal") {
                options.algorithm = SolverKind::CannibalGenetic;
                options.metric = OptimizationMetric::Moves;
            } else {
                options.algorithm = SolverKind::AStar;
                options.metric = OptimizationMetric::Pushes;
            }
            options.collectDebugState = true;
            const bool extendedSearch = game.board().size() > 2500 || game.currentState().boxes.size() > 6;
            options.timeLimit = std::chrono::milliseconds(extendedSearch ? 30000 : 8000);
            options.nodeLimit = extendedSearch ? 2000000 : 250000;
            options.memoryLimitBytes = (extendedSearch ? 512ULL : 256ULL) * 1024 * 1024;
            auto solver = SolverRegistry::create(options.algorithm);
            solver->start(game.board(), game.currentState(), options);
            std::vector<WebTracePoint> trace;
            trace.push_back({solver->progress(), solver->debugState()});
            while (true) {
                const std::size_t nodeBudget = trace.size() < 160 ? 1 : 1000;
                const auto status = solver->advance(nodeBudget);
                const auto point = solver->progress();
                if (trace.size() < 160 || status != SearchStatus::Running) {
                    if (trace.empty() || trace.back().progress.exploredStates != point.exploredStates ||
                        trace.back().progress.frontierSize != point.frontierSize ||
                        trace.back().progress.status != point.status) {
                        trace.push_back({point, solver->debugState()});
                    }
                }
                if (status != SearchStatus::Running) break;
            }
            auto stats = solver->statistics();
            auto solution = solver->solution();
            bool valid = solution && ReplayValidator::validate(game.board(), game.currentState(), *solution).valid;
            if (stats.status == SearchStatus::Solved && !valid) stats.status = SearchStatus::InternalError;
            std::cout << "{\"status\":\"" << statusToString(stats.status) << "\",\"moves\":\"";
            if (valid) for (auto d : solution->moves) std::cout << directionToChar(d);
            auto ms = [](auto time) { return std::chrono::duration<double, std::milli>(time).count(); };
            std::cout << "\",\"pushes\":" << (valid ? solution->pushCount : 0)
                << ",\"searchMs\":" << ms(stats.searchTime)
                << ",\"preprocessingMs\":" << ms(stats.preprocessingTime)
                << ",\"validationMs\":" << ms(stats.validationTime)
                << ",\"reconstructionMs\":" << ms(stats.reconstructionTime)
                << ",\"totalMs\":" << ms(stats.totalSolverTime)
                << ",\"explored\":" << stats.exploredStates
                << ",\"generated\":" << stats.generatedStates
                << ",\"deadlocks\":" << stats.deadlockPruned
                << ",\"frontier\":" << stats.maxFrontierSize
                << ",\"validated\":" << valid
                << ",\"trace\":[";
            for (std::size_t i = 0; i < trace.size(); ++i) {
                if (i) std::cout << ',';
                const auto& progress = trace[i].progress;
                std::cout << "{\"explored\":" << progress.exploredStates
                    << ",\"frontier\":" << progress.frontierSize
                    << ",\"elapsedMs\":" << ms(progress.elapsedSearchTime)
                    << ",\"status\":\"" << statusToString(progress.status) << "\"";
                if (trace[i].debug) {
                    const auto& debug = *trace[i].debug;
                    std::cout << ",\"state\":{\"player\":" << debug.state.player << ",\"boxes\":[";
                    for (std::size_t box = 0; box < debug.state.boxes.size(); ++box) {
                        if (box) std::cout << ',';
                        std::cout << debug.state.boxes[box];
                    }
                    std::cout << "]},\"g\":" << debug.g
                        << ",\"h\":" << debug.h
                        << ",\"depth\":" << debug.depth
                        << ",\"hasTransition\":" << debug.hasTransition
                        << ",\"pushed\":" << debug.pushed
                        << ",\"direction\":\"" << directionToChar(debug.direction) << "\""
                        << ",\"boxFrom\":";
                    if (debug.boxFrom == INVALID_CELL) std::cout << "null";
                    else std::cout << debug.boxFrom;
                    std::cout << ",\"boxTo\":";
                    if (debug.boxTo == INVALID_CELL) std::cout << "null";
                    else std::cout << debug.boxTo;
                }
                std::cout << '}';
            }
            std::cout << ']';
            auto printMoves = [](const auto& moves) {
                for (auto direction : moves) std::cout << directionToChar(direction);
            };
            if (const auto* aco = dynamic_cast<const AntColonySolver*>(solver.get())) {
                std::cout << ",\"evolution\":{\"kind\":\"aco\",\"generations\":[";
                const auto& history = aco->history();
                for (std::size_t generation = 0; generation < history.size(); ++generation) {
                    if (generation) std::cout << ',';
                    std::cout << '[';
                    for (std::size_t ant = 0; ant < history[generation].size(); ++ant) {
                        if (ant) std::cout << ',';
                        const auto& item = history[generation][ant];
                        std::cout << "{\"id\":" << item.antId << ",\"moves\":\"";
                        printMoves(item.moves);
                        std::cout << "\",\"cost\":" << item.cost
                            << ",\"pushes\":" << item.pushes
                            << ",\"won\":" << item.won << '}';
                    }
                    std::cout << ']';
                }
                std::cout << "]}";
            } else if (const auto* genetic = dynamic_cast<const GeneticSolver*>(solver.get())) {
                std::cout << ",\"evolution\":{\"kind\":\"genetic\",\"generations\":[";
                const auto& history = genetic->history();
                for (std::size_t generation = 0; generation < history.size(); ++generation) {
                    if (generation) std::cout << ',';
                    std::cout << '[';
                    for (std::size_t index = 0; index < history[generation].size(); ++index) {
                        if (index) std::cout << ',';
                        const auto& item = history[generation][index];
                        std::cout << "{\"id\":" << item.id << ",\"moves\":\"";
                        printMoves(item.moves);
                        std::cout << "\",\"genes\":\"";
                        printMoves(item.genes);
                        std::cout << "\",\"cost\":" << item.fitness
                            << ",\"won\":" << item.won
                            << ",\"parentA\":" << item.parentA
                            << ",\"parentB\":" << item.parentB
                            << ",\"mutationIndex\":" << item.mutationIndex << '}';
                    }
                    std::cout << ']';
                }
                std::cout << "]}";
            } else if (const auto* drain = dynamic_cast<const GravityDrainSolver*>(solver.get())) {
                std::cout << ",\"evolution\":{\"kind\":\"gravity\",\"generations\":[";
                const auto& history = drain->history();
                for (std::size_t generation = 0; generation < history.size(); ++generation) {
                    if (generation) std::cout << ',';
                    std::cout << '[';
                    for (std::size_t index = 0; index < history[generation].size(); ++index) {
                        if (index) std::cout << ',';
                        const auto& item = history[generation][index];
                        std::cout << "{\"id\":" << item.id << ",\"moves\":\"";
                        printMoves(item.moves);
                        std::cout << "\",\"cost\":" << item.cost
                            << ",\"pushes\":" << item.pushes
                            << ",\"shakes\":" << item.shakes
                            << ",\"won\":" << item.won << '}';
                    }
                    std::cout << ']';
                }
                std::cout << "]}";
            } else if (const auto* cannibal = dynamic_cast<const CannibalGeneticSolver*>(solver.get())) {
                std::cout << ",\"evolution\":{\"kind\":\"cannibal\",\"generations\":[";
                const auto& history = cannibal->history();
                for (std::size_t generation = 0; generation < history.size(); ++generation) {
                    if (generation) std::cout << ',';
                    std::cout << '[';
                    for (std::size_t index = 0; index < history[generation].size(); ++index) {
                        if (index) std::cout << ',';
                        const auto& item = history[generation][index];
                        std::cout << "{\"id\":" << item.id << ",\"moves\":\"";
                        printMoves(item.moves);
                        std::cout << "\",\"genes\":\"";
                        printMoves(item.genes);
                        std::cout << "\",\"cost\":" << item.fitness
                            << ",\"won\":" << item.won
                            << ",\"parentA\":" << item.predatorId
                            << ",\"parentB\":" << item.consumedId
                            << ",\"consumedId\":" << item.consumedId
                            << ",\"fatalGene\":" << item.fatalGene
                            << ",\"validPrefix\":" << item.validPrefix
                            << ",\"mutationIndex\":" << item.mutationIndex << '}';
                    }
                    std::cout << ']';
                }
                std::cout << "]}";
            }
            std::cout << '}';
            return 0;
        }
        bool accepted = true;
        if (argc > 4 && std::string(argv[4]).size() == 1)
            accepted = game.apply({charToDirection(argv[4][0]), CommandSource::Human}).success;
        const auto& board = game.board();
        const auto& state = game.currentState();
        std::cout << "{\"accepted\":" << accepted << ",\"won\":" << game.isWon()
            << ",\"moves\":" << game.moveCount() << ",\"pushes\":" << game.pushCount()
            << ",\"board\":{\"width\":" << board.width() << ",\"height\":" << board.height();
        for (int mask = 0; mask < 3; ++mask) {
            std::cout << (mask == 0 ? ",\"walls\":[" : mask == 1 ? ",\"goals\":[" : ",\"floor\":[");
            for (std::size_t i = 0; i < board.size(); ++i) {
                if (i) std::cout << ',';
                std::cout << (mask == 0 ? board.isWall(i) : mask == 1 ? board.isGoal(i) : board.isFloor(i));
            }
            std::cout << ']';
        }
        std::cout << "},\"state\":{\"player\":" << state.player << ",\"boxes\":[";
        for (std::size_t i = 0; i < state.boxes.size(); ++i) {
            if (i) std::cout << ',';
            std::cout << state.boxes[i];
        }
        std::cout << "]}}";
    } catch (const std::exception& error) {
        std::cerr << error.what();
        return 1;
    }
}
