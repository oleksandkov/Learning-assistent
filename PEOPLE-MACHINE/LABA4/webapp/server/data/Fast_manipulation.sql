-- =============================================================
-- FAST MANIPULATION (SQLite)
-- DB file: server/data/schedule.db
-- =============================================================

username-- 0) Show all tables
SELECT name
FROM sqlite_master
WHERE type = 'table'
ORDER BY name;

-- 1) Users: view all
SELECT id, username, created_at
FROM users
ORDER BY id;

-- 2) Users: delete ONE user by id (recommended)
-- !!! Replace 1 with real user id
-- DELETE FROM users
-- WHERE id = 4 OR id = 3;

-- 3) Users: delete ONE user by username
-- !!! Replace 'username_here' with real username
-- DELETE FROM users
-- WHERE username = 'username_here';

-- 4) Users: delete ALL users (danger)
-- DELETE FROM users;


-- ====Tables=========================================================
-- DELETE TablesANY ROW FROM ANY TABLE (templates)
-- =============================================================

-- Lessons: view
SELECT id, subject, day, lesson_time, room, group_name, week, teacher, lesson_type
FROM lessons
ORDER BY id;

-- Lessons: delete one row by id
-- !!! Replace 1 with real lesson idTables
-- DELETE FROM lessons
-- WHERE id = 1;


-- Subjects: view
SELECT id, name
FROM subjects
ORDER BY id;

-- Subjects: delete one row by id
-- !!! Replace 1 with real subject id
-- DELETE FROM subjects
-- WHERE id = 1;


-- Study groups: view
SELECT id, name
FROM study_groups
ORDER BY id;

-- Study groups: delete one row by id
-- !!! Replace 1 with real group id
-- DELETE FROM study_groups
-- WHERE id = 1;


-- Teachers: view
SELECT id, name
FROM teachers
ORDER BY id;

-- Teachers: delete one row by id
-- !!! Replace 1 with real teacher id
-- DELETE FROM teachers
-- WHERE id = 1;


-- =============================================================
-- SAFE WORKFLOW (manual):
-- 1) Run SELECT first
-- 2) Run matching DELETE
-- 3) Run SELECT again to verify
-- =============================================================