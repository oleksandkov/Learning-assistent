import LessonView from "@/components/LessonView";
import { LESSONS } from "@/lib/lessons";
export const metadata = { title: "Алгоритми" };
export default function About() {
  return (
    <LessonView
      key="bfs"
      lesson={LESSONS[0]}
      topics={LESSONS.map(({ id, nav }) => ({ id, nav }))}
    />
  );
}
