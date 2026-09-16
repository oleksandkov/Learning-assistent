#include "Zobrist.hpp"

namespace sokoban::solvers {

namespace {
// Deterministic 64-bit PRNG (SplitMix64)
std::uint64_t splitmix64(std::uint64_t& state) {
    state += 0x9e3779b97f4a7c15ULL;
    std::uint64_t z = state;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
} // namespace

Zobrist::Zobrist(std::size_t boardSize, std::uint64_t seed)
    : tablePlayer_(boardSize), tableBox_(boardSize) {
    std::uint64_t rng = seed;
    for (std::size_t i = 0; i < boardSize; ++i) {
        tablePlayer_[i] = splitmix64(rng);
        tableBox_[i] = splitmix64(rng);
    }
}

std::uint64_t Zobrist::computeHash(const core::GameState& state) const noexcept {
    std::uint64_t h = (state.player < tablePlayer_.size()) ? tablePlayer_[state.player] : 0ULL;
    for (core::CellIndex b : state.boxes) {
        if (b < tableBox_.size()) {
            h ^= tableBox_[b];
        }
    }
    return h;
}

std::uint64_t Zobrist::computeBoxHash(const std::vector<core::CellIndex>& boxes) const noexcept {
    std::uint64_t h = 0ULL;
    for (core::CellIndex b : boxes) {
        if (b < tableBox_.size()) {
            h ^= tableBox_[b];
        }
    }
    return h;
}

} // namespace sokoban::solvers
