#pragma once

#include "../core/Types.hpp"
#include "../core/GameState.hpp"
#include <vector>
#include <cstdint>

namespace sokoban::solvers {

class Zobrist {
public:
    explicit Zobrist(std::size_t boardSize, std::uint64_t seed = 0x504b42414e3230ULL);

    [[nodiscard]] std::uint64_t computeHash(const core::GameState& state) const noexcept;
    [[nodiscard]] std::uint64_t computeBoxHash(const std::vector<core::CellIndex>& boxes) const noexcept;

    [[nodiscard]] std::uint64_t movePlayer(std::uint64_t hash, core::CellIndex from, core::CellIndex to) const noexcept {
        return hash ^ tablePlayer_[from] ^ tablePlayer_[to];
    }

    [[nodiscard]] std::uint64_t moveBox(std::uint64_t hash, core::CellIndex from, core::CellIndex to) const noexcept {
        return hash ^ tableBox_[from] ^ tableBox_[to];
    }

private:
    std::vector<std::uint64_t> tablePlayer_;
    std::vector<std::uint64_t> tableBox_;
};

} // namespace sokoban::solvers
