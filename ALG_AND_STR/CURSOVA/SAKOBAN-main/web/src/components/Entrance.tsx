"use client";
import { useEffect, useState } from "react";
export default function Entrance() {
  const [show, setShow] = useState(false);
  useEffect(() => {
    if (window.matchMedia("(prefers-reduced-motion: reduce)").matches) return;
    try {
      if (sessionStorage.getItem("sokoban-intro")) return;
      sessionStorage.setItem("sokoban-intro", "1");
    } catch {}
    setShow(true);
    const timer = setTimeout(() => setShow(false), 1200);
    return () => clearTimeout(timer);
  }, []);
  return show ? (
    <div className="entrance" aria-hidden="true">
      <div className="intro-puzzle">
        <span className="intro-person" />
        <span className="intro-box" />
        <span className="intro-goal" />
      </div>
    </div>
  ) : null;
}
