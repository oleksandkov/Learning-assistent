#pragma once

#include "../core/Board.hpp"
#include "../core/GameState.hpp"
#include "../core/GameSession.hpp"
#include "../core/LevelParser.hpp"
#include <string>
#include <optional>

namespace sokoban::cli {

enum class KeyCommand {
    None,
    Up,
    Down,
    Left,
    Right,
    Undo,
    Redo,
    Restart,
    Hint,
    Solve,
    SolveBFS,
    SolveAStar,
    SolveMoves,   // A* з метрикою Moves
    SolvePushes,  // A* з метрикою Pushes (= Solve)
    SolveIDAStar, // IDA* штовхання (good: оптимальний, мало пам'яті)
    SolveGreedy,  // Greedy штовхання (bad: швидко, неоптимально)
    SolvePrototype1, // навчальний X -> Y baseline без урахування стін
    SolveAI,      // Зовнішній AI (авто: Groq або Google за ключем)
    CompareAll,   // порівняти всі алгоритми з поточної позиції (як у web)
    SetAiKey,     // ввести/оновити AI API-ключ
    TogglePause,
    NextStep,
    ToggleCamera,
    Save,
    Quit
};

class TerminalUI {
public:
    TerminalUI();
    ~TerminalUI();

    void clearScreen();
    void render(const core::GameSession& session,
                const std::string& modeName,
                const std::string& levelName,
                const std::string& statusMessage = "",
                const std::string& hintMessage = "",
                double playerTimeSec = 0.0,
                std::optional<double> aiSolveTimeMs = std::nullopt,
                const std::vector<std::string>& sidebarSteps = {},
                int activeStepIndex = -1,
                bool enableCamera = false);

    [[nodiscard]] KeyCommand readKey();
    [[nodiscard]] bool hasInput() const;

    [[nodiscard]] bool ansiSupported() const noexcept { return ansiSupported_; }

private:
    void enableVirtualTerminal();
    void disableVirtualTerminal();

    bool ansiSupported_ = true;
#ifdef _WIN32
    unsigned long originalOutMode_ = 0;
    unsigned int originalOutputCP_ = 0;
    unsigned int originalCP_ = 0;
#endif
};

int runGame(core::ParsedLevel parsed);

} // namespace sokoban::cli
