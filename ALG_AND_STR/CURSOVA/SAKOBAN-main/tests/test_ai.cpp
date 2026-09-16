#include "TestHarness.hpp"
#include "cli/AiClient.hpp"
#include "cli/AlgorithmComparator.hpp"
#include <sstream>
#include <filesystem>
#include <algorithm>

using sokoban::cli::AiClient;
using sokoban::cli::AiProvider;

SOKOBAN_TEST(Ai, DetectStripsQuotesAndSpaces) {
    SOKOBAN_ASSERT(AiClient::detectProvider("\"AIzaSyDUMMY\"") == AiProvider::Google);
    SOKOBAN_ASSERT(AiClient::detectProvider("  'gsk-abc'  ") == AiProvider::Groq);
    SOKOBAN_ASSERT(AiClient::detectProvider("AIzaSyDUMMY\n") == AiProvider::Google);
}

SOKOBAN_TEST(Ai, DetectGoogleByKeyPrefix) {
    SOKOBAN_ASSERT(AiClient::detectProvider("AIzaSyDUMMYKEY123") == AiProvider::Google);
    SOKOBAN_ASSERT(AiClient::detectProvider("AIzaSyDUMMYKEY123") == AiProvider::Google);
}

SOKOBAN_TEST(Ai, DetectGroqOtherwise) {
    SOKOBAN_ASSERT(AiClient::detectProvider("gsk-abc123") == AiProvider::Groq);
    SOKOBAN_ASSERT(AiClient::detectProvider("sk-anything") == AiProvider::Groq);
    SOKOBAN_ASSERT(AiClient::detectProvider("") == AiProvider::Groq);
}

SOKOBAN_TEST(Ai, EscapeJsonQuotesAndNewlines) {
    std::string out = AiClient::escapeJson("a\"b\\c\nd");
    SOKOBAN_ASSERT_EQ(out, "a\\\"b\\\\c\\nd");
}

SOKOBAN_TEST(Ai, PromptContainsXsbAndJsonInstruction) {
    std::string xsb = "#####\n#@$.#\n#####";
    std::string p = AiClient::buildPrompt(xsb);
    SOKOBAN_ASSERT(p.find(xsb) != std::string::npos);
    SOKOBAN_ASSERT(p.find("\"moves\"") != std::string::npos);
}

SOKOBAN_TEST(Ai, GroqBodyIsChatCompletions) {
    std::string body = AiClient::buildRequestBody("hello \"world\"", "openai/gpt-oss-120b", true);
    SOKOBAN_ASSERT(body.find("\"model\":\"openai/gpt-oss-120b\"") != std::string::npos);
    SOKOBAN_ASSERT(body.find("\"role\":\"user\"") != std::string::npos);
    SOKOBAN_ASSERT(body.find("response_format") != std::string::npos);
    SOKOBAN_ASSERT(body.find("hello \\\"world\\\"") != std::string::npos);
}

SOKOBAN_TEST(Ai, GroqMinimalBodyDropsConstrainedFields) {
    std::string body = AiClient::buildRequestBody("hi", "openai/gpt-oss-120b", false);
    SOKOBAN_ASSERT(body.find("response_format") == std::string::npos);
    SOKOBAN_ASSERT(body.find("temperature") == std::string::npos);
    SOKOBAN_ASSERT(body.find("\"model\":\"openai/gpt-oss-120b\"") != std::string::npos);
}

SOKOBAN_TEST(Ai, GoogleBodyHasCodeExecution) {
    std::string body = AiClient::buildGoogleBody("hello");
    SOKOBAN_ASSERT(body.find("codeExecution") != std::string::npos);
    SOKOBAN_ASSERT(body.find("responseSchema") != std::string::npos);
}

SOKOBAN_TEST(Ai, ExtractMovesChatCompletionStyle) {
    std::string payload =
        "{\"id\":\"chatcmpl-1\",\"choices\":[{\"message\":{"
        "\"content\":\"{\\\"moves\\\":\\\"UUDDLL\\\",\\\"explanation\\\":\\\"Short plan.\\\"}\"}}]}";
    std::string moves;
    SOKOBAN_ASSERT(AiClient::extractMoves(payload, moves));
    SOKOBAN_ASSERT_EQ(moves, "UUDDLL");
    std::string expl;
    AiClient::extractExplanation(payload, expl);
    SOKOBAN_ASSERT(expl.find("Short plan") != std::string::npos);
}

SOKOBAN_TEST(Ai, ExtractMovesPlainJson) {
    std::string moves;
    SOKOBAN_ASSERT(AiClient::extractMoves("{\"moves\":\"UURDL\",\"explanation\":\"x\"}", moves));
    SOKOBAN_ASSERT_EQ(moves, "UURDL");
}

SOKOBAN_TEST(Ai, ExtractMovesRejectsNonUdlr) {
    std::string moves = "SENTINEL";
    SOKOBAN_ASSERT(!AiClient::extractMoves("{\"moves\":\"UP-DOWN\"}", moves));
}

SOKOBAN_TEST(Ai, ExtractServerErrorMessage) {
    std::string msg;
    AiClient::extractServerMessage(
        "{\"type\":\"error\",\"error\":{\"type\":\"AuthError\",\"message\":\"Invalid API key.\"}}", msg);
    SOKOBAN_ASSERT_EQ(msg, "Invalid API key.");
}

SOKOBAN_TEST(Ai, SolveWithoutKeyFailsFastNoNetwork) {
    auto r = AiClient::solveFromXsb("#####\n#@$.#\n#####", "", 5);
    SOKOBAN_ASSERT(!r.ok);
    SOKOBAN_ASSERT(!r.error.empty());
}

SOKOBAN_TEST(Ai, SolveOversizedLevelRejected) {
    auto r = AiClient::solveFromXsb(std::string(10600, '#'), "dummy-key", 5);
    SOKOBAN_ASSERT(!r.ok);
    SOKOBAN_ASSERT(!r.error.empty());
}

SOKOBAN_TEST(Ai, CompareTableShowsAiRowWideAndCompact) {
    std::string lvl = "levels/01_simple.xsb";
    if (!std::filesystem::exists(lvl)) lvl = "../levels/01_simple.xsb";
    auto report = sokoban::cli::AlgorithmComparator::compareFile(lvl);

    sokoban::cli::AlgorithmComparisonResult grow;
    grow.algorithmName = "AI";
    grow.kind = sokoban::solvers::SolverKind::AStar;
    grow.metric = sokoban::solvers::OptimizationMetric::Pushes;
    grow.metricLabel = "AI-план";
    grow.status = sokoban::solvers::SearchStatus::Solved;
    grow.moves = 1;
    grow.pushes = 1;
    grow.replayValid = true;
    grow.isOptimal = true;
    grow.optimalityDescription = "AI (перевірено)";
    report.results.push_back(grow);
    SOKOBAN_ASSERT_EQ(report.results.size(), 6ULL);

    std::ostringstream wide;
    sokoban::cli::AlgorithmComparator::printTable(report, wide, false);
    SOKOBAN_ASSERT(wide.str().find("AI-план") != std::string::npos);
    SOKOBAN_ASSERT(wide.str().find("AI (перевірено)") != std::string::npos);

    std::ostringstream narrow;
    sokoban::cli::AlgorithmComparator::printTable(report, narrow, false, 70);
    SOKOBAN_ASSERT(narrow.str().find("AI") != std::string::npos); // row label stays
    SOKOBAN_ASSERT(narrow.str().find("Оптимальність") == std::string::npos);
    SOKOBAN_ASSERT(narrow.str().find("Розкрито") == std::string::npos);
}

SOKOBAN_TEST(Ai, FilterFreeModelsFromCatalogJson) {
    std::string json =
        "{\"object\":\"list\",\"data\":["
        "{\"id\":\"openai/gpt-oss-120b\",\"object\":\"model\"},"
        "{\"id\":\"llama-3.3-70b-versatile\",\"object\":\"model\"},"
        "{\"id\":\"openai/gpt-oss-120b\",\"object\":\"model\"}]}";
    auto ids = AiClient::filterFreeModels(json);
    SOKOBAN_ASSERT_EQ(ids.size(), 2ULL);
    SOKOBAN_ASSERT_EQ(ids[0], "openai/gpt-oss-120b");
    SOKOBAN_ASSERT_EQ(ids[1], "llama-3.3-70b-versatile");
}

SOKOBAN_TEST(Ai, DefaultFreeModelsNotEmpty) {
    auto ids = AiClient::defaultFreeModels();
    SOKOBAN_ASSERT(!ids.empty());
    SOKOBAN_ASSERT(std::find(ids.begin(), ids.end(), "openai/gpt-oss-120b") != ids.end());
    SOKOBAN_ASSERT(ids.size() >= 4ULL); // широкий вибір безкоштовних моделей
}

SOKOBAN_TEST(Ai, SetModelRejectsBadIdWithoutWriting) {    std::string err;
    SOKOBAN_ASSERT(!AiClient::setModel("../../evil key!", err));
    SOKOBAN_ASSERT(!err.empty());
    SOKOBAN_ASSERT(!AiClient::setModel("", err));
}

SOKOBAN_TEST(Ai, GroqErrorMappings) {
    SOKOBAN_ASSERT(AiClient::groqErrorText("401", "", 2).find("Недійсний API-ключ") != std::string::npos);
    SOKOBAN_ASSERT(AiClient::groqErrorText("429", "", 1).find("ліміт Groq") != std::string::npos);
    SOKOBAN_ASSERT(AiClient::groqErrorText("503", "", 3).find("перевантажений") != std::string::npos);
    SOKOBAN_ASSERT(AiClient::groqErrorText("", "", 3).find("зв'язку") != std::string::npos);
    SOKOBAN_ASSERT(AiClient::groqErrorText("404", "", 1).find("не знайдено в Groq") != std::string::npos);
}

SOKOBAN_TEST(Ai, GoogleErrorModelNotFound) {
    std::string e = AiClient::googleErrorText(
        "404", "This model models/gemini-3.8-flash is no longer available.", 1, "gemini-3.8-flash");
    SOKOBAN_ASSERT(e.find("gemini-3.8-flash") != std::string::npos);
    SOKOBAN_ASSERT(e.find("обери іншу зі списку") != std::string::npos);
    SOKOBAN_ASSERT(e.find("no longer available") != std::string::npos);
}
