import "dotenv/config";
import axios from "axios";
import FormData from "form-data";
import fs from "fs";

const GROQ_API_KEY = process.env.GROQ_API_KEY;
const GROQ_MODEL = process.env.GROQ_MODEL || "llama-3.1-8b-instant";
const GROQ_STT_MODEL = process.env.GROQ_STT_MODEL || "whisper-large-v3-turbo";

const groqClient = axios.create({
  baseURL: "https://api.groq.com/openai/v1",
  headers: {
    Authorization: `Bearer ${GROQ_API_KEY}`,
  },
});

export async function speechToText(audioBuffer) {
  if (!GROQ_API_KEY) {
    throw new Error("GROQ_API_KEY is not configured");
  }

  const formData = new FormData();
  formData.append("file", audioBuffer, {
    filename: "audio.wav",
    contentType: "audio/wav",
  });
  formData.append("model", GROQ_STT_MODEL);
  formData.append("language", process.env.STT_LANGUAGE || "en");

  try {
    const response = await groqClient.post("/audio/transcriptions", formData, {
      headers: formData.getHeaders(),
    });
    return response.data.text;
  } catch (error) {
    console.error("Speech to text error:", error.response?.data || error.message);
    throw new Error(
      `Failed to transcribe audio: ${error.response?.data?.error?.message || error.message}`
    );
  }
}

export async function generateResponse(prompt) {
  if (!GROQ_API_KEY) {
    throw new Error("GROQ_API_KEY is not configured");
  }

  try {
    const response = await groqClient.post("/chat/completions", {
      model: GROQ_MODEL,
      messages: [
        {
          role: "system",
          content: `You are a helpful assistant for a schedule management system.
You support both English and Ukrainian languages.
- If user writes in Ukrainian, respond in Ukrainian
- If user writes in English, respond in English
- Be concise and helpful
- For schedule operations, extract key information accurately
- Handle complex, long statements by breaking them down
- Preserve all details provided by the user`,
        },
        {
          role: "user",
          content: prompt,
        },
      ],
      temperature: 0.7,
      max_tokens: 1000,
    });

    return response.data.choices[0].message.content;
  } catch (error) {
    console.error("LLM error:", error.response?.data || error.message);
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
  // Parse voice command and extract database operation
  // Supports both English and Ukrainian
  const prompt = `You are a schedule management system assistant. Parse this voice command CAREFULLY and extract ALL mentioned information.
  
Commands can be in English or Ukrainian - understand and respond in the same language the user used.

Voice command: "${transcript}"

RESPOND WITH ONLY VALID JSON, NO OTHER TEXT. Return exactly this structure:
{
  "action": "add_lesson" | "delete_lesson" | "list_lessons" | "view_schedule" | "help" | "unknown",
  "parameters": {"subject": "...", "day": "...", "time": "...", "room": "...", "group": "...", "teacher": "...", "type": "...", "week": "..."},
  "missing_fields": ["field1", "field2"],
  "natural_response": "What you understood"
}

RULES:
- Extract EVERY detail from long statements
- For add_lesson, missing_fields must list: subject, day, time, room, group, teacher (only if NOT mentioned)
- If time is "9 am" convert to "09:00"
- RESPOND WITH ONLY JSON, START WITH { AND END WITH }`;

  try {
    const response = await generateResponse(prompt);
    
    // Try to extract JSON more carefully
    let jsonStr = response.trim();
    
    // Remove markdown code blocks if present
    jsonStr = jsonStr.replace(/^```(?:json)?\s*/i, '').replace(/```\s*$/, '');
    
    // Find the first { and last } for JSON
    const startIdx = jsonStr.indexOf('{');
    const endIdx = jsonStr.lastIndexOf('}');
    
    if (startIdx === -1 || endIdx === -1 || endIdx < startIdx) {
      console.error("No JSON found in response:", response);
      return {
        action: "unknown",
        parameters: {},
        missing_fields: [],
        natural_response: "I couldn't understand that command. Could you try again?",
      };
    }
    
    jsonStr = jsonStr.substring(startIdx, endIdx + 1);
    const parsed = JSON.parse(jsonStr);
    
    // Ensure required fields exist
    return {
      action: parsed.action || "unknown",
      parameters: parsed.parameters || {},
      missing_fields: Array.isArray(parsed.missing_fields) ? parsed.missing_fields : [],
      natural_response: parsed.natural_response || "Command processed",
    };
  } catch (error) {
    console.error("JSON parse error:", error.message, "Response:", response);
    return {
      action: "unknown",
      parameters: {},
      missing_fields: [],
      natural_response: "I had trouble understanding that. Please try again.",
    };
  }
}
