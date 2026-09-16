#pragma once

#include <vector>
#include <cstdint>
#include <limits>

namespace sokoban::solvers {

class Hungarian {
public:
    static constexpr std::int64_t INF_COST = 1'000'000'000LL;

    // Solves minimum weight bipartite matching for N rows and M cols (N <= M)
    // Returns pair: {min_total_cost, matching where matching[row] = col}
    static std::pair<std::int64_t, std::vector<int>> solve(const std::vector<std::vector<std::int64_t>>& costMatrix);
};

} // namespace sokoban::solvers
