import LessonView from "@/components/LessonView";
import { LESSONS } from "@/lib/lessons";
import { withSource } from "@/lib/lesson-source";
export const metadata = { title: "Алгоритми" };
export default async function About() {
  return (
    <LessonView
      key="bfs"
      lesson={await withSource(LESSONS[0])}
      topics={LESSONS.map(({ id, nav }) => ({ id, nav }))}
    />
  );
}
