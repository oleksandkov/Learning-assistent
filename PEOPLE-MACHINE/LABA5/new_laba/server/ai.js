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
          content: "You are a helpful assistant for a schedule management system.",
        },
        {
          role: "user",
          content: prompt,
        },
      ],
      temperature: 0.7,
      max_tokens: 500,
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
