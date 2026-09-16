#include "AiClient.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>

#ifdef _WIN32
#define AI_POPEN _popen
#define AI_PCLOSE _pclose
#else
#define AI_POPEN popen
#define AI_PCLOSE pclose
#endif

namespace sokoban::cli {

namespace {

std::string trimKey(std::string s) {
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n' || s.back() == '\t')) s.pop_back();
    while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.erase(s.begin());
    return s;
}

// Копіювання з лапками ("key", 'key') або пробілами — чистимо перед аналізом.
std::string cleanKey(std::string s) {
    s = trimKey(std::move(s));
    if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') ||
                          (s.front() == '\'' && s.back() == '\''))) {
        s = s.substr(1, s.size() - 2);
    }
    return trimKey(s);
}

std::string readFileAll(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f.is_open()) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

std::string homeDir() {
    const char* home = std::getenv(
#ifdef _WIN32
        "USERPROFILE"
#else
        "HOME"
#endif
    );
    return (home && *home) ? home : ".";
}

} // namespace

std::string AiClient::keyFilePath() {
    return homeDir() + "/.sokoban_ai_key";
}

std::string AiClient::getApiKey() {
    const char* v = std::getenv("GROQ_API_KEY");
    if (v && *v && cleanKey(v).size() <= 512) return cleanKey(v);
    std::string f = cleanKey(readFileAll(keyFilePath()));
    if (!f.empty() && f.size() <= 512) return f;
    // Legacy Gemini setups keep working without re-entering anything.
    for (const char* var : {"GEMINI_API_KEY", "GOOGLE_API_KEY", "GOOGLE_GEMINI_KEY"}) {
        const char* v2 = std::getenv(var);
        if (v2 && *v2 && cleanKey(v2).size() <= 512) return cleanKey(v2);
    }
    return cleanKey(readFileAll(homeDir() + "/.sokoban_gemini_key"));
}

std::string AiClient::apiKeySource() {
    const char* v = std::getenv("GROQ_API_KEY");
    if (v && *v && cleanKey(v).size() <= 512) return "env GROQ_API_KEY";
    std::string f = cleanKey(readFileAll(keyFilePath()));
    if (!f.empty() && f.size() <= 512) return "файл ~/.sokoban_ai_key";
    for (const char* var : {"GEMINI_API_KEY", "GOOGLE_API_KEY", "GOOGLE_GEMINI_KEY"}) {
        const char* v2 = std::getenv(var);
        if (v2 && *v2 && cleanKey(v2).size() <= 512) return std::string("env ") + var;
    }
    if (!cleanKey(readFileAll(homeDir() + "/.sokoban_gemini_key")).empty()) return "файл ~/.sokoban_gemini_key";
    return "—";
}

bool AiClient::hasApiKey() {
    return !getApiKey().empty();
}

bool AiClient::setApiKey(const std::string& key, std::string& error) {
    std::string k = cleanKey(key);
    if (k.empty() || k.size() > 512) {
        error = "Порожній або задовгий ключ (макс. 512 символів).";
        return false;
    }
    std::ofstream f(keyFilePath(), std::ios::binary | std::ios::trunc);
    if (!f.is_open()) {
        error = "Не вдалося записати файл ключа: " + keyFilePath();
        return false;
    }
    f << k;
    return true;
}

std::string AiClient::getModel() {
    const char* v = std::getenv("GROQ_MODEL");
    std::string m = v ? trimKey(v) : "";
    if (!m.empty()) return m;
    m = trimKey(readFileAll(modelFilePath()));
    return m.empty() ? kDefaultModel : m;
}

std::string AiClient::modelFilePath() {
    return homeDir() + "/.sokoban_ai_model";
}

bool AiClient::setModel(const std::string& id, std::string& error) {
    std::string m = trimKey(id);
    bool ok = !m.empty() && m.size() <= 128;
    for (char c : m) {
        if (!(c == '-' || c == '.' || c == '_' ||
              (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
            ok = false;
            break;
        }
    }
    if (!ok) {
        error = "Недійсний id моделі (дозволено літери, цифри, - . _ , макс. 128).";
        return false;
    }
    std::ofstream f(modelFilePath(), std::ios::binary | std::ios::trunc);
    if (!f.is_open()) {
        error = "Не вдалося записати файл моделі: " + modelFilePath();
        return false;
    }
    f << m;
    return true;
}

std::vector<std::string> AiClient::defaultFreeModels() {
    return {"openai/gpt-oss-120b", "llama-3.3-70b-versatile", "moonshotai/kimi-k2-instruct",
            "qwen/qwen3-32b", "openai/gpt-oss-20b", "llama-3.1-8b-instant"};
}

std::string AiClient::groqErrorText(const std::string& httpCode,
                                          const std::string& serverMsg,
                                          int attempts) {
    auto tries = " (спроб: " + std::to_string(attempts) + ")";
    std::string tail = serverMsg.empty() ? "" : (" Сервер: " + serverMsg);
    if (httpCode == "429") return "Досягнуто ліміт Groq" + tries + "." + tail + " Free tier має денні ліміти токенів. Зачекай або спробуй меншу позицію.";
    if (httpCode == "503" || (!httpCode.empty() && httpCode[0] == '5')) return "Сервер AI перевантажений (HTTP " + httpCode + ")" + tries + "." + tail + " Спробуй трохи пізніше.";
    if (httpCode == "404") return "Модель не знайдено в Groq." + tail + " Обери іншу зі списку (пункт 2 у «Налаштувати ШІ»).";
    if (httpCode == "400") return "AI не прийняв запит." + tail + " Перевір модель (GROQ_MODEL) і розмір рівня.";
    if (httpCode == "401" || httpCode == "403") return "Недійсний API-ключ Groq." + tail + " Встав ключ з console.groq.com ([K]).";
    return "Немає зв'язку з Groq" + tries + " — потрібні інтернет і curl у PATH.";
}

std::string AiClient::googleErrorText(const std::string& httpCode,
                                      const std::string& serverMsg,
                                      int attempts,
                                      const std::string& model) {
    auto tries = " (спроб: " + std::to_string(attempts) + ")";
    std::string tail = serverMsg.empty() ? "" : (" Сервер: " + serverMsg);
    if (httpCode == "429") return "Досягнуто ліміт Google AI" + tries + ". Зачекай або перевір квоту проєкту в Google AI Studio.";
    if (httpCode == "503") return "Google AI перевантажений (503)" + tries + " — це брак потужностей Google. Спробуй трохи пізніше або встав ключ Groq ([K]).";
    if (!httpCode.empty() && httpCode[0] == '5') return "Сервер Google AI недоступний (HTTP " + httpCode + ")" + tries + ". Спробуй трохи пізніше.";
    if (httpCode == "404") return "Модель " + model + " не знайдено в Google AI." + tail + " Google знімає старі моделі — обери іншу зі списку (пункт 2 у «Налаштувати ШІ»).";
    if (httpCode == "400") return "Google AI не прийняв запит (перевір ключ і розмір рівня)." + tail;
    if (httpCode == "401" || httpCode == "403") return "Недійсний API-ключ Google. Натисни [K] щоб оновити.";
    return "Немає зв'язку з Google" + tries + " — потрібні інтернет і curl у PATH.";
}

std::vector<std::string> AiClient::filterFreeModels(const std::string& modelsJson) {
    std::vector<std::string> out;
    std::size_t pos = 0;
    while ((pos = modelsJson.find("\"id\"", pos)) != std::string::npos) {
        std::size_t colon = modelsJson.find(':', pos + 4);
        if (colon == std::string::npos) break;
        std::size_t q = modelsJson.find('"', colon + 1);
        if (q == std::string::npos) break;
        std::size_t end = modelsJson.find('"', q + 1);
        if (end == std::string::npos) break;
        std::string id = modelsJson.substr(q + 1, end - q - 1);
        // Беремо всі id без дублів (формат списку моделей OpenAI-сумісний).
        if (!id.empty() && std::find(out.begin(), out.end(), id) == out.end()) out.push_back(id);
        pos = end + 1;
    }
    return out;
}

std::vector<std::string> AiClient::listFreeModels() {
    // Список Groq /openai/v1/models потребує ключа, тому лише перевірений статичний список.
    return defaultFreeModels();
}

AiProvider AiClient::detectProvider(const std::string& key) {
    std::string k = cleanKey(key);
    // Google: legacy AIza... + нові Auth keys AQ. (AI Studio видає лише AQ).
    if (k.rfind("AIza", 0) == 0 || k.rfind("AQ.", 0) == 0) return AiProvider::Google;
    return AiProvider::Groq;
}

std::string AiClient::escapeJson(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 16);
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}

std::string AiClient::buildPrompt(const std::string& xsb) {
    return "Solve this Sokoban position. # is a wall, @ the player, $ a box, . a goal, "
           "* a box on a goal and + a player on a goal. Think step by step and find a complete "
           "legal route that puts every box on a goal. Use only U, D, L and R for moves. "
           "A player may push one box but cannot pull boxes. "
           "Reply with JSON only, no markdown fences: "
           "{\"moves\": \"UDLR characters\", \"explanation\": \"one short sentence in Ukrainian\"}.\n\n" + xsb;
}

std::string AiClient::buildRequestBody(const std::string& prompt,
                                       const std::string& model,
                                       bool full) {
    std::ostringstream b;
    b << "{\"model\":\"" << escapeJson(model) << "\","
      << "\"messages\":[{\"role\":\"user\",\"content\":\"" << escapeJson(prompt) << "\"}]";
    if (full) {
        b << ",\"temperature\":0,\"max_tokens\":4096,"
          << "\"response_format\":{\"type\":\"json_object\"}";
    }
    b << "}";
    return b.str();
}

std::string AiClient::buildGoogleBody(const std::string& prompt) {
    std::ostringstream b;
    b << "{\"contents\":[{\"parts\":[{\"text\":\"" << escapeJson(prompt) << "\"}]}],"
      << "\"tools\":[{\"codeExecution\":{}}],"
      << "\"generationConfig\":{\"temperature\":0,\"maxOutputTokens\":8192,"
      << "\"responseMimeType\":\"application/json\","
      << "\"responseSchema\":{\"type\":\"OBJECT\","
      << "\"properties\":{"
      << "\"moves\":{\"type\":\"STRING\",\"description\":\"The complete solution as UDLR characters only.\"},"
      << "\"explanation\":{\"type\":\"STRING\",\"description\":\"One short sentence explaining the plan.\"}},"
      << "\"required\":[\"moves\"]}}}";
    return b.str();
}

// Finds quoted value of a JSON field; tolerant to nesting/escapes
// (answer may sit inside choices[].message.content as JSON text).
namespace {
// Field keys arrive plain ("moves") or escaped (\"moves\") when nested.
// Returns the colon position.
std::size_t findFieldColon(const std::string& s, const std::string& field, std::size_t from) {
    while (true) {
        std::size_t p = s.find(field, from);
        if (p == std::string::npos) return std::string::npos;
        std::size_t e = p + field.size();
        bool preOk = (p >= 1 && s[p - 1] == '"');
        bool postOk = (e < s.size() && s[e] == '"') ||
                      (e + 1 < s.size() && s[e] == '\\' && s[e + 1] == '"');
        if (preOk && postOk) {
            std::size_t c = (e < s.size() && s[e] == '"') ? e + 1 : e + 2;
            while (c < s.size() && (s[c] == ' ' || s[c] == '\t' || s[c] == '\n' || s[c] == '\r')) ++c;
            if (c < s.size() && s[c] == ':') return c;
        }
        from = p + 1;
    }
}
bool captureField(const std::string& payload, const std::string& field,
                  std::string& rawOut, bool udlrOnly) {
    std::string best;
    bool found = false;
    std::size_t pos = 0;
    auto isClose = [&](const std::string& s, std::size_t j) {
        // Bare quote, or escaped quote closing the (possibly nested) string:
        // \" followed by structural JSON or end of payload.
        if (j >= s.size()) return true;
        if (s[j] == '"') return true;
        if (s[j] == '\\' && j + 1 < s.size() && s[j + 1] == '"') {
            std::size_t k = j + 2;
            while (k < s.size() && (s[k] == ' ' || s[k] == '\t' || s[k] == '\n' || s[k] == '\r')) ++k;
            return k >= s.size() || s[k] == ',' || s[k] == '}' || s[k] == ']';
        }
        return false;
    };
    while ((pos = findFieldColon(payload, field, pos)) != std::string::npos) {
        std::size_t colon = pos;
        std::size_t q = payload.find('"', colon + 1);
        if (q == std::string::npos) break;
        std::string cur;
        bool ok = false;
        if (udlrOnly) {
            // UDLR routes never contain escapes: read the letter run, then expect close.
            std::size_t j = q + 1;
            while (j < payload.size()) {
                char c = payload[j];
                if (c == 'U' || c == 'D' || c == 'L' || c == 'R') { cur += c; ++j; }
                else break;
            }
            if (cur.size() <= 10000 && isClose(payload, j)) {
                if (!found || cur.size() > best.size()) { best = cur; found = true; }
            }
        } else {
            for (std::size_t i = q + 1; i < payload.size(); ++i) {
                if (isClose(payload, i)) { ok = true; break; }
                char c = payload[i];
                if (c == '\\' && i + 1 < payload.size()) {
                    ++i;
                    cur += (payload[i] == 'n') ? '\n' : payload[i];
                    continue;
                }
                cur += c;
            }
            if (ok) {
                rawOut = cur;
                return true;
            }
        }
        pos = q + 1;
    }
    if (udlrOnly && found) { rawOut = best; return true; }
    return false;
}
} // namespace

bool AiClient::extractMoves(const std::string& payload, std::string& movesOut) {
    return captureField(payload, "moves", movesOut, true);
}

void AiClient::extractExplanation(const std::string& payload, std::string& explOut) {
    std::string e;
    if (captureField(payload, "explanation", e, false)) {
        if (e.size() > 500) e.resize(500);
        explOut = e;
    } else {
        explOut.clear();
    }
}

void AiClient::extractServerMessage(const std::string& payload, std::string& msgOut) {
    std::string m;
    if (captureField(payload, "message", m, false) && !m.empty()) {
        if (m.size() > 200) m.resize(200);
        msgOut = m;
    } else {
        msgOut.clear();
    }
}

AiResult AiClient::solveFromXsb(const std::string& xsb,
                                const std::string& apiKey,
                                long timeoutSec,
                                AiProvider provider) {
    AiResult r;
    std::string key = cleanKey(apiKey);
    if (key.empty()) {
        r.error = "Немає API-ключа. Натисни [K] і встав ключ Groq або Google AI Studio.";
        return r;
    }
    if (xsb.size() > 10500) {
        r.error = "Рівень завеликий для запиту AI.";
        return r;
    }
    AiProvider prov = (provider == AiProvider::Auto) ? detectProvider(key) : provider;
    if (prov == AiProvider::Google) return solveGoogle(xsb, key, timeoutSec);
    return solveGroq(xsb, key, timeoutSec);
}

AiResult AiClient::solveGroq(const std::string& xsb,
                            const std::string& key,
                            long timeoutSec) {
    AiResult r;
    r.providerLabel = "Groq";
    r.model = getModel();
    r.prompt = buildPrompt(xsb);

    namespace fs = std::filesystem;
    std::string tmpDir;
    try {
        tmpDir = fs::temp_directory_path().string();
    } catch (...) {
        tmpDir = ".";
    }
    auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    std::string reqFile = tmpDir + "/sokoban_ai_req_" + std::to_string(stamp) + ".json";
    std::string resFile = tmpDir + "/sokoban_ai_res_" + std::to_string(stamp) + ".json";

    auto writeBody = [&](const std::string& body) {
        std::ofstream f(reqFile, std::ios::binary | std::ios::trunc);
        if (!f.is_open()) return false;
        f << body;
        return true;
    };
    if (!writeBody(buildRequestBody(r.prompt, r.model, true))) {
        r.error = "Не вдалося створити тимчасовий файл запиту.";
        return r;
    }

    // ponytail: shell out to curl.exe instead of an HTTP lib; needs curl in PATH (stock on Win10+).
    std::ostringstream cmd;
    cmd << "curl.exe -s -m " << timeoutSec << " -X POST"
        << " -H \"Content-Type: application/json\""
        << " -H \"Authorization: Bearer " << key << "\""
        << " --data-binary \"@" << reqFile << "\""
        << " -o \"" << resFile << "\""
        << " -w \"%{http_code}\" \"" << kEndpoint << "\" 2>&1";
    std::string cmdStr = cmd.str();

    auto modelStart = std::chrono::steady_clock::now();
    std::string httpCode, payload;
    bool minimal = false;
    for (int attempt = 0; attempt < 3; ++attempt) {
        std::string out;
        FILE* pipe = AI_POPEN(cmdStr.c_str(), "r");
        if (pipe) {
            char buf[256];
            while (std::fgets(buf, sizeof(buf), pipe)) out += buf;
            AI_PCLOSE(pipe);
        }
        httpCode = trimKey(out);
        payload = readFileAll(resFile);
        ++r.attempts;
        if (httpCode == "400" && !minimal) {
            // Деякі моделі відхиляють temperature/response_format — повтор без них.
            minimal = true;
            writeBody(buildRequestBody(r.prompt, r.model, false));
            continue;
        }
        bool okHttp = (httpCode == "200" && !payload.empty());
        bool retryable = (httpCode == "429" || httpCode == "408" || httpCode.empty() ||
                          payload.empty() || (!httpCode.empty() && httpCode[0] == '5'));
        if (okHttp || !retryable || attempt == 2) break;
        std::this_thread::sleep_for(std::chrono::seconds(2 << attempt)); // 2с, 4с
    }
    r.modelMs = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - modelStart).count();

    std::remove(reqFile.c_str());
    std::remove(resFile.c_str());

    if (httpCode != "200" || payload.empty()) {
        std::string srv;
        extractServerMessage(payload, srv);
        r.error = groqErrorText(httpCode, srv, r.attempts);
        return r;
    }

    std::string moves;
    if (!extractMoves(payload, moves)) {
        r.error = "AI не повернув маршрут у форматі UDLR.";
        return r;
    }
    extractExplanation(payload, r.explanation);
    r.moves = moves;
    r.ok = true; // caller validates the route against core rules before applying
    return r;
}

AiResult AiClient::solveGoogle(const std::string& xsb,
                               const std::string& key,
                               long timeoutSec) {
    AiResult r;
    r.providerLabel = "Google AI";
    r.model = "gemini-3.8-flash";
    r.prompt = buildPrompt(xsb);
    const char* endpoint =
        "https://generativelanguage.googleapis.com/v1beta/models/gemini-3.8-flash:generateContent";

    namespace fs = std::filesystem;
    std::string tmpDir;
    try {
        tmpDir = fs::temp_directory_path().string();
    } catch (...) {
        tmpDir = ".";
    }
    auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    std::string reqFile = tmpDir + "/sokoban_ai_req_" + std::to_string(stamp) + ".json";
    std::string resFile = tmpDir + "/sokoban_ai_res_" + std::to_string(stamp) + ".json";
    {
        std::ofstream f(reqFile, std::ios::binary | std::ios::trunc);
        if (!f.is_open()) {
            r.error = "Не вдалося створити тимчасовий файл запиту.";
            return r;
        }
        f << buildGoogleBody(r.prompt);
    }

    std::ostringstream cmd;
    cmd << "curl.exe -s -m " << timeoutSec << " -X POST"
        << " -H \"Content-Type: application/json\""
        << " -H \"x-goog-api-key: " << key << "\""
        << " --data-binary \"@" << reqFile << "\""
        << " -o \"" << resFile << "\""
        << " -w \"%{http_code}\" \"" << endpoint << "\" 2>&1";
    std::string cmdStr = cmd.str();

    auto modelStart = std::chrono::steady_clock::now();
    std::string httpCode, payload;
    for (int attempt = 0; attempt < 3; ++attempt) {
        std::string out;
        FILE* pipe = AI_POPEN(cmdStr.c_str(), "r");
        if (pipe) {
            char buf[256];
            while (std::fgets(buf, sizeof(buf), pipe)) out += buf;
            AI_PCLOSE(pipe);
        }
        httpCode = trimKey(out);
        payload = readFileAll(resFile);
        ++r.attempts;
        bool okHttp = (httpCode == "200" && !payload.empty());
        bool retryable = (httpCode == "429" || httpCode == "408" || httpCode.empty() ||
                          payload.empty() || (!httpCode.empty() && httpCode[0] == '5'));
        if (okHttp || !retryable || attempt == 2) break;
        std::this_thread::sleep_for(std::chrono::seconds(2 << attempt)); // 2с, 4с
    }
    r.modelMs = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - modelStart).count();

    std::remove(reqFile.c_str());
    std::remove(resFile.c_str());

    if (httpCode != "200" || payload.empty()) {
        std::string srv;
        extractServerMessage(payload, srv);
        r.error = googleErrorText(httpCode, srv, r.attempts, r.model);
        return r;
    }

    std::string moves;
    if (!extractMoves(payload, moves)) {
        r.error = "AI не повернув маршрут у форматі UDLR.";
        return r;
    }
    extractExplanation(payload, r.explanation);
    r.moves = moves;
    r.ok = true;
    return r;
}

} // namespace sokoban::cli
