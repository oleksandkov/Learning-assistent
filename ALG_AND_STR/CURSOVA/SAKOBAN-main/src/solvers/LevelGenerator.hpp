#pragma once

#include "../core/Board.hpp"
#include "../core/GameState.hpp"
#include "ISolver.hpp"
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <chrono>

namespace sokoban::solvers {

struct GeneratorOptions {
    int width = 7;                                    // Повна ширина карти (за замовчуванням: 7)
    int height = 7;                                   // Повна висота карти (за замовчуванням: 7)
    int boxCount = 2;                                 // Кількість ящиків (за замовчуванням: 2, типово 1-3)
    std::size_t maxAttempts = 100;                    // Ліміт спроб генерації кандидата
    std::optional<std::uint64_t> seed = std::nullopt; // Опція seed для детермінізму та відтворюваності
    std::chrono::milliseconds solveTimeout{2000};     // Ліміт часу на розв'язання кандидата
    std::size_t nodeLimit = 30000;                    // Ліміт вузлів для валідації кандидата
    int reverseSteps = 35;                            // Кількість зворотних кроків витягування
};

struct GeneratedLevel {
    bool success = false;
    core::Board board;
    core::GameState initialState;
    Solution solution;
    std::string xsb;
    std::size_t attempts = 0;
    std::uint64_t seedUsed = 0;
    std::string errorMessage;
};

class LevelGenerator {
public:
    static GeneratedLevel generate(const GeneratorOptions& options = GeneratorOptions{});

    static std::string serializeToXsb(const core::Board& board,
                                      const core::GameState& state,
                                      const std::string& levelName = "");
};

} // namespace sokoban::solvers
