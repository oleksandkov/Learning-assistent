# LABA 5 — Voice-Only AI Chat Website

This folder contains a minimal voice-only chat website for Lab #5:

- microphone audio is captured in the browser;
- audio is sent to backend STT and converted to text;
- recognized text appears in **Prompt Draft**;
- you can **Redo** or **Push**;
- push sends prompt to Groq (`/api/chat`) and appends response to chat history.

## Stack

- Frontend: HTML, CSS, JavaScript
- Backend: Node.js + Express
- STT: Groq Whisper transcription (default) or optional Vosk mode
- LLM: Groq API

## Setup

1. Install dependencies in this `LABA5` folder.
2. Copy `.env.example` to `.env` if needed.
3. Set `GROQ_API_KEY` in `.env`.
4. Start the server.

### Optional Vosk mode

If you need backend STT for the defense/demo:

1. Install Vosk separately: `npm install vosk`
2. Download a Vosk model and place it at `VOSK_MODEL_PATH`
3. Set `STT_PROVIDER=vosk` in `.env`

## Environment Variables

- `STT_PROVIDER=browser`
- `STT_PROVIDER=groq`
- `VOSK_MODEL_PATH=./models/vosk-model-small-en-us-0.15`
- `LLM_PROVIDER=groq`
- `GROQ_API_KEY=your_key_here`
- `GROQ_MODEL=llama-3.1-8b-instant`
- `GROQ_STT_MODEL=whisper-large-v3-turbo`
- `STT_LANGUAGE=`
- `PORT=3000`

## API Endpoints

- `GET /api/health` — service health and configuration status
- `POST /api/stt` — transcribes uploaded audio using Groq (default) or Vosk in optional mode
- `POST /api/chat` — receives `{ prompt, history }` and returns `{ answer }`

## Notes

- Voice-only means there is no manual text input field.
- If STT fails, verify microphone permissions and API key/quota.
- If chat fails, verify `GROQ_API_KEY` and model name.

## Sources

- Groq TypeScript SDK docs: <https://github.com/groq/groq-typescript>
- Groq chat completions reference: <https://console.groq.com/docs>
- Vosk API repository: <https://github.com/alphacep/vosk-api>
