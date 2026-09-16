#include "../core/LevelParser.hpp"
#include "../core/GameSession.hpp"
#include "../solvers/SolverRegistry.hpp"
#include "../solvers/DeadlockDetector.hpp"
#include "../solvers/ReplayValidator.hpp"
#include "../solvers/LevelGenerator.hpp"
#include "TerminalUI.hpp"
#include "AiClient.hpp"
#include "BenchmarkRunner.hpp"
#include "AlgorithmComparator.hpp"
#include "Menu.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>

#ifdef _WIN32
#include <windows.h>

namespace {
struct WindowsConsoleGuard {
    UINT prevOutputCP = 0;
    UINT prevCP = 0;
    DWORD prevOutMode = 0;
    bool modeSet = false;

    WindowsConsoleGuard() {
        prevOutputCP = GetConsoleOutputCP();
        prevCP = GetConsoleCP();
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            if (GetConsoleMode(hOut, &prevOutMode)) {
                if (SetConsoleMode(hOut, prevOutMode | 0x0004 /* ENABLE_VIRTUAL_TERMINAL_PROCESSING */)) {
                    modeSet = true;
                }
            }
        }
    }

    ~WindowsConsoleGuard() {
        if (prevOutputCP != 0) {
            SetConsoleOutputCP(prevOutputCP);
        }
        if (prevCP != 0) {
            SetConsoleCP(prevCP);
        }
        if (modeSet) {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut != INVALID_HANDLE_VALUE) {
                SetConsoleMode(hOut, prevOutMode);
            }
        }
    }
};
} // namespace
#endif

namespace {

void printHelp() {
    std::cout << "Sokoban CLI - Термінальна гра та система розв'язання рівня Sokoban (C++20)\n\n"
              << "Використання:\n"
              << "  sokoban_cli                                Головне інтерактивне меню\n"
              << "  sokoban_cli custom                         Створення та гра у власний рівень (WxH, мішки, кульки)\n"
              << "  sokoban_cli play <рівень.xsb>              Інтерактивна гра в терміналі\n"
              << "  sokoban_cli solve <рівень.xsb> [опції]     Автоматичне розв'язання рівня\n"
              << "  sokoban_cli hint <рівень.xsb> [опції]      Отримати підказку для наступного ходу\n"
              << "  sokoban_cli compare <рівень.xsb> [опції]   Порівняння BFS vs A*-Moves vs A*-Pushes vs IDA* vs Greedy\n"
              << "  sokoban_cli random [опції]                 Генерація та валідація випадкового рівня\n"
              << "  sokoban_cli benchmark <рівень|папка> [опції] Запуск бенчмарку швидкодії\n"
              << "  sokoban_cli --help                         Показати цю довідку\n\n"
               << "Опції solve / hint:\n"
               << "  --algorithm <bfs|astar|idastar|greedy|ai|gemini|groq> Алгоритм (за замовчуванням: astar)\n"
               << "    ai = зовнішній ШІ, провайдер авто за ключем: Groq (GROQ_API_KEY)\n"
               << "         або Google AI Studio (GEMINI_API_KEY, AIza.../AQ...); gemini/groq = примусово\n"
               << "         ключ також вводиться у грі клавішею K (файл ~/.sokoban_ai_key)\n"
              << "  --metric <moves|pushes>   Метрика оптимізації (за замовчуванням: pushes)\n"
              << "  --timeout <секунди>       Ліміт часу на пошук (за замовчуванням: 60)\n"
              << "  --node-limit <число>      Ліміт досліджених станів (за замовчуванням: 1000000)\n\n"
              << "Опції compare:\n"
              << "  --timeout <секунди>       Ліміт часу на кожен алгоритм (за замовчуванням: 30)\n\n"
              << "Опції random:\n"
              << "  --width <число>           Ширина поля (за замовчуванням: 7)\n"
              << "  --height <число>          Висота поля (за замовчуванням: 7)\n"
              << "  --boxes <число>           Кількість ящиків (за замовчуванням: 2)\n"
              << "  --seed <число>            Seed для детермінованої генерації\n"
              << "  --output <файл>           Шлях до файлу для збереження XSB\n\n"
              << "Опції benchmark:\n"
              << "  --repeat <число>          Кількість повторів для кожного рівня (за замовчуванням: 5)\n"
              << "  --format <table|csv|json> Формат виводу результатів (за замовчуванням: table)\n"
              << "  --output <файл>           Шлях до файлу для збереження результатів\n"
              << "  --algorithm <all|bfs|astar|idastar|greedy> Вибір алгоритмів для тесту (за замовчуванням: all)\n\n"
               << "Керування в інтерактивній грі:\n"
               << "  WASD / Стрілки            Рух гравця та штовхання ящиків\n"
               << "  B / M / P                 Розв'язати: BFS / A* ходи / A* штовхання (1 = P)\n"
               << "  I / O                     Розв'язати: IDA* штовхання (good, мало пам'яті) / Greedy (bad, швидко але неоптимально)\n"
               << "  G                         Розв'язати через AI (авто: Groq / Google; K - ввести ключ)\n"
               << "  T                         Порівняти всі алгоритми з поточної позиції і обрати рішення\n"
              << "  U                         Скасування ходу (Undo)\n"
              << "  Y / Ctrl+Z                Повторення ходу (Redo)\n"
              << "  R                         Перезапуск рівня (Restart)\n"
               << "  H                         Підказка ШІ (перший хід A* з поточної позиції)\n"
               << "  K                         Ввести/оновити AI API-ключ\n"
               << "  1 / P                     Швидкий розв'язок: A* з мінімумом штовхань\n"
              << "  Пробіл (Space)            Пауза / продовження авто-відтворення\n"
              << "  N                         Наступний крок розв'язку (AI Step)\n"
              << "  Q / Esc                   Вихід\n";
}

double toMs(std::chrono::nanoseconds ns) {
    return static_cast<double>(ns.count()) / 1'000'000.0;
}

int runPlay(const std::string& levelPath) {
    sokoban::core::ParsedLevel parsed;
    try {
        parsed = sokoban::core::LevelParser::parseFile(levelPath);
    } catch (const std::exception& e) {
        std::cerr << "Помилка завантаження рівня: " << e.what() << "\n";
        return 1;
    }

    return sokoban::cli::runGame(std::move(parsed));
}

int runSolve(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Помилка: вкажіть файл рівня. sokoban_cli solve <рівень.xsb>\n";
        return 1;
    }

    std::string levelPath = argv[2];
    sokoban::solvers::SolverKind kind = sokoban::solvers::SolverKind::AStar;
    bool useAi = false;
    sokoban::cli::AiProvider forceProvider = sokoban::cli::AiProvider::Auto;
    sokoban::solvers::OptimizationMetric metric = sokoban::solvers::OptimizationMetric::Pushes;
    int timeoutSec = 60;
    std::size_t nodeLimit = 1000000;
    std::string outputFile = "";

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--algorithm" && i + 1 < argc) {
            std::string v = argv[++i];
            std::string low = v;
            for (char& c : low) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (low == "ai" || low == "gemini" || low == "groq") {
                useAi = true;
                if (low == "gemini") forceProvider = sokoban::cli::AiProvider::Google;
                if (low == "groq") forceProvider = sokoban::cli::AiProvider::Groq;
            } else {
                kind = sokoban::solvers::SolverRegistry::parseKind(v);
            }
        } else if (arg == "--metric" && i + 1 < argc) {
            metric = sokoban::solvers::SolverRegistry::parseMetric(argv[++i]);
        } else if (arg == "--timeout" && i + 1 < argc) {
            timeoutSec = std::stoi(argv[++i]);
        } else if (arg == "--node-limit" && i + 1 < argc) {
            nodeLimit = std::stoull(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }

    auto loadStart = std::chrono::steady_clock::now();
    sokoban::core::ParsedLevel parsed;
    try {
        parsed = sokoban::core::LevelParser::parseFile(levelPath);
    } catch (const std::exception& e) {
        std::cerr << "Помилка парсера: " << e.what() << "\n";
        return 1;
    }
    auto loadEnd = std::chrono::steady_clock::now();
    auto loadDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(loadEnd - loadStart);

    if (useAi) {
        // Зовнішній ШІ: провайдер авто за ключем (Groq / Google AI), маршрут перевіряє ядро.
        std::string key = sokoban::cli::AiClient::getApiKey();
        if (key.empty()) {
            std::cerr << "Помилка: немає AI API-ключа. Вкажіть GROQ_API_KEY або GEMINI_API_KEY (Google), "
                         "або збережіть ключ через гру ([K]) чи файл ~/.sokoban_ai_key.\n";
            return 1;
        }
        std::string xsb = sokoban::solvers::LevelGenerator::serializeToXsb(
            parsed.board, parsed.initialState, parsed.name);
        auto gr = sokoban::cli::AiClient::solveFromXsb(xsb, key, timeoutSec, forceProvider);
        std::cout << "\nРівень:   " << levelPath << "\nАлгоритм: AI · " << gr.providerLabel << "\n";
        if (!gr.ok) {
            std::cout << "Статус:   InternalError\nПомилка:  " << gr.error << "\n";
            return 2;
        }
        sokoban::solvers::Solution sol;
        for (char c : gr.moves) {
            if (c == 'U') sol.moves.push_back(sokoban::core::Direction::Up);
            else if (c == 'D') sol.moves.push_back(sokoban::core::Direction::Down);
            else if (c == 'L') sol.moves.push_back(sokoban::core::Direction::Left);
            else if (c == 'R') sol.moves.push_back(sokoban::core::Direction::Right);
            else {
                std::cout << "Статус:   InternalError\nПомилка:  Недійсний символ маршруту від AI.\n";
                return 2;
            }
        }
        auto val = sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, sol);
        sol.moveCount = val.actualMoves;
        sol.pushCount = val.actualPushes;
        std::cout << "Статус:   " << (val.valid ? "Solved (ядро підтвердило)" : "InternalError") << "\n";
        if (!val.valid) {
            std::cout << "Помилка:  AI запропонував маршрут, але ядро відхилило його: "
                      << val.errorMessage << "\n";
            return 2;
        }
        std::cout << "Довжина (ходів):  " << sol.moveCount << "\n"
                  << "Штовхань:         " << sol.pushCount << "\n"
                  << "Послідовність дій: " << gr.moves << "\n";
        if (!gr.model.empty()) std::cout << "Модель:           " << gr.model << "\n";
        std::cout << "Час відповіді AI:  " << std::fixed << std::setprecision(3) << gr.modelMs << " мс\n";
        if (!gr.explanation.empty()) std::cout << "Пояснення: " << gr.explanation << "\n";
        std::cout << "ReplayValidator:  ВЕРИФІКОВАНО (OK)\n";
        return 0;
    }

    std::unique_ptr<sokoban::solvers::ISolver> solver = sokoban::solvers::SolverRegistry::create(kind);
    sokoban::solvers::SolverOptions opt{
        .algorithm = kind,
        .metric = metric,
        .timeLimit = std::chrono::seconds(timeoutSec),
        .nodeLimit = nodeLimit,
        .memoryLimitBytes = 512 * 1024 * 1024,
        .enableDeadlockDetection = true,
        .enableSafeMode = true
    };

    solver->start(parsed.board, parsed.initialState, opt);
    while (solver->advance(10000) == sokoban::solvers::SearchStatus::Running) {}

    auto stats = solver->statistics();
    auto sol = solver->solution();

    std::cout << "\n=======================================================\n";
    std::cout << "               РЕЗУЛЬТАТИ РОЗВ'ЯЗАННЯ                 \n";
    std::cout << "=======================================================\n";
    std::cout << "Рівень:           " << levelPath << "\n";
    std::cout << "Алгоритм:         " << sokoban::solvers::SolverRegistry::toString(kind) << "\n";
    std::cout << "Метрика:          " << sokoban::solvers::SolverRegistry::toString(metric) << "\n";
    std::cout << "Статус:           " << sokoban::solvers::statusToString(stats.status) << "\n";

    if (sol.has_value()) {
        std::cout << "Довжина (ходів):  " << sol->moveCount << "\n";
        std::cout << "Штовхань:         " << sol->pushCount << "\n";
        std::cout << "Послідовність дій: ";
        for (sokoban::core::Direction d : sol->moves) {
            std::cout << sokoban::core::directionToChar(d);
        }
        std::cout << "\n";

        auto val = sokoban::solvers::ReplayValidator::validate(parsed.board, parsed.initialState, *sol);
        std::cout << "ReplayValidator:  " << (val.valid ? "ВЕРИФІКОВАНО (OK)" : "ПОМИЛКА: " + val.errorMessage) << "\n";
    }

    std::cout << "\n--- Часові показники (steady_clock) ---\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Час завантаження рівня:        " << std::setw(10) << toMs(loadDuration) << " мс\n";
    std::cout << "Час preprocessing (підготовка):" << std::setw(10) << toMs(stats.preprocessingTime) << " мс\n";
    std::cout << "Чистий час пошуку (Search):    " << std::setw(10) << toMs(stats.searchTime) << " мс\n";
    std::cout << "Час реконструкції шляху:       " << std::setw(10) << toMs(stats.reconstructionTime) << " мс\n";
    std::cout << "Час валідації рішення:         " << std::setw(10) << toMs(stats.validationTime) << " мс\n";
    std::cout << "Загальний час розв'язання:     " << std::setw(10) << toMs(stats.totalSolverTime) << " мс\n";

    std::cout << "\n--- Статистика простору станів ---\n";
    std::cout << "Згенеровано станів:            " << stats.generatedStates << "\n";
    std::cout << "Досліджено станів:             " << stats.exploredStates << "\n";
    std::cout << "Відкинуто дублікатів:          " << stats.duplicatePruned << "\n";
    std::cout << "Відкинуто тупиків (Deadlock):  " << stats.deadlockPruned << "\n";
    std::cout << "Макс. розмір черги (Frontier): " << stats.maxFrontierSize << "\n";
    std::cout << "Оцінка пікової пам'яті:        " << (stats.estimatedPeakBytes / 1024) << " КБ\n";
    std::cout << "=======================================================\n\n";

    return (stats.status == sokoban::solvers::SearchStatus::Solved) ? 0 : 2;
}

int runHint(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Помилка: вкажіть файл рівня. sokoban_cli hint <рівень.xsb>\n";
        return 1;
    }

    std::string levelPath = argv[2];
    sokoban::solvers::SolverKind kind = sokoban::solvers::SolverKind::AStar;
    for (int i = 3; i < argc; ++i) {
        if (std::string(argv[i]) == "--algorithm" && i + 1 < argc) {
            kind = sokoban::solvers::SolverRegistry::parseKind(argv[++i]);
        }
    }

    sokoban::core::ParsedLevel parsed;
    try {
        parsed = sokoban::core::LevelParser::parseFile(levelPath);
    } catch (const std::exception& e) {
        std::cerr << "Помилка парсера: " << e.what() << "\n";
        return 1;
    }

    std::unique_ptr<sokoban::solvers::ISolver> solver = sokoban::solvers::SolverRegistry::create(kind);
    sokoban::solvers::SolverOptions opt{
        .algorithm = kind,
        .metric = sokoban::solvers::OptimizationMetric::Pushes,
        .timeLimit = std::chrono::seconds(10),
        .enableDeadlockDetection = true,
        .enableSafeMode = true
    };

    solver->start(parsed.board, parsed.initialState, opt);
    while (solver->advance(10000) == sokoban::solvers::SearchStatus::Running) {}

    auto sol = solver->solution();
    if (sol.has_value() && !sol->moves.empty()) {
        std::cout << "Рекомендований перший хід: " << sokoban::core::directionToChar(sol->moves[0]) << "\n";
        std::cout << "Повний знайдений маршрут:  ";
        for (sokoban::core::Direction d : sol->moves) {
            std::cout << sokoban::core::directionToChar(d);
        }
        std::cout << "\nКількість ходів: " << sol->moveCount << ", штовхань: " << sol->pushCount << "\n";
        return 0;
    } else {
        std::cout << "Не вдалося знайти розв'язок або підказку для даного рівня.\n";
        return 1;
    }
}

int runCompare(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Помилка: вкажіть файл рівня. sokoban_cli compare <рівень.xsb> [--timeout <sec>]\n";
        return 1;
    }

    std::string levelPath = argv[2];
    int timeoutSec = 30;
    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--timeout" && i + 1 < argc) {
            timeoutSec = std::stoi(argv[++i]);
        }
    }

    try {
        auto report = sokoban::cli::AlgorithmComparator::compareFile(levelPath, std::chrono::seconds(timeoutSec));
        sokoban::cli::AlgorithmComparator::printTable(report, std::cout, true,
            sokoban::cli::AlgorithmComparator::terminalWidth());
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Помилка порівняння: " << e.what() << "\n";
        return 1;
    }
}

int runRandom(int argc, char* argv[]) {
    sokoban::solvers::GeneratorOptions opt;
    std::string outputFile = "";

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--width" && i + 1 < argc) {
            opt.width = std::stoi(argv[++i]);
        } else if (arg == "--height" && i + 1 < argc) {
            opt.height = std::stoi(argv[++i]);
        } else if (arg == "--boxes" && i + 1 < argc) {
            opt.boxCount = std::stoi(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            opt.seed = std::stoull(argv[++i]);
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }

    std::cout << "Генерація та валідація випадкового розв'язного рівня Sokoban...\n";
    auto genRes = sokoban::solvers::LevelGenerator::generate(opt);
    if (!genRes.success) {
        std::cerr << "Помилка: " << genRes.errorMessage << "\n";
        return 1;
    }

    std::cout << "\nРівень успішно згенеровано та верифіковано!\n"
              << "Спроб: " << genRes.attempts << " | Seed: " << genRes.seedUsed << "\n"
              << "Підтверджено розв'язок: " << genRes.solution.moveCount << " ходів, "
              << genRes.solution.pushCount << " штовхань (ReplayValidator: OK)\n\n"
              << genRes.xsb << "\n";

    if (!outputFile.empty()) {
        std::ofstream f(outputFile);
        if (f.is_open()) {
            f << genRes.xsb;
            std::cout << "Рівень збережено у файл: " << outputFile << "\n";
        } else {
            std::cerr << "Помилка запису у файл: " << outputFile << "\n";
            return 1;
        }
    }

    return 0;
}

int runBenchmark(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Помилка: вкажіть шлях до рівня або директорії. sokoban_cli benchmark <шлях> [опції]\n";
        return 1;
    }

    sokoban::cli::BenchmarkOptions opt;
    opt.levelPaths.push_back(argv[2]);

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--repeat" && i + 1 < argc) {
            opt.repeats = std::stoi(argv[++i]);
        } else if (arg == "--format" && i + 1 < argc) {
            std::string fmt = argv[++i];
            if (fmt == "csv") opt.format = sokoban::cli::OutputFormat::CSV;
            else if (fmt == "json") opt.format = sokoban::cli::OutputFormat::JSON;
            else opt.format = sokoban::cli::OutputFormat::Table;
        } else if (arg == "--output" && i + 1 < argc) {
            opt.outputFile = argv[++i];
        } else if (arg == "--algorithm" && i + 1 < argc) {
            std::string a = argv[++i];
            for (char& c : a) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            if (a == "bfs") {
                opt.runBFS = true;
                opt.runAStarMoves = false;
                opt.runAStarPushes = false;
                opt.runIDAStar = false;
                opt.runGreedy = false;
            } else if (a == "astar") {
                opt.runBFS = false;
                opt.runAStarMoves = true;
                opt.runAStarPushes = true;
                opt.runIDAStar = false;
                opt.runGreedy = false;
            } else if (a == "idastar" || a == "ida*" || a == "ida") {
                opt.runBFS = false;
                opt.runAStarMoves = false;
                opt.runAStarPushes = false;
                opt.runIDAStar = true;
                opt.runGreedy = false;
            } else if (a == "greedy" || a == "gbfs") {
                opt.runBFS = false;
                opt.runAStarMoves = false;
                opt.runAStarPushes = false;
                opt.runIDAStar = false;
                opt.runGreedy = true;
            } else {
                opt.runBFS = true;
                opt.runAStarMoves = true;
                opt.runAStarPushes = true;
                opt.runIDAStar = true;
                opt.runGreedy = true;
            }
        }
    }

    return sokoban::cli::BenchmarkRunner::run(opt);
}

} // namespace

int main(int argc, char* argv[]) {
#ifdef _WIN32
    WindowsConsoleGuard consoleGuard;
#endif
    if (argc < 2) {
        // Run interactive main menu when executed without arguments
        sokoban::cli::Menu::run(std::cin, std::cout);
        return 0;
    }

    std::string mode = argv[1];
    if (mode == "--help" || mode == "-h") {
        printHelp();
        return 0;
    }
    if (mode == "--version" || mode == "-v") {
        std::cout << "Sokoban CLI v1.0.0 (C++20)\n";
        return 0;
    }

    if (mode == "play") {
        if (argc < 3) {
            std::cerr << "Помилка: вкажіть файл рівня. sokoban_cli play <рівень.xsb>\n";
            return 1;
        }
        return runPlay(argv[2]);
    } else if (mode == "solve") {
        return runSolve(argc, argv);
    } else if (mode == "hint") {
        return runHint(argc, argv);
    } else if (mode == "compare") {
        return runCompare(argc, argv);
    } else if (mode == "random") {
        return runRandom(argc, argv);
    } else if (mode == "custom" || mode == "create") {
        bool cancelled = false;
        auto level = sokoban::cli::Menu::createCustomLevel(std::cin, std::cout, cancelled);
        if (!cancelled) {
            std::string xsbToSave = level.xsbSource.empty()
                ? sokoban::solvers::LevelGenerator::serializeToXsb(level.board, level.initialState, level.name)
                : level.xsbSource;
            std::string title = level.name;
            sokoban::cli::runGame(std::move(level));
            sokoban::cli::Menu::promptSaveCustomLevel(std::cin, std::cout, xsbToSave, title);
        }
        return 0;
    } else {
        std::cerr << "Невідома команда: " << mode << "\n\n";
        printHelp();
        return 1;
    }
}
