#include "Menu.hpp"
#include "TerminalUI.hpp"
#include "AiClient.hpp"
#include "BenchmarkRunner.hpp"
#include "AlgorithmComparator.hpp"
#include "../solvers/LevelGenerator.hpp"
#include "../solvers/SolverRegistry.hpp"
#include "../solvers/ReplayValidator.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

namespace sokoban::cli {

namespace {
double toMs(std::chrono::nanoseconds ns) {
    return static_cast<double>(ns.count()) / 1'000'000.0;
}
} // namespace

std::vector<std::string> Menu::discoverLevels(const std::string& baseDir) {
    std::vector<std::string> files;
    namespace fs = std::filesystem;
    std::string dir = baseDir;
    if (!fs::exists(dir) && fs::exists("../" + baseDir)) {
        dir = "../" + baseDir;
    }
    if (!fs::exists(dir)) {
        return files;
    }
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".xsb") {
            files.push_back(entry.path().string());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

namespace {

std::string resolveLevelPath(const std::string& path) {
    namespace fs = std::filesystem;
    if (fs::exists(path)) return path;
    if (fs::exists("../" + path)) return "../" + path;
    return path;
}

void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void cleanInput(std::string& s) {
    if (s.size() >= 3 && static_cast<unsigned char>(s[0]) == 0xEF &&
        static_cast<unsigned char>(s[1]) == 0xBB && static_cast<unsigned char>(s[2]) == 0xBF) {
        s.erase(0, 3);
    }
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\t' || s.back() == '\n')) s.pop_back();
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
}

} // namespace

core::ParsedLevel Menu::loadCustomLevelFile(std::istream& in, std::ostream& out, bool& cancelled) {
    cancelled = false;
    out << "\nВведіть шлях до файлу рівня (.xsb): ";
    std::string path;
    if (!std::getline(in, path)) {
        cancelled = true;
        return {};
    }
    while (!path.empty() && (path.back() == ' ' || path.back() == '\r')) path.pop_back();
    while (!path.empty() && path.front() == ' ') path.erase(path.begin());

    if (path.empty()) {
        cancelled = true;
        return {};
    }

    std::string resolved = resolveLevelPath(path);
    try {
        auto pl = core::LevelParser::parseFile(resolved);
        out << "[OK] Рівень успішно завантажено з: " << resolved << "\n";
        return pl;
    } catch (const std::exception& e) {
        out << "[ПОМИЛКА] Не вдалося завантажити рівень (" << resolved << "): " << e.what() << "\n";
        cancelled = true;
        return {};
    }
}

core::ParsedLevel Menu::createManualLevel(std::istream& in, std::ostream& out,
                                          int expectedW, int expectedH, int expectedBags, bool& cancelled) {
    cancelled = false;
    out << "\n--- РУЧНЕ ВВЕДЕННЯ КАРТИ XSB ---\n"
        << "Очікуваний розмір: " << expectedW << "x" << expectedH << ", орієнтовно мішків: " << expectedBags << "\n"
        << "Символи XSB:\n"
        << "  # - стіна\n"
        << "  @ - гравець (@ на вільній клітинці, + на цілі)\n"
        << "  $ - мішок / ящик ($ на підлозі, * на цілі)\n"
        << "  . - кулька / ціль\n"
        << "  пробіл - підлога\n"
        << "Введіть карту рядок за рядком (введіть 'END' або порожній рядок двічі для завершення):\n\n";

    std::string mapText;
    std::string line;
    int emptyStreak = 0;
    while (std::getline(in, line)) {
        while (!line.empty() && line.back() == '\r') line.pop_back();
        if (line == "END" || line == "end") {
            break;
        }
        if (line.empty()) {
            emptyStreak++;
            if (emptyStreak >= 2 || !mapText.empty()) {
                break;
            }
            continue;
        }
        emptyStreak = 0;
        mapText += line + "\n";
    }

    if (mapText.empty()) {
        out << "Карта порожня, повернення в меню.\n";
        cancelled = true;
        return {};
    }

    try {
        auto pl = core::LevelParser::parseString(mapText, "Власний рівень (ручний)");
        pl.isCustom = true;
        pl.xsbSource = mapText;
        out << "[OK] Карту успішно розпізнано! Розмір: " << pl.board.width() << "x" << pl.board.height()
            << ", ящиків/цілей: " << pl.initialState.boxCount() << "\n";
        return pl;
    } catch (const std::exception& e) {
        out << "[ПОМИЛКА ВАЛІДАЦІЇ]: " << e.what() << "\n";
        cancelled = true;
        return {};
    }
}

core::ParsedLevel Menu::createCustomLevel(std::istream& in, std::ostream& out, bool& cancelled) {
    cancelled = false;
    out << "\n=======================================================\n";
    out << "            СТВОРЕННЯ ВЛАСНОГО РІВНЯ SOKOBAN           \n";
    out << "=======================================================\n";
    out << "Вкажіть параметри поля (натисніть Enter для значень за замовчуванням):\n\n";

    // 1. Width
    int width = 7;
    out << "1. Ширина поля (Width) [5-100] (за замовчуванням 7, B - назад): ";
    std::string wStr;
    if (!std::getline(in, wStr)) { cancelled = true; return {}; }
    cleanInput(wStr);
    if (wStr == "B" || wStr == "b") { cancelled = true; return {}; }
    if (!wStr.empty()) {
        try {
            width = std::stoi(wStr);
        } catch (...) {
            out << "   Некоректний формат ширини, використано 7.\n";
            width = 7;
        }
    }
    if (width < 5) {
        out << "   Ширина менше мінімальної (5). Встановлено: 5\n";
        width = 5;
    } else if (width > 100) {
        out << "   Ширина більше максимальної (100). Встановлено: 100\n";
        width = 100;
    }

    // 2. Height
    int height = 7;
    out << "2. Висота поля (Height) [5-100] (за замовчуванням 7, B - назад): ";
    std::string hStr;
    if (!std::getline(in, hStr)) { cancelled = true; return {}; }
    cleanInput(hStr);
    if (hStr == "B" || hStr == "b") { cancelled = true; return {}; }
    if (!hStr.empty()) {
        try {
            height = std::stoi(hStr);
        } catch (...) {
            out << "   Некоректний формат висоти, використано 7.\n";
            height = 7;
        }
    }
    if (height < 5) {
        out << "   Висота менше мінімальної (5). Встановлено: 5\n";
        height = 5;
    } else if (height > 100) {
        out << "   Висота більше максимальної (100). Встановлено: 100\n";
        height = 100;
    }

    // Inner usable area and box bounds
    int innerCells = (width - 2) * (height - 2);
    int maxBags = std::min(50, std::max(1, (innerCells - 3) / 4));
    int defaultBags = std::min(2, maxBags);

    // 3. Number of bags
    int bags = defaultBags;
    out << "3. Кількість мішків / ящиків (Bags) [1-" << maxBags << "] (за замовчуванням " << defaultBags << ", B - назад): ";
    std::string bStr;
    if (!std::getline(in, bStr)) { cancelled = true; return {}; }
    cleanInput(bStr);
    if (bStr == "B" || bStr == "b") { cancelled = true; return {}; }
    if (!bStr.empty()) {
        try {
            bags = std::stoi(bStr);
        } catch (...) {
            out << "   Некоректний формат, використано " << defaultBags << ".\n";
            bags = defaultBags;
        }
    }
    if (bags < 1) {
        out << "   Кількість мішків має бути щонайменше 1. Встановлено: 1\n";
        bags = 1;
    } else if (bags > maxBags) {
        out << "   Забагато мішків для поля " << width << "x" << height << ". Встановлено: " << maxBags << "\n";
        bags = maxBags;
    }

    // 4. Number of balls
    int balls = bags;
    out << "4. Кількість кульок / цілей (Balls) [1-" << maxBags << "] (за замовчуванням " << bags << ", B - назад): ";
    std::string ballsStr;
    if (!std::getline(in, ballsStr)) { cancelled = true; return {}; }
    cleanInput(ballsStr);
    if (ballsStr == "B" || ballsStr == "b") { cancelled = true; return {}; }
    if (!ballsStr.empty()) {
        try {
            balls = std::stoi(ballsStr);
        } catch (...) {
            out << "   Некоректний формат, використано " << bags << ".\n";
            balls = bags;
        }
    }
    if (balls < 1) {
        balls = 1;
    } else if (balls > maxBags) {
        balls = maxBags;
    }

    // Sokoban requirement: K bags must match K balls
    if (bags != balls) {
        out << "\n[ПРАВИЛО SOKOBAN] Кількість мішків (ящиків) повинна дорівнювати кількості кульок (цілей).\n";
        out << "Введено: " << bags << " мішків та " << balls << " кульок.\n";
        int matched = std::min(bags, balls);
        out << "-> Автоматично встановлено рівну кількість: " << matched << " мішків на " << matched << " кульок.\n\n";
        bags = matched;
        balls = matched;
    }

    // 5. Method: Procedural vs Manual
    out << "5. Спосіб побудови рівня:\n"
        << "   1. Процедурна генерація з перевіркою розв'язку ШІ (A*) [Рекомендовано]\n"
        << "   2. Ручне введення карти XSB рядок за рядком\n"
        << "   B. Назад у меню\n"
        << "Оберіть варіант (1-2 / B / Enter для 1): ";
    std::string methodStr;
    if (!std::getline(in, methodStr)) { cancelled = true; return {}; }
    cleanInput(methodStr);
    if (methodStr == "B" || methodStr == "b") { cancelled = true; return {}; }

    if (methodStr == "2") {
        return createManualLevel(in, out, width, height, bags, cancelled);
    }

    // 6. Seed
    out << "6. Seed для генератора (Enter для випадкового): ";
    std::string seedStr;
    std::optional<std::uint64_t> seed = std::nullopt;
    if (std::getline(in, seedStr)) {
        cleanInput(seedStr);
        if (!seedStr.empty()) {
            try {
                seed = std::stoull(seedStr);
            } catch (...) {}
        }
    }

    out << "\nГенерація розв'язного рівня (" << width << "x" << height << ", " << bags << " мішків)..." << std::endl;

    solvers::GeneratorOptions genOpt{
        .width = width,
        .height = height,
        .boxCount = bags,
        .maxAttempts = 200,
        .seed = seed,
        .solveTimeout = std::chrono::milliseconds(5000),
        .nodeLimit = 200000,
        .reverseSteps = 25 + bags * 6
    };

    auto genRes = solvers::LevelGenerator::generate(genOpt);
    if (!genRes.success) {
        out << "\n[ПОМИЛКА ГЕНЕРАЦІЇ]: " << genRes.errorMessage << "\n";
        out << "Спробуйте змінити розмір або зменшити кількість мішків.\n";
        cancelled = true;
        return {};
    }

    out << "\n[УСПІХ] Рівень успішно згенеровано!\n"
        << "  Спроб генератора:        " << genRes.attempts << "\n"
        << "  Seed:                    " << genRes.seedUsed << "\n"
        << "  Перевірено розв'язок ШІ: " << genRes.solution.moveCount << " ходів, "
        << genRes.solution.pushCount << " штовхань (ReplayValidator: ВАЛІДНО)\n\n";

    if (width <= 30 && height <= 30) {
        out << "Згенерована карта:\n" << genRes.xsb << "\n";
    } else {
        out << "[Попередній перегляд XSB скорочено для карти " << width << "x" << height
            << ": повне ігрове поле відображатиметься у грі. Натисніть 'C' у грі для перемикання зуму камери]\n\n";
    }

    out << "Натисніть Enter, щоб випробувати цей рівень у грі (або 'B' для виходу в меню): ";
    std::string cont;
    if (std::getline(in, cont)) {
        while (!cont.empty() && (cont.back() == ' ' || cont.back() == '\r')) cont.pop_back();
        while (!cont.empty() && cont.front() == ' ') cont.erase(cont.begin());
        if (cont == "B" || cont == "b") {
            cancelled = true;
            return {};
        }
    }

    core::ParsedLevel parsed;
    parsed.name = "Власний рівень (" + std::to_string(width) + "x" + std::to_string(height) +
                  ", " + std::to_string(bags) + " мішків)";
    parsed.board = std::move(genRes.board);
    parsed.initialState = std::move(genRes.initialState);
    parsed.isCustom = true;
    parsed.xsbSource = genRes.xsb;
    return parsed;
}

core::ParsedLevel Menu::selectPrebuiltLevel(std::istream& in, std::ostream& out, bool& cancelled) {
    cancelled = false;

    struct LevelItem {
        std::string path;
        std::string title;
        std::string desc;
    };

    std::vector<LevelItem> prebuilt = {
        {"levels/01_simple.xsb",     "Рівень 1: Вступний",          "1 ящик, 1 ціль"},
        {"levels/02_microban.xsb",   "Рівень 2: Класичний Microban","1 ящик, 1 ціль"},
        {"levels/03_two_boxes.xsb",  "Рівень 3: Подвійний",         "2 ящики, 2 цілі"},
        {"levels/04_warehouse.xsb",  "Рівень 4: Склад",             "2 ящики, 2 цілі"},
        {"levels/05_triple.xsb",     "Рівень 5: Потрійний лабіринт","3 ящики, 3 цілі"}
    };

    out << "\nРІВНІ ГРИ:\n";
    for (std::size_t i = 0; i < prebuilt.size(); ++i) {
        out << "  " << (i + 1) << ". " << prebuilt[i].title << " (" << prebuilt[i].desc << ")\n";
    }
    out << "  6. Створити власний рівень (Custom: WxH, мішки, кульки)\n";
    out << "  7. Завантажити рівень з файлу (.xsb)\n";
    out << "  B. Назад у меню\n";
    out << "Оберіть рівень (1-7 / C / B): ";

    std::string choice;
    if (!std::getline(in, choice)) {
        cancelled = true;
        return {};
    }

    while (!choice.empty() && (choice.back() == ' ' || choice.back() == '\r')) choice.pop_back();
    while (!choice.empty() && choice.front() == ' ') choice.erase(choice.begin());

    if (choice.empty()) {
        choice = "1";
    }

    if (choice == "B" || choice == "b" || choice == "0") {
        cancelled = true;
        return {};
    }

    if (choice == "6" || choice == "C" || choice == "c") {
        return createCustomLevel(in, out, cancelled);
    }

    if (choice == "7" || choice == "F" || choice == "f") {
        return loadCustomLevelFile(in, out, cancelled);
    }

    int idx = 1;
    try {
        idx = std::stoi(choice);
    } catch (...) {
        out << "Некоректний вибір, повернення в меню.\n";
        cancelled = true;
        return {};
    }

    if (idx < 1 || idx > static_cast<int>(prebuilt.size())) {
        out << "Номер рівня має бути від 1 до " << prebuilt.size() << " (або 6 для створення власного рівня).\n";
        cancelled = true;
        return {};
    }

    std::string resolved = resolveLevelPath(prebuilt[idx - 1].path);
    try {
        auto pl = core::LevelParser::parseFile(resolved);
        pl.name = prebuilt[idx - 1].title;
        return pl;
    } catch (const std::exception& e) {
        out << "Помилка завантаження рівня (" << resolved << "): " << e.what() << "\n";
        cancelled = true;
        return {};
    }
}

core::ParsedLevel Menu::selectLevel(std::istream& in, std::ostream& out, bool& cancelled) {
    return selectPrebuiltLevel(in, out, cancelled);
}

void Menu::showBFSArticle(std::ostream& out) {
    out << "\n--- 1. ПОШУК У ШИРИНУ (BFS) ---\n\n"
        << "Як працює:\n"
        << "• Досліджує всі можливі ходи рівень за рівнем через чергу (FIFO).\n"
        << "• Для кожного кроку перевіряються 4 напрямки (вгору, вниз, вліво, вправо).\n"
        << "• Усі відвідані стани запам'ятовуються у списку, щоб не повторюватись.\n\n"
        << "Головна властивість:\n"
        << "• Гарантує знаходження найкоротшого шляху за кількістю рухів (Moves).\n\n"
        << "Складність:\n"
        << "• Зі збільшенням поля кількість варіантів зростає експоненційно,\n"
        << "  тому для складних рівнів алгоритм вимагає багато пам'яті.\n\n";
}

void Menu::animateBFS(std::ostream& out) {
    out << "[Анімація BFS]:\n\n";

    out << "Кадр 1: Початок. Гравець @ шукає шлях до цілі G.\n"
        << "#######\n"
        << "#@   G#\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 2: Хвиля 1. Перевірено першу сусідню клітинку.\n"
        << "#######\n"
        << "#.1  G#\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 3: Хвиля 2. Перевірено другу клітинку.\n"
        << "#######\n"
        << "#.12 G#\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 4: Хвиля 3. Ціль G досягнута!\n"
        << "#######\n"
        << "#.123*#  Найкоротший шлях знайдено за 3 кроки.\n"
        << "#######\n\n";
    sleepMs(400);
}

void Menu::showAStarArticle(std::ostream& out) {
    out << "\n--- 2. ЕВРИСТИЧНИЙ ПОШУК A* ТА HUNGARIAN MATCHING ---\n\n"
        << "Як працює:\n"
        << "• A* використовує розумну оцінку: f = пройдений шлях + приблизна відстань до мети.\n"
        << "• Завдяки цьому алгоритм рухається цілеспрямовано до ящиків і цілей,\n"
        << "  не витрачаючи час на порожні кути карти.\n\n"
        << "Угорський алгоритм (Hungarian matching):\n"
        << "• Знаходить найкраще призначення кожного ящика до своєї цілі,\n"
        << "  щоб сумарна відстань була мінімальною.\n\n"
        << "Головна властивість:\n"
        << "• Знаходить оптимальний розв'язок у 10-50 разів швидше за звичайний BFS.\n\n";
}

void Menu::animateAStar(std::ostream& out) {
    out << "[Анімація A*]:\n\n";

    out << "Кадр 1: Оцінка відстані. A* спрямовує гравця @ до ящика $.\n"
        << "#######\n"
        << "#@ $ .#\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 2: Гравець підійшов до ящика.\n"
        << "#######\n"
        << "# @$ .#\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 3: Перший поштовх ящика вправо.\n"
        << "#######\n"
        << "#  @$*#\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 4: Ящик доставлено на ціль [*]!\n"
        << "#######\n"
        << "#   @*#  Рішення знайдено за мінімум дій!\n"
        << "#######\n\n";
    sleepMs(400);
}

void Menu::showDeadlockArticle(std::ostream& out) {
    out << "\n--- 3. ДЕТЕКТОР ТУПИКІВ (DEADLOCK DETECTION) ---\n\n"
        << "Як працює:\n"
        << "• У Sokoban дуже легко загнати ящик у глухий кут, звідки його вже не витягнути.\n"
        << "• Детектор тупиків перевіряє такі позиції заздалегідь:\n"
        << "  1. Кут із двох стін (де немає цілі);\n"
        << "  2. Блок 2х2 із ящиків і стін;\n"
        << "  3. Мертві клітинки вздовж глухих стін.\n\n"
        << "Користь:\n"
        << "• Миттєво відкидає програшні ходи та економить до 99% пам'яті.\n\n";
}

void Menu::animateDeadlocks(std::ostream& out) {
    out << "[Анімація виявлення тупика]:\n\n";

    out << "Кадр 1: Ящик $ біля кутка стін.\n"
        << "#######\n"
        << "#  $@ #\n"
        << "# ### #\n"
        << "#   . #\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 2: Гравець штовхнув ящик у кут (#$).\n"
        << "#######\n"
        << "#$ @  #  <- ТУПИК! Ящик затиснутий у кутку.\n"
        << "# ### #\n"
        << "#   . #\n"
        << "#######\n";
    sleepMs(500);

    out << "\nКадр 3: Детектор тупиків відкидає цю гілку.\n"
        << "#######\n"
        << "#[X]  #  <- Хід скасовано, алгоритм обирає правильний шлях!\n"
        << "# ### #\n"
        << "#   . #\n"
        << "#######\n\n";
    sleepMs(400);
}

void Menu::showAboutMenu(std::istream& in, std::ostream& out) {
    while (true) {
        out << "\nПРО АЛГОРИТМИ:\n"
            << "  1. Пошук у ширину (BFS)\n"
            << "  2. Евристичний пошук A*\n"
            << "  3. Детектор тупиків (Deadlock Detection)\n"
            << "  4. Переглянути всі по черзі\n"
            << "  5. Назад у головне меню\n"
            << "Оберіть пункт (1-5): ";

        std::string choice;
        if (!std::getline(in, choice)) break;

        while (!choice.empty() && (choice.back() == ' ' || choice.back() == '\r')) choice.pop_back();
        while (!choice.empty() && choice.front() == ' ') choice.erase(choice.begin());

        if (choice == "1") {
            showBFSArticle(out);
            animateBFS(out);
            out << "Натисніть Enter для продовження...";
            std::string d; std::getline(in, d);
        } else if (choice == "2") {
            showAStarArticle(out);
            animateAStar(out);
            out << "Натисніть Enter для продовження...";
            std::string d; std::getline(in, d);
        } else if (choice == "3") {
            showDeadlockArticle(out);
            animateDeadlocks(out);
            out << "Натисніть Enter для продовження...";
            std::string d; std::getline(in, d);
        } else if (choice == "4") {
            showBFSArticle(out);
            animateBFS(out);
            showAStarArticle(out);
            animateAStar(out);
            showDeadlockArticle(out);
            animateDeadlocks(out);
            out << "Натисніть Enter для продовження...";
            std::string d; std::getline(in, d);
        } else if (choice == "5" || choice == "B" || choice == "b" || choice == "0") {
            break;
        } else {
            out << "Будь ласка, введіть число від 1 до 5.\n";
        }
    }
}

void Menu::promptSaveCustomLevel(std::istream& in, std::ostream& out,
                                 const std::string& xsbContent,
                                 const std::string& defaultTitle) {
    if (xsbContent.empty()) return;

    out << "\n=======================================================\n";
    out << "               ЗБЕРЕЖЕННЯ ВЛАСНОГО РІВНЯ               \n";
    out << "=======================================================\n";
    out << "Ви щойно випробували рівень (" << defaultTitle << ").\n";
    out << "Бажаєте зберегти його у файл? (Y/n / Enter для 'так'): ";

    std::string ans;
    if (!std::getline(in, ans)) {
        return;
    }
    while (!ans.empty() && (ans.back() == ' ' || ans.back() == '\r')) ans.pop_back();
    while (!ans.empty() && ans.front() == ' ') ans.erase(ans.begin());

    if (ans == "N" || ans == "n" || ans == "0" || ans == "ні" || ans == "no") {
        out << "Рівень не збережено. Повернення в меню.\n";
        return;
    }

    out << "Введіть назву або шлях для збереження (за замовчуванням: levels/my_custom_level.xsb): ";
    std::string path;
    if (!std::getline(in, path)) {
        return;
    }
    while (!path.empty() && (path.back() == ' ' || path.back() == '\r')) path.pop_back();
    while (!path.empty() && path.front() == ' ') path.erase(path.begin());

    if (path.empty()) {
        path = "levels/my_custom_level.xsb";
    }

    if (path.find('.') == std::string::npos) {
        path += ".xsb";
    }

    std::ofstream f(path);
    if (!f.is_open() && path.find('/') == std::string::npos && path.find('\\') == std::string::npos) {
        f.open("levels/" + path);
        if (f.is_open()) {
            path = "levels/" + path;
        }
    }
    if (!f.is_open()) {
        f.open("../" + path);
        if (f.is_open()) {
            path = "../" + path;
        }
    }

    if (f.is_open()) {
        f << xsbContent;
        out << "\n[OK] Рівень успішно збережено у файл: " << path << "\n";
        out << "Ви можете завантажити його будь-коли через пункт 'Завантажити рівень з файлу'.\n";
    } else {
        out << "\n[ПОМИЛКА] Не вдалося створити файл: " << path << "\n";
    }
}

void Menu::promptAiKey(std::istream& in, std::ostream& out) {
    auto providerName = [](const std::string& k) {
        return AiClient::detectProvider(k) == AiProvider::Google
            ? "Google AI" : "Groq";
    };
    while (true) {
        std::string cur = AiClient::getApiKey();
        bool isGoogle = !cur.empty() &&
            AiClient::detectProvider(cur) == AiProvider::Google;
        out << "\n--- НАЛАШТУВАТИ ШІ ---\n";
        if (!cur.empty()) {
            std::string tail = cur.size() > 4 ? cur.substr(cur.size() - 4) : cur;
            out << "Ключ: " << providerName(cur) << " (****" << tail
                << ", " << AiClient::apiKeySource() << ")\n";
        } else {
            out << "Ключ: не введено\n";
        }
        out << "Модель: " << (isGoogle ? "gemini-3.8-flash (фіксована для Google)" : AiClient::getModel())
            << "\n  1. Ввести API-ключ (Groq / Google AI Studio)\n"
            << "  2. Обрати модель Groq\n"
            << "  B. Назад\n"
            << "Оберіть дію: " << std::flush;

        std::string choice;
        if (!std::getline(in, choice)) return;
        cleanInput(choice);
        if (choice.empty()) continue;
        if (choice == "B" || choice == "b" || choice == "0") return;

        if (choice == "1") {
            out << "Встав ключ Groq (GROQ_API_KEY, console.groq.com) або Google AI Studio (AIza.../AQ...).\n"
                << "Порожньо = скасувати: ";
            std::string key;
            if (!std::getline(in, key)) return;
            cleanInput(key);
            if (key.empty()) {
                out << "Скасовано, ключ не змінено.\n";
                continue;
            }
            std::string err;
            if (AiClient::setApiKey(key, err)) {
                out << "[OK] Ключ збережено (" << providerName(key)
                    << "). У грі AI запускається клавішею [G].\n";
                const char* ov = std::getenv("GROQ_API_KEY");
                if (ov && *ov && AiClient::detectProvider(key) == AiProvider::Google) {
                    out << "[УВАГА] Змінна GROQ_API_KEY перекриває файл — зараз використається Groq. "
                        << "Видали її (Remove-Item Env:\\GROQ_API_KEY) щоб працював Google.\n";
                }
            } else {
                out << "[ПОМИЛКА] " << err << "\n";
            }
        } else if (choice == "2") {
            if (isGoogle) {
                out << "Для Google AI модель фіксована: gemini-3.8-flash.\n"
                    << "Щоб обирати модель, встав ключ Groq (пункт 1).\n";
                continue;
            }
            auto models = AiClient::listFreeModels();
            out << "Моделі Groq (поточна: " << AiClient::getModel() << "):\n";
            for (std::size_t i = 0; i < models.size(); ++i) {
                out << "  " << (i + 1) << ". " << models[i] << "\n";
            }
            out << "Номер моделі (B = скасувати): ";
            std::string num;
            if (!std::getline(in, num)) return;
            cleanInput(num);
            if (num.empty() || num == "B" || num == "b") {
                out << "Скасовано, модель не змінено.\n";
                continue;
            }
            try {
                int idx = std::stoi(num);
                if (idx < 1 || idx > static_cast<int>(models.size())) throw std::out_of_range("range");
                std::string err;
                if (AiClient::setModel(models[static_cast<std::size_t>(idx) - 1], err)) {
                    out << "[OK] Модель збережено: " << AiClient::getModel() << "\n";
                } else {
                    out << "[ПОМИЛКА] " << err << "\n";
                }
            } catch (...) {
                out << "Введи номер від 1 до " << models.size() << ".\n";
            }
        } else {
            out << "Введи 1, 2 або B.\n";
        }
    }
}

int Menu::handleMenuChoice(int choice, std::istream& in, std::ostream& out) {
    switch (choice) {
        case 1: { // Play level (prebuilt or custom)
            bool cancelled = false;
            auto level = selectLevel(in, out, cancelled);
            if (!cancelled) {
                bool wasCustom = level.isCustom;
                std::string xsbToSave = level.xsbSource.empty()
                    ? solvers::LevelGenerator::serializeToXsb(level.board, level.initialState, level.name)
                    : level.xsbSource;
                std::string title = level.name;

                runGame(std::move(level));

                if (wasCustom) {
                    promptSaveCustomLevel(in, out, xsbToSave, title);
                }
            }
            return 0;
        }
        case 2: { // About options (articles & animations)
            showAboutMenu(in, out);
            return 0;
        }
        case 4: { // AI API key (Groq / Google)
            promptAiKey(in, out);
            return 0;
        }
        case 3:   // Exit options
        case 6: { // Compatibility with test suite
            out << "\nДякуємо за гру! Завершення роботи.\n";
            return 1;
        }
        default: {
            out << "Невідомий пункт меню. Спробуйте ще раз.\n";
            return 0;
        }
    }
}

void Menu::run(std::istream& in, std::ostream& out) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    while (true) {
        out << "\nSOKOBAN - ГОЛОВНЕ МЕНЮ\n"
            << "  1. Грати рівень (готові або власний)\n"
            << "  2. Про алгоритми (теорія та анімація)\n"
            << "  3. Вихід\n"
            << "  4. Налаштувати ШІ (ключ + безкоштовна модель)\n"
            << "Оберіть дію (1-4): " << std::flush;

        std::string line;
        if (!std::getline(in, line)) {
            break;
        }

        while (!line.empty() && (line.back() == ' ' || line.back() == '\r')) line.pop_back();
        while (!line.empty() && line.front() == ' ') line.erase(line.begin());

        if (line.empty()) {
            continue;
        }

        int choice = 0;
        try {
            choice = std::stoi(line);
        } catch (...) {
            out << "Будь ласка, введіть число від 1 до 4.\n";
            continue;
        }

        int res = handleMenuChoice(choice, in, out);
        if (res != 0) {
            break;
        }
    }
}

} // namespace sokoban::cli
