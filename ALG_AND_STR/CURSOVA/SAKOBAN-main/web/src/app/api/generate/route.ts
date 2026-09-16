import { NextRequest, NextResponse } from "next/server";
import { generateNativeLevel } from "@/lib/native";

export const runtime = "nodejs";

export async function POST(request: NextRequest) {
  try {
    const text = await request.text();
    if (text.length > 1000)
      return NextResponse.json({ error: "Запит завеликий." }, { status: 413 });
    const body = JSON.parse(text);
    const width = Number(body?.width);
    const height = Number(body?.height);
    const boxes = Number(body?.boxes);
    const walls = Number(body?.walls);
    const seed = body?.seed === "" || body?.seed === undefined ? "" : String(body.seed);
    const innerCells = (width - 2) * (height - 2);
    const maxBoxes = Math.min(50, Math.max(1, Math.floor((innerCells - 3) / 4)));
    const maxWalls = Math.min(
      20,
      Math.max(0, Math.floor((innerCells - (boxes * 3 + 4)) / 6)),
    );
    const validSeed =
      seed === "" ||
      (/^\d{1,20}$/.test(seed) &&
        BigInt(seed) <= BigInt("18446744073709551615"));
    if (
      ![width, height, boxes, walls].every(Number.isInteger) ||
      width < 5 ||
      width > 100 ||
      height < 5 ||
      height > 100 ||
      boxes < 1 ||
      boxes > maxBoxes ||
      walls < 0 ||
      walls > maxWalls ||
      !validSeed
    ) {
      return NextResponse.json(
        { error: "Перевір розмір поля, кількість ящиків і стін." },
        { status: 400 },
      );
    }
    const generated = await generateNativeLevel(
      width,
      height,
      boxes,
      walls,
      seed,
      request.signal,
    );
    if (!generated.success) {
      return NextResponse.json(
        {
          error:
            "Не вдалося побудувати рівень із цими параметрами. Зменш кількість ящиків або стін і спробуй ще раз.",
          ...generated,
        },
        { status: 422 },
      );
    }
    return NextResponse.json(generated, {
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
              : "Не вдалося згенерувати рівень.",
      },
      { status: error instanceof SyntaxError ? 400 : 503 },
    );
  }
}
