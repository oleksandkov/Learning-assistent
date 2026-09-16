import { NextRequest, NextResponse } from "next/server";
import { nativeRequest } from "@/lib/native";
import { LOCAL_ALGORITHMS } from "@/lib/types";
import { LEVELS } from "@/lib/levels";
export const runtime = "nodejs";
export async function POST(request: NextRequest) {
  try {
    const text = await request.text();
    if (text.length > 30000)
      return NextResponse.json({ error: "Запит завеликий." }, { status: 413 });
    const body = JSON.parse(text);
    if (
      !body ||
      (!LEVELS.some((l) => l.id === body.levelId) && body.levelId !== "custom") ||
      typeof body.history !== "string" ||
      !/^[UDLR]{0,10000}$/.test(body.history) ||
      !["state", "solve"].includes(body.mode) ||
      (body.direction && !/^[UDLR]$/.test(body.direction)) ||
      !LOCAL_ALGORITHMS.some((a) => a.id === body.algorithm) ||
      (body.levelId === "custom" &&
        (typeof body.customXsb !== "string" ||
          body.customXsb.length > 10500 ||
          !/^[# .$@*+\r\n]+$/.test(body.customXsb)))
    ) {
      return NextResponse.json(
        { error: "Некоректні параметри гри." },
        { status: 400 },
      );
    }
    const result = await nativeRequest(
      body.levelId,
      body.history,
      body.mode,
      body.algorithm,
      body.direction,
      request.signal,
      body.levelId === "custom" ? body.customXsb : undefined,
    );
    return NextResponse.json(result, {
      headers: { "Cache-Control": "no-store" },
    });
  } catch (error) {
    return NextResponse.json(
      {
        error:
          error instanceof SyntaxError
            ? "Некоректний JSON."
            : error instanceof Error
              ? error.message
              : "Не вдалося виконати запит.",
      },
      { status: error instanceof SyntaxError ? 400 : 503 },
    );
  }
}
