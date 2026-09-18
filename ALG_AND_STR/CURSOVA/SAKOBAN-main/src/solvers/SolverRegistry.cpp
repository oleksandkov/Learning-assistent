#include "SolverRegistry.hpp"
#include "BFSSolver.hpp"
#include "AStarSolver.hpp"
#include "IDAStarSolver.hpp"
#include "GreedySolver.hpp"
#include "Prototype1Solver.hpp"
#include "AntColonySolver.hpp"
#include "GeneticSolver.hpp"
#include <algorithm>
#include <stdexcept>

namespace sokoban::solvers {

namespace {
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}
} // namespace

std::unique_ptr<ISolver> SolverRegistry::create(SolverKind kind) {
    switch (kind) {
        case SolverKind::BFS:
            return std::make_unique<BFSSolver>();
        case SolverKind::AStar:
            return std::make_unique<AStarSolver>();
        case SolverKind::IDAStar:
            return std::make_unique<IDAStarSolver>();
        case SolverKind::Greedy:
            return std::make_unique<GreedySolver>();
        case SolverKind::Prototype1:
            return std::make_unique<Prototype1Solver>();
        case SolverKind::AntColony:
            return std::make_unique<AntColonySolver>();
        case SolverKind::Genetic:
            return std::make_unique<GeneticSolver>();
    }
    throw std::invalid_argument("Unknown solver kind");
}

std::unique_ptr<ISolver> SolverRegistry::create(const std::string& name) {
    return create(parseKind(name));
}

SolverKind SolverRegistry::parseKind(const std::string& name) {
    std::string s = toLower(name);
    if (s == "bfs") {
        return SolverKind::BFS;
    }
    if (s == "astar" || s == "a*" || s == "a_star") {
        return SolverKind::AStar;
    }
    if (s == "idastar" || s == "ida*" || s == "ida_star" || s == "ida") {
        return SolverKind::IDAStar;
    }
    if (s == "greedy" || s == "gbfs" || s == "best-first" || s == "bestfirst" || s == "best_first") {
        return SolverKind::Greedy;
    }
    if (s == "prototype1" || s == "prototype-1" || s == "prototype_1") {
        return SolverKind::Prototype1;
    }
    throw std::invalid_argument("Unknown solver algorithm name: " + name + ". Available: bfs, astar, idastar, greedy, prototype1");
}

std::string SolverRegistry::toString(SolverKind kind) {
    switch (kind) {
        case SolverKind::BFS: return "BFS";
        case SolverKind::AStar: return "A*";
        case SolverKind::IDAStar: return "IDA*";
        case SolverKind::Greedy: return "Greedy";
        case SolverKind::Prototype1: return "Prototype 1";
        case SolverKind::AntColony: return "ACO";
        case SolverKind::Genetic: return "Genetic";
    }
    return "Unknown";
}

OptimizationMetric SolverRegistry::parseMetric(const std::string& name) {
    std::string s = toLower(name);
    if (s == "moves" || s == "move") {
        return OptimizationMetric::Moves;
    }
    if (s == "pushes" || s == "push") {
        return OptimizationMetric::Pushes;
    }
    throw std::invalid_argument("Unknown optimization metric: " + name + ". Available: moves, pushes");
}

std::string SolverRegistry::toString(OptimizationMetric metric) {
    switch (metric) {
        case OptimizationMetric::Moves: return "Moves";
        case OptimizationMetric::Pushes: return "Pushes";
    }
    return "Unknown";
}

std::vector<std::string> SolverRegistry::availableSolvers() {
    return {"bfs", "astar", "idastar", "greedy", "prototype1"};
}

} // namespace sokoban::solvers
