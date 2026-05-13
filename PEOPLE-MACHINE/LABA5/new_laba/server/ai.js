import "dotenv/config";
import axios from "axios";
import FormData from "form-data";

const ASSEMBLYAI_API_KEY = process.env.ASSEMBLYAI_API_KEY;
const GOOGLE_GEMINI_API_KEY = process.env.GOOGLE_GEMINI_API_KEY;
const GOOGLE_GEMINI_MODEL = process.env.GOOGLE_GEMINI_MODEL || "gemini-2.5-flash";
const STT_PROVIDER = (process.env.STT_PROVIDER || "assemblyai").toLowerCase();
const ASSEMBLYAI_BASE_URL = "https://api.assemblyai.com";
const ASSEMBLYAI_SPEECH_MODELS = String(process.env.ASSEMBLYAI_SPEECH_MODELS || "universal-3-pro,universal-2")
  .split(",")
  .map((model) => model.trim())
  .filter(Boolean);
const GOOGLE_GEMINI_BASE_URL = "https://generativelanguage.googleapis.com/v1beta/models";

const geminiClient = axios.create({
  baseURL: GOOGLE_GEMINI_BASE_URL,
  headers: {
    "Content-Type": "application/json",
  },
});

const assemblyClient = axios.create({
  baseURL: ASSEMBLYAI_BASE_URL,
  headers: {
    authorization: ASSEMBLYAI_API_KEY,
  },
});

function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

export async function speechToText(audioBuffer) {
  if (STT_PROVIDER !== "assemblyai") {
    throw new Error(`Unsupported STT_PROVIDER: ${STT_PROVIDER}`);
  }

  if (!ASSEMBLYAI_API_KEY) {
    throw new Error("ASSEMBLYAI_API_KEY is not configured");
  }

  try {
    const uploadResponse = await assemblyClient.post("/v2/upload", audioBuffer, {
      headers: {
        authorization: ASSEMBLYAI_API_KEY,
        "content-type": "application/octet-stream",
      },
      maxBodyLength: Infinity,
      maxContentLength: Infinity,
    });

    const uploadUrl = uploadResponse.data?.upload_url;
    if (!uploadUrl) {
      throw new Error("AssemblyAI did not return an upload URL");
    }

    const transcriptResponse = await assemblyClient.post(
      "/v2/transcript",
      {
        audio_url: uploadUrl,
        speech_models: ASSEMBLYAI_SPEECH_MODELS,
        language_detection: true,
      },
      {
        headers: {
          authorization: ASSEMBLYAI_API_KEY,
          "content-type": "application/json",
        },
      }
    );

    const transcriptId = transcriptResponse.data?.id;
    if (!transcriptId) {
      throw new Error("AssemblyAI did not return a transcript id");
    }

    while (true) {
      const pollResponse = await assemblyClient.get(`/v2/transcript/${transcriptId}`, {
        headers: { authorization: ASSEMBLYAI_API_KEY },
      });

      const result = pollResponse.data;
      if (result.status === "completed") {
        return result.text || "";
      }

      if (result.status === "error") {
        throw new Error(
          `Failed to transcribe audio: ${result.error || "AssemblyAI transcription failed"}`
        );
      }

      await sleep(3000);
    }
  } catch (error) {
    console.error("Speech to text error:", error.response?.data || error.message);
    throw new Error(
      `Failed to transcribe audio: ${error.response?.data?.error || error.message}`
    );
  }
}

export async function generateResponse(prompt) {
  if (!GOOGLE_GEMINI_API_KEY) {
    throw new Error("GOOGLE_GEMINI_API_KEY is not configured");
  }

  try {
    const systemInstruction = `Ви помічник системи керування розкладом.
Відповідайте виключно українською мовою.
Поверніть лише один валідний JSON-об'єкт без markdown, без пояснень і без коду.`;

    const responseSchema = {
      type: "object",
      properties: {
        action: {
          type: "string",
          enum: ["add_lesson", "edit_lesson", "delete_lesson", "list_lessons", "describe_curriculum", "unknown"],
        },
        parameters: {
          type: "object",
          properties: {
            id: { type: "string" },
            subject: { type: "string" },
            day: { type: "string" },
            time: { type: "string" },
            room: { type: "string" },
            group: { type: "string" },
            teacher: { type: "string" },
            week: { type: "string" },
            type: { type: "string" },
          },
          required: ["id", "subject", "day", "time", "room", "group", "teacher", "week", "type"],
        },
        missing_fields: {
          type: "array",
          items: { type: "string" },
        },
        delete_all_matching: { type: "boolean" },
        table_summary: { type: "string" },
        curriculum_notes: { type: "string" },
        natural_response: { type: "string" },
      },
      required: ["action", "parameters", "missing_fields", "delete_all_matching", "table_summary", "curriculum_notes", "natural_response"],
    };

    const url = `${GOOGLE_GEMINI_BASE_URL}/${GOOGLE_GEMINI_MODEL}:generateContent?key=${GOOGLE_GEMINI_API_KEY}`;
    console.log("🔄 Calling Gemini API:", url.substring(0, 80) + "...");
    
    const response = await geminiClient.post(url, {
      contents: [
        {
          parts: [
            {
              text: systemInstruction + "\n\n" + prompt,
            },
          ],
        },
      ],
      generationConfig: {
        temperature: 0,
        maxOutputTokens: 2048,
        responseMimeType: "application/json",
        responseJsonSchema: responseSchema,
      },
    }, {
      timeout: 30000, // 30 second timeout
    });

    console.log("✅ Gemini API Response Status:", response.status);

    const parts = response.data?.candidates?.[0]?.content?.parts || [];
    const textContent = parts
      .map((part) => part?.text)
      .filter((text) => typeof text === "string" && text.trim())
      .join("");

    if (!textContent) {
      console.error("❌ Gemini API returned no text content:", JSON.stringify(response.data, null, 2));
      throw new Error("No text content in Gemini response");
    }

    console.log("✅ Got text content from Gemini, length:", textContent.length);
    return textContent;
  } catch (error) {
    console.error("❌ LLM API Error:", error.response?.data || error.message);
    throw new Error(
      `Failed to generate response: ${error.response?.data?.error?.message || error.message}`
    );
  }
}

export async function processVoiceCommand(audioBuffer) {
  try {
    const transcript = await speechToText(audioBuffer);
    const response = await generateResponse(transcript);
    return {
      transcript,
      response,
      success: true,
    };
  } catch (error) {
    return {
      success: false,
      error: error.message,
    };
  }
}

export async function parseVoiceCommand(transcript, context = {}) {
  // Parse voice command and extract database operation (Ukrainian)
  // Supports both English and Ukrainian inputs, but returns JSON in Ukrainian
  const transcriptText = String(transcript || "");
  const deleteAllMatching = /\b(всі|усі|все|усіх|all)\b/i.test(transcriptText) && /\b(зань?ятт?я|урок(и|ів)?|заняття|lesson|lessons)\b/i.test(transcriptText);
  
  // Build concise context about the curriculum table
  const subjects = (context.lookups?.subjects || []).slice(0, 10).join(", ");
  const days = (context.lookups?.days || []).join(", ");
  const times = (context.lookups?.times || []).slice(0, 5).join(", ");
  const rooms = (context.lookups?.rooms || []).slice(0, 10).join(", ");
  const groups = (context.lookups?.groups || []).join(", ");
  const teachers = (context.lookups?.teachers || []).slice(0, 5).join(", ");
  const types = (context.lookups?.types || []).join(", ");
  const weeks = (context.lookups?.weeks || []).join(", ");

  // Build full lookup context for better fuzzy matching
  const allSubjects = (context.lookups?.subjects || []).join(" | ");
  const allDays = (context.lookups?.days || []).join(" | ");
  const allTimes = (context.lookups?.times || []).join(" | ");
  const allRooms = (context.lookups?.rooms || []).join(" | ");
  const allGroups = (context.lookups?.groups || []).join(" | ");
  const allTeachers = (context.lookups?.teachers || []).join(" | ");
  const allTypes = (context.lookups?.types || []).join(" | ");
  const allWeeks = (context.lookups?.weeks || []).join(" | ");

  // Format full lessons table for context
  const lessonsFull = (context.lessons_full || []);
  let lessonsTableStr = "";
  if (lessonsFull.length > 0) {
    lessonsTableStr = "ПОТОЧНА ТАБЛИЦЯ РОЗКЛАДУ (CURRICULUM TABLE):\n";
    lessonsTableStr += "ID | Предмет | День | Час | Ауд. | Група | Тиждень | Викладач | Тип\n";
    lessonsTableStr += "---+----------+------+-------+------+-------+---------+-------+--------\n";
    lessonsTableStr += lessonsFull.slice(0, 30).map(l => 
      `${l.id} | ${l.subject} | ${l.day} | ${l.time} | ${l.room} | ${l.group_name} | ${l.week} | ${l.teacher} | ${l.lesson_type}`
    ).join("\n");
    if (lessonsFull.length > 30) {
      lessonsTableStr += `\n... та ще ${lessonsFull.length - 30} занять`;
    }
  }

  const prompt = `Ви помічник розкладу. Поверніть ТІЛЬКИ один валідний JSON без коду.

=== СИСТЕМА РОЗПІЗНАВАННЯ ПАРАМЕТРІВ ===

РОЗПІЗНАВАЙТЕ ПАРАМЕТРИ НАВІТЬ ЯКЩО:
- Користувач каже неповне ім'я (вчителя/групи/предмета)
- Параметри сказані в будь-якому порядку
- Користувач каже їх розділяючи паузами ("Ткаченко", "практика", "третя")
- Використовує скорочення або похідні ("ООП" = "об'єктно-орієнтоване програмування")
- Каже часи розмовною мовою ("п'ятнадцять" = "15:00", "третя" = "15:00" для полудня)
- Каже просто число ("15", "8", "14") = вважати за години в форматі HH:00 ("15" = "15:00", "8" = "08:00")
- Групи називає як "один пи", "два пи" = "1ПІ-25Б", "2ПІ-25Б"

СТРАТЕГІЯ РОЗПІЗНАВАННЯ:
1. УЧИТЕЛ: Шукайте БУДЬ-ЯКИ слова, які ПОЧИНАЮТЬСЯ з літер учителя (Іваненко → "Іван", Петренко → "Петр", Коваль → "Коваль", Ткаченко → "Ткач")
2. ЧАС: 
   - Розмовна мова: "п'ятнадцять" = "15:00", "третя" = "15:00" (полудень)
   - Просто число: "15" = "15:00", "8" = "08:00"
   - Формат HH:MM: "14:30" залишити як є
   - Формат 12-годинний: "3 pm" = "15:00", "8 am" = "08:00"
3. ДЕНЬ: Шукайте дні тижня в будь-якій формі (пн, понеділок, вторник, вт, і т.д.)
4. ГРУПА: Видобудьте число та букви (1пі, 2пі, 3пі → матч з доступних груп)
5. ПРЕДМЕТ: Шукайте будь-який префікс предмета (ООП, математика, фізика, людино-машинна)
6. ТИП: Шукайте лекція, практика, лабораторна (в будь-якій формі)

ДОСТУПНІ ЗНАЧЕННЯ (для точного збіру):
Предмети: ${allSubjects}
Дні: ${allDays}
Часи: ${allTimes}
Аудиторії: ${allRooms}
Групи: ${allGroups}
Викладачі: ${allTeachers}
Типи: ${allTypes}
Тижні: ${allWeeks}

${lessonsTableStr}

КОМАНДА КОРИСТУВАЧА: "${transcriptText}"

ВИВЕДІТЬ JSON:
{
  "action": "add_lesson | edit_lesson | delete_lesson | list_lessons | describe_curriculum | unknown",
  "parameters": {
    "id": "", "subject": "", "day": "", "time": "", "room": "",
    "group": "", "groups": [], "teacher": "", "week": "", "type": ""
  },
  "missing_fields": [],
  "delete_all_matching": false,
  "natural_response": "коротка відповідь українською"
}

ПРИМІТКА: Коли користувач називає КІЛЬКА груп (наприклад, "1пі, 2пі, 3пі"), поставте ВСІ в масив "groups": ["1ПІ-25Б", "2ПІ-25Б", "3ПІ-25Б"]. Поле "group" може бути пустим або першим групи.

=== ПРАВИЛА ОБРОБКИ ===
1. JSON-ONLY: Тільки JSON, без коду, без пояснень.
2. НЕЧІТКИЙ МАТЧ: Якщо слово в команді ПОЧИНАЄТЬСЯ з літер в списку (Ткач→Ткаченко), це збіг.
3. ЧАС (ВАЖЛИВО):
   - Якщо просто число ("15", "8", "14") → конвертуйте як HH:00 ("15:00", "08:00", "14:00")
   - Розмовні числа: "п'ятнадцять" → "15:00", "вісім" → "08:00"
   - Оформ HH:MM: "14:30" залишити як є
   - 12-годинний: "3 pm" → "15:00", "8 am" → "08:00"
4. НЕПОВНІ ГРУПИ: "один пи" → знайти групу, що починається з "1", "два пи" → "2", тощо
5. ГРУПИ МНОЖЕННЯ: Якщо кілька груп названо ("один пі, два пі, три пі"), поставити перше як group, інші теж включити
6. НЕВІДОМО: Якщо АБСОЛЮТНО не можна зрозуміти після всіх спроб нечіткого збігу → "Невідомо"
7. ADD: Заповніть missing_fields для полів, які НЕ розпізнані, навіть якщо вони не названі явно
8. EDIT: Потрібен ID. Дивіться на таблицю вище, знаходьте рядок, який потрібно змінити, вилучіть його ID. Заповніть тільки поля, які ЗМІНЮЮТЬСЯ
9. DELETE: ID один, або мінімум 2 фільтра. Дивіться на таблицю, щоб знайти правильний рядок. "Видали всі" → delete_all_matching=true
10. КОНТЕКСТ ТАБЛИЦІ: Коли користувач говорить про редагування/видалення, завжди спробуйте знайти відповідний рядок в таблиці за параметрами, які вказані.
11. УКРАЇНСЬКА: Вся відповідь та логіка в українській мові.`;

  try {
    const response = await generateResponse(prompt);
    console.log("📤 LLM Raw Response:", response);
    
    // Try to extract JSON more carefully
    let jsonStr = response.trim();
    
    // Remove markdown code blocks if present (handle ``` at start/end)
    jsonStr = jsonStr.replace(/^```(?:json)?\s*/i, '').replace(/\s*```\s*$/i, '');
    
    // Find the first { and last } for JSON extraction
    const startIdx = jsonStr.indexOf('{');
    const endIdx = jsonStr.lastIndexOf('}');
    
    if (startIdx === -1 || endIdx === -1 || endIdx < startIdx) {
      console.error("❌ No JSON found in response:", response);
      return {
        action: "unknown",
        parameters: {},
        missing_fields: [],
        table_summary: "",
        curriculum_notes: "",
        natural_response: "Не вдалося розібрати команду. Спробуйте ще раз.",
      };
    }
    
    jsonStr = jsonStr.substring(startIdx, endIdx + 1).trim();
    console.log("✅ Extracted JSON:", jsonStr);
    
    const parsed = JSON.parse(jsonStr);

    const parsedDeleteAll = Boolean(parsed.delete_all_matching) || deleteAllMatching;
    
    // Ensure required fields exist
    return {
      action: parsed.action || "unknown",
      parameters: parsed.parameters || {},
      missing_fields: Array.isArray(parsed.missing_fields) ? parsed.missing_fields : [],
      delete_all_matching: parsedDeleteAll,
      table_summary: parsed.table_summary || "",
      curriculum_notes: parsed.curriculum_notes || "",
      natural_response: parsed.natural_response || "Команда обробена",
    };
  } catch (error) {
    console.error("JSON parse error:", error.message);
    return {
      action: "unknown",
      parameters: {},
      missing_fields: [],
      delete_all_matching: false,
      table_summary: "",
      curriculum_notes: "",
      natural_response: "Помилка при обробці команди. Спробуйте ще раз.",
    };
  }
}
