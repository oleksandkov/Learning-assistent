import { useRef, useState, useCallback } from "react";

/**
 * SpeechManager - Unified speech control component implementing SAPI principles
 * Supports speech-to-text (STT) and text-to-speech (TTS)
 * Handles both English and Ukrainian languages
 */
export function SpeechManager({ onCommandExecuted, token }) {
  // Recording state
  const [isRecording, setIsRecording] = useState(false);
  const [isSpeaking, setIsSpeaking] = useState(false);
  const [draft, setDraft] = useState("");
  const [commandResult, setCommandResult] = useState("");
  const [error, setError] = useState("");
  const [status, setStatus] = useState("🎤 Ready to record.");
  const [isExecuting, setIsExecuting] = useState(false);
  const [detectedLanguage, setDetectedLanguage] = useState("en");

  // Refs
  const mediaRecorderRef = useRef(null);
  const audioChunksRef = useRef([]);
  const streamRef = useRef(null);
  const synthRef = useRef(window.speechSynthesis);

  /**
   * Initialize speech synthesis with best available voice
   */
  const getSynthesisVoice = useCallback((lang = "en") => {
    const voices = synthRef.current.getVoices();
    const langCode = lang === "uk" ? "uk-UA" : "en-US";

    let selectedVoice =
      voices.find((v) => v.lang.startsWith(langCode)) ||
      voices.find((v) => v.lang.startsWith(lang)) ||
      voices[0];

    return selectedVoice;
  }, []);

  /**
   * Speak text using Web Speech API (TTS)
   */
  const speakText = useCallback(
    (text, lang = "en") => {
      return new Promise((resolve) => {
        // Cancel any ongoing speech
        if (synthRef.current.speaking) {
          synthRef.current.cancel();
        }

        const utterance = new SpeechSynthesisUtterance(text);
        const voice = getSynthesisVoice(lang);

        utterance.voice = voice;
        utterance.lang = lang === "uk" ? "uk-UA" : "en-US";
        utterance.rate = 0.9;
        utterance.pitch = 1.0;
        utterance.volume = 1.0;

        utterance.onstart = () => {
          setIsSpeaking(true);
        };

        utterance.onend = () => {
          setIsSpeaking(false);
          resolve();
        };

        utterance.onerror = (event) => {
          console.error("Speech synthesis error:", event.error);
          setIsSpeaking(false);
          resolve();
        };

        setIsSpeaking(true);
        synthRef.current.speak(utterance);
      });
    },
    [getSynthesisVoice]
  );

  /**
   * Detect language from text (simple heuristic)
   */
  const detectLanguage = useCallback((text) => {
    const ukrainianChars = /[а-яєіїґ]/gi;
    const match = text.match(ukrainianChars);
    return match ? "uk" : "en";
  }, []);

  /**
   * Start recording audio from microphone
   */
  const startRecording = async () => {
    try {
      setError("");
      setStatus("🔴 Recording... Speak clearly.";

      const stream = await navigator.mediaDevices.getUserMedia({
        audio: {
          echoCancellation: true,
          noiseSuppression: true,
          autoGainControl: true,
          sampleRate: 16000, // Optimal for speech recognition
        },
      });

      streamRef.current = stream;
      audioChunksRef.current = [];

      const mediaRecorder = new MediaRecorder(stream, {
        mimeType: "audio/webm",
      });
      mediaRecorderRef.current = mediaRecorder;

      mediaRecorder.ondataavailable = (event) => {
        if (event.data.size > 0) {
          audioChunksRef.current.push(event.data);
        }
      };

      mediaRecorder.start();
      setIsRecording(true);

      // Auto-stop after 60 seconds
      setTimeout(() => {
        if (mediaRecorderRef.current?.state === "recording") {
          stopRecording();
        }
      }, 60000);
    } catch (err) {
      let errorMsg = err.message;
      if (err.name === "NotAllowedError") {
        errorMsg = "🔒 Microphone permission denied. Please allow microphone access.";
      } else if (err.name === "NotFoundError") {
        errorMsg = "🎙️ No microphone found. Please connect a microphone.";
      } else if (err.name === "NotSupportedError") {
        errorMsg = "🚫 Audio recording not supported in this browser.";
      }
      console.error("Recording error:", err);
      setError(errorMsg);
      setStatus("❌ Error. Try again.");
    }
  };

  /**
   * Stop recording and send to speech-to-text service
   */
  const stopRecording = async () => {
    if (!mediaRecorderRef.current) return;

    mediaRecorderRef.current.onstop = async () => {
      const audioBlob = new Blob(audioChunksRef.current, {
        type: "audio/webm",
      });

      const reader = new FileReader();
      reader.onload = async (e) => {
        const base64Audio = e.target.result.split(",")[1];

        try {
          setStatus("⏳ Transcribing audio...";

          const response = await fetch("/api/stt", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
              Authorization: `Bearer ${token}`,
            },
            body: JSON.stringify({ audio: base64Audio }),
          });

          const contentType = response.headers.get("content-type");
          let data;

          if (contentType?.includes("application/json")) {
            data = await response.json();
          } else {
            const text = await response.text();
            throw new Error(`Invalid response: ${text.substring(0, 100)}`);
          }

          if (response.ok) {
            const transcript = data.transcript || "";
            setDraft(transcript);

            // Detect language from transcript
            const lang = detectLanguage(transcript);
            setDetectedLanguage(lang);

            if (transcript) {
              setStatus("✅ Draft ready. Use Push to send.");
              // Optional: Provide audio feedback
              await speakText(
                lang === "uk"
                  ? "Транскрипцію завершено"
                  : "Transcription complete.",
                lang
              );
            } else {
              setStatus("⚠️ No text recognized. Try again.");
            }
          } else {
            setError(data.error || `HTTP ${response.status}`);
            setStatus("❌ Error. Try again.");
          }
        } catch (err) {
          console.error("Transcription error:", err);
          setError(`Transcription error: ${err.message}`);
          setStatus("❌ Error. Try again.");
        }
      };
      reader.readAsDataURL(audioBlob);

      streamRef.current?.getTracks().forEach((track) => track.stop());
      setIsRecording(false);
    };

    mediaRecorderRef.current.stop();
  };

  /**
   * Clear draft and start over
   */
  const redoDraft = () => {
    if (synthRef.current.speaking) {
      synthRef.current.cancel();
    }
    setDraft("");
    setCommandResult("");
    setError("");
    setStatus("🔄 Draft cleared. Record again.");
  };

  /**
   * Execute the voice command
   */
  const pushCommand = async () => {
    if (!draft.trim()) {
      setError("No command to execute");
      return;
    }

    setIsExecuting(true);
    setError("");
    setCommandResult("");
    setStatus("⚙️ Executing command...");

    try {
      const response = await fetch("/api/command", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify({ prompt: draft }),
      });

      const contentType = response.headers.get("content-type");
      let data;

      if (contentType?.includes("application/json")) {
        data = await response.json();
      } else {
        const text = await response.text();
        throw new Error(
          `Invalid response from server: ${text.substring(0, 100)}`
        );
      }

      if (response.ok) {
        const result = data.answer || "Done.";
        setCommandResult(result);
        setStatus("✅ Done. Record again.");
        setDraft("");

        // Speak the result
        await speakText(result, detectedLanguage);

        onCommandExecuted?.();
      } else {
        const errorMsg = data.error || data.message || "Command execution failed";
        setError(errorMsg);
        setStatus("❌ Error. Try again.");
        await speakText(
          errorMsg,
          detectedLanguage
        );
      }
    } catch (err) {
      console.error("Command execution error:", err);
      const errorMsg = `Execution error: ${err.message}`;
      setError(errorMsg);
      setStatus("❌ Error. Try again.");
      await speakText(errorMsg, detectedLanguage);
    } finally {
      setIsExecuting(false);
    }
  };

  /**
   * Stop all speech output
   */
  const stopSpeaking = () => {
    if (synthRef.current.speaking) {
      synthRef.current.cancel();
      setIsSpeaking(false);
    }
  };

  const hasDraft = Boolean(draft.trim());

  return (
    <div style={styles.container}>
      <h3 style={styles.title}>🎤 Speech Control Interface</h3>
      <p style={styles.subtitle}>Record commands in English or Ukrainian</p>

      <div style={styles.statusBox}>
        <span style={styles.statusText}>{status}</span>
        {isSpeaking && <span style={styles.speakingIndicator}>🔊 Speaking...</span>}
      </div>

      {/* Recording Controls */}
      <div style={styles.buttonGroup}>
        <button
          onClick={startRecording}
          disabled={isRecording || isSpeaking}
          style={{
            ...styles.button,
            ...styles.recordBtn,
            opacity: isRecording || isSpeaking ? 0.5 : 1,
          }}
          title="Start recording voice command"
        >
          {isRecording ? "🔴 Recording..." : "🎤 Record"}
        </button>
        <button
          onClick={stopRecording}
          disabled={!isRecording}
          style={{
            ...styles.button,
            ...styles.stopBtn,
            opacity: !isRecording ? 0.5 : 1,
          }}
          title="Stop recording"
        >
          ⏹ Stop
        </button>
        {isSpeaking && (
          <button
            onClick={stopSpeaking}
            style={{ ...styles.button, ...styles.stopSpeakBtn }}
            title="Stop speech output"
          >
            🔇 Stop Speaking
          </button>
        )}
      </div>

      {/* Language Indicator */}
      {draft && (
        <div style={styles.languageIndicator}>
          🌐 Detected Language: {detectedLanguage === "uk" ? "🇺🇦 Ukrainian" : "🇬🇧 English"}
        </div>
      )}

      {/* Draft Section */}
      {draft && (
        <>
          <div style={styles.draftBox}>
            <strong>📝 Draft:</strong>
            <p style={styles.draftText}>{draft}</p>
          </div>

          <div style={styles.actionGroup}>
            <button
              onClick={redoDraft}
              disabled={isRecording || isExecuting}
              style={{
                ...styles.button,
                ...styles.redoBtn,
              }}
              title="Clear draft and record again"
            >
              🔄 Redo
            </button>
            <button
              onClick={pushCommand}
              disabled={isExecuting || !hasDraft}
              style={{
                ...styles.button,
                ...styles.pushBtn,
                opacity: isExecuting || !hasDraft ? 0.5 : 1,
              }}
              title="Execute the voice command"
            >
              {isExecuting ? "⏳ Pushing..." : "📤 Push"}
            </button>
          </div>
        </>
      )}

      {/* Result Section */}
      {commandResult && (
        <div style={styles.resultBox}>
          <strong>✅ Result:</strong>
          <p style={styles.text}>{commandResult}</p>
        </div>
      )}

      {/* Error Section */}
      {error && (
        <div style={styles.errorBox}>
          <strong>❌ Error:</strong>
          <p style={styles.text}>{error}</p>
        </div>
      )}
    </div>
  );
}

// Styling
const styles = {
  container: {
    padding: "20px",
    backgroundColor: "#f8f9fa",
    borderRadius: "8px",
    marginBottom: "20px",
    boxShadow: "0 2px 4px rgba(0,0,0,0.1)",
    fontFamily: "Arial, sans-serif",
  },
  title: {
    margin: "0 0 5px 0",
    color: "#333",
    fontSize: "1.3em",
  },
  subtitle: {
    margin: "0 0 15px 0",
    color: "#666",
    fontSize: "0.9em",
  },
  statusBox: {
    padding: "12px",
    backgroundColor: "#e7f3ff",
    border: "2px solid #0066cc",
    borderRadius: "4px",
    marginBottom: "15px",
    display: "flex",
    justifyContent: "space-between",
    alignItems: "center",
  },
  statusText: {
    fontWeight: "bold",
    color: "#0066cc",
  },
  speakingIndicator: {
    animation: "pulse 1s infinite",
    color: "#ff6b6b",
    fontWeight: "bold",
  },
  buttonGroup: {
    display: "flex",
    gap: "10px",
    marginBottom: "15px",
    flexWrap: "wrap",
  },
  button: {
    padding: "10px 15px",
    border: "none",
    borderRadius: "4px",
    cursor: "pointer",
    fontWeight: "bold",
    fontSize: "1em",
    transition: "all 0.3s ease",
  },
  recordBtn: {
    backgroundColor: "#007bff",
    color: "white",
  },
  stopBtn: {
    backgroundColor: "#dc3545",
    color: "white",
  },
  stopSpeakBtn: {
    backgroundColor: "#ff6b6b",
    color: "white",
  },
  redoBtn: {
    backgroundColor: "#6c757d",
    color: "white",
  },
  pushBtn: {
    backgroundColor: "#28a745",
    color: "white",
  },
  languageIndicator: {
    padding: "8px",
    backgroundColor: "#fff3cd",
    border: "1px solid #ffc107",
    borderRadius: "4px",
    marginBottom: "12px",
    fontSize: "0.9em",
    color: "#856404",
  },
  draftBox: {
    padding: "12px",
    backgroundColor: "#e2e3e5",
    border: "1px solid #ccc",
    borderRadius: "4px",
    marginBottom: "12px",
  },
  draftText: {
    margin: "8px 0 0 0",
    color: "#333",
  },
  actionGroup: {
    display: "flex",
    gap: "10px",
    marginBottom: "15px",
  },
  resultBox: {
    padding: "12px",
    backgroundColor: "#d4edda",
    border: "1px solid #28a745",
    borderRadius: "4px",
    marginBottom: "12px",
  },
  errorBox: {
    padding: "12px",
    backgroundColor: "#f8d7da",
    border: "1px solid #f5c6cb",
    borderRadius: "4px",
    marginBottom: "12px",
  },
  text: {
    margin: "8px 0 0 0",
    color: "#333",
  },
};
