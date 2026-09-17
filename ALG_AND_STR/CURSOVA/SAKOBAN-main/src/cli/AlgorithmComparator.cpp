#include "AlgorithmComparator.hpp"
#include "../core/LevelParser.hpp"
#include "../solvers/BFSSolver.hpp"
#include "../solvers/AStarSolver.hpp"
#include "../solvers/IDAStarSolver.hpp"
#include "../solvers/GreedySolver.hpp"
#include "../solvers/ReplayValidator.hpp"
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/ioctl.h>
#endif

namespace sokoban::cli {

namespace {
double toMs(std::chrono::nanoseconds ns) {
    return static_cast<double>(ns.count()) / 1'000'000.0;
}

std::size_t utf8Length(const std::string& s) {
    std::size_t count = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80) {
            count++;
        }
    }
    return count;
}

std::string padRight(const std::string& s, std::size_t width) {
    std::size_t len = utf8Length(s);
    std::string res = s;
    if (width > len) {
        res.append(width - len, ' ');
    }
    return res;
}

std::string padLeft(const std::string& s, std::size_t width) {
    std::size_t len = utf8Length(s);
    std::string res;
    if (width > len) {
        res.append(width - len, ' ');
    }
    res += s;
    return res;
}

std::string formatDouble(double val, int precision) {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << val;
    return ss.str();
}

} // namespace

LevelComparisonReport AlgorithmComparator::compareFile(const std::string& levelPath,
                                                       std::chrono::milliseconds timeLimit,
                                                       std::size_t nodeLimit) {
    core::ParsedLevel parsed = core::LevelParser::parseFile(levelPath);
    std::string name = std::filesystem::path(levelPath).filename().string();
    return compare(parsed.board, parsed.initialState, name, timeLimit, nodeLimit);
}

LevelComparisonReport AlgorithmComparator::compare(const core::Board& board,
                                                   const core::GameState& initialState,
                                                   const std::string& levelName,
                                                   std::chrono::milliseconds timeLimit,
                                                   std::size_t nodeLimit) {
    LevelComparisonReport report;
    report.levelName = levelName;
    report.width = board.width();
    report.height = board.height();
    report.boxCount = static_cast<int>(initialState.boxes.size());

    struct RunTarget {
        std::string name;
        solvers::SolverKind kind;
        solvers::OptimizationMetric metric;
        std::string optimalityTag;
    };

    const RunTarget targets[5] = {
        {"BFS", solvers::SolverKind::BFS, solvers::OptimizationMetric::Moves, "Yes (Moves)"},
        {"A* (Moves)", solvers::SolverKind::AStar, solvers::OptimizationMetric::Moves, "Yes (Moves)"},
        {"A* (Pushes)", solvers::SolverKind::AStar, solvers::OptimizationMetric::Pushes, "Yes (Pushes)"},
        {"IDA* (Pushes)", solvers::SolverKind::IDAStar, solvers::OptimizationMetric::Pushes, "Yes (Pushes)"},
        {"Greedy", solvers::SolverKind::Greedy, solvers::OptimizationMetric::Pushes, "No"}
    };

    for (const auto& target : targets) {
        AlgorithmComparisonResult res;
        res.algorithmName = target.name;
        res.kind = target.kind;
        res.metric = target.metric;

        std::unique_ptr<solvers::ISolver> solver;
        if (target.kind == solvers::SolverKind::BFS) {
            solver = std::make_unique<solvers::BFSSolver>();
        } else if (target.kind == solvers::SolverKind::IDAStar) {
            solver = std::make_unique<solvers::IDAStarSolver>();
        } else if (target.kind == solvers::SolverKind::Greedy) {
            solver = std::make_unique<solvers::GreedySolver>();
        } else {
            solver = std::make_unique<solvers::AStarSolver>();
        }

        solvers::SolverOptions opt{
            .algorithm = target.kind,
            .metric = target.metric,
            .timeLimit = timeLimit,
            .nodeLimit = nodeLimit,
            .memoryLimitBytes = 512 * 1024 * 1024,
            .enableDeadlockDetection = true,
            .enableSafeMode = true
        };

        solver->start(board, initialState, opt);
        while (solver->advance(10000) == solvers::SearchStatus::Running) {}

        auto stats = solver->statistics();
        res.status = stats.status;
        res.searchTime = stats.searchTime;
        res.exploredStates = stats.exploredStates;
        res.generatedStates = stats.generatedStates;
        res.deadlockPruned = stats.deadlockPruned;

        auto sol = solver->solution();
        if (stats.status == solvers::SearchStatus::Solved && sol.has_value()) {
            auto valResult = solvers::ReplayValidator::validate(board, initialState, *sol);
            res.replayValid = valResult.valid;
            if (valResult.valid) {
                res.moves = sol->moveCount;
                res.pushes = sol->pushCount;
                res.isOptimal = (target.optimalityTag.rfind("Yes", 0) == 0);
                res.optimalityDescription = target.optimalityTag;
                res.solution = sol;
            } else {
                res.replayErrorMessage = valResult.errorMessage;
                res.isOptimal = false;
                res.optimalityDescription = "Validation Failed";
            }
        } else {
            res.replayValid = false;
            res.isOptimal = false;
            res.optimalityDescription = (stats.status == solvers::SearchStatus::LimitReached) ? "Limit Reached" : "N/A";
        }

        report.results.push_back(std::move(res));
    }

    return report;
}

void AlgorithmComparator::printTable(const LevelComparisonReport& report,
                                     std::ostream& out,
                                     bool ansiSupported,
                                     int width) {
    bool compact = (width > 0 && width < 105); // повна таблиця ~115 символів
    if (compact) {
        // Вузька консоль (як на скріншоті): без колонок метрики (є в назві),
        // розкритих станів, тупиків і оптимальності — нічого не переноситься.
        std::string bar(62, '=');
        if (ansiSupported) out << "\n\033[1;36m" << bar << "\033[0m\n";
        else out << "\n" << bar << "\n";
        if (ansiSupported) out << "  \033[1;32mПОРІВНЯННЯ АЛГОРИТМІВ\033[0m " << report.levelName << "\n";
        else out << "  ПОРІВНЯННЯ АЛГОРИТМІВ " << report.levelName << "\n";
        if (ansiSupported) out << "\033[1;36m" << bar << "\033[0m\n";
        else out << bar << "\n";

        out << padRight("Алгоритм", 12) << " "
            << padRight("Статус", 14) << " "
            << padLeft("Ходів", 6) << " "
            << padLeft("Штовх", 7) << " "
            << padLeft("Search(мс)", 10) << " "
            << padRight("Replay", 8) << "\n";
        out << std::string(62, '-') << "\n";

        for (const auto& r : report.results) {
            std::string statusStr = solvers::statusToString(r.status);
            std::string replayStr = r.replayValid ? "Passed" : (r.status == solvers::SearchStatus::Solved ? "Failed" : "-");
            out << padRight(r.algorithmName, 12) << " "
                << padRight(statusStr, 14) << " "
                << padLeft(r.replayValid ? std::to_string(r.moves) : "-", 6) << " "
                << padLeft(r.replayValid ? std::to_string(r.pushes) : "-", 7) << " "
                << padLeft(formatDouble(toMs(r.searchTime), 3), 10) << " "
                << padRight(replayStr, 8) << "\n";
        }

        if (ansiSupported) out << "\033[1;36m" << bar << "\033[0m\n";
        else out << bar << "\n";
        out << "* Скорочена таблиця (вузький термінал). Розгорни вікно для повної.\n\n";
        return;
    }

    if (ansiSupported) {
        out << "\n\033[1;36m====================================================================================================\033[0m\n";
        out << "                   \033[1;32mПОРІВНЯННЯ АЛГОРИТМІВ (ALGORITHM COMPARISON)\033[0m\n";
        out << "Рівень: \033[1m" << report.levelName << "\033[0m  Розмір: " << report.width << "x" << report.height
            << "  Ящиків/Цілей: " << report.boxCount << "\n";
        out << "\033[1;36m====================================================================================================\033[0m\n";
    } else {
        out << "\n====================================================================================================\n";
        out << "                   ПОРІВНЯННЯ АЛГОРИТМІВ (ALGORITHM COMPARISON)\n";
        out << "Рівень: " << report.levelName << "  Розмір: " << report.width << "x" << report.height
            << "  Ящиків/Цілей: " << report.boxCount << "\n";
        out << "====================================================================================================\n";
    }

    out << padRight("Алгоритм", 14)
        << padRight("Метрика", 9)
        << padRight("Статус", 14)
        << padLeft("Ходів", 7)
        << padLeft("Штовхань", 10)
        << padLeft("Розкрито", 11)
        << padLeft("Тупиків", 10)
        << padLeft("Search(ms)", 13)
        << "  "
        << padRight("Оптимальність", 15)
        << padRight("Replay", 10)
        << "\n";

    out << std::string(105, '-') << "\n";

    for (const auto& r : report.results) {
        std::string statusStr = solvers::statusToString(r.status);
        std::string metricStr = !r.metricLabel.empty() ? r.metricLabel
            : (r.metric == solvers::OptimizationMetric::Moves) ? "Moves" : "Pushes";
        std::string replayStr = r.replayValid ? "Passed" : (r.status == solvers::SearchStatus::Solved ? "Failed" : "-");

        out << padRight(r.algorithmName, 14)
            << padRight(metricStr, 9)
            << padRight(statusStr, 14)
            << padLeft(r.replayValid ? std::to_string(r.moves) : "-", 7)
            << padLeft(r.replayValid ? std::to_string(r.pushes) : "-", 10)
            << padLeft(std::to_string(r.exploredStates), 11)
            << padLeft(std::to_string(r.deadlockPruned), 10)
            << padLeft(formatDouble(toMs(r.searchTime), 3), 13)
            << "  "
            << padRight(r.optimalityDescription, 15)
            << padRight(replayStr, 10)
            << "\n";
    }

    if (ansiSupported) {
        out << "\033[1;36m====================================================================================================\033[0m\n\n";
    } else {
        out << "====================================================================================================\n\n";
    }
}

} // namespace sokoban::cli

int sokoban::cli::AlgorithmComparator::terminalWidth() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return 0;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return 0; // pipe/файл — повна таблиця
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return 0;
    int w = static_cast<int>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
    return (w > 0) ? w : 0;
#else
    if (!isatty(STDOUT_FILENO)) return 0;
    struct winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) return ws.ws_col;
    const char* c = std::getenv("COLUMNS");
    if (c) {
        int w = std::atoi(c);
        if (w > 0) return w;
    }
    return 0;
#endif
}
