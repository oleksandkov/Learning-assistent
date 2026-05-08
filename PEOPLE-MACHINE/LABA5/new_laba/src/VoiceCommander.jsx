import { useRef, useState } from "react";

export function VoiceCommander({ onCommandExecuted, token }) {
  const [isRecording, setIsRecording] = useState(false);
  const [transcript, setTranscript] = useState("");
  const [commandResult, setCommandResult] = useState("");
  const [error, setError] = useState("");
  const [isExecuting, setIsExecuting] = useState(false);
  const mediaRecorderRef = useRef(null);
  const audioChunksRef = useRef([]);
  const streamRef = useRef(null);

  const startRecording = async () => {
    try {
      setError("");
      setTranscript("");
      setCommandResult("");

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
      setError(`Recording failed: ${err.message}`);
    }
  };

  const stopRecording = async () => {
    if (!mediaRecorderRef.current) return;

    mediaRecorderRef.current.onstop = async () => {
      const audioBlob = new Blob(audioChunksRef.current, {
        type: "audio/wav",
      });

      const reader = new FileReader();
      reader.onload = async (e) => {
        const base64Audio = e.target.result.split(",")[1];

        try {
          const response = await fetch("/api/ai/transcribe", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
              Authorization: `Bearer ${token}`,
            },
            body: JSON.stringify({ audio: base64Audio }),
          });

          const data = await response.json();

          if (data.success) {
            setTranscript(data.transcript);
          } else {
            setError(data.error || "Failed to transcribe");
          }
        } catch (err) {
          setError(`Transcription error: ${err.message}`);
        }
      };
      reader.readAsDataURL(audioBlob);

      streamRef.current?.getTracks().forEach((track) => track.stop());
      setIsRecording(false);
    };

    mediaRecorderRef.current.stop();
  };

  const executeCommand = async () => {
    if (!transcript.trim()) {
      setError("No command to execute");
      return;
    }

    setIsExecuting(true);
    setError("");
    setCommandResult("");

    try {
      const response = await fetch("/api/ai/execute-command", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify({ command: transcript }),
      });

      const data = await response.json();

      if (data.success) {
        setCommandResult(data.result);
        onCommandExecuted?.(data);
      } else {
        setError(data.error || "Command execution failed");
      }
    } catch (err) {
      setError(`Execution error: ${err.message}`);
    } finally {
      setIsExecuting(false);
    }
  };

  const clearTranscript = () => {
    setTranscript("");
    setCommandResult("");
    setError("");
  };

  return (
    <div style={styles.container}>
      <h3 style={styles.title}>🎤 Voice Command Agent</h3>

      <div style={styles.section}>
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
            {isRecording ? "🔴 Recording..." : "🎤 Start Recording"}
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
            ⏹ Stop Recording
          </button>
        </div>

        {transcript && (
          <div style={styles.transcriptBox}>
            <strong>Transcribed:</strong>
            <p style={styles.text}>{transcript}</p>
          </div>
        )}

        {transcript && (
          <div style={styles.executeSection}>
            <button
              onClick={executeCommand}
              disabled={isExecuting || !transcript}
              style={{
                ...styles.button,
                ...styles.executeBtn,
                opacity: isExecuting || !transcript ? 0.5 : 1,
              }}
            >
              {isExecuting ? "⏳ Executing..." : "✨ Execute Command"}
            </button>
            <button onClick={clearTranscript} style={styles.clearBtn}>
              Clear
            </button>
          </div>
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
  },
  title: {
    margin: "0 0 16px 0",
    color: "#2c3e50",
    fontSize: "18px",
  },
  section: {
    display: "flex",
    flexDirection: "column",
    gap: "12px",
  },
  buttonGroup: {
    display: "flex",
    gap: "8px",
    flexWrap: "wrap",
  },
  button: {
    padding: "10px 16px",
    border: "none",
    borderRadius: "4px",
    cursor: "pointer",
    fontWeight: "bold",
    fontSize: "14px",
    transition: "all 0.3s",
  },
  recordBtn: {
    backgroundColor: "#FF6B6B",
    color: "white",
  },
  stopBtn: {
    backgroundColor: "#FFA07A",
    color: "white",
  },
  executeBtn: {
    backgroundColor: "#4CAF50",
    color: "white",
    flex: 1,
  },
  clearBtn: {
    backgroundColor: "#95a5a6",
    color: "white",
    padding: "10px 16px",
    border: "none",
    borderRadius: "4px",
    cursor: "pointer",
  },
  executeSection: {
    display: "flex",
    gap: "8px",
  },
  transcriptBox: {
    backgroundColor: "#e8f5e9",
    padding: "12px",
    borderRadius: "4px",
    borderLeft: "4px solid #4CAF50",
  },
  resultBox: {
    backgroundColor: "#c8e6c9",
    padding: "12px",
    borderRadius: "4px",
    borderLeft: "4px solid #388E3C",
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
