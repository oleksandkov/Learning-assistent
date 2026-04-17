import path from 'node:path';
import { fileURLToPath } from 'node:url';
import express from 'express';
import cors from 'cors';
import multer from 'multer';
import dotenv from 'dotenv';
import Groq, { toFile } from 'groq-sdk';
import { transcribeWavWithVosk, validateVoskConfig } from './src/stt-vosk.js';

dotenv.config();

const app = express();
const upload = multer({
  storage: multer.memoryStorage(),
  limits: { fileSize: 15 * 1024 * 1024 }
});

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const publicDir = path.join(__dirname, 'public');

const port = Number(process.env.PORT || 3000);
const sttProvider = (process.env.STT_PROVIDER || 'browser').toLowerCase();
const groqSttModel = process.env.GROQ_STT_MODEL || 'whisper-large-v3-turbo';
const sttLanguage = process.env.STT_LANGUAGE || undefined;
const groqModel = process.env.GROQ_MODEL || 'llama-3.1-8b-instant';
const groqApiKey = process.env.GROQ_API_KEY;

const groqClient = groqApiKey && groqApiKey !== 'your_key_here'
  ? new Groq({ apiKey: groqApiKey })
  : null;

app.use(cors());
app.use(express.json({ limit: '1mb' }));
app.use(express.static(publicDir));

app.get('/api/health', (_req, res) => {
  const voskStatus = sttProvider === 'vosk'
    ? validateVoskConfig()
    : { ok: true, modelPath: 'Not required in browser mode' };

  res.json({
    ok: true,
    sttProvider,
    sttModel: sttProvider === 'groq' ? groqSttModel : null,
    llmProvider: process.env.LLM_PROVIDER || 'groq',
    groqConfigured: Boolean(groqClient),
    voskConfigured: voskStatus.ok,
    voskDetails: voskStatus.ok ? voskStatus.modelPath : voskStatus.error
  });
});

app.post('/api/stt', upload.single('audio'), async (req, res) => {
  try {
    if (sttProvider === 'groq') {
      if (!groqClient) {
        return res.status(500).json({ error: 'Groq is not configured. Set GROQ_API_KEY in .env.' });
      }

      if (!req.file?.buffer) {
        return res.status(400).json({ error: 'No audio file provided (field name: audio).' });
      }

      const uploadedFile = await toFile(
        req.file.buffer,
        req.file.originalname || 'recording.webm'
      );

      const transcription = await groqClient.audio.transcriptions.create({
        file: uploadedFile,
        model: groqSttModel,
        language: sttLanguage,
        response_format: 'verbose_json'
      });

      const transcript = (transcription?.text || '').trim();
      return res.json({ transcript });
    }

    if (sttProvider === 'browser') {
      const text = typeof req.body?.text === 'string' ? req.body.text.trim() : '';
      return res.json({ transcript: text });
    }

    if (!req.file?.buffer) {
      return res.status(400).json({ error: 'No audio file provided (field name: audio).' });
    }

    const transcript = await transcribeWavWithVosk(req.file.buffer);

    return res.json({ transcript });
  } catch (error) {
    return res.status(500).json({ error: `STT failed: ${error.message}` });
  }
});

app.post('/api/chat', async (req, res) => {
  try {
    const prompt = typeof req.body?.prompt === 'string' ? req.body.prompt.trim() : '';
    const history = Array.isArray(req.body?.history) ? req.body.history : [];

    if (!prompt) {
      return res.status(400).json({ error: 'Prompt is required.' });
    }

    if (!groqClient) {
      return res.status(500).json({
        error: 'Groq is not configured. Set GROQ_API_KEY in .env.'
      });
    }

    const sanitizedHistory = history
      .filter((item) => item && typeof item.content === 'string' && typeof item.role === 'string')
      .filter((item) => ['system', 'user', 'assistant'].includes(item.role))
      .map((item) => ({
        role: item.role,
        content: item.content.slice(0, 4000)
      }));

    const messages = [
      {
        role: 'system',
        content: `You are a concise and helpful assistant.

Language instruction:
- Prefer answering in Ukrainian or English.
- If user writes in one of them, answer in the same language.

Website control rule:
- If user asks to control website UI, prepend exactly one envelope in this format:
[UI_ACTIONS:{"actions":[{"action":"...","target":"...","value":"..."}]}]

Allowed actions (full UI control for this app):
- clear_chat
- clear_draft
- open_device_settings
- toggle_compact_mode
- set_compact_mode
- focus_record
- start_recording
- stop_recording
- redo_draft
- push_draft
- refresh_devices
- select_device
- focus
- click
- set_status

Allowed targets for focus/click:
- record
- stop
- redo
- push
- refresh_devices
- device_select

Rules:
- Use actions only when explicitly requested.
- Prefer minimal, safe actions.
- After envelope, add a short human response.`
      },
      ...sanitizedHistory,
      { role: 'user', content: prompt }
    ];

    const completion = await groqClient.chat.completions.create({
      model: groqModel,
      messages,
      temperature: 0.4,
      max_tokens: 700
    });

    const answer = completion.choices?.[0]?.message?.content?.trim() || 'No response generated.';

    return res.json({ answer });
  } catch (error) {
    return res.status(500).json({ error: `Chat failed: ${error.message}` });
  }
});

app.get('*', (_req, res) => {
  res.sendFile(path.join(publicDir, 'index.html'));
});

app.listen(port, () => {
  console.log(`LABA5 Voice Website running on http://localhost:${port}`);
});


