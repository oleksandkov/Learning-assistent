import "dotenv/config";
import bcrypt from "bcryptjs";
import cors from "cors";
import express from "express";
import jwt from "jsonwebtoken";
import { all, get, initDb, run } from "./db.js";
import { processVoiceCommand, speechToText, generateResponse, parseVoiceCommand } from "./ai.js";

const app = express();
const port = Number(process.env.PORT || 4000);
const jwtSecret = process.env.JWT_SECRET || "dev-secret-change-me";

function normalizeAdminUsername(value) {
  return String(value || "")
    .trim()
    .replace(/^['\"]+|['\"]+$/g, "")
    .trim()
    .toLowerCase();
}

function parseAdminUsernames(values = []) {
  return [...new Set(values.flatMap((value) => String(value).split(",")))]
    .map(normalizeAdminUsername)
    .filter(Boolean);
}

const adminUsernames = new Set(
  parseAdminUsernames([
    process.env.ADMIN_USERNAMES || "",
    process.env.ADMIN_USERNAME || "",
  ]),
);

app.use(cors());
app.use(express.json({ limit: "50mb" }));
app.use(express.urlencoded({ limit: "50mb", extended: true }));

function createToken(user) {
  return jwt.sign({ userId: user.id, username: user.username }, jwtSecret, {
    expiresIn: "8h",
  });
}

function isAdminUsername(username) {
  return adminUsernames.has(normalizeAdminUsername(username));
}

function statusByUsername(username) {
  return isAdminUsername(username) ? "admin" : "user";
}

async function syncUserStatus(userId, username, currentStatus = "") {
  const nextStatus = statusByUsername(username);
  if (currentStatus !== nextStatus) {
    await run("UPDATE users SET status = ? WHERE id = ?", [nextStatus, userId]);
  }
  return nextStatus;
}

async function syncAllUsersStatus() {
  const admins = [...adminUsernames];
  if (admins.length === 0) {
    await run("UPDATE users SET status = 'user' WHERE status <> 'user'");
    return;
  }

  const placeholders = admins.map(() => "?").join(", ");
  await run(
    `UPDATE users
     SET status = CASE
       WHEN LOWER(TRIM(username)) IN (${placeholders}) THEN 'admin'
       ELSE 'user'
     END`,
    admins,
  );
}

function isValidOptionValue(value) {
  const text = String(value || "").trim();
  return (
    Boolean(text) &&
    !text.includes("?") &&
    !text.includes("�") &&
    !/^webtest(updated)?$/i.test(text)
  );
}

function sanitizeOptionList(values = []) {
  return [...new Set(values.map((x) => String(x || "").trim()))].filter(
    isValidOptionValue,
  );
}

function splitMultiValues(value) {
  return String(value || "")
    .split(/[;,\n]+/)
    .map((x) => x.trim())
    .filter(Boolean);
}

function collectLessonGroups(payload = {}) {
  const groupsFromArray = Array.isArray(payload.groups)
    ? payload.groups.map((x) => String(x || "").trim())
    : [];

  const merged = [
    String(payload.group || "").trim(),
    ...groupsFromArray,
    ...splitMultiValues(payload.groupsText),
  ];

  return sanitizeOptionList(merged);
}

// Fuzzy match - find best match in list with partial names
function fuzzyMatchTeacher(input, teachers = []) {
  if (!input || !teachers.length) return input;
  const lowerInput = String(input).toLowerCase().trim();
  
  // Exact match
  const exact = teachers.find(t => String(t).toLowerCase() === lowerInput);
  if (exact) return exact;
  
  // Starts with match (for partial names like "Ткач" → "Ткаченко О.М.")
  const startsWithMatch = teachers.find(t => String(t).toLowerCase().startsWith(lowerInput));
  if (startsWithMatch) return startsWithMatch;
  
  // Contains match
  const containsMatch = teachers.find(t => String(t).toLowerCase().includes(lowerInput));
  if (containsMatch) return containsMatch;
  
  // Levenshtein-like: check if input is similar to any teacher's first word or surname
  const possibleMatches = teachers.filter(t => {
    const parts = String(t).toLowerCase().split(/\s+/);
    return parts.some(part => part.includes(lowerInput) || lowerInput.includes(part));
  });
  if (possibleMatches.length > 0) return possibleMatches[0];
  
  return input;
}

// Fuzzy match for groups - handle "1пі", "2пі" → "1ПІ-25Б", "2ПІ-25Б"
function fuzzyMatchGroup(input, groups = []) {
  if (!input || !groups.length) return input;
  const lowerInput = String(input).toLowerCase().trim();
  
  // Exact match
  const exact = groups.find(g => String(g).toLowerCase() === lowerInput);
  if (exact) return exact;
  
  // Try to extract number and match (1пі → find group with "1")
  const numberMatch = lowerInput.match(/(\d+)/);
  if (numberMatch) {
    const num = numberMatch[1];
    const groupByNumber = groups.find(g => String(g).toLowerCase().startsWith(num));
    if (groupByNumber) return groupByNumber;
  }
  
  // Starts with match
  const startsWithMatch = groups.find(g => String(g).toLowerCase().startsWith(lowerInput));
  if (startsWithMatch) return startsWithMatch;
  
  // Contains match
  const containsMatch = groups.find(g => String(g).toLowerCase().includes(lowerInput));
  if (containsMatch) return containsMatch;
  
  return input;
}

// Convert spoken time words to 24-hour format
function normalizeSpokenTime(timeInput) {
  if (!timeInput) return timeInput;
  let input = String(timeInput).toLowerCase().trim();
  
  // Handle hyphen notation: "16-30" → "16:30", "8-15" → "08:15"
  const hyphenMatch = input.match(/^(\d{1,2})-(\d{1,2})$/);
  if (hyphenMatch) {
    const hours = parseInt(hyphenMatch[1]);
    const minutes = parseInt(hyphenMatch[2]);
    if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59) {
      const result = `${String(hours).padStart(2, "0")}:${String(minutes).padStart(2, "0")}`;
      console.log(`✅ Hyphen notation converted: "${timeInput}" → "${result}"`);
      return result;
    }
  }

  // Handle dot notation: "16.30" → "16:30", "8.15" → "08:15"
  const dotMatch = input.match(/^(\d{1,2})\.(\d{1,2})$/);
  if (dotMatch) {
    const hours = parseInt(dotMatch[1]);
    const minutes = parseInt(dotMatch[2]);
    if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59) {
      const result = `${String(hours).padStart(2, "0")}:${String(minutes).padStart(2, "0")}`;
      console.log(`✅ Dot notation converted: "${timeInput}" → "${result}"`);
      return result;
    }
  }

  // Handle compact numeric time: "930" → "09:30", "1630" → "16:30"
  const compactMatch = input.match(/^(\d{3,4})$/);
  if (compactMatch) {
    const digits = compactMatch[1].padStart(4, "0");
    const hours = parseInt(digits.slice(0, 2));
    const minutes = parseInt(digits.slice(2));
    if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59) {
      const result = `${String(hours).padStart(2, "0")}:${String(minutes).padStart(2, "0")}`;
      console.log(`✅ Compact time converted: "${timeInput}" → "${result}"`);
      return result;
    }
  }
  
  // First, check if it's just a number like "15", "8", "14" → convert to HH:00
  const justNumber = input.match(/^(\d{1,2})$/);
  if (justNumber) {
    let hours = parseInt(justNumber[1]);
    if (hours >= 0 && hours <= 23) {
      return `${String(hours).padStart(2, "0")}:00`;
    }
  }
  
  // Handle spoken Ukrainian numbers (п'ятнадцять, дев'ятнадцять, etc.)
  const spokenNumbers = {
    'нуль': '0', 'нульова': '0',
    'одна': '1', 'один': '1', 'першої': '1', 'перший': '1',
    'два': '2', 'дві': '2', 'другої': '2', 'другий': '2',
    'три': '3', 'третя': '3', 'третій': '3',
    'чотири': '4', 'четверта': '4', 'четвертої': '4',
    "п'ять": '5', 'п\'ятої': '5', 'п\'ятої': '5',
    'шість': '6', 'шостої': '6',
    'сім': '7', 'сьомої': '7',
    'вісім': '8', 'восьмої': '8',
    'дев\'ять': '9', 'дев\'ятої': '9',
    'десять': '10', 'десятої': '10',
    'одинадцять': '11', 'одинадцятої': '11',
    'дванадцять': '12', 'дванадцятої': '12',
    'тринадцять': '13', 'тринадцятої': '13',
    'чотирнадцять': '14', 'чотирнадцятої': '14',
    "п'ятнадцять": '15', "п'ятнадцятої": '15',
    'шістнадцять': '16', 'шістнадцятої': '16',
    'сімнадцять': '17', 'сімнадцятої': '17',
    'вісімнадцять': '18', 'вісімнадцятої': '18',
    'дев\'ятнадцять': '19', 'дев\'ятнадцятої': '19',
    'двадцять': '20', 'двадцятої': '20',
  };
  
  // Check for spoken numbers
  for (const [word, num] of Object.entries(spokenNumbers)) {
    if (input.includes(word)) {
      input = input.replace(word, num);
      break;
    }
  }
  
  // Now normalize using standard format
  const match = input.match(/(\d{1,2})(?::(\d{2}))?\s*(am|pm)?/i);
  if (match) {
    let hours = parseInt(match[1]);
    const minutes = match[2] ? parseInt(match[2]) : 0;
    const period = match[3]?.toLowerCase();

    if (period === "pm" && hours !== 12) hours += 12;
    if (period === "am" && hours === 12) hours = 0;

    if (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59) {
      return `${String(hours).padStart(2, "0")}:${String(minutes).padStart(2, "0")}`;
    }
  }

  return timeInput;
}

// Map Ukrainian day names to schedule abbreviations
function normalizeDay(day) {
  if (!day) return day;
  const ukrainianToDays = {
    понеділок: "Пн",
    пн: "Пн",
    вівторок: "Вт",
    вт: "Вт",
    середа: "Ср",
    ср: "Ср",
    четвер: "Чт",
    чотири: "Чт",
    чт: "Чт",
    "п'ятниця": "Пт",
    пт: "Пт",
    субота: "Сб",
    сб: "Сб",
    неділя: "Нд",
    нд: "Нд",
  };
  const lowerDay = String(day).toLowerCase();
  return ukrainianToDays[lowerDay] || day;
}

// Map Ukrainian type names to schedule values
function normalizeType(type) {
  if (!type) return type;
  const ukrainianToTypes = {
    лекція: "Лекція",
    лекция: "Лекція",
    лекц: "Лекція",
    практика: "Практика",
    практ: "Практика",
    "практичне заняття": "Практика",
    лабораторна: "Лабораторна",
    лаба: "Лабораторна",
    "лабораторне заняття": "Лабораторна",
  };
  const lowerType = String(type).toLowerCase();
  return ukrainianToTypes[lowerType] || type;
}

// Normalize time to HH:MM format
function normalizeTime(time) {
  if (!time) return time;
  
  // First try spoken time conversion
  const spokenConverted = normalizeSpokenTime(time);
  if (spokenConverted !== time) {
    return spokenConverted;
  }
  
  let timeStr = String(time).toLowerCase().trim();

  // Handle "9 am" -> "09:00", "14:30", etc.
  const match = timeStr.match(/(\d{1,2})(?::(\d{2}))?\s*(am|pm)?/i);
  if (match) {
    let hours = parseInt(match[1]);
    const minutes = match[2] ? parseInt(match[2]) : 0;
    const period = match[3]?.toLowerCase();

    if (period === "pm" && hours !== 12) hours += 12;
    if (period === "am" && hours === 12) hours = 0;

    return `${String(hours).padStart(2, "0")}:${String(minutes).padStart(
      2,
      "0",
    )}`;
  }

  return timeStr;
}

function isValidTimeFormat(timeStr) {
  // Check if time is in valid HH:MM format with valid hours (0-23) and minutes (0-59)
  const validTimeRegex = /^([0-1]?[0-9]|2[0-3]):([0-5][0-9])$/;
  return validTimeRegex.test(String(timeStr || "").trim());
}

function isPlaceholderValue(value) {
  const text = String(value || "").trim().toLowerCase();
  return !text || ["tbd", "unknown", "невідомо", "не відомо", "n/a", "na"].includes(text);
}

function auth(req, res, next) {
  const isPublicAuthRoute =
    req.path === "/api/auth/register" || req.path === "/api/auth/login";
  if (req.path === "/health" || isPublicAuthRoute) {
    return next();
  }

  const header = req.headers.authorization || "";
  const [, token] = header.split(" ");
  if (!token) {
    return res.status(401).json({ error: "Необхідна авторизація" });
  }

  try {
    const payload = jwt.verify(token, jwtSecret);
    req.user = payload;
    return next();
  } catch {
    return res.status(401).json({ error: "Недійсний токен" });
  }
}

async function requireAdmin(req, res, next) {
  const userId = req.user?.userId;
  if (!userId) {
    return res.status(401).json({ error: "Необхідна авторизація" });
  }

  const user = await get(
    "SELECT id, username, status FROM users WHERE id = ?",
    [userId],
  );
  if (!user) {
    return res.status(401).json({ error: "Користувач не знайдений" });
  }

  const effectiveStatus = await syncUserStatus(
    user.id,
    user.username,
    user.status,
  );
  if (effectiveStatus === "admin") {
    return next();
  }

  return res
    .status(403)
    .json({ error: "Лише адміністратор може змінювати дані" });
}

function validateLesson(payload, groups = []) {
  const required = [
    "subject",
    "day",
    "time",
    "room",
    "week",
    "teacher",
    "type",
  ];
  for (const field of required) {
    if (!String(payload[field] || "").trim()) {
      return `Поле \"${field}\" є обов'язковим`;
    }
  }

  if (!groups.length) {
    return 'Поле "group" є обов\'язковим';
  }

  return null;
}

function normalizeTimeForStorage(value) {
  const normalized = normalizeTime(value);
  return isValidTimeFormat(normalized) ? normalized : String(value || "").trim();
}

async function upsertLookupsFromLesson(lesson) {
  await run("INSERT OR IGNORE INTO subjects(name) VALUES(?)", [lesson.subject]);
  for (const group of lesson.groups || []) {
    await run("INSERT OR IGNORE INTO study_groups(name) VALUES(?)", [group]);
  }
  await run("INSERT OR IGNORE INTO teachers(name) VALUES(?)", [lesson.teacher]);
}

app.use(auth);

app.get("/health", (_req, res) => {
  res.json({ ok: true });
});

app.post("/api/auth/register", async (req, res) => {
  try {
    const username = String(req.body.username || "").trim();
    const password = String(req.body.password || "").trim();

    if (!username || !password) {
      return res.status(400).json({ error: "Логін і пароль обовʼязкові" });
    }
    if (password.length < 6) {
      return res
        .status(400)
        .json({ error: "Пароль має містити щонайменше 6 символів" });
    }

    const passwordHash = await bcrypt.hash(password, 10);
    const status = statusByUsername(username);
    const result = await run(
      "INSERT INTO users(username, password_hash, status) VALUES(?, ?, ?)",
      [username, passwordHash, status],
    );
    const user = { id: result.lastID, username };
    return res.status(201).json({
      token: createToken(user),
      user: {
        id: user.id,
        username: user.username,
        status,
        isAdmin: status === "admin",
      },
    });
  } catch (error) {
    if (String(error.message).includes("UNIQUE")) {
      return res
        .status(409)
        .json({ error: "Користувач з таким логіном вже існує" });
    }
    return res.status(500).json({ error: "Помилка сервера" });
  }
});

app.post("/api/auth/login", async (req, res) => {
  try {
    const username = String(req.body.username || "").trim();
    const password = String(req.body.password || "").trim();

    const user = await get(
      "SELECT id, username, password_hash, status FROM users WHERE username = ?",
      [username],
    );
    if (!user) {
      return res.status(401).json({ error: "Невірний логін або пароль" });
    }

    const ok = await bcrypt.compare(password, user.password_hash);
    if (!ok) {
      return res.status(401).json({ error: "Невірний логін або пароль" });
    }

    const status = await syncUserStatus(user.id, user.username, user.status);

    return res.json({
      token: createToken(user),
      user: {
        id: user.id,
        username: user.username,
        status,
        isAdmin: status === "admin",
      },
    });
  } catch {
    return res.status(500).json({ error: "Помилка сервера" });
  }
});

app.get("/api/auth/me", async (req, res) => {
  const userId = req.user?.userId;
  if (!userId) {
    return res.status(401).json({ error: "Необхідна авторизація" });
  }

  const user = await get(
    "SELECT id, username, status FROM users WHERE id = ?",
    [userId],
  );
  if (!user) {
    return res.status(401).json({ error: "Користувач не знайдений" });
  }

  const status = await syncUserStatus(user.id, user.username, user.status);
  return res.json({
    user: {
      id: user.id,
      username: user.username,
      status,
      isAdmin: status === "admin",
    },
  });
});

app.get("/api/lookups", async (_req, res) => {
  const [subjects, groups, teachers, times, rooms, days, weeks, types] =
    await Promise.all([
      all("SELECT name FROM subjects ORDER BY name COLLATE NOCASE ASC"),
      all("SELECT name FROM study_groups ORDER BY name COLLATE NOCASE ASC"),
      all("SELECT name FROM teachers ORDER BY name COLLATE NOCASE ASC"),
      all(
        "SELECT DISTINCT lesson_time AS value FROM lessons ORDER BY lesson_time COLLATE NOCASE ASC",
      ),
      all(
        "SELECT DISTINCT room AS value FROM lessons ORDER BY room COLLATE NOCASE ASC",
      ),
      all(
        "SELECT DISTINCT day AS value FROM lessons ORDER BY day COLLATE NOCASE ASC",
      ),
      all(
        "SELECT DISTINCT week AS value FROM lessons ORDER BY week COLLATE NOCASE ASC",
      ),
      all(
        "SELECT DISTINCT lesson_type AS value FROM lessons ORDER BY lesson_type COLLATE NOCASE ASC",
      ),
    ]);

  res.json({
    subjects: sanitizeOptionList(subjects.map((x) => x.name)),
    groups: sanitizeOptionList(groups.map((x) => x.name)),
    teachers: sanitizeOptionList(teachers.map((x) => x.name)),
    times: sanitizeOptionList(times.map((x) => x.value)),
    rooms: sanitizeOptionList(rooms.map((x) => x.value)),
    days: sanitizeOptionList(days.map((x) => x.value)),
    weeks: sanitizeOptionList(weeks.map((x) => x.value)),
    types: sanitizeOptionList(types.map((x) => x.value)),
  });
});

app.post("/api/lookups/:entity", requireAdmin, async (req, res) => {
  const entity = req.params.entity;
  const name = String(req.body.name || "").trim();

  if (!name) {
    return res.status(400).json({ error: "Порожнє значення не дозволено" });
  }
  if (!isValidOptionValue(name)) {
    return res.status(400).json({ error: "Некоректне значення довідника" });
  }

  const map = {
    subjects: "subjects",
    groups: "study_groups",
    teachers: "teachers",
  };

  const table = map[entity];
  if (!table) {
    return res.status(400).json({ error: "Невідомий тип довідника" });
  }

  await run(`INSERT OR IGNORE INTO ${table}(name) VALUES(?)`, [name]);
  return res.status(201).json({ ok: true });
});

app.get("/api/lessons", async (req, res) => {
  const groupFilters = sanitizeOptionList([
    String(req.query.group || "").trim(),
    ...splitMultiValues(req.query.groups),
  ]);

  const filters = {
    subject: String(req.query.subject || "").trim(),
    day: String(req.query.day || "").trim(),
    time: String(req.query.time || "").trim(),
    room: String(req.query.room || "").trim(),
    week: String(req.query.week || "").trim(),
    teacher: String(req.query.teacher || "").trim(),
    type: String(req.query.type || "").trim(),
  };

  const clauses = [];
  const params = [];
  const map = {
    subject: "subject",
    day: "day",
    time: "lesson_time",
    room: "room",
    week: "week",
    teacher: "teacher",
    type: "lesson_type",
  };

  if (groupFilters.length) {
    const groupClauses = groupFilters.map(
      () => "LOWER(TRIM(group_name)) = LOWER(TRIM(?))",
    );
    clauses.push(`(${groupClauses.join(" OR ")})`);
    groupFilters.forEach((value) => params.push(value));
  }

  for (const [key, value] of Object.entries(filters)) {
    if (!value) continue;
    clauses.push(`LOWER(${map[key]}) LIKE LOWER(?)`);
    params.push(`%${value}%`);
  }

  const whereSql = clauses.length ? `WHERE ${clauses.join(" AND ")}` : "";
  const rows = await all(
    `SELECT id, subject, day, lesson_time, room, group_name, week, teacher, lesson_type
     FROM lessons
     ${whereSql}
     ORDER BY day ASC, lesson_time ASC`,
    params,
  );

  return res.json(
    rows.map((x) => ({
      id: x.id,
      subject: x.subject,
      day: x.day,
      time: normalizeTimeForStorage(x.lesson_time),
      room: x.room,
      group: x.group_name,
      week: x.week,
      teacher: x.teacher,
      type: x.lesson_type,
    })),
  );
});

app.post("/api/lessons", requireAdmin, async (req, res) => {
  const groups = collectLessonGroups(req.body);
  const lesson = {
    subject: String(req.body.subject || "").trim(),
    day: String(req.body.day || "").trim(),
    time: normalizeTimeForStorage(req.body.time),
    room: String(req.body.room || "").trim(),
    group: groups[0] || "",
    groups,
    week: String(req.body.week || "").trim(),
    teacher: String(req.body.teacher || "").trim(),
    type: String(req.body.type || "").trim(),
  };

  const validationError = validateLesson(lesson, groups);
  if (validationError) {
    return res.status(400).json({ error: validationError });
  }

  await upsertLookupsFromLesson(lesson);
  const ids = [];
  for (const group of groups) {
    const result = await run(
      `INSERT INTO lessons(subject, day, lesson_time, room, group_name, week, teacher, lesson_type)
       VALUES(?, ?, ?, ?, ?, ?, ?, ?)`,
      [
        lesson.subject,
        lesson.day,
        lesson.time,
        lesson.room,
        group,
        lesson.week,
        lesson.teacher,
        lesson.type,
      ],
    );
    ids.push(result.lastID);
  }

  return res.status(201).json({ ids, count: ids.length });
});

app.put("/api/lessons/:id", requireAdmin, async (req, res) => {
  const groups = collectLessonGroups(req.body);
  const lesson = {
    subject: String(req.body.subject || "").trim(),
    day: String(req.body.day || "").trim(),
    time: normalizeTimeForStorage(req.body.time),
    room: String(req.body.room || "").trim(),
    group: groups[0] || "",
    groups: groups[0] ? [groups[0]] : [],
    week: String(req.body.week || "").trim(),
    teacher: String(req.body.teacher || "").trim(),
    type: String(req.body.type || "").trim(),
  };

  const validationError = validateLesson(lesson, lesson.groups);
  if (validationError) {
    return res.status(400).json({ error: validationError });
  }

  await upsertLookupsFromLesson(lesson);
  const result = await run(
    `UPDATE lessons
     SET subject = ?, day = ?, lesson_time = ?, room = ?, group_name = ?, week = ?, teacher = ?, lesson_type = ?
     WHERE id = ?`,
    [
      lesson.subject,
      lesson.day,
      lesson.time,
      lesson.room,
      lesson.group,
      lesson.week,
      lesson.teacher,
      lesson.type,
      req.params.id,
    ],
  );

  if (result.changes === 0) {
    return res.status(404).json({ error: "Запис не знайдено" });
  }

  return res.json({ ok: true });
});

app.delete("/api/lessons/:id", requireAdmin, async (req, res) => {
  const result = await run("DELETE FROM lessons WHERE id = ?", [req.params.id]);
  if (result.changes === 0) {
    return res.status(404).json({ error: "Запис не знайдено" });
  }
  return res.json({ ok: true });
});

// AI Integration Endpoints
app.post("/api/ai/speech-to-text", async (req, res) => {
  try {
    if (!req.body.audio) {
      return res.status(400).json({ error: "Audio data is required" });
    }

    const audioBuffer = Buffer.from(req.body.audio, "base64");
    const transcript = await speechToText(audioBuffer);

    return res.json({ transcript, success: true });
  } catch (error) {
    return res.status(500).json({
      error: error.message,
      success: false,
    });
  }
});

app.post("/api/ai/chat", async (req, res) => {
  try {
    const { prompt } = req.body;
    if (!prompt) {
      return res.status(400).json({ error: "Prompt is required" });
    }

    const response = await generateResponse(prompt);
    return res.json({ response, success: true });
  } catch (error) {
    return res.status(500).json({
      error: error.message,
      success: false,
    });
  }
});

app.post("/api/ai/voice-command", async (req, res) => {
  try {
    if (!req.body.audio) {
      return res.status(400).json({ error: "Audio data is required" });
    }

    const audioBuffer = Buffer.from(req.body.audio, "base64");
    const result = await processVoiceCommand(audioBuffer);

    if (!result.success) {
      return res.status(500).json({
        error: result.error,
        success: false,
      });
    }

    return res.json(result);
  } catch (error) {
    return res.status(500).json({
      error: error.message,
      success: false,
    });
  }
});

// Transcribe audio - similar to LABA5 /api/stt
app.post("/api/stt", async (req, res) => {
  try {
    if (!req.body.audio) {
      return res.status(400).json({ error: "Audio data is required" });
    }

    const audioBuffer = Buffer.from(req.body.audio, "base64");
    const transcript = await speechToText(audioBuffer);

    return res.json({
      transcript: transcript || "",
    });
  } catch (error) {
    console.error("STT error:", error);
    return res.status(500).json({
      error: error.message,
    });
  }
});

// Execute command via chat
app.post("/api/command", async (req, res) => {
  try {
    const prompt = typeof req.body?.prompt === "string" ? req.body.prompt.trim() : "";
    if (!prompt) {
      return res.status(400).json({ error: "Prompt is required" });
    }

    // Load current lookup values and FULL lessons table to provide complete context
    const [subjectsRows, groupsRows, teachersRows, timesRows, roomsRows, daysRows, weeksRows, typesRows, allLessonsRows, totalCountResult] = await Promise.all([
      all("SELECT name FROM subjects ORDER BY name COLLATE NOCASE ASC"),
      all("SELECT name FROM study_groups ORDER BY name COLLATE NOCASE ASC"),
      all("SELECT name FROM teachers ORDER BY name COLLATE NOCASE ASC"),
      all("SELECT DISTINCT lesson_time AS value FROM lessons ORDER BY lesson_time COLLATE NOCASE ASC"),
      all("SELECT DISTINCT room AS value FROM lessons ORDER BY room COLLATE NOCASE ASC"),
      all("SELECT DISTINCT day AS value FROM lessons ORDER BY day COLLATE NOCASE ASC"),
      all("SELECT DISTINCT week AS value FROM lessons ORDER BY week COLLATE NOCASE ASC"),
      all("SELECT DISTINCT lesson_type AS value FROM lessons ORDER BY lesson_type COLLATE NOCASE ASC"),
      all("SELECT id, subject, day, lesson_time AS time, room, group_name, week, teacher, lesson_type FROM lessons ORDER BY day ASC, lesson_time ASC"),
      get("SELECT COUNT(*) AS total FROM lessons"),
    ]);

    const lookupsContext = {
      subjects: sanitizeOptionList(subjectsRows.map((x) => x.name)),
      groups: sanitizeOptionList(groupsRows.map((x) => x.name)),
      teachers: sanitizeOptionList(teachersRows.map((x) => x.name)),
      times: sanitizeOptionList(timesRows.map((x) => x.value)),
      rooms: sanitizeOptionList(roomsRows.map((x) => x.value)),
      days: sanitizeOptionList(daysRows.map((x) => x.value)),
      weeks: sanitizeOptionList(weeksRows.map((x) => x.value)),
      types: sanitizeOptionList(typesRows.map((x) => x.value)),
    };

    const lessonsFull = (allLessonsRows || []).map((r) => ({
      id: r.id,
      subject: r.subject,
      day: r.day,
      time: r.time,
      room: r.room,
      group: r.group_name,
      week: r.week,
      teacher: r.teacher,
      type: r.lesson_type,
    }));

    function mapToLookup(value, options = [], normalizeValue, fieldType = "") {
      const raw = String(value || "").trim();
      if (!raw) return "Невідомо";
      const normalized = normalizeValue
        ? String(normalizeValue(raw) || "").trim()
        : raw;
      const low = normalized.toLowerCase();

      // Use fuzzy matching for specific field types
      if (fieldType === "teacher") {
        const matched = fuzzyMatchTeacher(normalized, options.map(o => String(o).trim()));
        if (matched !== normalized) return matched;
      }
      if (fieldType === "group") {
        const matched = fuzzyMatchGroup(normalized, options.map(o => String(o).trim()));
        if (matched !== normalized) return matched;
      }

      // Exact match
      for (const option of options) {
        if (String(option || "").toLowerCase() === low) return option;
      }

      // Substring match
      for (const option of options) {
        const opt = String(option || "").toLowerCase();
        if (opt.includes(low) || low.includes(opt)) return option;
      }

      // Special case for "Обидва" (Both weeks)
      if (options.includes("Обидва")) {
        if (
          /\b(обид|перш.*друг|1\D*2|1\s+та\s+2|first and second|both)\b/i.test(
            raw,
          )
        ) {
          return "Обидва";
        }
      }

      // Numeric match
      for (const option of options) {
        const opt = String(option || "").trim();
        if (/^\d+$/.test(opt) && raw.includes(opt)) return option;
      }

      return "Невідомо";
    }

    function mapTimeToLookup(value, options = []) {
      const rawStr = String(value || "").toLowerCase().trim();
      
      // Check if user just said a number like "15" → find any time starting with "15:"
      const justHour = rawStr.match(/^(\d{1,2})$/);
      if (justHour) {
        const hour = String(parseInt(justHour[1])).padStart(2, "0");
        const hourTimeMatch = options.find(
          (option) => String(option || "").toLowerCase().startsWith(hour + ":")
        );
        if (hourTimeMatch) return hourTimeMatch;
      }
      
      const normalized = normalizeTime(value);
      if (isValidTimeFormat(normalized)) {
        return normalized;
      }
      const normLower = String(normalized || "").toLowerCase();
      
      // Exact match
      const exact = options.find(
        (option) =>
          String(option || "").toLowerCase() === normLower,
      );
      if (exact) return exact;
      
      // Try substring match
      const substringMatch = options.find(option => {
        const opt = String(option || "").toLowerCase();
        return opt.includes(normLower) || normLower.includes(opt);
      });
      if (substringMatch) return substringMatch;
      
      return mapToLookup(value, options, normalizeTime, "time");
    }

    const commandData = await parseVoiceCommand(prompt, { lookups: lookupsContext, lessons_full: lessonsFull, total_lessons: totalCountResult?.total || 0 });
    console.log("📥 LLM Parsed Command:", JSON.stringify(commandData, null, 2));
    
    const action = commandData.action?.toLowerCase() || "unknown";

    let result = commandData.natural_response;

    if (action === "add_lesson") {
      const p = commandData.parameters || {};
      const missing = commandData.missing_fields || [];
      const allMissing = [...new Set(missing)];

      // load current lookup values from DB so we only save existing options
      const [subjectsRows, groupsRows, teachersRows, timesRows, roomsRows, daysRows, weeksRows, typesRows] = await Promise.all([
        all("SELECT name FROM subjects ORDER BY name COLLATE NOCASE ASC"),
        all("SELECT name FROM study_groups ORDER BY name COLLATE NOCASE ASC"),
        all("SELECT name FROM teachers ORDER BY name COLLATE NOCASE ASC"),
        all("SELECT DISTINCT lesson_time AS value FROM lessons ORDER BY lesson_time COLLATE NOCASE ASC"),
        all("SELECT DISTINCT room AS value FROM lessons ORDER BY room COLLATE NOCASE ASC"),
        all("SELECT DISTINCT day AS value FROM lessons ORDER BY day COLLATE NOCASE ASC"),
        all("SELECT DISTINCT week AS value FROM lessons ORDER BY week COLLATE NOCASE ASC"),
        all("SELECT DISTINCT lesson_type AS value FROM lessons ORDER BY lesson_type COLLATE NOCASE ASC"),
      ]);

      const lookups = {
        subjects: sanitizeOptionList(subjectsRows.map((x) => x.name)),
        groups: sanitizeOptionList(groupsRows.map((x) => x.name)),
        teachers: sanitizeOptionList(teachersRows.map((x) => x.name)),
        times: sanitizeOptionList(timesRows.map((x) => x.value)),
        rooms: sanitizeOptionList(roomsRows.map((x) => x.value)),
        days: sanitizeOptionList(daysRows.map((x) => x.value)),
        weeks: sanitizeOptionList(weeksRows.map((x) => x.value)),
        types: sanitizeOptionList(typesRows.map((x) => x.value)),
      };

      function pickOption(value, options = [], fieldType = "") {
        const v = String(value || "").trim();
        if (!v) return "Невідомо";
        
        // Use fuzzy matching for specific field types
        if (fieldType === "teacher") {
          const matched = fuzzyMatchTeacher(v, options.map(o => String(o).trim()));
          if (matched !== v) return matched;
        }
        if (fieldType === "group") {
          const matched = fuzzyMatchGroup(v, options.map(o => String(o).trim()));
          if (matched !== v) return matched;
        }
        
        const low = v.toLowerCase();

        // exact match
        for (const o of options) {
          if (String(o || "").toLowerCase() === low) return o;
        }
        // substring match (option contains value or value contains option)
        for (const o of options) {
          const ol = String(o || "").toLowerCase();
          if (ol.includes(low) || low.includes(ol)) return o;
        }

        // special heuristics for weeks
        if (options.includes("Обидва")) {
          if (/\b(обид|перш.*друг|1\D*2|1\s+та\s+2|first and second|both)\b/i.test(v)) return "Обидва";
        }

        // try numeric match for weeks ("1", "2")
        for (const o of options) {
          if (/^\d+$/.test(String(o || "")) && low.includes(String(o))) return o;
        }

        return "Невідомо";
      }

      // Prepare groups: try to map each provided group to existing group option
      const rawGroups = p.group ? [p.group] : Array.isArray(p.groups) ? p.groups : splitMultiValues(p.groupsText || "");
      const mappedGroups = (rawGroups || []).map((g) => pickOption(g, lookups.groups, "group")).filter(Boolean);
      const finalGroups = mappedGroups.length ? mappedGroups : ["Невідомо"];

      // map other fields to existing lookup options or "Невідомо"
      const mappedSubject = pickOption(p.subject, lookups.subjects, "subject");
      const mappedDay = pickOption(p.day, lookups.days, "day");
      // Handle time mapping - CRITICAL: Accept any valid HH:MM format
      const mappedTime = (function () {
        // Get raw time value from parsed parameters
        const rawTime = p.time || "";
        if (!rawTime) return "Невідомо";
        
        // Step 1: Normalize the time using all available strategies
        const normalized = normalizeTime(rawTime);
        
        // Step 2: Check if it's now in valid HH:MM format
        if (isValidTimeFormat(normalized)) {
          console.log(`✅ TIME ACCEPTED: "${rawTime}" → "${normalized}" (valid format)`);
          return normalized;
        }
        
        // Step 3: Try to find exact match in database times (for validation consistency)
        const exactMatch = lookups.times.find(
          t => String(t || "").toLowerCase().trim() === String(normalized || "").toLowerCase().trim()
        );
        if (exactMatch) {
          console.log(`✅ TIME ACCEPTED: "${rawTime}" → "${exactMatch}" (found in database)`);
          return exactMatch;
        }
        
        // Step 4: Last resort - if normalized is NOT valid format but rawTime is plainly HH:MM, accept it
        if (isValidTimeFormat(rawTime)) {
          console.log(`✅ TIME ACCEPTED: "${rawTime}" (raw format valid)`);
          return rawTime;
        }
        
        // Step 5: Only if ALL else fails
        console.warn(`⚠️ TIME REJECTED: "${rawTime}" → "${normalized}" (invalid format - no fallback)`);
        return "Невідомо";
      })();
      const mappedRoom = pickOption(p.room, lookups.rooms, "room");
      const mappedWeek = pickOption(p.week, lookups.weeks, "week");
      const mappedTeacher = pickOption(p.teacher, lookups.teachers, "teacher");
      const mappedType = pickOption(normalizeType(p.type) || p.type, lookups.types, "type");

      const lesson = {
        subject: isPlaceholderValue(mappedSubject) ? "Невідомо" : mappedSubject,
        day: isPlaceholderValue(mappedDay) ? "Невідомо" : mappedDay,
        // CRITICAL: For time, NEVER default to "00:00" - keep the valid time or "Невідомо"
        time: isPlaceholderValue(mappedTime) ? "Невідомо" : mappedTime,
        room: isPlaceholderValue(mappedRoom) ? "Невідомо" : mappedRoom,
        week: isPlaceholderValue(mappedWeek) ? "Невідомо" : mappedWeek,
        teacher: isPlaceholderValue(mappedTeacher) ? "Невідомо" : mappedTeacher,
        type: isPlaceholderValue(mappedType) ? "Будь-який" : mappedType,
        groups: finalGroups,
      };

      try {
        // Add one lesson for EACH group
        const addedLessons = [];
        for (const group of finalGroups) {
          await run(
            `INSERT INTO lessons(subject, day, lesson_time, room, group_name, week, teacher, lesson_type)
             VALUES(?, ?, ?, ?, ?, ?, ?, ?)`,
            [
              lesson.subject,
              lesson.day,
              lesson.time,
              lesson.room,
              group,
              lesson.week,
              lesson.teacher,
              lesson.type,
            ]
          );
          addedLessons.push(group);
        }

        const missingNote = allMissing.length
          ? `\n\nSome details were missing, so I used defaults for: ${allMissing.join(
              ", "
            )}.`
          : "";
        const groupsDisplay = addedLessons.length > 1 ? `групи ${addedLessons.join(", ")}` : `група ${addedLessons[0]}`;
        result = `✅ Added lesson: ${lesson.subject} on ${lesson.day} at ${lesson.time} in room ${lesson.room} for ${groupsDisplay}${missingNote}`;
      } catch (err) {
        result = `Error adding lesson: ${err.message}`;
      }
    } else if (action === "edit_lesson") {
      const p = commandData.parameters || {};
      const missing = commandData.missing_fields || [];
      const unknownFields = [];

      // edit_lesson REQUIRES an id
      if (!p.id) {
        result = "Для редагування заняття необхідно вказати його ID або достатньо полів для ідентифікації.";
      } else {
        const lessonId = p.id;
        const updateFields = [];
        const updateValues = [];

        const mappedSubject = mapToLookup(p.subject, lookupsContext.subjects, null, "subject");
        const mappedDay = mapToLookup(p.day, lookupsContext.days, normalizeDay, "day");
        const mappedTime = mapTimeToLookup(p.time, lookupsContext.times);
        const mappedRoom = mapToLookup(p.room, lookupsContext.rooms, null, "room");
        const mappedGroup = mapToLookup(p.group, lookupsContext.groups, null, "group");
        const mappedWeek = mapToLookup(p.week, lookupsContext.weeks, null, "week");
        const mappedTeacher = mapToLookup(p.teacher, lookupsContext.teachers, null, "teacher");
        const mappedType = mapToLookup(p.type, lookupsContext.types, normalizeType, "type");

        // Only allow edits to existing fields with values from lookups
        if (p.subject) {
          if (mappedSubject === "Невідомо") unknownFields.push("subject");
          else {
            updateFields.push("subject = ?");
            updateValues.push(mappedSubject);
          }
        }
        if (p.day) {
          if (mappedDay === "Невідомо") unknownFields.push("day");
          else {
            updateFields.push("day = ?");
            updateValues.push(mappedDay);
          }
        }
        if (p.time) {
          if (mappedTime === "Невідомо") unknownFields.push("time");
          else {
            updateFields.push("lesson_time = ?");
            updateValues.push(mappedTime);
          }
        }
        if (p.room) {
          if (mappedRoom === "Невідомо") unknownFields.push("room");
          else {
            updateFields.push("room = ?");
            updateValues.push(mappedRoom);
          }
        }
        if (p.group) {
          if (mappedGroup === "Невідомо") unknownFields.push("group");
          else {
            updateFields.push("group_name = ?");
            updateValues.push(mappedGroup);
          }
        }
        if (p.week) {
          if (mappedWeek === "Невідомо") unknownFields.push("week");
          else {
            updateFields.push("week = ?");
            updateValues.push(mappedWeek);
          }
        }
        if (p.teacher) {
          if (mappedTeacher === "Невідомо") unknownFields.push("teacher");
          else {
            updateFields.push("teacher = ?");
            updateValues.push(mappedTeacher);
          }
        }
        if (p.type) {
          if (mappedType === "Невідомо") unknownFields.push("type");
          else {
            updateFields.push("lesson_type = ?");
            updateValues.push(mappedType);
          }
        }

        if (updateFields.length === 0) {
          result = "Немає полів для редагування. Вкажіть принаймні одне нове значення з доступних довідників.";
        } else {
          try {
            updateValues.push(lessonId);
            const sql = `UPDATE lessons SET ${updateFields.join(", ")} WHERE id = ?`;
            const updateResult = await run(sql, updateValues);
            
            if (updateResult.changes === 0) {
              result = `Заняття з ID ${lessonId} не знайдено.`;
            } else {
              result = `✅ Заняття з ID ${lessonId} успішно оновлено.`;
              if (missing.length > 0) {
                result += `\n⚠️ Деякі поля не були оновлені (не було вказано або значення невідоме): ${missing.join(", ")}`;
              }
              if (unknownFields.length > 0) {
                result += `\n⚠️ Нерозпізнані значення замінено на "Невідомо" і не застосовано: ${[...new Set(unknownFields)].join(", ")}`;
              }
            }
          } catch (err) {
            result = `Помилка при редагуванні: ${err.message}`;
          }
        }
      }
    } else if (action === "delete_lesson") {
      const p = commandData.parameters || {};
      const deleteAllMatching = Boolean(commandData.delete_all_matching);

      // If id provided, delete directly
      if (p.id) {
        try {
          await run("DELETE FROM lessons WHERE id = ?", [p.id]);
          result = `✅ Видалено заняття з ID ${p.id}`;
        } catch (err) {
          result = `Помилка при видаленні: ${err.message}`;
        }
      } else {
        // Build filters from provided parameters to find matching lessons
        const filters = [];
        const paramsSql = [];
        const map = {
          subject: "subject",
          day: "day",
          time: "lesson_time",
          room: "room",
          group: "group_name",
          teacher: "teacher",
        };

        const mapped = {
          subject: mapToLookup(p.subject, lookupsContext.subjects, null, "subject"),
          day: mapToLookup(p.day, lookupsContext.days, normalizeDay, "day"),
          time: mapTimeToLookup(p.time, lookupsContext.times),
          room: mapToLookup(p.room, lookupsContext.rooms, null, "room"),
          group: mapToLookup(p.group, lookupsContext.groups, null, "group"),
          teacher: mapToLookup(p.teacher, lookupsContext.teachers, null, "teacher"),
        };

        for (const key of Object.keys(map)) {
          const v = mapped[key];
          if (!v || v === "Невідомо") continue;
          // Use case-insensitive LIKE matching for flexibility
          filters.push(`LOWER(${map[key]}) LIKE LOWER(?)`);
          paramsSql.push(`%${v}%`);
        }

        if (filters.length === 0) {
          result = "Потрібен id або принаймні два розпізнані поля (subject, day, time, room, group, teacher), щоб знайти заняття для видалення.";
        } else {
          const where = `WHERE ${filters.join(" AND ")}`;
          const rows = await all(
            `SELECT id, subject, day, lesson_time AS time, room, group_name AS group_name, week, teacher, lesson_type AS type FROM lessons ${where} ORDER BY day, lesson_time LIMIT 50`,
            paramsSql,
          );

          if (!rows || rows.length === 0) {
            result = "Збігів не знайдено.";
          } else if (rows.length === 1) {
            const r = rows[0];
            try {
              await run("DELETE FROM lessons WHERE id = ?", [r.id]);
              result = `✅ Видалено заняття: ${r.subject}, ${r.day} ${r.time}, ауд.: ${r.room}, група: ${r.group_name}`;
            } catch (err) {
              result = `Помилка при видаленні: ${err.message}`;
            }
          } else if (deleteAllMatching) {
            try {
              const ids = rows.map((r) => r.id);
              const placeholders = ids.map(() => "?").join(", ");
              await run(`DELETE FROM lessons WHERE id IN (${placeholders})`, ids);
              result = `✅ Видалено ${rows.length} занять за заданими критеріями: ${rows[0].subject}`;
            } catch (err) {
              result = `Помилка при видаленні кількох занять: ${err.message}`;
            }
          } else {
            // Multiple matches — return a short summary and IDs for user to choose
            const summary = rows
              .map((x) => `ID ${x.id}: ${x.subject} — ${x.day} ${x.time}, ауд.: ${x.room}, гр.: ${x.group_name}`)
              .join("\n");
            result = `Знайдено декілька збігів. Будь ласка, вкажіть ID того заняття, яке потрібно видалити:\n${summary}`;
          }
        }
      }
    } else if (action === "describe_curriculum") {
      // Provide detailed analysis and answers about the curriculum/schedule
      const tableStats = {
        total_lessons: lessonsFull.length,
        subjects: lookupsContext.subjects.length,
        groups: lookupsContext.groups.length,
        teachers: lookupsContext.teachers.length,
        days: lookupsContext.days.length,
        unique_times: lookupsContext.times.length,
        unique_rooms: lookupsContext.rooms.length,
      };

      // Detect conflicts (same time in same room)
      const timeRoomConflicts = {};
      for (const lesson of lessonsFull) {
        const key = `${lesson.day}_${lesson.time}_${lesson.room}`;
        if (!timeRoomConflicts[key]) {
          timeRoomConflicts[key] = [];
        }
        timeRoomConflicts[key].push(lesson);
      }

      const conflicts = Object.entries(timeRoomConflicts)
        .filter(([_, lessons]) => lessons.length > 1)
        .map(([key, lessons]) => {
          const [day, time, room] = key.split("_");
          return `${day} ${time} в ауд. ${room}: ${lessons.map((l) => l.group).join(", ")}`;
        });

      const curriculumResponse = `
📚 АНАЛІЗ РОЗКЛАДУ ЗАНЯТЬ
Загалом: ${tableStats.total_lessons} занять
Предметів: ${tableStats.subjects}
Груп: ${tableStats.groups}
Викладачів: ${tableStats.teachers}
Днів у розписі: ${tableStats.days}
Унікальних часів: ${tableStats.unique_times}
Унікальних аудиторій: ${tableStats.unique_rooms}

${commandData.curriculum_notes || ""}

${conflicts.length > 0 ? `⚠️ МОЖЛИВІ КОНФЛІКТИ (кілька груп в одній аудиторії одночасно):\n${conflicts.map((c) => `  - ${c}`).join("\n")}` : "✅ Конфліктів часу не виявлено."}
`;

      result = curriculumResponse;
    } else if (action === "list_lessons" || action === "view_schedule") {
      try {
        const lessons = await all("SELECT * FROM lessons LIMIT 10");
        result = `Found ${lessons.length} lessons:\n${lessons
          .map(
            (l) =>
              `- ${l.subject} (${l.day} ${l.lesson_time}) in ${l.room} - ${l.group_name}`
          )
          .join("\n")}`;
      } catch (err) {
        result = `Error fetching lessons: ${err.message}`;
      }
    } else if (action === "help") {
      result = `I can help you with:
- Adding lessons: "Add Math on Monday at 9:00 in room 101 for group 1"
- Deleting lessons: "Delete lesson with ID 5"
- Viewing schedule: "Show me all lessons" or "List schedule"
- Adding dropdowns: "Add new subject called Physics"`;
    } else {
      result = commandData.natural_response || "I didn't understand that command.";
    }

    return res.json({
      answer: result,
    });
  } catch (error) {
    console.error("Command error:", error);
    return res.status(500).json({
      error: error.message,
    });
  }
});

initDb()
  .then(async () => {
    await syncAllUsersStatus();
    app.listen(port, () => {
      // eslint-disable-next-line no-console
      console.log(`API server started on http://localhost:${port}`);
    });
  })
  .catch((error) => {
    // eslint-disable-next-line no-console
    console.error("Database initialization failed:", error);
    process.exit(1);
  });
