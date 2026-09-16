import { notFound } from "next/navigation";
import { LEVELS } from "@/lib/levels";
import Game from "@/components/Game";
import CustomGame from "@/components/CustomGame";
export default function PlayPage({ params }: { params: { id: string } }) {
  if (params.id === "custom") return <CustomGame />;
  const level = LEVELS.find((l) => l.id === params.id);
  if (!level) notFound();
  return <Game key={level.id} level={level} />;
}
