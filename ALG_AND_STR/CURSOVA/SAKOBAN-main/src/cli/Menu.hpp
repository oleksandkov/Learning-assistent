#pragma once

#include "../core/LevelParser.hpp"
#include <string>
#include <vector>
#include <istream>
#include <ostream>

namespace sokoban::cli {

class Menu {
public:
    static void run(std::istream& in, std::ostream& out);

    // Helpers for menu dispatch and testing
    static std::vector<std::string> discoverLevels(const std::string& baseDir = "levels");
    static int handleMenuChoice(int choice, std::istream& in, std::ostream& out);

    // About menu and animations
    static void showAboutMenu(std::istream& in, std::ostream& out);
    static void showBFSArticle(std::ostream& out);
    static void showAStarArticle(std::ostream& out);
    static void showIDAStarArticle(std::ostream& out);
    static void showGreedyArticle(std::ostream& out);
    static void showDeadlockArticle(std::ostream& out);
    static void animateBFS(std::ostream& out);
    static void animateAStar(std::ostream& out);
    static void animateIDAStar(std::ostream& out);
    static void animateGreedy(std::ostream& out);
    static void animateDeadlocks(std::ostream& out);

    static core::ParsedLevel selectPrebuiltLevel(std::istream& in, std::ostream& out, bool& cancelled);
    static core::ParsedLevel selectLevel(std::istream& in, std::ostream& out, bool& cancelled);
    static core::ParsedLevel createCustomLevel(std::istream& in, std::ostream& out, bool& cancelled);
    static core::ParsedLevel createManualLevel(std::istream& in, std::ostream& out,
                                               int expectedW, int expectedH, int expectedBags, bool& cancelled);
    static core::ParsedLevel loadCustomLevelFile(std::istream& in, std::ostream& out, bool& cancelled);
    static void promptSaveCustomLevel(std::istream& in, std::ostream& out,
                                      const std::string& xsbContent,
                                      const std::string& defaultTitle);
    static void promptAiKey(std::istream& in, std::ostream& out);
};

} // namespace sokoban::cli
