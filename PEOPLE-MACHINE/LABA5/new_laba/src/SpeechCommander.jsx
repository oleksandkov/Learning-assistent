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
    <section className="voiceCard panel">
      <div className="voiceHeader">
        <h3>🎤 Voice Command</h3>
        <span className={`voiceBadge ${isRecording ? "recording" : ""}`}>
          {isRecording ? "Recording" : "Ready"}
        </span>
      </div>

      <p className="voiceStatus">{status}</p>

      <div className="voiceActions">
        <button
          className="voiceBtn primary"
          onClick={startRecording}
          disabled={isRecording}
        >
          {isRecording ? "🔴 Recording..." : "🎤 Record"}
        </button>
        <button
          className="voiceBtn secondary"
          onClick={stopRecording}
          disabled={!isRecording}
        >
          ⏹ Stop
        </button>
      </div>

      {draft && (
        <>
          <div className="voiceBlock voiceDraft">
            <div className="voiceBlockHeader">Draft</div>
            <p>{draft}</p>
          </div>

          <div className="voiceActions split">
            <button
              className="voiceBtn secondary"
              onClick={redoDraft}
              disabled={isRecording}
            >
              🔄 Redo
            </button>
            <button
              className="voiceBtn success"
              onClick={pushCommand}
              disabled={isExecuting || !hasDraft}
            >
              {isExecuting ? "⏳ Pushing..." : "📤 Push"}
            </button>
          </div>
        </>
      )}

      {commandResult && (
        <div className="voiceBlock voiceResult">
          <div className="voiceBlockHeader">Result</div>
          <p>{commandResult}</p>
        </div>
      )}

      {error && (
        <div className="voiceBlock voiceError">
          <div className="voiceBlockHeader">Error</div>
          <p>{error}</p>
        </div>
      )}
    </section>
  );
}
