#include "Hungarian.hpp"
#include <algorithm>

namespace sokoban::solvers {

std::pair<std::int64_t, std::vector<int>> Hungarian::solve(const std::vector<std::vector<std::int64_t>>& costMatrix) {
    int n = static_cast<int>(costMatrix.size());
    if (n == 0) {
        return {0, {}};
    }
    int m = static_cast<int>(costMatrix[0].size());
    if (n > m) {
        return {INF_COST, {}};
    }

    std::vector<std::int64_t> u(n + 1, 0);
    std::vector<std::int64_t> v(m + 1, 0);
    std::vector<int> p(m + 1, 0);
    std::vector<int> way(m + 1, 0);

    for (int i = 1; i <= n; ++i) {
        p[0] = i;
        int j0 = 0;
        std::vector<std::int64_t> minv(m + 1, INF_COST);
        std::vector<bool> used(m + 1, false);

        do {
            used[j0] = true;
            int i0 = p[j0];
            std::int64_t delta = INF_COST;
            int j1 = 0;

            for (int j = 1; j <= m; ++j) {
                if (!used[j]) {
                    std::int64_t cur = costMatrix[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < minv[j]) {
                        minv[j] = cur;
                        way[j] = j0;
                    }
                    if (minv[j] < delta) {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            }

            for (int j = 0; j <= m; ++j) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (p[j0] != 0);

        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0 != 0);
    }

    std::vector<int> matching(n, -1);
    for (int j = 1; j <= m; ++j) {
        if (p[j] > 0 && p[j] <= n) {
            matching[p[j] - 1] = j - 1;
        }
    }

    std::int64_t totalCost = -v[0];
    return {totalCost, matching};
}

} // namespace sokoban::solvers
