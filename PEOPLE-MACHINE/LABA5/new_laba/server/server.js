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

// Map Ukrainian day names to English
function normalizeDay(day) {
  if (!day) return day;
  const ukrainianToDays = {
    понеділок: "Monday",
    пн: "Monday",
    вівторок: "Tuesday",
    вт: "Tuesday",
    середа: "Wednesday",
    ср: "Wednesday",
    чотири: "Thursday",
    чт: "Thursday",
    "п'ятниця": "Friday",
    пт: "Friday",
    субота: "Saturday",
    сб: "Saturday",
    неділя: "Sunday",
    нд: "Sunday",
  };
  const lowerDay = String(day).toLowerCase();
  return ukrainianToDays[lowerDay] || day;
}

// Map Ukrainian type names to English
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
      "0"
    )}`;
  }

  return timeStr;
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
      time: x.lesson_time,
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
    time: String(req.body.time || "").trim(),
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
    time: String(req.body.time || "").trim(),
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

    const commandData = await parseVoiceCommand(prompt);
    console.log("Parsed command:", JSON.stringify(commandData));
    
    const action = commandData.action?.toLowerCase() || "unknown";

    let result = commandData.natural_response;

    if (action === "add_lesson") {
      const p = commandData.parameters || {};
      const missing = commandData.missing_fields || [];

      // Check for required fields
      const requiredFields = ["subject", "day", "time", "room", "teacher"];
      const actualMissing = requiredFields.filter(
        (field) => !p[field] || (field === "room" && !p.room)
      );

      if (actualMissing.length > 0 || missing.length > 0) {
        const allMissing = [...new Set([...actualMissing, ...missing])];
        return res.json({
          answer: `To add a lesson, I need more information:\n${allMissing
            .map((f) => `• ${f}`)
            .join(
              "\n"
            )}\n\nPlease provide these details and I'll add the lesson.`,
          status: "incomplete",
        });
      }

      const groups = p.group ? [p.group] : p.groups || [];
      if (!groups.length) {
        return res.json({
          answer: "I need to know which group(s) this lesson is for.",
          status: "incomplete",
        });
      }

      try {
        const lesson = {
          subject: p.subject,
          day: normalizeDay(p.day),
          time: normalizeTime(p.time),
          room: p.room,
          week: p.week || "1",
          teacher: p.teacher,
          type: normalizeType(p.type) || "Лекція",
          groups,
        };

        await run(
          `INSERT INTO lessons(subject, day, lesson_time, room, group_name, week, teacher, lesson_type)
           VALUES(?, ?, ?, ?, ?, ?, ?, ?)`,
          [
            lesson.subject,
            lesson.day,
            lesson.time,
            lesson.room,
            lesson.groups.join(", "),
            lesson.week,
            lesson.teacher,
            lesson.type,
          ]
        );

        result = `✅ Added lesson: ${lesson.subject} on ${lesson.day} at ${lesson.time} in room ${lesson.room}`;
      } catch (err) {
        result = `Error adding lesson: ${err.message}`;
      }
    } else if (action === "delete_lesson") {
      const p = commandData.parameters || {};
      if (p.id) {
        try {
          await run("DELETE FROM lessons WHERE id = ?", [p.id]);
          result = `✅ Deleted lesson with ID ${p.id}`;
        } catch (err) {
          result = `Error deleting lesson: ${err.message}`;
        }
      } else {
        result = "I need the lesson ID to delete it.";
      }
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
