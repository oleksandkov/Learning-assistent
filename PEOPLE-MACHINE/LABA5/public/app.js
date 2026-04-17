const recordBtn = document.getElementById('recordBtn');
const stopBtn = document.getElementById('stopBtn');
const redoBtn = document.getElementById('redoBtn');
const pushBtn = document.getElementById('pushBtn');
const refreshDevicesBtn = document.getElementById('refreshDevicesBtn');
const inputDeviceSelect = document.getElementById('inputDeviceSelect');
const devicesStatusEl = document.getElementById('devicesStatus');
const statusEl = document.getElementById('status');
const draftEl = document.getElementById('draft');
const chatList = document.getElementById('chatList');

let mediaRecorder = null;
let mediaStream = null;
let audioChunks = [];
let isRecording = false;
let currentDraft = '';
let selectedInputDeviceId = 'default';
const chatHistory = [];

function setStatus(message) {
  statusEl.textContent = message;
}

function setDraft(text) {
  currentDraft = (text || '').trim();
  draftEl.textContent = currentDraft || 'Draft appears here.';

  const hasDraft = Boolean(currentDraft);
  redoBtn.disabled = !hasDraft;
  pushBtn.disabled = !hasDraft;
}

function appendMessage(role, text) {
  const li = document.createElement('li');
  li.className = role;
  li.textContent = `${role === 'user' ? 'You' : 'AI'}: ${text}`;
  chatList.appendChild(li);
  li.scrollIntoView({ behavior: 'smooth', block: 'end' });
}

function setDevicesStatus(message) {
  devicesStatusEl.textContent = message;
}

function clearChatHistory() {
  chatList.innerHTML = '';
  chatHistory.length = 0;
}

const uiTargets = {
  record: recordBtn,
  stop: stopBtn,
  redo: redoBtn,
  push: pushBtn,
  refresh_devices: refreshDevicesBtn,
  device_select: inputDeviceSelect,
  draft: draftEl,
  status: statusEl,
  chat: chatList
};

function extractUiEnvelope(rawText) {
  if (typeof rawText !== 'string' || !rawText.trim()) {
    return { actions: [], cleanedText: '' };
  }

  const match = rawText.match(/^\s*\[UI_ACTIONS:(\{[\s\S]*?\})\]\s*/i);
  if (!match) {
    return { actions: [], cleanedText: rawText.trim() };
  }

  const cleanedText = rawText.replace(match[0], '').trim();
  try {
    const parsed = JSON.parse(match[1]);
    const actions = Array.isArray(parsed.actions) ? parsed.actions : [];
    return { actions, cleanedText };
  } catch (_error) {
    return { actions: [], cleanedText };
  }
}

async function selectInputDeviceByName(name) {
  if (typeof name !== 'string' || !name.trim()) {
    return false;
  }

  const lower = name.trim().toLowerCase();
  const options = [...inputDeviceSelect.options];
  const match = options.find((option) => option.textContent.trim().toLowerCase().includes(lower));

  if (!match) {
    return false;
  }

  inputDeviceSelect.value = match.value;
  selectedInputDeviceId = match.value;
  setDevicesStatus(`Selected: ${match.textContent}`);
  return true;
}

async function executeUiAction(actionItem) {
  if (!actionItem || typeof actionItem !== 'object') {
    return false;
  }

  const action = typeof actionItem.action === 'string' ? actionItem.action.toLowerCase() : '';
  const targetKey = typeof actionItem.target === 'string' ? actionItem.target : '';
  const value = actionItem.value;

  switch (action) {
    case 'clear_chat':
      clearChatHistory();
      setStatus('AI: chat cleared.');
      return true;
    case 'clear_draft':
      setDraft('');
      setStatus('AI: draft cleared.');
      return true;
    case 'open_device_settings':
      refreshDevicesBtn.focus();
      await refreshInputDevices();
      setStatus('AI: device settings ready.');
      return true;
    case 'toggle_compact_mode':
      document.body.classList.toggle('compact');
      setStatus('AI: compact mode toggled.');
      return true;
    case 'set_compact_mode':
      document.body.classList.toggle('compact', Boolean(value));
      setStatus(`AI: compact mode ${Boolean(value) ? 'on' : 'off'}.`);
      return true;
    case 'focus_record':
      recordBtn.focus();
      setStatus('AI: record focused.');
      return true;
    case 'start_recording':
      await startRecording();
      return true;
    case 'stop_recording':
      stopRecording();
      return true;
    case 'redo_draft':
      redoDraft();
      return true;
    case 'push_draft':
      await pushDraft();
      return true;
    case 'refresh_devices':
      await refreshInputDevices();
      return true;
    case 'select_device':
      if (typeof value === 'string') {
        if (value === 'default') {
          inputDeviceSelect.value = 'default';
          selectedInputDeviceId = 'default';
          return true;
        }

        if (await selectInputDeviceByName(value)) {
          return true;
        }
      }
      return false;
    case 'focus': {
      const target = uiTargets[targetKey];
      if (!target || typeof target.focus !== 'function') {
        return false;
      }
      target.focus();
      return true;
    }
    case 'click': {
      const target = uiTargets[targetKey];
      if (!target || typeof target.click !== 'function') {
        return false;
      }
      target.click();
      return true;
    }
    case 'set_status':
      setStatus(typeof value === 'string' ? value : 'Done.');
      return true;
    default:
      return false;
  }
}

function renderInputDevices(audioInputs) {
  const current = selectedInputDeviceId;
  inputDeviceSelect.innerHTML = '';

  const defaultOption = document.createElement('option');
  defaultOption.value = 'default';
  defaultOption.textContent = 'Default microphone';
  inputDeviceSelect.appendChild(defaultOption);

  audioInputs.forEach((device, index) => {
    const option = document.createElement('option');
    option.value = device.deviceId;
    option.textContent = device.label || `Microphone ${index + 1}`;
    inputDeviceSelect.appendChild(option);
  });

  if ([...inputDeviceSelect.options].some((option) => option.value === current)) {
    inputDeviceSelect.value = current;
  } else {
    inputDeviceSelect.value = 'default';
    selectedInputDeviceId = 'default';
  }
}

async function refreshInputDevices() {
  if (!navigator.mediaDevices?.enumerateDevices) {
    setDevicesStatus('Device enumeration is not supported in this browser.');
    refreshDevicesBtn.disabled = true;
    return;
  }

  try {
    const devices = await navigator.mediaDevices.enumerateDevices();
    const audioInputs = devices.filter((device) => device.kind === 'audioinput');

    renderInputDevices(audioInputs);

    if (audioInputs.length === 0) {
      setDevicesStatus('No microphone devices found. Connect a microphone and press Refresh devices.');
      return;
    }

    const hiddenLabels = audioInputs.some((device) => !device.label);
    if (hiddenLabels) {
      setDevicesStatus('Microphones detected. Labels will appear after microphone permission is granted.');
      return;
    }

    setDevicesStatus(`Detected ${audioInputs.length} microphone device(s).`);
  } catch (error) {
    setDevicesStatus(`Cannot enumerate devices: ${error.message}`);
  }
}

function ensureRecordingSupport() {
  const canRecord = Boolean(navigator.mediaDevices?.getUserMedia) && Boolean(window.MediaRecorder);

  if (!canRecord) {
    setStatus('This browser does not support microphone recording (MediaRecorder). Try Chrome or Edge.');
    recordBtn.disabled = true;
    return false;
  }

  return true;
}

async function transcribeRecordedAudio(audioBlob) {
  setStatus('Transcribing audio...');

  const formData = new FormData();
  formData.append('audio', audioBlob, 'recording.webm');

  const sttResponse = await fetch('/api/stt', {
    method: 'POST',
    body: formData
  });

  const sttData = await sttResponse.json();

  if (!sttResponse.ok) {
    throw new Error(sttData.error || 'STT request failed.');
  }

  setDraft(sttData.transcript || '');
  setStatus(currentDraft ? 'Draft ready. Use Push to send.' : 'No text recognized. Try again.');
}

async function startRecording() {
  if (!ensureRecordingSupport() || isRecording) {
    return;
  }

  try {
    const audioConstraints = {
      channelCount: 1,
      echoCancellation: true,
      noiseSuppression: true
    };

    if (selectedInputDeviceId !== 'default') {
      audioConstraints.deviceId = { exact: selectedInputDeviceId };
    }

    mediaStream = await navigator.mediaDevices.getUserMedia({
      audio: audioConstraints
    });

    await refreshInputDevices();

    const preferredMime = 'audio/webm;codecs=opus';
    const mimeType = MediaRecorder.isTypeSupported(preferredMime) ? preferredMime : undefined;

    audioChunks = [];
    mediaRecorder = new MediaRecorder(mediaStream, mimeType ? { mimeType } : undefined);

    mediaRecorder.ondataavailable = (event) => {
      if (event.data && event.data.size > 0) {
        audioChunks.push(event.data);
      }
    };

    mediaRecorder.onstart = () => {
      isRecording = true;
      recordBtn.disabled = true;
      stopBtn.disabled = false;
      setStatus('Recording... Speak clearly.');
    };

    mediaRecorder.onstop = async () => {
      isRecording = false;
      recordBtn.disabled = false;
      stopBtn.disabled = true;

      const audioBlob = new Blob(audioChunks, {
        type: mediaRecorder?.mimeType || 'audio/webm'
      });

      mediaStream?.getTracks().forEach((track) => track.stop());
      mediaStream = null;

      try {
        await transcribeRecordedAudio(audioBlob);
      } catch (error) {
        setStatus(`Transcription failed: ${error.message}`);
      }
    };

    mediaRecorder.onerror = (event) => {
      setStatus(`Recording error: ${event.error?.name || 'unknown error'}`);
      isRecording = false;
      recordBtn.disabled = false;
      stopBtn.disabled = true;
    };

    mediaRecorder.start(250);
  } catch (error) {
    if (error?.name === 'NotAllowedError') {
      setStatus('Microphone permission denied. Allow microphone in browser site settings for localhost and retry.');
      setDevicesStatus('Permission denied: open browser site permissions, allow microphone, then click Refresh devices.');
    } else if (error?.name === 'NotFoundError') {
      setStatus('No microphone device found. Connect or enable a microphone and try again.');
    } else {
      setStatus(`Microphone access failed: ${error.message}`);
    }

    isRecording = false;
    recordBtn.disabled = false;
    stopBtn.disabled = true;
  }
}

function stopRecording() {
  if (!mediaRecorder || !isRecording) {
    return;
  }

  mediaRecorder.stop();
}

function redoDraft() {
  setDraft('');
  setStatus('Draft cleared. Record again.');
}

async function pushDraft() {
  if (!currentDraft) {
    return;
  }

  try {
    pushBtn.disabled = true;
    redoBtn.disabled = true;

    appendMessage('user', currentDraft);
    chatHistory.push({ role: 'user', content: currentDraft });

    setStatus('Waiting for AI response...');

    const chatResponse = await fetch('/api/chat', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        prompt: currentDraft,
        history: chatHistory.slice(-8)
      })
    });

    const chatData = await chatResponse.json();

    if (!chatResponse.ok) {
      throw new Error(chatData.error || 'Chat request failed.');
    }

    const rawAnswer = chatData.answer || 'No response generated.';
    const { actions, cleanedText } = extractUiEnvelope(rawAnswer);
    const finalAnswer = cleanedText || 'Done.';

    appendMessage('assistant', finalAnswer);
    chatHistory.push({ role: 'assistant', content: finalAnswer });

    for (const actionItem of actions) {
      await executeUiAction(actionItem);
    }

    setDraft('');
    if (!actions.length) {
      setStatus('Done. Record again.');
    }
  } catch (error) {
    setStatus(`AI response failed: ${error.message}`);
  } finally {
    pushBtn.disabled = !currentDraft;
    redoBtn.disabled = !currentDraft;
  }
}

recordBtn.addEventListener('click', startRecording);
stopBtn.addEventListener('click', stopRecording);
redoBtn.addEventListener('click', redoDraft);
pushBtn.addEventListener('click', pushDraft);
refreshDevicesBtn.addEventListener('click', refreshInputDevices);
inputDeviceSelect.addEventListener('change', (event) => {
  selectedInputDeviceId = event.target.value || 'default';
});

navigator.mediaDevices?.addEventListener?.('devicechange', refreshInputDevices);

setStatus('Ready to record.');
ensureRecordingSupport();
refreshInputDevices();
