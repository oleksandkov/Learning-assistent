#include "SolverRegistry.hpp"
#include "BFSSolver.hpp"
#include "AStarSolver.hpp"
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
    throw std::invalid_argument("Unknown solver algorithm name: " + name + ". Available: bfs, astar");
}

std::string SolverRegistry::toString(SolverKind kind) {
    switch (kind) {
        case SolverKind::BFS: return "BFS";
        case SolverKind::AStar: return "A*";
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
    return {"bfs", "astar"};
}

} // namespace sokoban::solvers
