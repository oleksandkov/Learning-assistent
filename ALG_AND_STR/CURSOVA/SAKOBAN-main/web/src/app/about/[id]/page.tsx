import { notFound } from "next/navigation";
import LessonView from "@/components/LessonView";
import { LESSONS } from "@/lib/lessons";
import { withSource } from "@/lib/lesson-source";
export function generateStaticParams() {
  return LESSONS.map((l) => ({ id: l.id }));
}
export function generateMetadata({ params }: { params: { id: string } }) {
  return {
    title: LESSONS.find((l) => l.id === params.id)?.nav || "Тему не знайдено",
  };
}
export default async function LessonPage({ params }: { params: { id: string } }) {
  const lesson = LESSONS.find((l) => l.id === params.id);
  if (!lesson) notFound();
  return (
    <LessonView
      key={lesson.id}
      lesson={await withSource(lesson)}
      topics={LESSONS.map(({ id, nav }) => ({ id, nav }))}
    />
  );
}
