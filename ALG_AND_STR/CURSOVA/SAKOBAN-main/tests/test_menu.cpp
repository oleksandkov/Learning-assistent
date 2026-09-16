#include "TestHarness.hpp"
#include "cli/Menu.hpp"
#include "cli/TerminalUI.hpp"
#include "solvers/LevelGenerator.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>

SOKOBAN_TEST(Menu, DiscoverLevelsFindsXsbFiles) {
    auto levels = sokoban::cli::Menu::discoverLevels("levels");
    SOKOBAN_ASSERT(!levels.empty());

    bool foundSimple = false;
    for (const auto& l : levels) {
        if (l.find("01_simple.xsb") != std::string::npos) {
            foundSimple = true;
            break;
        }
    }
    SOKOBAN_ASSERT(foundSimple);
}

SOKOBAN_TEST(Menu, HandleMenuChoiceExitSignalsTermination) {
    std::istringstream in("");
    std::ostringstream out;

    int ret = sokoban::cli::Menu::handleMenuChoice(6, in, out);
    SOKOBAN_ASSERT_EQ(ret, 1);
    SOKOBAN_ASSERT(out.str().find("Завершення роботи") != std::string::npos);
}

SOKOBAN_TEST(Menu, HandleMenuChoiceInvalidContinuesLoop) {
    std::istringstream in("");
    std::ostringstream out;

    int ret = sokoban::cli::Menu::handleMenuChoice(999, in, out);
    SOKOBAN_ASSERT_EQ(ret, 0);
    SOKOBAN_ASSERT(out.str().find("Невідомий пункт") != std::string::npos);
}

SOKOBAN_TEST(Menu, NonInteractiveStreamExit) {
    std::istringstream in("6\n");
    std::ostringstream out;

    sokoban::cli::Menu::run(in, out);
    std::string text = out.str();

    SOKOBAN_ASSERT(text.find("SOKOBAN - ГОЛОВНЕ МЕНЮ") != std::string::npos);
    SOKOBAN_ASSERT(text.find("Завершення роботи") != std::string::npos);
}

SOKOBAN_TEST(Menu, NonInteractiveStreamImmediateEof) {
    std::istringstream in("");
    std::ostringstream out;

    // Should return immediately without hanging
    sokoban::cli::Menu::run(in, out);
    SOKOBAN_ASSERT(!out.str().empty());
}

SOKOBAN_TEST(Menu, SelectPrebuiltLevelContainsCustomOption) {
    std::istringstream in("B\n");
    std::ostringstream out;
    bool cancelled = false;

    auto level = sokoban::cli::Menu::selectPrebuiltLevel(in, out, cancelled);
    SOKOBAN_ASSERT(cancelled);
    std::string text = out.str();
    SOKOBAN_ASSERT(text.find("Створити власний рівень") != std::string::npos);
    SOKOBAN_ASSERT(text.find("Завантажити рівень з файлу") != std::string::npos);
}

SOKOBAN_TEST(Menu, CreateCustomLevelProceduralDefault) {
    // Width (default 7), Height (default 7), Bags (default 2), Balls (default 2), Method 1 (procedural), Seed 42, Enter to play
    std::istringstream in("\n\n\n\n1\n42\n\n");
    std::ostringstream out;
    bool cancelled = false;

    auto level = sokoban::cli::Menu::createCustomLevel(in, out, cancelled);
    SOKOBAN_ASSERT(!cancelled);
    SOKOBAN_ASSERT_EQ(level.initialState.boxCount(), 2);
    SOKOBAN_ASSERT_EQ(level.board.goalCount(), 2);
    SOKOBAN_ASSERT(level.board.width() >= 5);
    SOKOBAN_ASSERT(level.board.height() >= 5);
    SOKOBAN_ASSERT(out.str().find("Рівень успішно згенеровано") != std::string::npos);
}

SOKOBAN_TEST(Menu, CreateCustomLevelBagsBallsMismatchBalancing) {
    // Width 8, Height 8, Bags 3, Balls 2 -> automatically balanced to 2, Method 1 (procedural), Seed 101, Enter to play
    std::istringstream in("8\n8\n3\n2\n1\n101\n\n");
    std::ostringstream out;
    bool cancelled = false;

    auto level = sokoban::cli::Menu::createCustomLevel(in, out, cancelled);
    SOKOBAN_ASSERT(!cancelled);
    SOKOBAN_ASSERT_EQ(level.initialState.boxCount(), 2);
    SOKOBAN_ASSERT_EQ(level.board.goalCount(), 2);
    SOKOBAN_ASSERT(out.str().find("Синхронізовано") != std::string::npos || out.str().find("встановлено рівну кількість") != std::string::npos);
}

SOKOBAN_TEST(Menu, CreateCustomLevelCancellation) {
    // Width default, Height default, Bags default, Balls default, Method 1, Seed 42, then choose 'B' to cancel play
    std::istringstream in("\n\n\n\n1\n42\nB\n");
    std::ostringstream out;
    bool cancelled = false;

    auto level = sokoban::cli::Menu::createCustomLevel(in, out, cancelled);
    SOKOBAN_ASSERT(cancelled);
}

SOKOBAN_TEST(Menu, CreateManualLevelValid) {
    std::istringstream in("#####\n#@$.#\n#####\nEND\n");
    std::ostringstream out;
    bool cancelled = false;

    auto level = sokoban::cli::Menu::createManualLevel(in, out, 5, 3, 1, cancelled);
    SOKOBAN_ASSERT(!cancelled);
    SOKOBAN_ASSERT_EQ(level.initialState.boxCount(), 1);
    SOKOBAN_ASSERT_EQ(level.board.goalCount(), 1);
    SOKOBAN_ASSERT(out.str().find("Карту успішно розпізнано") != std::string::npos);
}

SOKOBAN_TEST(Menu, CreateCustomLevel100x100) {
    // Width 100, Height 100, Bags 2, Balls 2, Method 1, Seed 777, Enter to proceed
    std::istringstream in("100\n100\n2\n2\n1\n777\n\n");
    std::ostringstream out;
    bool cancelled = false;

    auto level = sokoban::cli::Menu::createCustomLevel(in, out, cancelled);
    SOKOBAN_ASSERT(!cancelled);
    SOKOBAN_ASSERT(level.board.width() >= 100);
    SOKOBAN_ASSERT(level.board.height() >= 100);
    SOKOBAN_ASSERT_EQ(level.initialState.boxCount(), 2);
    SOKOBAN_ASSERT_EQ(level.board.goalCount(), 2);
    SOKOBAN_ASSERT(level.isCustom);
    SOKOBAN_ASSERT(!level.xsbSource.empty());
}

SOKOBAN_TEST(Menu, PromptSaveCustomLevelSavesFile) {
    std::string testXsb = "#####\n#@$.#\n#####\n";
    std::string saveFile = "test_custom_save.xsb";
    std::filesystem::remove(saveFile);

    std::istringstream in("Y\n" + saveFile + "\n\n");
    std::ostringstream out;

    sokoban::cli::Menu::promptSaveCustomLevel(in, out, testXsb, "Тестовий");
    SOKOBAN_ASSERT(std::filesystem::exists(saveFile));

    // Verify content
    std::ifstream f(saveFile);
    std::stringstream buf;
    buf << f.rdbuf();
    SOKOBAN_ASSERT_EQ(buf.str(), testXsb);
    f.close();
    std::filesystem::remove(saveFile);
}

SOKOBAN_TEST(Menu, PromptSaveCustomLevelDeclined) {
    std::string testXsb = "#####\n#@$.#\n#####\n";
    std::istringstream in("n\n");
    std::ostringstream out;

    sokoban::cli::Menu::promptSaveCustomLevel(in, out, testXsb, "Тестовий");
    SOKOBAN_ASSERT(out.str().find("не збережено") != std::string::npos);
}

SOKOBAN_TEST(Menu, FullBoardRenderingLargeField) {
    sokoban::solvers::GeneratorOptions opt{
        .width = 50,
        .height = 50,
        .boxCount = 2,
        .seed = 12345
    };
    auto res = sokoban::solvers::LevelGenerator::generate(opt);
    SOKOBAN_ASSERT(res.success);
    SOKOBAN_ASSERT(res.board.width() >= 50);
    SOKOBAN_ASSERT(res.board.height() >= 50);

    sokoban::core::GameSession session(std::move(res.board), std::move(res.initialState));
    sokoban::cli::TerminalUI ui;

    std::stringstream ss;
    auto* oldBuf = std::cout.rdbuf(ss.rdbuf());
    ui.render(session, "Тест", "50x50", "Статус", "Підказка", 0.0, std::nullopt, {}, -1, false);
    std::cout.rdbuf(oldBuf);

    std::string rendered = ss.str();
    SOKOBAN_ASSERT(rendered.find("Повний огляд") != std::string::npos || rendered.find("50x50") != std::string::npos);
    SOKOBAN_ASSERT(rendered.find('@') != std::string::npos || rendered.find('+') != std::string::npos);
    SOKOBAN_ASSERT(rendered.find('$') != std::string::npos || rendered.find('*') != std::string::npos);
}

SOKOBAN_TEST(Menu, AiKeyMenuCancelDoesNotWrite) {
    std::istringstream in("B\n");
    std::ostringstream out;
    int ret = sokoban::cli::Menu::handleMenuChoice(4, in, out);
    SOKOBAN_ASSERT_EQ(ret, 0);
    SOKOBAN_ASSERT(out.str().find("НАЛАШТУВАТИ ШІ") != std::string::npos);
}

SOKOBAN_TEST(Menu, MainMenuListsAiKeyOption) {
    std::istringstream in("3\n");
    std::ostringstream out;
    sokoban::cli::Menu::run(in, out);
    SOKOBAN_ASSERT(out.str().find("Налаштувати ШІ") != std::string::npos);
}



