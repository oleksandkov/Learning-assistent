#pragma once

// External AI solver for CLI via Groq (https://api.groq.com/openai/v1,
// OpenAI-compatible chat/completions, free tier, Bearer key). Board serialized to XSB,
// model returns JSON (moves/explanation), every route validated by core rules.
// Transport: curl.exe via popen (no new deps). Key: GROQ_API_KEY or ~/.sokoban_ai_key.

#include <string>
#include <optional>
#include <vector>
#include <cstdint>

namespace sokoban::cli {

// Ключ аналізується автоматично: AIza.../AQ. → Google AI, інше → Groq.
// --algorithm ai = авто, gemini/groq = примусово.
enum class AiProvider { Auto, Groq, Google };

struct AiResult {
    bool ok = false;
    std::string moves;        // UDLR string
    std::string explanation;  // Ukrainian, max ~500 chars
    double modelMs = 0.0;
    double validationMs = 0.0;
    std::uint64_t pushes = 0;
    std::string error;        // user-facing message on failure
    std::string prompt;       // sent prompt (transparency, like web aiSession)
    std::string model;        // model id actually used (Groq) or "" (Google default)
    std::string providerLabel; // "Groq" | "Google AI"
    int attempts = 0;         // HTTP attempts used (auto-retry on 429/5xx/network)
};

class AiClient {
public:
    static constexpr const char* kEndpoint =
        "https://api.groq.com/openai/v1/chat/completions";
    static constexpr const char* kDefaultModel = "openai/gpt-oss-120b"; // reasoning, free tier

    // GROQ_API_KEY wins, else key file.
    // Legacy Gemini env/file also read so old setups keep working.
    static std::string getApiKey();
    static bool hasApiKey();
    // Where the active key comes from (menu status line).
    static std::string apiKeySource();
    // Saves to per-user file (web stores in localStorage; CLI equivalent).
    static bool setApiKey(const std::string& key, std::string& error);
    static std::string keyFilePath();

    // Model id: GROQ_MODEL override, else saved file, else default.
    static std::string getModel();
    static bool setModel(const std::string& id, std::string& error);
    static std::string modelFilePath();

    // Groq models: verified free-tier list (API list needs a key, so static).
    static std::vector<std::string> listFreeModels();
    static std::vector<std::string> filterFreeModels(const std::string& modelsJson); // pure
    static std::vector<std::string> defaultFreeModels();
    // Pure helper: user-facing text for a failed Groq call.
    static std::string groqErrorText(const std::string& httpCode,
                                     const std::string& serverMsg,
                                     int attempts);
    // Pure helper: user-facing text for a failed Google call.
    static std::string googleErrorText(const std::string& httpCode,
                                       const std::string& serverMsg,
                                       int attempts,
                                       const std::string& model);

    // AIza... / AQ. → Google, все інше → Groq.
    static AiProvider detectProvider(const std::string& key);

    // Pure helpers (unit-tested, no network).
    static std::string buildPrompt(const std::string& xsb);
    static std::string buildRequestBody(const std::string& prompt,
                                        const std::string& model,
                                        bool full = true); // full=false: minimal fallback on 400
    static std::string buildGoogleBody(const std::string& prompt); // generateContent + codeExecution
    static bool extractMoves(const std::string& payload, std::string& movesOut);
    static void extractExplanation(const std::string& payload, std::string& explOut);
    static void extractServerMessage(const std::string& payload, std::string& msgOut);
    static std::string escapeJson(const std::string& s);

    // Full flow: serialize -> POST -> parse. Caller validates via core rules.
    static AiResult solveFromXsb(const std::string& xsb,
                                 const std::string& apiKey,
                                 long timeoutSec = 60,
                                 AiProvider provider = AiProvider::Auto);

private:
    static AiResult solveGroq(const std::string& xsb,
                             const std::string& key,
                             long timeoutSec);
    static AiResult solveGoogle(const std::string& xsb,
                                const std::string& key,
                                long timeoutSec);
};

} // namespace sokoban::cli
