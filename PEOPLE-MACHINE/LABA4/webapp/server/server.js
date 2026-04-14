import 'dotenv/config';
import bcrypt from 'bcryptjs';
import cors from 'cors';
import express from 'express';
import jwt from 'jsonwebtoken';
import { all, get, initDb, run } from './db.js';

const app = express();
const port = Number(process.env.PORT || 4000);
const jwtSecret = process.env.JWT_SECRET || 'dev-secret-change-me';

app.use(cors());
app.use(express.json());

function createToken(user) {
  return jwt.sign({ userId: user.id, username: user.username }, jwtSecret, { expiresIn: '8h' });
}

function isValidOptionValue(value) {
  const text = String(value || '').trim();
  return (
    Boolean(text) &&
    !text.includes('?') &&
    !text.includes('�') &&
    !/^webtest(updated)?$/i.test(text)
  );
}

function sanitizeOptionList(values = []) {
  return [...new Set(values.map((x) => String(x || '').trim()))].filter(isValidOptionValue);
}

function auth(req, res, next) {
  if (req.path === '/health' || req.path.startsWith('/api/auth/')) {
    return next();
  }

  const header = req.headers.authorization || '';
  const [, token] = header.split(' ');
  if (!token) {
    return res.status(401).json({ error: 'Необхідна авторизація' });
  }

  try {
    const payload = jwt.verify(token, jwtSecret);
    req.user = payload;
    return next();
  } catch {
    return res.status(401).json({ error: 'Недійсний токен' });
  }
}

function validateLesson(payload) {
  const required = ['subject', 'day', 'time', 'room', 'group', 'week', 'teacher', 'type'];
  for (const field of required) {
    if (!String(payload[field] || '').trim()) {
      return `Поле \"${field}\" є обов'язковим`;
    }
  }
  return null;
}

async function upsertLookupsFromLesson(lesson) {
  await run('INSERT OR IGNORE INTO subjects(name) VALUES(?)', [lesson.subject]);
  await run('INSERT OR IGNORE INTO study_groups(name) VALUES(?)', [lesson.group]);
  await run('INSERT OR IGNORE INTO teachers(name) VALUES(?)', [lesson.teacher]);
}

app.use(auth);

app.get('/health', (_req, res) => {
  res.json({ ok: true });
});

app.post('/api/auth/register', async (req, res) => {
  try {
    const username = String(req.body.username || '').trim();
    const password = String(req.body.password || '').trim();

    if (!username || !password) {
      return res.status(400).json({ error: 'Логін і пароль обовʼязкові' });
    }
    if (password.length < 6) {
      return res.status(400).json({ error: 'Пароль має містити щонайменше 6 символів' });
    }

    const passwordHash = await bcrypt.hash(password, 10);
    const result = await run('INSERT INTO users(username, password_hash) VALUES(?, ?)', [
      username,
      passwordHash
    ]);
    const user = { id: result.lastID, username };
    return res.status(201).json({ token: createToken(user), user });
  } catch (error) {
    if (String(error.message).includes('UNIQUE')) {
      return res.status(409).json({ error: 'Користувач з таким логіном вже існує' });
    }
    return res.status(500).json({ error: 'Помилка сервера' });
  }
});

app.post('/api/auth/login', async (req, res) => {
  try {
    const username = String(req.body.username || '').trim();
    const password = String(req.body.password || '').trim();

    const user = await get('SELECT id, username, password_hash FROM users WHERE username = ?', [username]);
    if (!user) {
      return res.status(401).json({ error: 'Невірний логін або пароль' });
    }

    const ok = await bcrypt.compare(password, user.password_hash);
    if (!ok) {
      return res.status(401).json({ error: 'Невірний логін або пароль' });
    }

    return res.json({ token: createToken(user), user: { id: user.id, username: user.username } });
  } catch {
    return res.status(500).json({ error: 'Помилка сервера' });
  }
});

app.get('/api/lookups', async (_req, res) => {
  const [subjects, groups, teachers, times, rooms, days, weeks, types] = await Promise.all([
    all('SELECT name FROM subjects ORDER BY name COLLATE NOCASE ASC'),
    all('SELECT name FROM study_groups ORDER BY name COLLATE NOCASE ASC'),
    all('SELECT name FROM teachers ORDER BY name COLLATE NOCASE ASC'),
    all('SELECT DISTINCT lesson_time AS value FROM lessons ORDER BY lesson_time COLLATE NOCASE ASC'),
    all('SELECT DISTINCT room AS value FROM lessons ORDER BY room COLLATE NOCASE ASC'),
    all('SELECT DISTINCT day AS value FROM lessons ORDER BY day COLLATE NOCASE ASC'),
    all('SELECT DISTINCT week AS value FROM lessons ORDER BY week COLLATE NOCASE ASC'),
    all('SELECT DISTINCT lesson_type AS value FROM lessons ORDER BY lesson_type COLLATE NOCASE ASC')
  ]);

  res.json({
    subjects: sanitizeOptionList(subjects.map((x) => x.name)),
    groups: sanitizeOptionList(groups.map((x) => x.name)),
    teachers: sanitizeOptionList(teachers.map((x) => x.name)),
    times: sanitizeOptionList(times.map((x) => x.value)),
    rooms: sanitizeOptionList(rooms.map((x) => x.value)),
    days: sanitizeOptionList(days.map((x) => x.value)),
    weeks: sanitizeOptionList(weeks.map((x) => x.value)),
    types: sanitizeOptionList(types.map((x) => x.value))
  });
});

app.post('/api/lookups/:entity', async (req, res) => {
  const entity = req.params.entity;
  const name = String(req.body.name || '').trim();

  if (!name) {
    return res.status(400).json({ error: 'Порожнє значення не дозволено' });
  }
  if (!isValidOptionValue(name)) {
    return res.status(400).json({ error: 'Некоректне значення довідника' });
  }

  const map = {
    subjects: 'subjects',
    groups: 'study_groups',
    teachers: 'teachers'
  };

  const table = map[entity];
  if (!table) {
    return res.status(400).json({ error: 'Невідомий тип довідника' });
  }

  await run(`INSERT OR IGNORE INTO ${table}(name) VALUES(?)`, [name]);
  return res.status(201).json({ ok: true });
});

app.get('/api/lessons', async (req, res) => {
  const filters = {
    subject: String(req.query.subject || '').trim(),
    day: String(req.query.day || '').trim(),
    time: String(req.query.time || '').trim(),
    room: String(req.query.room || '').trim(),
    group: String(req.query.group || '').trim(),
    week: String(req.query.week || '').trim(),
    teacher: String(req.query.teacher || '').trim(),
    type: String(req.query.type || '').trim()
  };

  const clauses = [];
  const params = [];
  const map = {
    subject: 'subject',
    day: 'day',
    time: 'lesson_time',
    room: 'room',
    group: 'group_name',
    week: 'week',
    teacher: 'teacher',
    type: 'lesson_type'
  };

  for (const [key, value] of Object.entries(filters)) {
    if (!value) continue;
    clauses.push(`LOWER(${map[key]}) LIKE LOWER(?)`);
    params.push(`%${value}%`);
  }

  const whereSql = clauses.length ? `WHERE ${clauses.join(' AND ')}` : '';
  const rows = await all(
    `SELECT id, subject, day, lesson_time, room, group_name, week, teacher, lesson_type
     FROM lessons
     ${whereSql}
     ORDER BY day ASC, lesson_time ASC`,
    params
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
      type: x.lesson_type
    }))
  );
});

app.post('/api/lessons', async (req, res) => {
  const lesson = {
    subject: String(req.body.subject || '').trim(),
    day: String(req.body.day || '').trim(),
    time: String(req.body.time || '').trim(),
    room: String(req.body.room || '').trim(),
    group: String(req.body.group || '').trim(),
    week: String(req.body.week || '').trim(),
    teacher: String(req.body.teacher || '').trim(),
    type: String(req.body.type || '').trim()
  };

  const validationError = validateLesson(lesson);
  if (validationError) {
    return res.status(400).json({ error: validationError });
  }

  await upsertLookupsFromLesson(lesson);
  const result = await run(
    `INSERT INTO lessons(subject, day, lesson_time, room, group_name, week, teacher, lesson_type)
     VALUES(?, ?, ?, ?, ?, ?, ?, ?)`,
    [
      lesson.subject,
      lesson.day,
      lesson.time,
      lesson.room,
      lesson.group,
      lesson.week,
      lesson.teacher,
      lesson.type
    ]
  );

  return res.status(201).json({ id: result.lastID });
});

app.put('/api/lessons/:id', async (req, res) => {
  const lesson = {
    subject: String(req.body.subject || '').trim(),
    day: String(req.body.day || '').trim(),
    time: String(req.body.time || '').trim(),
    room: String(req.body.room || '').trim(),
    group: String(req.body.group || '').trim(),
    week: String(req.body.week || '').trim(),
    teacher: String(req.body.teacher || '').trim(),
    type: String(req.body.type || '').trim()
  };

  const validationError = validateLesson(lesson);
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
      req.params.id
    ]
  );

  if (result.changes === 0) {
    return res.status(404).json({ error: 'Запис не знайдено' });
  }

  return res.json({ ok: true });
});

app.delete('/api/lessons/:id', async (req, res) => {
  const result = await run('DELETE FROM lessons WHERE id = ?', [req.params.id]);
  if (result.changes === 0) {
    return res.status(404).json({ error: 'Запис не знайдено' });
  }
  return res.json({ ok: true });
});

initDb()
  .then(() => {
    app.listen(port, () => {
      // eslint-disable-next-line no-console
      console.log(`API server started on http://localhost:${port}`);
    });
  })
  .catch((error) => {
    // eslint-disable-next-line no-console
    console.error('Database initialization failed:', error);
    process.exit(1);
  });
