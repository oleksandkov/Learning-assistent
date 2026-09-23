import { readFile } from "node:fs/promises";
import path from "node:path";
import type { Lesson, ResolvedLesson } from "./lessons";

// Server/build only: keep explanations attached to the actual implementation.
export async function withSource(lesson: Lesson): Promise<ResolvedLesson> {
  const files = Array.from(new Set(lesson.frames.map(frame => frame.source ?? lesson.source)));
  const sources = new Map(await Promise.all(files.map(async file => [
    file, (await readFile(path.join(process.cwd(), "..", file), "utf8")).split(/\r?\n/),
  ] as const)));
  const excerpts = lesson.frames.map(frame => {
    const file = frame.source ?? lesson.source;
    const lines = sources.get(file)!;
    const start = lines.findIndex(line => line.includes(frame.anchor));
    if (start < 0) throw new Error(`Lesson ${lesson.id}: missing source anchor in ${file}: ${frame.anchor}`);
    const excerpt = lines.slice(start, start + frame.count);
    const indent = Math.min(...excerpt.filter(line => line.trim()).map(line => line.search(/\S/)));
    return { file, start: start + 1, lines: excerpt.map(line => line.slice(indent)) };
  });
  return { ...lesson, excerpts };
}
