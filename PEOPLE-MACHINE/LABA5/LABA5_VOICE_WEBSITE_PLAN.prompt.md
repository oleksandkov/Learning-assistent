# LABA 5 Plan — Voice-Only AI Chat Website

## Goal
Build a minimal web app for Lab #5 where the user communicates with AI **only by voice**.

## Core Functional Requirements
- Voice-only input (no manual text input field).
- Speech-to-text (STT) converts microphone audio to text.
- A **Prompt Draft** section shows recognized text before sending.
- User can:
  - **Redo** (record again and replace draft)
  - **Push** (send draft to AI)
- AI response is displayed in chat history.

## Suggested Tech Stack (Minimal)
- Frontend: HTML + CSS + JavaScript (or React if preferred)
- Backend: Node.js + Express
- STT: **Vosk** (free, open-source)
- AI API: **Groq free tier** (or OpenRouter free models)

## Why This Stack
- Matches Lab #5 topic (speech interface + practical implementation).
- Keeps project small and demo-friendly.
- Uses free tools/APIs.
- Easy to explain during defense.

## System Flow
1. User clicks **Record**.
2. Browser records audio from microphone.
3. Audio is sent to backend endpoint `/api/stt`.
4. Backend returns recognized text.
5. Frontend shows recognized text in **Prompt Draft**.
6. User clicks:
   - **Redo** → discard and re-record
   - **Push** → send text to `/api/chat`
7. Backend sends prompt to LLM API and returns answer.
8. Frontend appends both prompt and response to chat history.

## API Options
### STT (choose one)
1. **Vosk** (recommended)
   - Open-source, offline-capable
   - Good for educational demos
2. Whisper / Faster-Whisper
   - Better quality, heavier setup

### AI (choose one)
1. **Groq API** (recommended free tier)
2. OpenRouter (free models)
3. Ollama local model (fully local, no cloud key)

## Chosen Default for This Plan
- STT: **Vosk**
- AI: **Groq**

## Environment Variables (.env)
- `STT_PROVIDER=vosk`
- `VOSK_MODEL_PATH=./models/vosk-model-small-en-us-0.15`
- `LLM_PROVIDER=groq`
- `GROQ_API_KEY=your_key_here`
- `GROQ_MODEL=llama-3.1-8b-instant`
- `PORT=3000`

## Minimal Pages / UI Blocks
- Header: project title
- Voice controls: Record / Stop
- Prompt Draft panel:
  - recognized text preview
  - buttons: Redo / Push
- Chat panel:
  - message list (User voice prompt + AI response)

## Backend Endpoints
- `POST /api/stt` → audio file → transcript
- `POST /api/chat` → prompt text → AI response
- `GET /api/health` → status check (optional)

## Milestones
1. Project scaffold + env config
2. Microphone recording in browser
3. STT integration (`/api/stt`)
4. Prompt Draft UI (Redo/Push)
5. AI integration (`/api/chat`)
6. Chat history rendering
7. Final testing + demo script

## Demo Scenario (for defense)
1. Click Record and say: “Explain what SAPI is.”
2. Show recognized text in Prompt Draft.
3. Click Push.
4. Show AI response in chat.
5. Repeat with Redo flow.

## Risks and Mitigation
- Microphone permission denied → show clear instruction and retry button.
- STT quality issues → use short commands and clear speech; allow Redo.
- API key missing → validate on backend startup and show readable error.

## Deliverables
- Running web app (voice-only prompt flow)
- Source code in `LABA5` folder
- Short report/screenshots for lab defense
