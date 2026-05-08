import { useRef, useState } from "react";

export function SpeechCommander({ onCommandExecuted, token }) {
  const [isRecording, setIsRecording] = useState(false);
  const [draft, setDraft] = useState("");
  const [commandResult, setCommandResult] = useState("");
  const [error, setError] = useState("");
  const [status, setStatus] = useState("Ready to record.");
  const [isExecuting, setIsExecuting] = useState(false);
  const mediaRecorderRef = useRef(null);
  const audioChunksRef = useRef([]);
  const streamRef = useRef(null);

  const startRecording = async () => {
    try {
      setError("");
      setStatus("Recording... Speak clearly.");

      const stream = await navigator.mediaDevices.getUserMedia({
        audio: {
          echoCancellation: true,
          noiseSuppression: true,
          autoGainControl: true,
        },
      });

      streamRef.current = stream;
      audioChunksRef.current = [];

      const mediaRecorder = new MediaRecorder(stream);
      mediaRecorderRef.current = mediaRecorder;

      mediaRecorder.ondataavailable = (event) => {
        if (event.data.size > 0) {
          audioChunksRef.current.push(event.data);
        }
      };

      mediaRecorder.start();
      setIsRecording(true);
    } catch (err) {
      let errorMsg = err.message;
      if (err.name === "NotAllowedError") {
        errorMsg = "Microphone permission denied. Allow microphone in browser settings.";
      } else if (err.name === "NotFoundError") {
        errorMsg = "No microphone found. Connect a microphone.";
      }
      console.error("Recording error:", err);
      setError(errorMsg);
      setStatus("Error. Try again.");
    }
  };

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
          setStatus("Transcribing audio...");
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
            setDraft(data.transcript || "");
            setStatus(
              data.transcript
                ? "Draft ready. Use Push to send."
                : "No text recognized. Try again."
            );
          } else {
            setError(data.error || `HTTP ${response.status}`);
            setStatus("Error. Try again.");
          }
        } catch (err) {
          console.error("Transcription error:", err);
          setError(`Transcription error: ${err.message}`);
          setStatus("Error. Try again.");
        }
      };
      reader.readAsDataURL(audioBlob);

      streamRef.current?.getTracks().forEach((track) => track.stop());
      setIsRecording(false);
    };

    mediaRecorderRef.current.stop();
  };

  const redoDraft = () => {
    setDraft("");
    setCommandResult("");
    setError("");
    setStatus("Draft cleared. Record again.");
  };

  const pushCommand = async () => {
    if (!draft.trim()) {
      setError("No command to execute");
      return;
    }

    setIsExecuting(true);
    setError("");
    setCommandResult("");
    setStatus("Executing command...");

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
        throw new Error(`Invalid response from server: ${text.substring(0, 100)}`);
      }

      if (response.ok) {
        setCommandResult(data.answer || "Done.");
        setStatus("Done. Record again.");
        setDraft("");
        onCommandExecuted?.();
      } else {
        setError(data.error || data.message || "Command execution failed");
        setStatus("Error. Try again.");
      }
    } catch (err) {
      console.error("Command execution error:", err);
      setError(`Execution error: ${err.message}`);
      setStatus("Error. Try again.");
    } finally {
      setIsExecuting(false);
    }
  };

  const hasDraft = Boolean(draft.trim());

  return (
    <div style={styles.container}>
      <h3 style={styles.title}>🎤 Voice Command</h3>

      <div style={styles.statusBox}>{status}</div>

      <div style={styles.buttonGroup}>
        <button
          onClick={startRecording}
          disabled={isRecording}
          style={{
            ...styles.button,
            ...styles.recordBtn,
            opacity: isRecording ? 0.5 : 1,
          }}
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
        >
          ⏹ Stop
        </button>
      </div>

      {draft && (
        <>
          <div style={styles.draftBox}>
            <strong>Draft:</strong>
            <p style={styles.draftText}>{draft}</p>
          </div>

          <div style={styles.actionGroup}>
            <button
              onClick={redoDraft}
              disabled={isRecording}
              style={{
                ...styles.button,
                ...styles.redoBtn,
              }}
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
            >
              {isExecuting ? "⏳ Pushing..." : "📤 Push"}
            </button>
          </div>
        </>
      )}

      {commandResult && (
        <div style={styles.resultBox}>
          <strong>Result:</strong>
          <p style={styles.text}>{commandResult}</p>
        </div>
      )}

      {error && (
        <div style={styles.errorBox}>
          <strong>❌ Error:</strong>
          <p style={styles.text}>{error}</p>
        </div>
      )}
    </div>
  );
}

const styles = {
  container: {
    border: "2px solid #4CAF50",
    borderRadius: "8px",
    padding: "16px",
    marginBottom: "20px",
    backgroundColor: "#f9f9f9",
    fontFamily: "system-ui, -apple-system, sans-serif",
  },
  title: {
    margin: "0 0 12px 0",
    color: "#2c3e50",
    fontSize: "18px",
    fontWeight: "bold",
  },
  statusBox: {
    backgroundColor: "#e3f2fd",
    padding: "10px 12px",
    borderRadius: "4px",
    marginBottom: "12px",
    fontSize: "14px",
    color: "#1565c0",
    fontWeight: "500",
  },
  buttonGroup: {
    display: "flex",
    gap: "8px",
    marginBottom: "12px",
  },
  actionGroup: {
    display: "flex",
    gap: "8px",
    marginBottom: "12px",
  },
  button: {
    padding: "10px 16px",
    border: "none",
    borderRadius: "4px",
    cursor: "pointer",
    fontWeight: "bold",
    fontSize: "14px",
    transition: "all 0.2s",
  },
  recordBtn: {
    backgroundColor: "#FF6B6B",
    color: "white",
    flex: 1,
  },
  stopBtn: {
    backgroundColor: "#FFA07A",
    color: "white",
    flex: 1,
  },
  redoBtn: {
    backgroundColor: "#FFB74D",
    color: "white",
    flex: 1,
  },
  pushBtn: {
    backgroundColor: "#4CAF50",
    color: "white",
    flex: 1,
  },
  draftBox: {
    backgroundColor: "#e8f5e9",
    padding: "12px",
    borderRadius: "4px",
    borderLeft: "4px solid #4CAF50",
    marginBottom: "12px",
  },
  draftText: {
    margin: "8px 0 0 0",
    fontSize: "14px",
    color: "#2e7d32",
  },
  resultBox: {
    backgroundColor: "#c8e6c9",
    padding: "12px",
    borderRadius: "4px",
    borderLeft: "4px solid #388E3C",
    marginBottom: "12px",
  },
  errorBox: {
    backgroundColor: "#ffebee",
    padding: "12px",
    borderRadius: "4px",
    borderLeft: "4px solid #f44336",
  },
  text: {
    margin: "8px 0 0 0",
    fontSize: "14px",
  },
};
