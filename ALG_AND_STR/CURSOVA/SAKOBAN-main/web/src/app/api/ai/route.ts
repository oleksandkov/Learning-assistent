import { NextRequest, NextResponse } from "next/server";
import { nativeRequest } from "@/lib/native";
import { LEVELS } from "@/lib/levels";
import {
  GROQ_MODELS,
  DEFAULT_GROQ_MODEL,
  DEFAULT_GOOGLE_MODEL,
  GOOGLE_MODELS,
  detectAiProvider,
  normalizeAiKey,
} from "@/lib/types";
import type { SearchResult, Snapshot } from "@/lib/types";

export const runtime = "nodejs";

export async function GET() {
  return NextResponse.json({
    groqModels: GROQ_MODELS,
    googleModels: GOOGLE_MODELS,
    defaultGroqModel: process.env.GROQ_MODEL || DEFAULT_GROQ_MODEL,
    defaultGoogleModel: DEFAULT_GOOGLE_MODEL,
  });
}

function toXsb(snapshot: Snapshot) {
  const boxes = new Set(snapshot.state.boxes);
  const rows: string[] = [];
  for (let y = 0; y < snapshot.board.height; y++) {
    let row = "";
    for (let x = 0; x < snapshot.board.width; x++) {
      const cell = y * snapshot.board.width + x;
      row += snapshot.board.walls[cell]
        ? "#"
        : boxes.has(cell)
          ? snapshot.board.goals[cell]
            ? "*"
            : "$"
          : snapshot.state.player === cell
            ? snapshot.board.goals[cell]
              ? "+"
              : "@"
            : snapshot.board.goals[cell]
              ? "."
              : " ";
    }
    rows.push(row.trimEnd());
  }
  return rows.join("\n");
}

function result(
  moves: string,
  pushes: number,
  validated: boolean,
  modelMs: number,
  validationMs: number,
  totalMs: number,
  explanation = "",
  aiSession?: SearchResult["aiSession"],
): SearchResult {
  return {
    algorithm: "gemini",
    status: validated ? "Solved" : "InternalError",
    moves,
    pushes,
    searchMs: 0,
    preprocessingMs: 0,
    validationMs,
    reconstructionMs: 0,
    totalMs,
    explored: 0,
    generated: 0,
    deadlocks: 0,
    frontier: 0,
    validated,
    trace: [],
    remoteMs: modelMs,
    explanation,
    aiSession,
  };
}

function extractMoves(text: string): string {
  const clean = text.replace(/^```(?:json)?/i, "").replace(/```$/i, "").trim();
  try {
    const parsed: unknown = JSON.parse(clean);
    if (parsed && typeof parsed === "object") {
      const moves = (parsed as { moves?: unknown }).moves;
      if (typeof moves === "string" && /^[UDLR]*$/.test(moves) && moves.length <= 10000)
        return moves;
    }
  } catch {}
  const match = /"moves"\s*:\s*"([UDLR]*)"/.exec(clean);
  if (match && match[1].length <= 10000) return match[1];
  return "";
}

function extractExplanation(text: string): string {
  try {
    const parsed: unknown = JSON.parse(
      text.replace(/^```(?:json)?/i, "").replace(/```$/i, "").trim(),
    );
    if (parsed && typeof parsed === "object") {
      const explanation = (parsed as { explanation?: unknown }).explanation;
      if (typeof explanation === "string") return explanation.slice(0, 500);
    }
  } catch {}
  const match = /"explanation"\s*:\s*"((?:[^"\\]|\\.)*)"/.exec(text);
  if (match) {
    try {
      return JSON.parse(`"${match[1]}"`).slice(0, 500);
    } catch {
      return match[1].slice(0, 500);
    }
  }
  return "";
}

function groqPrompt(xsb: string) {
  return (
    `Solve this Sokoban position. # is a wall, @ the player, $ a box, . a goal, * a box on a goal and + a player on a goal. Think step by step and find a complete legal route that puts every box on a goal. Use only U, D, L and R for moves. A player may push one box but cannot pull boxes. Reply with JSON only, no markdown fences: {"moves": "UDLR characters", "explanation": "one short sentence in Ukrainian"}.\n\n${xsb}`
  );
}

function googlePrompt(xsb: string) {
  return (
    `Solve this Sokoban position. # is a wall, @ the player, $ a box, . a goal, * a box on a goal and + a player on a goal. Use Python code execution to search for and verify the exact solution. Return a complete legal route that puts every box on a goal. Use only U, D, L and R for moves. A player may push one box but cannot pull boxes. Write the short explanation in Ukrainian.\n\n${xsb}`
  );
}

async function callGroq(
  key: string,
  model: string,
  prompt: string,
  signal: AbortSignal,
): Promise<{ status: number; payload: unknown; ms: number }> {
  const baseBody = { model, messages: [{ role: "user", content: prompt }] };
  const fullBody = {
    ...baseBody,
    temperature: 0,
    max_tokens: 4096,
    response_format: { type: "json_object" },
  };
  const started = performance.now();
  let response: Response | null = null;
  let payload: unknown = null;
  for (let attempt = 0; attempt < 2; attempt++) {
    response = await fetch("https://api.groq.com/openai/v1/chat/completions", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${key}`,
      },
      body: JSON.stringify(attempt === 0 ? fullBody : baseBody),
      signal,
    });
    try {
      payload = await response.json();
    } catch {
      payload = null;
    }
    // Деякі моделі відхиляють temperature/response_format — повтор без них.
    if (response.status !== 400 || attempt === 1) break;
  }
  return { status: response?.status ?? 500, payload, ms: performance.now() - started };
}

async function callGoogle(
  key: string,
  model: string,
  prompt: string,
  signal: AbortSignal,
): Promise<{ status: number; payload: unknown; ms: number }> {
  const started = performance.now();
  const response = await fetch(
    `https://generativelanguage.googleapis.com/v1beta/models/${model}:generateContent`,
    {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        "x-goog-api-key": key,
      },
      body: JSON.stringify({
        contents: [{ parts: [{ text: prompt }] }],
        tools: [{ codeExecution: {} }],
        generationConfig: {
          temperature: 0,
          maxOutputTokens: 8192,
          responseMimeType: "application/json",
          responseSchema: {
            type: "OBJECT",
            properties: {
              moves: {
                type: "STRING",
                description: "The complete solution as UDLR characters only.",
              },
              explanation: {
                type: "STRING",
                description: "One short sentence explaining the plan.",
              },
            },
            required: ["moves"],
          },
        },
      }),
      signal,
    },
  );
  let payload: unknown = null;
  try {
    payload = await response.json();
  } catch {
    payload = null;
  }
  return { status: response.status, payload, ms: performance.now() - started };
}

function answerText(provider: "google" | "groq", payload: unknown): string {
  if (!payload || typeof payload !== "object") return "";
  if (provider === "groq") {
    const content = (payload as { choices?: { message?: { content?: unknown } }[] })
      .choices?.[0]?.message?.content;
    return typeof content === "string" ? content : content ? JSON.stringify(content) : "";
  }
  const parts = (payload as { candidates?: { content?: { parts?: { text?: string }[] } }[] })
    .candidates?.[0]?.content?.parts;
  return parts?.map((part) => part.text ?? "").join("") ?? "";
}

export async function POST(request: NextRequest) {
  const started = performance.now();
  try {
    const text = await request.text();
    if (text.length > 30000)
      return NextResponse.json({ error: "Запит завеликий." }, { status: 413 });
    let body;
    try {
      body = JSON.parse(text);
    } catch {
      return NextResponse.json({ error: "Некоректний JSON." }, { status: 400 });
    }
    const key =
      typeof body?.apiKey === "string" ? normalizeAiKey(body.apiKey) : "";
    if (!key || key.length > 512)
      return NextResponse.json(
        { error: "Додай API-ключ у налаштуваннях ШІ." },
        { status: 400 },
      );
    const provider = detectAiProvider(key);
    const allowed =
      provider === "google" ? GOOGLE_MODELS : GROQ_MODELS;
    const fallback =
      provider === "google"
        ? DEFAULT_GOOGLE_MODEL
        : process.env.GROQ_MODEL || DEFAULT_GROQ_MODEL;
    const wanted = typeof body?.model === "string" ? body.model : "";
    const model = allowed.includes(wanted) ? wanted : fallback;
    if (
      (!LEVELS.some((level) => level.id === body.levelId) &&
        body.levelId !== "custom") ||
      typeof body.history !== "string" ||
      !/^[UDLR]{0,10000}$/.test(body.history) ||
      (body.levelId === "custom" &&
        (typeof body.customXsb !== "string" ||
          body.customXsb.length > 10500 ||
          !/^[# .$@*+\r\n]+$/.test(body.customXsb)))
    )
      return NextResponse.json(
        { error: "Некоректні параметри AI." },
        { status: 400 },
      );

    const initial = (await nativeRequest(
      body.levelId,
      body.history,
      "state",
      "astar-pushes",
      "",
      request.signal,
      body.levelId === "custom" ? body.customXsb : undefined,
    )) as Snapshot;
    if (initial.won)
      return NextResponse.json(result("", 0, true, 0, 0, performance.now() - started));

    const xsb = toXsb(initial);
    const prompt = provider === "google" ? googlePrompt(xsb) : groqPrompt(xsb);
    const call =
      provider === "google"
        ? await callGoogle(key, model, prompt, request.signal)
        : await callGroq(key, model, prompt, request.signal);
    if (call.status !== 200 || !call.payload || typeof call.payload !== "object") {
      const errObj = (call.payload as { error?: unknown } | null)?.error;
      const serverMessage =
        (errObj && typeof errObj === "object"
          ? (errObj as { message?: unknown }).message
          : (call.payload as { message?: unknown })?.message) ?? undefined;
      const tail =
        typeof serverMessage === "string" && serverMessage
          ? ` Сервер: ${serverMessage.slice(0, 200)}`
          : "";
      const google = provider === "google";
      return NextResponse.json(
        {
          error:
            call.status === 429
              ? google
                ? `Досягнуто ліміт Google AI.${tail} Зачекай або перевір квоту проєкту в Google AI Studio.`
                : `Досягнуто ліміт Groq.${tail} Free tier має денні ліміти токенів. Зачекай або спробуй меншу позицію.`
              : call.status === 401 || call.status === 403
                ? google
                  ? `Недійсний ключ Google.${tail} Перевір ключ у налаштуваннях ШІ.`
                  : `Недійсний ключ Groq.${tail} Встав ключ з console.groq.com.`
                  : call.status === 503 && google
                    ? `Google AI перевантажений.${tail} Спробуй трохи пізніше або встав ключ Groq.`
                    : call.status === 404
                      ? `Модель ${model} не знайдено у ${google ? "Google AI" : "Groq"}.${tail} ${google ? "Google знімає старі моделі — обери іншу зі списку." : "Список оновлюється — обери іншу зі списку."}`
                      : `AI не відповів (HTTP ${call.status}).${tail} Спробуй трохи пізніше.`,
        },
        { status: call.status },
      );
    }

    const answer = answerText(provider, call.payload);
    const moves = extractMoves(answer);
    const route = body.history.length + moves.length <= 10000 ? moves : "";
    const validationStarted = performance.now();
    let final: Snapshot | null = null;
    if (route)
      try {
        final = (await nativeRequest(
          body.levelId,
          body.history + route,
          "state",
          "astar-pushes",
          "",
          request.signal,
          body.levelId === "custom" ? body.customXsb : undefined,
        )) as Snapshot;
      } catch {}
    const validationMs = performance.now() - validationStarted;
    const rawResponse = JSON.stringify(call.payload, null, 2);
    return NextResponse.json(
      result(
        route,
        final ? Math.max(0, final.pushes - initial.pushes) : 0,
        Boolean(final?.won),
        call.ms,
        validationMs,
        performance.now() - started,
        final?.won
          ? extractExplanation(answer) || "AI побудував перевірений маршрут."
          : route
            ? "AI запропонував маршрут, але ядро відхилило його. ШІ не зміг розв’язати цю позицію."
            : "AI не повернув маршрут у форматі UDLR. ШІ не зміг розв’язати цю позицію.",
        {
          model,
          prompt,
          response:
            rawResponse.length <= 120000
              ? rawResponse
              : `${rawResponse.slice(0, 120000)}\n… response truncated`,
        },
      ),
      { headers: { "Cache-Control": "no-store" } },
    );
  } catch (error) {
    return NextResponse.json(
      {
        error:
          error instanceof SyntaxError
            ? "AI повернув некоректну відповідь."
            : error instanceof Error
              ? error.message
              : "Не вдалося виконати запит AI.",
      },
      { status: error instanceof SyntaxError ? 502 : 503 },
    );
  }
}
