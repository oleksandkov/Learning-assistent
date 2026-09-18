#include "TerminalUI.hpp"
#include "AlgorithmComparator.hpp"
#include "AiClient.hpp"
#include "../solvers/SolverRegistry.hpp"
#include "../solvers/DeadlockDetector.hpp"
#include "../solvers/LevelGenerator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#endif

namespace sokoban::cli {

TerminalUI::TerminalUI() {
    const char* noColor = std::getenv("NO_COLOR");
    const char* term = std::getenv("TERM");
    if (noColor != nullptr || (term != nullptr && std::string(term) == "dumb")) {
        ansiSupported_ = false;
    } else {
        enableVirtualTerminal();
    }
}

TerminalUI::~TerminalUI() {
    disableVirtualTerminal();
}

void TerminalUI::enableVirtualTerminal() {
#ifdef _WIN32
    originalOutputCP_ = GetConsoleOutputCP();
    originalCP_ = GetConsoleCP();
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            originalOutMode_ = mode;
            mode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
            if (!SetConsoleMode(hOut, mode)) {
                ansiSupported_ = false;
            }
        } else {
            ansiSupported_ = false;
        }
    } else {
        ansiSupported_ = false;
    }
#endif
    if (ansiSupported_) {
        std::cout << "\033[?25l" << std::flush; // Hide cursor during UI rendering
    }
}

void TerminalUI::disableVirtualTerminal() {
    if (ansiSupported_) {
        std::cout << "\033[?25h" << std::flush; // Restore cursor visibility
    }
#ifdef _WIN32
    // Do not restore to non-UTF8 code pages, as the CLI menu and logs require UTF-8
    if (originalOutputCP_ == CP_UTF8) {
        SetConsoleOutputCP(originalOutputCP_);
    }
    if (originalCP_ == CP_UTF8) {
        SetConsoleCP(originalCP_);
    }
    if (originalOutMode_ != 0) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            SetConsoleMode(hOut, originalOutMode_);
        }
    }
#endif
}

void TerminalUI::clearScreen() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
            COORD coord = { 0, 0 };
            DWORD count;
            DWORD cellCount = csbi.dwSize.X * std::min(static_cast<int>(csbi.dwSize.Y), 500);
            FillConsoleOutputCharacterA(hOut, ' ', cellCount, coord, &count);
            FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, coord, &count);
            SetConsoleCursorPosition(hOut, coord);

            SMALL_RECT rect = csbi.srWindow;
            short h = rect.Bottom - rect.Top;
            rect.Top = 0;
            rect.Bottom = h;
            SetConsoleWindowInfo(hOut, TRUE, &rect);
        }
    }
#endif
    if (ansiSupported_) {
        // \033[H: cursor home, \033[2J: clear screen, \033[3J: clear scrollback
        std::cout << "\033[H\033[2J\033[3J" << std::flush;
    } else {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }
}

namespace {

struct TermSize {
    int width = 80;
    int height = 25;
};

TermSize getTerminalSize() {
    TermSize ts;
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
            ts.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            ts.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            if (ts.width <= 0) ts.width = 80;
            if (ts.height <= 0) ts.height = 25;
            return ts;
        }
    }
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_col > 0) ts.width = ws.ws_col;
        if (ws.ws_row > 0) ts.height = ws.ws_row;
        return ts;
    }
#endif
    return ts;
}

int visualLength(const std::string& s) {
    int len = 0;
    bool inEscape = false;
    for (std::size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\033') {
            inEscape = true;
        } else if (inEscape) {
            if (s[i] == 'm') {
                inEscape = false;
            }
        } else {
            unsigned char c = static_cast<unsigned char>(s[i]);
            if ((c & 0xC0) != 0x80) {
                len++;
            }
        }
    }
    return len;
}

std::string formatDuration(double totalSec) {
    if (totalSec < 0.0) totalSec = 0.0;
    int mins = static_cast<int>(totalSec) / 60;
    double secs = totalSec - (mins * 60);
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%02d:%04.1f", mins, secs);
    return std::string(buf);
}

struct StepEntry {
    std::string text;
    int fromMoveIdx = 0;
    int toMoveIdx = 0;
};

std::vector<StepEntry> buildStepsFromMoves(const std::vector<core::Direction>& moves,
                                           const std::vector<bool>& pushes) {
    std::vector<StepEntry> result;
    if (moves.empty()) return result;

    struct RawGroup {
        core::Direction dir;
        int count = 0;
        bool hasPush = false;
        int startIdx = 0;
        int endIdx = 0;
    };

    std::vector<RawGroup> groups;
    for (std::size_t i = 0; i < moves.size(); ++i) {
        bool push = (i < pushes.size()) ? pushes[i] : false;
        if (!groups.empty() && groups.back().dir == moves[i] && !groups.back().hasPush && !push) {
            groups.back().count++;
            groups.back().endIdx = static_cast<int>(i);
        } else {
            groups.push_back(RawGroup{
                .dir = moves[i],
                .count = 1,
                .hasPush = push,
                .startIdx = static_cast<int>(i),
                .endIdx = static_cast<int>(i)
            });
        }
    }

    for (std::size_t i = 0; i < groups.size(); ++i) {
        const auto& g = groups[i];
        std::string dStr;
        switch (g.dir) {
            case core::Direction::Up:    dStr = "вперед"; break;
            case core::Direction::Down:  dStr = "вниз"; break;
            case core::Direction::Left:  dStr = "вліво"; break;
            case core::Direction::Right: dStr = "вправо"; break;
        }
        std::string cStr = (g.count == 1) ? "один" : std::to_string(g.count);
        std::string desc = std::to_string(i + 1) + ". " + dStr + " на " + cStr;
        if (g.hasPush) {
            desc += " (зсув ящика)";
        }
        result.push_back(StepEntry{
            .text = desc,
            .fromMoveIdx = g.startIdx,
            .toMoveIdx = g.endIdx
        });
    }

    return result;
}

} // namespace

void TerminalUI::render(const core::GameSession& session,
                        const std::string& modeName,
                        const std::string& levelName,
                        const std::string& statusMessage,
                        const std::string& hintMessage,
                        double playerTimeSec,
                        std::optional<double> aiSolveTimeMs,
                        const std::vector<std::string>& sidebarSteps,
                        int activeStepIndex,
                        bool enableCamera) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hOut, &csbi)) {
            SHORT reqW = static_cast<SHORT>(session.board().width() + 4);
            if (reqW > csbi.dwSize.X) {
                COORD newSize = csbi.dwSize;
                newSize.X = reqW;
                SetConsoleScreenBufferSize(hOut, newSize);
            }
            if (!ansiSupported_) {
                COORD coord = { 0, 0 };
                SetConsoleCursorPosition(hOut, coord);
            }
        }
    }
#endif

    std::ostringstream out;
    if (ansiSupported_) {
        // Atomic screen refresh: cursor home, clear visible screen, clear scrollback
        out << "\033[H\033[2J\033[3J";
    } else {
        clearScreen();
    }

    const auto& board = session.board();
    const auto& state = session.currentState();
    int px = board.toX(state.player);
    int py = board.toY(state.player);

    TermSize ts = getTerminalSize();

    // 1. Заголовок та інформація про стан
    out << "SOKOBAN\n";
    out << "Рівень: " << levelName << " | Режим: " << modeName << "\n";
    out << "Ходи: " << session.moveCount()
        << " | Зсуви: " << session.pushCount()
        << " | Час гравця: " << static_cast<int>(playerTimeSec) << " с"
        << " | Час ШІ: ";
    if (aiSolveTimeMs.has_value()) {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "%.3f мс", *aiSolveTimeMs);
        out << buf << "\n";
    } else {
        out << "--\n";
    }
    out << "\n";

    // 2. Компактна легенда позначень
    out << "Легенда: @ Гравець | $ Ящик | . Ціль | * Ящик на цілі | + Гравець на цілі | # Стіна\n\n";

    // 3. Режим камери вмикається лише явно через 'C' (за замовчуванням вимкнено - повний огляд поля)
    bool useCamera = enableCamera;

    int startX = 0, endX = board.width();
    int startY = 0, endY = board.height();
    int camW = board.width();
    int camH = board.height();

    if (useCamera) {
        // Камера адаптується до розміру консолі
        int maxAllowedW = std::clamp(ts.width - 6, 24, 70);
        int maxAllowedH = std::clamp(ts.height - 10, 10, 25);

        camW = std::min(board.width(), maxAllowedW);
        camH = std::min(board.height(), maxAllowedH);

        // Якщо все поле вміщується в розміри камери - показуємо повне поле
        if (camW >= board.width() && camH >= board.height()) {
            useCamera = false;
        } else {
            // Камера строго фокусується на гравцеві (px, py)
            startX = std::clamp(px - camW / 2, 0, std::max(0, board.width() - camW));
            endX = startX + camW;
            startY = std::clamp(py - camH / 2, 0, std::max(0, board.height() - camH));
            endY = startY + camH;
        }
    }

    // 4. Підготовка рядків поля (з естетичною рамкою для камери)
    std::vector<std::string> boardLines;
    if (useCamera) {
        std::string camTitle = " Камера (зум): X " + std::to_string(startX) + "-" + std::to_string(endX - 1) +
                               ", Y " + std::to_string(startY) + "-" + std::to_string(endY - 1) + " | 'C' - повне поле ";
        int fillDashes = camW - static_cast<int>(camTitle.size());
        if (fillDashes < 0) fillDashes = 0;
        int leftDashes = fillDashes / 2;
        int rightDashes = fillDashes - leftDashes;
        boardLines.push_back("+" + std::string(leftDashes, '-') + camTitle + std::string(rightDashes, '-') + "+");

        for (int y = startY; y < endY; ++y) {
            std::string line = "|";
            for (int x = startX; x < endX; ++x) {
                core::CellIndex idx = board.toIndex(x, y);
                bool isPlayer = (state.player == idx);
                bool hasBox = state.hasBox(idx);
                bool isGoal = board.isGoal(idx);
                bool isWall = board.isWall(idx);
                bool isFloor = board.isFloor(idx);

                if (isPlayer) {
                    line += isGoal ? "+" : "@";
                } else if (hasBox) {
                    line += isGoal ? "*" : "$";
                } else if (isGoal) {
                    line += ".";
                } else if (isWall) {
                    line += "#";
                } else if (isFloor) {
                    line += " ";
                } else {
                    line += " ";
                }
            }
            line += "|";
            boardLines.push_back(line);
        }
        boardLines.push_back("+" + std::string(camW, '-') + "+");
    } else {
        for (int y = startY; y < endY; ++y) {
            std::string line;
            for (int x = startX; x < endX; ++x) {
                core::CellIndex idx = board.toIndex(x, y);
                bool isPlayer = (state.player == idx);
                bool hasBox = state.hasBox(idx);
                bool isGoal = board.isGoal(idx);
                bool isWall = board.isWall(idx);
                bool isFloor = board.isFloor(idx);

                if (isPlayer) {
                    line += isGoal ? "+" : "@";
                } else if (hasBox) {
                    line += isGoal ? "*" : "$";
                } else if (isGoal) {
                    line += ".";
                } else if (isWall) {
                    line += "#";
                } else if (isFloor) {
                    line += " ";
                } else {
                    line += " ";
                }
            }
            boardLines.push_back(line);
        }
    }

    // 5. Підготовка списку кроків (з ковзним вікном)
    std::vector<std::string> stepLines;
    if (!sidebarSteps.empty()) {
        int maxVisibleSteps = std::min(static_cast<int>(boardLines.size()) - 1, 10);
        if (maxVisibleSteps < 5) maxVisibleSteps = 5;

        int totalSteps = static_cast<int>(sidebarSteps.size());
        if (totalSteps <= maxVisibleSteps) {
            for (int i = 0; i < totalSteps; ++i) {
                if (i == activeStepIndex) {
                    stepLines.push_back("-> " + sidebarSteps[i] + " [поточний]");
                } else {
                    stepLines.push_back("   " + sidebarSteps[i]);
                }
            }
        } else {
            int cur = (activeStepIndex >= 0 && activeStepIndex < totalSteps) ? activeStepIndex : 0;
            int half = maxVisibleSteps / 2;
            int winStart = std::clamp(cur - half, 0, totalSteps - maxVisibleSteps);
            int winEnd = winStart + maxVisibleSteps;

            if (winStart > 0) {
                stepLines.push_back("[... " + std::to_string(winStart) + " попер. ...]");
            }
            for (int i = winStart; i < winEnd; ++i) {
                if (i == activeStepIndex) {
                    stepLines.push_back("-> " + sidebarSteps[i] + " [поточний]");
                } else {
                    stepLines.push_back("   " + sidebarSteps[i]);
                }
            }
            if (winEnd < totalSteps) {
                stepLines.push_back("[... ще " + std::to_string(totalSteps - winEnd) + " далі ...]");
            }
        }
    }

    // 6. Вивід поля та кроків (пліч-о-пліч у 2 колонки)
    int maxBoardWidth = 0;
    for (const auto& bl : boardLines) {
        if (static_cast<int>(bl.size()) > maxBoardWidth) {
            maxBoardWidth = static_cast<int>(bl.size());
        }
    }

    if (maxBoardWidth + 28 <= ts.width) {
        int colWidth = maxBoardWidth + 4;
        std::size_t totalRows = std::max(boardLines.size(), stepLines.size() + 1);

        std::string fieldHdr = useCamera
            ? "Поле [Камера " + std::to_string(camW) + "x" + std::to_string(camH) + " | Рівень " + std::to_string(board.width()) + "x" + std::to_string(board.height()) + "]:"
            : "Поле (" + std::to_string(board.width()) + "x" + std::to_string(board.height()) + " | Повний огляд):";
        std::string stepHdr = !stepLines.empty()
            ? (aiSolveTimeMs.has_value() || activeStepIndex >= 0
                ? "План ШІ (" + std::to_string(sidebarSteps.size()) + "):"
                : "Історія ходів:")
            : "";
        int spacesHdr = colWidth - static_cast<int>(fieldHdr.size());
        if (spacesHdr < 2) spacesHdr = 2;
        out << fieldHdr << std::string(spacesHdr, ' ') << stepHdr << "\n";

        for (std::size_t r = 0; r < totalRows; ++r) {
            std::string bPart = (r < boardLines.size()) ? boardLines[r] : "";
            int spaces = colWidth - static_cast<int>(bPart.size());
            if (spaces < 2) spaces = 2;
            out << bPart << std::string(spaces, ' ');

            if (r < stepLines.size()) {
                out << stepLines[r];
            }
            out << "\n";
        }
    } else {
        if (useCamera) {
            out << "Поле [Рівень " << board.width() << "x" << board.height()
                << " | Камера (зум): X " << startX << "-" << (endX - 1)
                << ", Y " << startY << "-" << (endY - 1)
                << " | Гравець: (" << px << "," << py << ")]:\n";
        } else {
            out << "Поле (" << board.width() << "x" << board.height()
                << " | Повний огляд):\n";
        }
        for (const auto& bl : boardLines) {
            out << bl << "\n";
        }
        if (!stepLines.empty()) {
            out << "\nКроки розв'язку / ходи (" << sidebarSteps.size() << "):\n";
            for (const auto& sl : stepLines) {
                out << sl << "\n";
            }
        }
    }
    out << "\n";

    // 7. Повідомлення про статус / перемогу
    if (session.isWon()) {
        out << ">>> РІВЕНЬ ПРОЙДЕНО! ВІТАЄМО! <<<\n\n";
    }
    if (!statusMessage.empty()) {
        out << "Статус: " << statusMessage << "\n";
    }
    if (!hintMessage.empty()) {
        out << "Підказка: " << hintMessage << "\n";
    }

    // 8. Компактний блок керування
    out << "\nКерування:\n"
        << "  WASD/Стрілки - Рух | 1/P - A* штовхання | B - BFS | M - A* ходи | G - AI\n"
        << "  I - IDA* | O - Greedy | X - Прототип 1 | T - Порівняти всі | Пробіл - Симуляція | N - Крок | H - Підказка\n"
        << "  U - Undo | Y - Redo | R - Перезапуск | K - AI-ключ | C - Камера | V - Зберегти | Q - Меню\n";

    std::string frame = out.str();
    std::cout.write(frame.data(), static_cast<std::streamsize>(frame.size()));
    std::cout.flush();
}

bool TerminalUI::hasInput() const {
#ifdef _WIN32
    return _kbhit() != 0;
#else
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
#endif
}

KeyCommand TerminalUI::readKey() {
#ifdef _WIN32
    int c = _getch();
    if (c == -1 || c == EOF) {
        return KeyCommand::Quit;
    }
    if (c == 0 || c == 224) {
        int c2 = _getch();
        if (c2 == -1 || c2 == EOF) {
            return KeyCommand::Quit;
        }
        switch (c2) {
            case 72: return KeyCommand::Up;
            case 80: return KeyCommand::Down;
            case 75: return KeyCommand::Left;
            case 77: return KeyCommand::Right;
            default: return KeyCommand::None;
        }
    }

    switch (c) {
        case 'w': case 'W': return KeyCommand::Up;
        case 's': case 'S': return KeyCommand::Down;
        case 'a': case 'A': return KeyCommand::Left;
        case 'd': case 'D': return KeyCommand::Right;
        case '1': return KeyCommand::Solve;
        case 'b': case 'B': return KeyCommand::SolveBFS;
        case 'm': case 'M': return KeyCommand::SolveMoves;
        case 'p': case 'P': return KeyCommand::SolvePushes;
        case 'i': case 'I': return KeyCommand::SolveIDAStar;
        case 'o': case 'O': return KeyCommand::SolveGreedy;
        case 'x': case 'X': return KeyCommand::SolvePrototype1;
        case 'g': case 'G': return KeyCommand::SolveAI;
        case 't': case 'T': return KeyCommand::CompareAll;
        case 'k': case 'K': return KeyCommand::SetAiKey;
        case 'u': case 'U': return KeyCommand::Undo;
        case 'y': case 'Y': case 26: /* Ctrl+Z */ return KeyCommand::Redo;
        case 'r': case 'R': return KeyCommand::Restart;
        case 'h': case 'H': return KeyCommand::Hint;
        case ' ': return KeyCommand::TogglePause;
        case 'n': case 'N': return KeyCommand::NextStep;
        case 'c': case 'C': return KeyCommand::ToggleCamera;
        case 'v': case 'V': return KeyCommand::Save;
        case 13: /* Enter */ return KeyCommand::Solve;
        case 'q': case 'Q': case 27: /* Esc */ return KeyCommand::Quit;
        default: return KeyCommand::None;
    }
#else
    char c = 0;
    if (read(0, &c, 1) <= 0) return KeyCommand::None;
    if (c == '\033') {
        char seq[2];
        if (read(0, &seq[0], 1) <= 0) return KeyCommand::Quit;
        if (read(0, &seq[1], 1) <= 0) return KeyCommand::Quit;
        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KeyCommand::Up;
                case 'B': return KeyCommand::Down;
                case 'D': return KeyCommand::Left;
                case 'C': return KeyCommand::Right;
            }
        }
        return KeyCommand::None;
    }

    switch (c) {
        case 'w': case 'W': return KeyCommand::Up;
        case 's': case 'S': return KeyCommand::Down;
        case 'a': case 'A': return KeyCommand::Left;
        case 'd': case 'D': return KeyCommand::Right;
        case '1': return KeyCommand::Solve;
        case 'b': case 'B': return KeyCommand::SolveBFS;
        case 'm': case 'M': return KeyCommand::SolveMoves;
        case 'p': case 'P': return KeyCommand::SolvePushes;
        case 'i': case 'I': return KeyCommand::SolveIDAStar;
        case 'o': case 'O': return KeyCommand::SolveGreedy;
        case 'x': case 'X': return KeyCommand::SolvePrototype1;
        case 'g': case 'G': return KeyCommand::SolveAI;
        case 't': case 'T': return KeyCommand::CompareAll;
        case 'k': case 'K': return KeyCommand::SetAiKey;
        case 'u': case 'U': return KeyCommand::Undo;
        case 'y': case 'Y': return KeyCommand::Redo;
        case 'r': case 'R': return KeyCommand::Restart;
        case 'h': case 'H': return KeyCommand::Hint;
        case ' ': return KeyCommand::TogglePause;
        case 'n': case 'N': return KeyCommand::NextStep;
        case 'c': case 'C': return KeyCommand::ToggleCamera;
        case 'v': case 'V': return KeyCommand::Save;
        case 10: case 13: return KeyCommand::Solve;
        case 'q': case 'Q': return KeyCommand::Quit;
        default: return KeyCommand::None;
    }
#endif
}

int runGame(core::ParsedLevel parsed) {
    core::GameSession session(std::move(parsed.board), std::move(parsed.initialState));
    TerminalUI ui;

    std::string mode = "Ручна гра";
    std::string statusMsg = "";
    std::string hintMsg = "";

    std::optional<solvers::Solution> currentSolution;
    std::vector<StepEntry> plannedStepEntries;
    std::vector<std::string> displayedSidebar;
    std::size_t solutionMoveIndex = 0;
    bool isAutoplay = false;
    bool cameraEnabled = false;

    // Таймери
    auto playerStart = std::chrono::steady_clock::now();
    double finalPlayerTime = 0.0;
    bool gameWonFrozen = false;
    std::optional<double> aiSolveTimeMs = std::nullopt;

    auto updateManualSidebar = [&]() {
        const auto& history = session.history();
        if (history.empty()) {
            displayedSidebar = {
                "Поки що немає ходів.",
                "Рухайтесь клавішами WASD або Стрілками.",
                "Або натисніть [1] щоб ШІ знайшов рішення."
            };
            return;
        }

        std::vector<core::Direction> hMoves;
        std::vector<bool> hPushes;
        for (const auto& rec : history) {
            hMoves.push_back(rec.direction);
            hPushes.push_back(rec.pushedBox());
        }
        auto entries = buildStepsFromMoves(hMoves, hPushes);
        displayedSidebar.clear();
        for (std::size_t i = 0; i < entries.size(); ++i) {
            displayedSidebar.push_back(entries[i].text);
        }
    };

    updateManualSidebar();

    // Спільне застосування знайденого маршруту: зберігає рішення для
    // симуляції ([Пробіл]) та покрокового виконання ([N]), як у web "Застосувати".
    auto applySolution = [&](std::vector<core::Direction> moves, std::uint64_t pushCount,
                             double searchMs, const std::string& label,
                             const std::string& extra = "") {
        solvers::Solution sol;
        sol.moves = std::move(moves);
        sol.moveCount = sol.moves.size();
        sol.pushCount = pushCount;
        currentSolution = std::move(sol);
        solutionMoveIndex = 0;
        isAutoplay = false;
        mode = label;

        core::GameSession sim(session.board(), session.currentState());
        std::vector<bool> solutionPushes;
        for (core::Direction d : currentSolution->moves) {
            auto r = sim.apply(core::MoveCommand{.direction = d, .source = core::CommandSource::AI});
            solutionPushes.push_back(r.pushedBox);
        }
        plannedStepEntries = buildStepsFromMoves(currentSolution->moves, solutionPushes);

        char buf[512];
        std::snprintf(buf, sizeof(buf),
            "Розв'язок знайдено (%zu ходів, %llu зсувів, час пошуку: %.3f мс)! [Пробіл] - симуляція, [N] - кроки.%s",
            currentSolution->moves.size(), static_cast<unsigned long long>(pushCount),
            searchMs, extra.empty() ? "" : (" " + extra).c_str());
        statusMsg = buf;
        aiSolveTimeMs = searchMs;
    };

    // Локальний розв'язок одним із вбудованих алгоритмів (як web dropdown).
    auto solveLocal = [&](solvers::SolverKind kind, solvers::OptimizationMetric metric,
                          const std::string& label, double curTime) {
        mode = "Пошук ШІ (" + label + ")";
        statusMsg = "ШІ розраховує розв'язок (" + label + ")...";
        ui.render(session, mode, parsed.name, statusMsg, "",
                  curTime, aiSolveTimeMs, displayedSidebar, -1, cameraEnabled);

        std::unique_ptr<solvers::ISolver> solver = solvers::SolverRegistry::create(kind);
        solvers::SolverOptions opt{
            .algorithm = kind,
            .metric = metric,
            .timeLimit = std::chrono::milliseconds(30000),
            .nodeLimit = 1000000,
            .enableDeadlockDetection = true,
            .enableSafeMode = true
        };

        solver->start(session.board(), session.currentState(), opt);
        while (solver->advance(10000) == solvers::SearchStatus::Running) {}

        auto stats = solver->statistics();
        if (stats.status == solvers::SearchStatus::Solved) {
            auto sol = solver->solution();
            double searchMs = static_cast<double>(stats.searchTime.count()) / 1'000'000.0;
            applySolution(sol->moves, stats.solutionPushes, searchMs, "ШІ розв'язано (" + label + ")");
        } else {
            statusMsg = "ШІ (" + label + ") не зміг знайти розв'язок (можливо, поточний стан є тупиковим).";
            mode = "Ручна гра";
        }
    };

    // Зовнішній ШІ як у web /api/ai: той самий промпт, маршрут перевіряє ядро.
    auto solveAi = [&](double curTime) {
        std::string key = AiClient::getApiKey();
        if (key.empty()) {
            mode = "Ручна гра";
            statusMsg = "Немає AI-ключа. Натисни [K] і встав ключ Groq або Google AI Studio.";
            return;
        }
        mode = "AI думає...";
        statusMsg = "Запит до AI (провайдер — авто за ключем: Groq / Google)...";
        ui.render(session, mode, parsed.name, statusMsg, "",
                  curTime, aiSolveTimeMs, displayedSidebar, -1, cameraEnabled);

        std::string xsb = solvers::LevelGenerator::serializeToXsb(
            session.board(), session.currentState(), parsed.name);
        AiResult gr = AiClient::solveFromXsb(xsb, key, 60);
        if (!gr.ok) {
            mode = "Ручна гра";
            statusMsg = gr.error;
            return;
        }

        auto valStart = std::chrono::steady_clock::now();
        core::GameSession sim(session.board(), session.currentState());
        std::vector<core::Direction> moves;
        bool legal = true;
        for (char c : gr.moves) {
            core::Direction d = core::Direction::Up;
            if (c == 'U') d = core::Direction::Up;
            else if (c == 'D') d = core::Direction::Down;
            else if (c == 'L') d = core::Direction::Left;
            else if (c == 'R') d = core::Direction::Right;
            else { legal = false; break; }
            auto r = sim.apply(core::MoveCommand{.direction = d, .source = core::CommandSource::AI});
            if (!r.success) { legal = false; break; }
            moves.push_back(d);
        }
        double validationMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - valStart).count();
        (void)validationMs;

        if (!legal || !sim.isWon()) {
            mode = "Ручна гра";
            statusMsg = "AI запропонував маршрут, але ядро відхилило його. ШІ не зміг розв'язати цю позицію.";
            return;
        }
        std::uint64_t pushes = 0;
        {
            core::GameSession cnt(session.board(), session.currentState());
            for (core::Direction d : moves) {
                if (cnt.apply(core::MoveCommand{.direction = d, .source = core::CommandSource::AI}).pushedBox) pushes++;
            }
        }
        std::string extra = gr.explanation.empty() ? "" : ("AI: " + gr.explanation);
        applySolution(std::move(moves), pushes, gr.modelMs,
                      "ШІ розв'язано (AI · " + gr.providerLabel + ")", extra);
    };

    while (true) {
        // Розрахунок часу гравця
        double curPlayerTime = 0.0;
        if (!session.isWon()) {
            auto now = std::chrono::steady_clock::now();
            curPlayerTime = std::chrono::duration<double>(now - playerStart).count();
        } else {
            if (!gameWonFrozen) {
                gameWonFrozen = true;
                auto now = std::chrono::steady_clock::now();
                finalPlayerTime = std::chrono::duration<double>(now - playerStart).count();
            }
            curPlayerTime = finalPlayerTime;
        }

        // Обчислення активного кроку у сайдбарі
        int activeStepIdx = -1;
        if (currentSolution.has_value() && !plannedStepEntries.empty()) {
            displayedSidebar.clear();
            for (std::size_t i = 0; i < plannedStepEntries.size(); ++i) {
                displayedSidebar.push_back(plannedStepEntries[i].text);
                if (static_cast<int>(solutionMoveIndex) >= plannedStepEntries[i].fromMoveIdx &&
                    static_cast<int>(solutionMoveIndex) <= plannedStepEntries[i].toMoveIdx) {
                    activeStepIdx = static_cast<int>(i);
                }
            }
        }

        ui.render(session, mode, parsed.name, statusMsg, hintMsg,
                  curPlayerTime, aiSolveTimeMs, displayedSidebar, activeStepIdx, cameraEnabled);

        // Обробка симуляції (Autoplay)
        if (isAutoplay && currentSolution.has_value()) {
            if (solutionMoveIndex < currentSolution->moves.size()) {
                core::MoveCommand cmd{
                    .direction = currentSolution->moves[solutionMoveIndex++],
                    .source = core::CommandSource::AI
                };
                session.apply(cmd);
                std::this_thread::sleep_for(std::chrono::milliseconds(130));

                if (ui.hasInput()) {
                    KeyCommand k = ui.readKey();
                    if (k == KeyCommand::TogglePause || k == KeyCommand::Quit) {
                        isAutoplay = false;
                        mode = "Пауза";
                        statusMsg = "Симуляцію призупинено [Пробіл - продовжити].";
                    }
                }
                continue;
            } else {
                isAutoplay = false;
                mode = "Завершено";
                statusMsg = "Симуляцію успішно завершено! Всі ящики на цілях.";
            }
        }

        KeyCommand key = ui.readKey();
        if (key == KeyCommand::Quit) {
            break;
        }

        hintMsg.clear();

        switch (key) {
            case KeyCommand::Up:
            case KeyCommand::Down:
            case KeyCommand::Left:
            case KeyCommand::Right: {
                isAutoplay = false;
                currentSolution.reset();
                plannedStepEntries.clear();
                mode = "Ручна гра";

                core::Direction dir = core::Direction::Up;
                if (key == KeyCommand::Down)  dir = core::Direction::Down;
                if (key == KeyCommand::Left)  dir = core::Direction::Left;
                if (key == KeyCommand::Right) dir = core::Direction::Right;

                core::MoveCommand cmd{ .direction = dir, .source = core::CommandSource::Human };
                auto res = session.apply(cmd);
                if (!res.success) {
                    statusMsg = "Неможливий хід (перешкода / стіна)!";
                } else {
                    statusMsg = res.pushedBox ? "Ящик штовхнуто." : "Гравець зробив крок.";
                    if (res.pushedBox && solvers::DeadlockDetector::isDeadlock(session.board(), session.currentState(), res.boxTo)) {
                        statusMsg += " [УВАГА: виявлено ТУПИК (Deadlock)! Натисніть 'U' для скасування].";
                    }
                }
                updateManualSidebar();
                break;
            }
            case KeyCommand::Undo: {
                isAutoplay = false;
                currentSolution.reset();
                plannedStepEntries.clear();
                mode = "Ручна гра";
                if (session.undo()) {
                    statusMsg = "Хід скасовано (Undo).";
                } else {
                    statusMsg = "Неможливо скасувати: історія порожня.";
                }
                updateManualSidebar();
                break;
            }
            case KeyCommand::Redo: {
                isAutoplay = false;
                currentSolution.reset();
                plannedStepEntries.clear();
                mode = "Ручна гра";
                if (session.redo()) {
                    statusMsg = "Хід повторено (Redo).";
                } else {
                    statusMsg = "Неможливо повторити: черга Redo порожня.";
                }
                updateManualSidebar();
                break;
            }
            case KeyCommand::Restart: {
                isAutoplay = false;
                currentSolution.reset();
                plannedStepEntries.clear();
                mode = "Ручна гра";
                session.restart();
                solutionMoveIndex = 0;
                playerStart = std::chrono::steady_clock::now();
                gameWonFrozen = false;
                aiSolveTimeMs = std::nullopt;
                statusMsg = "Рівень перезапущено спочатку.";
                updateManualSidebar();
                break;
            }
            case KeyCommand::Solve:
            case KeyCommand::SolveAStar:
            case KeyCommand::SolvePushes: {
                solveLocal(solvers::SolverKind::AStar,
                           solvers::OptimizationMetric::Pushes, "A* штовхання", curPlayerTime);
                break;
            }
            case KeyCommand::SolveBFS: {
                solveLocal(solvers::SolverKind::BFS,
                           solvers::OptimizationMetric::Moves, "BFS", curPlayerTime);
                break;
            }
            case KeyCommand::SolveMoves: {
                solveLocal(solvers::SolverKind::AStar,
                           solvers::OptimizationMetric::Moves, "A* ходи", curPlayerTime);
                break;
            }
            case KeyCommand::SolveIDAStar: {
                solveLocal(solvers::SolverKind::IDAStar,
                           solvers::OptimizationMetric::Pushes, "IDA* штовхання", curPlayerTime);
                break;
            }
            case KeyCommand::SolveGreedy: {
                solveLocal(solvers::SolverKind::Greedy,
                           solvers::OptimizationMetric::Pushes, "Greedy (неоптимально)", curPlayerTime);
                break;
            }
            case KeyCommand::SolvePrototype1: {
                solveLocal(solvers::SolverKind::Prototype1,
                           solvers::OptimizationMetric::Pushes, "Прототип 1 · X → Y", curPlayerTime);
                break;
            }
            case KeyCommand::SolveAI: {
                solveAi(curPlayerTime);
                break;
            }
            case KeyCommand::Hint: {
                auto solver = solvers::SolverRegistry::create(solvers::SolverKind::AStar);
                solvers::SolverOptions opt{
                    .algorithm = solvers::SolverKind::AStar,
                    .metric = solvers::OptimizationMetric::Pushes,
                    .timeLimit = std::chrono::milliseconds(10000),
                    .nodeLimit = 500000,
                    .enableDeadlockDetection = true,
                    .enableSafeMode = true
                };
                solver->start(session.board(), session.currentState(), opt);
                while (solver->advance(10000) == solvers::SearchStatus::Running) {}
                auto sol = solver->solution();
                if (solver->statistics().status == solvers::SearchStatus::Solved &&
                    sol.has_value() && !sol->moves.empty()) {
                    std::string dStr = "?";
                    switch (sol->moves[0]) {
                        case core::Direction::Up: dStr = "вперед"; break;
                        case core::Direction::Down: dStr = "вниз"; break;
                        case core::Direction::Left: dStr = "вліво"; break;
                        case core::Direction::Right: dStr = "вправо"; break;
                    }
                    hintMsg = "Наступний хід: " + dStr + " (всього " +
                              std::to_string(sol->moveCount) + " ходів, " +
                              std::to_string(sol->pushCount) + " зсувів).";
                    statusMsg = "Підказку побудовано (A* штовхання).";
                } else {
                    hintMsg = "Підказку не знайдено (можливо, поточний стан є тупиковим).";
                }
                break;
            }
            case KeyCommand::SetAiKey: {
                std::cout << "\nAI API-ключ: Groq (console.groq.com) або Google AI Studio (порожньо = скасувати): " << std::flush;
                std::string key;
                std::getline(std::cin, key);
                while (!key.empty() && (key.back() == '\r' || key.back() == '\n')) key.pop_back();
                if (key.empty()) {
                    statusMsg = "Ключ не змінено.";
                } else {
                    std::string err;
                    if (AiClient::setApiKey(key, err)) {
                        statusMsg = "AI-ключ збережено. Натисни [G] щоб розв'язати позицію.";
                        const char* ov = std::getenv("GROQ_API_KEY");
                        if (ov && *ov) {
                            statusMsg += " [УВАГА: env GROQ_API_KEY перекриває файл — пункт 4 меню покаже деталі].";
                        }
                    } else {
                        statusMsg = err;
                    }
                }
                mode = "Ручна гра";
                break;
            }
            case KeyCommand::CompareAll: {
                mode = "Порівняння ШІ";
                statusMsg = "Алгоритми працюють з поточної позиції...";
                ui.render(session, mode, parsed.name, statusMsg, "",
                          curPlayerTime, aiSolveTimeMs, displayedSidebar, -1, cameraEnabled);
                auto report = AlgorithmComparator::compare(
                    session.board(), session.currentState(), parsed.name,
                    std::chrono::milliseconds(30000));
                // AI 4-м рядком таблиці (як картка у web): той самий запит,
                // маршрут перевіряє ядро. Без ключа — рядок-підказка замість помилки.
                std::string aiNote, aiExplanation;
                {
                    AlgorithmComparisonResult grow;
                    grow.algorithmName = "AI";
                    grow.kind = solvers::SolverKind::AStar;
                    grow.metric = solvers::OptimizationMetric::Pushes;
                    grow.metricLabel = "AI-план";
                    std::string key = AiClient::getApiKey();
                    if (key.empty()) {
                        grow.status = solvers::SearchStatus::Cancelled;
                        grow.optimalityDescription = "Немає ключа [K]";
                        aiNote = "Немає AI-ключа. Натисни [K] і встав ключ Groq або Google AI Studio.";
                    } else {
                        std::cout << "AI думає...\n" << std::flush;
                        std::string xsb = solvers::LevelGenerator::serializeToXsb(
                            session.board(), session.currentState(), parsed.name);
                        AiResult gr = AiClient::solveFromXsb(xsb, key, 60);
                        if (!gr.ok) {
                            grow.status = solvers::SearchStatus::InternalError;
                            grow.optimalityDescription = "Помилка AI";
                            aiNote = gr.error;
                        } else {
                            core::GameSession sim(session.board(), session.currentState());
                            solvers::Solution gsol;
                            gsol.optimizedFor = solvers::OptimizationMetric::Pushes;
                            bool legal = true;
                            for (char c : gr.moves) {
                                core::Direction d = core::Direction::Up;
                                if (c == 'U') d = core::Direction::Up;
                                else if (c == 'D') d = core::Direction::Down;
                                else if (c == 'L') d = core::Direction::Left;
                                else if (c == 'R') d = core::Direction::Right;
                                else { legal = false; break; }
                                if (!sim.apply(core::MoveCommand{.direction = d, .source = core::CommandSource::AI}).success) { legal = false; break; }
                                gsol.moves.push_back(d);
                            }
                            std::uint64_t gpush = 0;
                            if (legal && sim.isWon()) {
                                core::GameSession cnt(session.board(), session.currentState());
                                for (core::Direction d : gsol.moves) {
                                    if (cnt.apply(core::MoveCommand{.direction = d, .source = core::CommandSource::AI}).pushedBox) gpush++;
                                }
                                gsol.moveCount = gsol.moves.size();
                                gsol.pushCount = gpush;
                                grow.status = solvers::SearchStatus::Solved;
                                grow.moves = gsol.moveCount;
                                grow.pushes = gpush;
                                grow.searchTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                    std::chrono::duration<double, std::milli>(gr.modelMs));
                                grow.replayValid = true;
                                grow.isOptimal = true;
                                grow.optimalityDescription = "AI (перевірено)";
                                grow.solution = std::move(gsol);
                                aiExplanation = gr.explanation;
                            } else {
                                grow.status = solvers::SearchStatus::InternalError;
                                grow.optimalityDescription = "Помилка AI";
                                aiNote = "AI запропонував маршрут, але ядро відхилило його.";
                            }
                        }
                    }
                    report.results.push_back(std::move(grow));
                }
                std::ostringstream tab;
                AlgorithmComparator::printTable(report, tab, ui.ansiSupported(),
                                               getTerminalSize().width);
                if (ui.ansiSupported()) std::cout << "\033[H\033[2J\033[3J" << std::flush;
                std::cout << tab.str() << std::flush;
                std::cout << "Застосувати: [B] BFS  [M] A* ходи  [P] A* штовхання  [I] IDA*  [O] Greedy  "
                             "[G] AI  [інше] скасувати: " << std::flush;
                KeyCommand pick = ui.readKey();
                std::cout << "\n" << std::flush;
                int idx = -1;
                if (pick == KeyCommand::SolveBFS) idx = 0;
                else if (pick == KeyCommand::SolveMoves) idx = 1;
                else if (pick == KeyCommand::SolvePushes || pick == KeyCommand::Solve) idx = 2;
                else if (pick == KeyCommand::SolveIDAStar) idx = 3;
                else if (pick == KeyCommand::SolveGreedy) idx = 4;
                else if (pick == KeyCommand::SolveAI) idx = 5;
                if (idx >= 0 && idx < static_cast<int>(report.results.size()) &&
                    report.results[static_cast<std::size_t>(idx)].solution.has_value() &&
                    report.results[static_cast<std::size_t>(idx)].replayValid) {
                    const auto& r = report.results[static_cast<std::size_t>(idx)];
                    double searchMs = static_cast<double>(r.searchTime.count()) / 1'000'000.0;
                    std::string extra = (idx == 5 && !aiExplanation.empty())
                        ? ("AI: " + aiExplanation) : "";
                    applySolution(r.solution->moves, r.pushes, searchMs,
                                  "ШІ розв'язано (" + r.algorithmName + ")", extra);
                } else {
                    mode = "Ручна гра";
                    if (idx == 5 && !aiNote.empty()) statusMsg = aiNote;
                    else statusMsg = (idx >= 0)
                        ? "Обраний алгоритм не дав перевіреного рішення з цієї позиції."
                        : "Порівняння завершено без застосування (рішення не обрано).";
                }
                break;
            }
            case KeyCommand::TogglePause: {
                if (!currentSolution.has_value()) {
                    // Якщо розв'язок ще не знайдено - знайти його і запустити симуляцію
                    mode = "Пошук ШІ";
                    statusMsg = "Пошук розв'язку перед симуляцією...";
                    ui.render(session, mode, parsed.name, statusMsg, "",
                              curPlayerTime, aiSolveTimeMs, displayedSidebar, -1, cameraEnabled);

                    auto solver = solvers::SolverRegistry::create(solvers::SolverKind::AStar);
                    solvers::SolverOptions opt{
                        .algorithm = solvers::SolverKind::AStar,
                        .metric = solvers::OptimizationMetric::Pushes,
                        .timeLimit = std::chrono::milliseconds(30000),
                        .nodeLimit = 1000000,
                        .enableDeadlockDetection = true,
                        .enableSafeMode = true
                    };
                    solver->start(session.board(), session.currentState(), opt);
                    while (solver->advance(10000) == solvers::SearchStatus::Running) {}

                    auto stats = solver->statistics();
                    if (stats.status == solvers::SearchStatus::Solved) {
                        currentSolution = solver->solution();
                        solutionMoveIndex = 0;
                        isAutoplay = true;
                        mode = "Симуляція";
                        double searchMs = static_cast<double>(stats.searchTime.count()) / 1'000'000.0;
                        aiSolveTimeMs = searchMs;

                        core::GameSession sim(session.board(), session.currentState());
                        std::vector<bool> solutionPushes;
                        for (core::Direction d : currentSolution->moves) {
                            auto r = sim.apply(core::MoveCommand{.direction = d, .source = core::CommandSource::AI});
                            solutionPushes.push_back(r.pushedBox);
                        }
                        plannedStepEntries = buildStepsFromMoves(currentSolution->moves, solutionPushes);
                        statusMsg = "Початок симуляції знайденого рішення!";
                    } else {
                        statusMsg = "ШІ не зміг знайти рішення для симуляції.";
                        mode = "Ручна гра";
                    }
                } else {
                    if (solutionMoveIndex >= currentSolution->moves.size()) {
                        solutionMoveIndex = 0;
                    }
                    isAutoplay = !isAutoplay;
                    mode = isAutoplay ? "Симуляція" : "Пауза";
                    statusMsg = isAutoplay ? "Симуляцію відновлено." : "Симуляцію призупинено [Пробіл - продовжити, N - наступний крок].";
                }
                break;
            }
            case KeyCommand::NextStep: {
                if (currentSolution.has_value() && solutionMoveIndex < currentSolution->moves.size()) {
                    isAutoplay = false;
                    mode = "Покроково";
                    core::MoveCommand cmd{
                        .direction = currentSolution->moves[solutionMoveIndex++],
                        .source = core::CommandSource::AI
                    };
                    session.apply(cmd);
                    statusMsg = "Виконано дію " + std::to_string(solutionMoveIndex) + " з " +
                                std::to_string(currentSolution->moves.size());
                }
                break;
            }
            case KeyCommand::ToggleCamera: {
                cameraEnabled = !cameraEnabled;
                statusMsg = cameraEnabled ? "Режим камери: ЗУМ НА ГРАВЦЯ (натисніть 'C' для повного огляду)."
                                          : "Режим камери: ПОВНИЙ ОГЛЯД (відображається все поле повністю).";
                break;
            }
            case KeyCommand::Save: {
                std::string path = "levels/my_custom_level.xsb";
                std::ofstream f(path);
                if (!f.is_open()) f.open("../" + path);
                if (f.is_open()) {
                    f << solvers::LevelGenerator::serializeToXsb(session.board(), session.currentState(), parsed.name);
                    statusMsg = "Рівень збережено у: " + path;
                } else {
                    statusMsg = "Помилка збереження у: " + path;
                }
                break;
            }
            default:
                break;
        }
    }

    return 0;
}

} // namespace sokoban::cli
