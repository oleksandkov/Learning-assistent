import fs from "node:fs";
import path from "node:path";
import sqlite3 from "sqlite3";

const dataDir = path.resolve("server", "data");
const dbPath = path.join(dataDir, "schedule.db");

if (!fs.existsSync(dataDir)) {
  fs.mkdirSync(dataDir, { recursive: true });
}

const sqlite = sqlite3.verbose();
const db = new sqlite.Database(dbPath);

export function run(sql, params = []) {
  return new Promise((resolve, reject) => {
    db.run(sql, params, function onRun(err) {
      if (err) return reject(err);
      return resolve(this);
    });
  });
}

export function get(sql, params = []) {
  return new Promise((resolve, reject) => {
    db.get(sql, params, (err, row) => {
      if (err) return reject(err);
      return resolve(row);
    });
  });
}

export function all(sql, params = []) {
  return new Promise((resolve, reject) => {
    db.all(sql, params, (err, rows) => {
      if (err) return reject(err);
      return resolve(rows);
    });
  });
}

async function ensureUsersStatusColumn() {
  const columns = await all("PRAGMA table_info(users)");
  const hasStatusColumn = columns.some((column) => column.name === "status");

  if (!hasStatusColumn) {
    await run(
      "ALTER TABLE users ADD COLUMN status TEXT NOT NULL DEFAULT 'user'",
    );
  }

  await run(
    "UPDATE users SET status = 'user' WHERE status IS NULL OR TRIM(status) = ''",
  );
}

async function seedLookups() {
  const subjects = [
    "Вища Математика",
    "ООП",
    "Фізика",
    "Людино-машинна взаємодія",
    "АВ ПЗ",
  ];
  const groups = ["2ПІ-25Б", "3ПІ-25Б", "1ПІ-25Б"];
  const teachers = [
    "Іваненко І.І.",
    "Петренко О.В.",
    "Коваль М.С.",
    "Ткаченко О.М.",
  ];

  for (const name of subjects) {
    await run("INSERT OR IGNORE INTO subjects(name) VALUES(?)", [name]);
  }
  for (const name of groups) {
    await run("INSERT OR IGNORE INTO study_groups(name) VALUES(?)", [name]);
  }
  for (const name of teachers) {
    await run("INSERT OR IGNORE INTO teachers(name) VALUES(?)", [name]);
  }
}

async function seedLessons() {
  const row = await get("SELECT COUNT(*) AS cnt FROM lessons");
  if ((row?.cnt ?? 0) > 0) {
    return;
  }

  const seed = [
    [
      "Вища Математика",
      "Пн",
      "08:30",
      "201",
      "2ПІ-25Б",
      "1",
      "Іваненко І.І.",
      "Лекція",
    ],
    [
      "ООП",
      "Вт",
      "10:15",
      "302",
      "2ПІ-25Б",
      "Обидва",
      "Петренко О.В.",
      "Практика",
    ],
    [
      "Фізика",
      "Ср",
      "13:45",
      "105",
      "3ПІ-25Б",
      "2",
      "Коваль М.С.",
      "Лабораторна",
    ],
    ["АВ ПЗ", "Чт", "08:30", "210", "1ПІ-25Б", "1", "Іваненко І.І.", "Лекція"],
    [
      "Людино-машинна взаємодія",
      "Ср",
      "14:00",
      "2320",
      "2ПІ-25Б",
      "1",
      "Ткаченко О.М.",
      "Лекція",
    ],
  ];

  for (const lesson of seed) {
    await run(
      `INSERT INTO lessons(subject, day, lesson_time, room, group_name, week, teacher, lesson_type)
       VALUES(?, ?, ?, ?, ?, ?, ?, ?)`,
      lesson,
    );
  }
}

export async function initDb() {
  await run(`
    CREATE TABLE IF NOT EXISTS users (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      username TEXT NOT NULL UNIQUE,
      password_hash TEXT NOT NULL,
      status TEXT NOT NULL DEFAULT 'user',
      created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
    )
  `);

  await ensureUsersStatusColumn();

  await run(`
    CREATE TABLE IF NOT EXISTS subjects (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL UNIQUE
    )
  `);

  await run(`
    CREATE TABLE IF NOT EXISTS study_groups (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL UNIQUE
    )
  `);

  await run(`
    CREATE TABLE IF NOT EXISTS teachers (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      name TEXT NOT NULL UNIQUE
    )
  `);

  await run(`
    CREATE TABLE IF NOT EXISTS lessons (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      subject TEXT NOT NULL,
      day TEXT NOT NULL,
      lesson_time TEXT NOT NULL,
      room TEXT NOT NULL,
      group_name TEXT NOT NULL,
      week TEXT NOT NULL,
      teacher TEXT NOT NULL,
      lesson_type TEXT NOT NULL,
      created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
    )
  `);

  await seedLookups();
  await seedLessons();
}
