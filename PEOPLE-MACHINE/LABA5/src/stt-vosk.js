import fs from 'node:fs';
import path from 'node:path';

const WAV_HEADER_BYTES = 44;
let model = null;
let modelPathCache = null;
let vosk = null;

async function getVoskModule() {
  if (vosk) {
    return vosk;
  }

  try {
    const imported = await import('vosk');
    vosk = imported.default || imported;
    return vosk;
  } catch (_error) {
    throw new Error('Vosk package is not installed. Install optional dependency with: npm install vosk');
  }
}

function ensureVoskModelPath() {
  const configuredPath = process.env.VOSK_MODEL_PATH;

  if (!configuredPath) {
    throw new Error('VOSK_MODEL_PATH is not set in .env');
  }

  const resolvedPath = path.resolve(process.cwd(), configuredPath);
  if (!fs.existsSync(resolvedPath)) {
    throw new Error(`Vosk model path does not exist: ${resolvedPath}`);
  }

  return resolvedPath;
}

function getSampleRateFromWavHeader(buffer) {
  if (buffer.length < WAV_HEADER_BYTES) {
    return 16000;
  }

  return buffer.readUInt32LE(24);
}

function ensureWavBuffer(buffer) {
  if (!buffer || buffer.length < WAV_HEADER_BYTES) {
    throw new Error('Invalid WAV file: file is too small.');
  }

  const riff = buffer.toString('ascii', 0, 4);
  const wave = buffer.toString('ascii', 8, 12);

  if (riff !== 'RIFF' || wave !== 'WAVE') {
    throw new Error('Invalid WAV file: expected RIFF/WAVE format.');
  }
}

async function getOrCreateModel() {
  const voskModule = await getVoskModule();
  const resolvedPath = ensureVoskModelPath();

  if (!model || modelPathCache !== resolvedPath) {
    if (model) {
      model.free();
      model = null;
    }

    voskModule.setLogLevel(0);
    model = new voskModule.Model(resolvedPath);
    modelPathCache = resolvedPath;
  }

  return model;
}

export function validateVoskConfig() {
  try {
    const resolvedPath = ensureVoskModelPath();
    return { ok: true, modelPath: resolvedPath };
  } catch (error) {
    return { ok: false, error: error.message };
  }
}

export async function transcribeWavWithVosk(wavBuffer) {
  ensureWavBuffer(wavBuffer);

  const voskModule = await getVoskModule();
  const voskModel = await getOrCreateModel();
  const sampleRate = getSampleRateFromWavHeader(wavBuffer) || 16000;
  const pcmBuffer = wavBuffer.subarray(WAV_HEADER_BYTES);
  const recognizer = new voskModule.Recognizer({ model: voskModel, sampleRate });

  try {
    const chunkSize = 4000;

    for (let offset = 0; offset < pcmBuffer.length; offset += chunkSize) {
      const chunk = pcmBuffer.subarray(offset, offset + chunkSize);
      recognizer.acceptWaveform(chunk);
    }

    const finalJson = recognizer.finalResult();
    const parsed = JSON.parse(finalJson || '{}');
    return (parsed.text || '').trim();
  } finally {
    recognizer.free();
  }
}

process.on('exit', () => {
  if (model) {
    model.free();
    model = null;
  }
});
