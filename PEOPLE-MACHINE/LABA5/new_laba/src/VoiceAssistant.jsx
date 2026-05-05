import { useEffect, useRef, useState } from "react";

export function VoiceAssistant({ onCommand, onTranscript }) {
  const [isListening, setIsListening] = useState(false);
  const [transcript, setTranscript] = useState("");
  const [aiResponse, setAiResponse] = useState("");
  const [error, setError] = useState("");
  const mediaRecorderRef = useRef(null);
  const audioChunksRef = useRef([]);
  const streamRef = useRef(null);

  const startListening = async () => {
    try {
      setError("");
      setTranscript("");
      setAiResponse("");

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

      mediaRecorder.onstop = async () => {
        const audioBlob = new Blob(audioChunksRef.current, {
          type: "audio/wav",
        });

        const reader = new FileReader();
        reader.onload = async (e) => {
          const base64Audio = e.target.result.split(",")[1];

          try {
            const response = await fetch("/api/ai/voice-command", {
              method: "POST",
              headers: { "Content-Type": "application/json" },
              body: JSON.stringify({ audio: base64Audio }),
            });

            const data = await response.json();

            if (data.success) {
              setTranscript(data.transcript);
              setAiResponse(data.response);
              onTranscript?.(data.transcript);
              onCommand?.(data.transcript, data.response);
            } else {
              setError(data.error || "Failed to process voice command");
            }
          } catch (err) {
            setError(err.message);
          }
        };
        reader.readAsDataURL(audioBlob);

        stream.getTracks().forEach((track) => track.stop());
      };

      mediaRecorder.start();
      setIsListening(true);
    } catch (err) {
      setError(err.message);
    }
  };

  const stopListening = () => {
    if (mediaRecorderRef.current && isListening) {
      mediaRecorderRef.current.stop();
      setIsListening(false);
    }
  };

  useEffect(() => {
    return () => {
      if (streamRef.current) {
        streamRef.current.getTracks().forEach((track) => track.stop());
      }
    };
  }, []);

  return (
    <div style={styles.container}>
      <div style={styles.controls}>
        <button
          onClick={startListening}
          disabled={isListening}
          style={{
            ...styles.button,
            ...styles.startButton,
            opacity: isListening ? 0.5 : 1,
          }}
        >
          🎤 Почати запис
        </button>
        <button
          onClick={stopListening}
          disabled={!isListening}
          style={{
            ...styles.button,
            ...styles.stopButton,
            opacity: !isListening ? 0.5 : 1,
          }}
        >
          ⏹ Зупинити запис
        </button>
      </div>

      {isListening && (
        <div style={styles.recording}>
          <span style={styles.recordingDot}></span> Запис...
        </div>
      )}

      {error && (
        <div style={styles.error}>
          <strong>Помилка:</strong> {error}
        </div>
      )}

      {transcript && (
        <div style={styles.section}>
          <strong>Ваша команда:</strong>
          <p style={styles.text}>{transcript}</p>
        </div>
      )}

      {aiResponse && (
        <div style={styles.section}>
          <strong>Відповідь AI:</strong>
          <p style={styles.text}>{aiResponse}</p>
        </div>
      )}
    </div>
  );
}

const styles = {
  container: {
    padding: "20px",
    border: "1px solid #ddd",
    borderRadius: "8px",
    marginBottom: "20px",
    backgroundColor: "#f9f9f9",
  },
  controls: {
    display: "flex",
    gap: "10px",
    marginBottom: "15px",
  },
  button: {
    padding: "10px 20px",
    border: "none",
    borderRadius: "4px",
    cursor: "pointer",
    fontWeight: "bold",
    fontSize: "14px",
    transition: "background-color 0.3s",
  },
  startButton: {
    backgroundColor: "#4CAF50",
    color: "white",
  },
  stopButton: {
    backgroundColor: "#f44336",
    color: "white",
  },
  recording: {
    display: "flex",
    alignItems: "center",
    gap: "8px",
    color: "#f44336",
    fontWeight: "bold",
    marginBottom: "10px",
  },
  recordingDot: {
    display: "inline-block",
    width: "12px",
    height: "12px",
    backgroundColor: "#f44336",
    borderRadius: "50%",
    animation: "blink 0.5s infinite",
  },
  error: {
    backgroundColor: "#ffebee",
    color: "#c62828",
    padding: "10px",
    borderRadius: "4px",
    marginBottom: "10px",
  },
  section: {
    marginTop: "15px",
    paddingTop: "10px",
    borderTop: "1px solid #ddd",
  },
  text: {
    margin: "8px 0",
    color: "#333",
    lineHeight: "1.5",
  },
};
