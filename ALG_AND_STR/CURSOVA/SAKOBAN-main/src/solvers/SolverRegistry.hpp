#pragma once

#include "ISolver.hpp"
#include <memory>
#include <string>
#include <vector>

namespace sokoban::solvers {

class SolverRegistry {
public:
    static std::unique_ptr<ISolver> create(SolverKind kind);
    static std::unique_ptr<ISolver> create(const std::string& name);

    static SolverKind parseKind(const std::string& name);
    static std::string toString(SolverKind kind);

    static OptimizationMetric parseMetric(const std::string& name);
    static std::string toString(OptimizationMetric metric);

    static std::vector<std::string> availableSolvers();
};

} // namespace sokoban::solvers
