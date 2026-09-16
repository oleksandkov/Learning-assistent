"use client";

import Link from "next/link";
import { useEffect, useState } from "react";
import {
  AI_KEY_STORAGE,
  AI_MODEL_STORAGE,
  GROQ_MODELS,
  DEFAULT_GROQ_MODEL,
  DEFAULT_GOOGLE_MODEL,
  GOOGLE_MODELS,
  detectAiProvider,
  normalizeAiKey,
} from "@/lib/types";

export default function AiSettings() {
  const [key, setKey] = useState("");
  const [model, setModel] = useState("");
  const [saved, setSaved] = useState(false);

  useEffect(() => {
    try {
      setKey(sessionStorage.getItem(AI_KEY_STORAGE) || "");
      setModel(sessionStorage.getItem(AI_MODEL_STORAGE) || "");
    } catch {
      setKey("");
      setModel("");
    }
  }, []);

  const provider = key.trim() ? detectAiProvider(key) : null;
  const models =
    provider === "google" ? GOOGLE_MODELS : GROQ_MODELS;
  const activeModel =
    model && models.includes(model)
      ? model
      : provider === "google"
        ? DEFAULT_GOOGLE_MODEL
        : DEFAULT_GROQ_MODEL;

  function save() {
    const value = normalizeAiKey(key);
    try {
      if (value) {
        sessionStorage.setItem(AI_KEY_STORAGE, value);
        sessionStorage.setItem(AI_MODEL_STORAGE, activeModel);
      } else {
        sessionStorage.removeItem(AI_KEY_STORAGE);
        sessionStorage.removeItem(AI_MODEL_STORAGE);
      }
    } catch {}
    setKey(value);
    setModel(value ? activeModel : "");
    setSaved(true);
  }

  return (
    <main id="main" className="page ai-settings">
      <Link className="back" href="/">
        ← Головне меню
      </Link>
      <div className="page-heading">
        <p className="small-label">Зовнішній гравець</p>
        <h1>Налаштувати AI</h1>
        <p>
          Встав ключ Groq (безкоштовний free tier) або Google AI Studio —
          провайдер визначиться сам, а список моделей підбереться під нього.
          Ключ зберігається лише в цій вкладці.
        </p>
      </div>
      <form
        className="ai-key-form"
        onSubmit={(event) => {
          event.preventDefault();
          save();
        }}
      >
        <label htmlFor="ai-key">AI API key (Groq або Google)</label>
        <input
          id="ai-key"
          type="password"
          value={key}
          onChange={(event) => {
            setKey(event.target.value);
            setSaved(false);
          }}
          autoComplete="off"
          spellCheck={false}
          placeholder="Встав API-ключ"
        />
        {provider ? (
          <>
            <p className="message" role="status" aria-live="polite">
              ✓ Визначено: {provider === "google" ? "Google AI" : "Groq"}
            </p>
            <label htmlFor="ai-model-select">
              Модель {provider === "google" ? "Google" : "Groq (швидкі, free tier)"}
            </label>
            <select
              id="ai-model-select"
              value={activeModel}
              onChange={(event) => {
                setModel(event.target.value);
                setSaved(false);
              }}
            >
              {models.map((m) => (
                <option key={m} value={m}>
                  {m}
                </option>
              ))}
            </select>
          </>
        ) : null}
        <div className="ai-key-actions">
          <button className="button primary" type="submit">
            Зберегти для цієї вкладки
          </button>
          <a
            className="button secondary"
            href="https://console.groq.com"
            target="_blank"
            rel="noreferrer"
          >
            Ключ Groq ↗
          </a>
          <a
            className="button secondary"
            href="https://aistudio.google.com/app/apikey"
            target="_blank"
            rel="noreferrer"
          >
            Ключ Google ↗
          </a>
        </div>
        <p className="message" role="status" aria-live="polite">
          {saved ? (key ? "Ключ готовий." : "Ключ видалено.") : ""}
        </p>
      </form>
      <p className="ai-note">
        AI пропонує маршрут, але не змінює поле напряму: кожен план
        перевіряється правилами гри перед відтворенням.
      </p>
    </main>
  );
}
