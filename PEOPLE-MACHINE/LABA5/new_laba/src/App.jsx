import { useEffect, useMemo, useState } from "react";
import { SpeechCommander } from "./SpeechCommander";

const DAYS = ["Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Нд"];
const WEEKS = ["1", "2", "Обидва"];
const TYPES = ["Лекція", "Практика", "Лабораторна"];

function buildTimeOptions(startHour = 8, endHour = 18, intervalMinutes = 15) {
  const result = [];
  for (let hour = startHour; hour <= endHour; hour += 1) {
    for (let minute = 0; minute < 60; minute += intervalMinutes) {
      if (hour === endHour && minute > 0) break;
      const hh = String(hour).padStart(2, "0");
      const mm = String(minute).padStart(2, "0");
      result.push(`${hh}:${mm}`);
    }
  }
  return result;
}

const TIMES = buildTimeOptions(8, 18, 15);

function isValidOptionValue(value) {
  const text = String(value || "").trim();
  return (
    Boolean(text) &&
    !text.includes("?") &&
    !text.includes("�") &&
    !/^webtest(updated)?$/i.test(text)
  );
}

function normalizeOptions(values = []) {
  const unique = [...new Set(values.map((x) => String(x || "").trim()))];
  return unique.filter(isValidOptionValue);
}

function appendUniqueOption(list = [], value = "") {
  const nextValue = String(value || "").trim();
  if (!nextValue) return normalizeOptions(list);
  return normalizeOptions([...list, nextValue]);
}

function lessonGroupKey(lesson) {
  return [
    lesson.subject,
    lesson.day,
    lesson.time,
    lesson.room,
    lesson.week,
    lesson.teacher,
    lesson.type,
  ]
    .map((x) =>
      String(x || "")
        .trim()
        .toLowerCase(),
    )
    .join("||");
}

function api(path, options = {}, token = "") {
  const headers = {
    "Content-Type": "application/json",
    ...(options.headers || {}),
  };
  if (token) headers.Authorization = `Bearer ${token}`;
  return fetch(path, { ...options, headers }).then(async (res) => {
    const contentType = res.headers.get('content-type');
    let payload;
    
    if (contentType?.includes('application/json')) {
      payload = await res.json().catch(() => ({}));
    } else {
      const text = await res.text();
      payload = { error: text || 'Invalid response' };
    }
    
    if (!res.ok) {
      throw new Error(payload.error || `Request failed: ${res.status}`);
    }
    return payload;
  });
}

function EmptyForm() {
  return {
    subject: "",
    day: "",
    time: "",
    room: "",
    group: "",
    groups: [],
    groupInput: "",
    week: "",
    teacher: "",
    type: "",
  };
}

function LessonModal({ open, initial, lookups, onClose, onSave }) {
  const [form, setForm] = useState(initial || EmptyForm());
  const [error, setError] = useState("");
  const isEdit = Boolean(initial?.id);

  useEffect(() => {
    if (initial) {
      setForm({
        ...EmptyForm(),
        ...initial,
        groups: normalizeOptions(
          initial.groups?.length ? initial.groups : [initial.group],
        ),
      });
    } else {
      setForm(EmptyForm());
    }
    setError("");
  }, [initial, open]);

  if (!open) return null;

  const set = (field, value) =>
    setForm((prev) => ({ ...prev, [field]: value }));

  const addCreateGroup = () => {
    setForm((prev) => ({
      ...prev,
      groups: appendUniqueOption(prev.groups, prev.groupInput),
      groupInput: "",
    }));
  };

  const onCreateGroupKeyDown = (event) => {
    if (event.key !== "Enter") return;
    event.preventDefault();
    addCreateGroup();
  };

  const removeCreateGroup = (value) => {
    setForm((prev) => ({
      ...prev,
      groups: (prev.groups || []).filter((x) => x !== value),
    }));
  };

  const submit = (e) => {
    e.preventDefault();
    const required = [
      "subject",
      "day",
      "time",
      "room",
      "week",
      "teacher",
      "type",
    ];
    for (const key of required) {
      if (!String(form[key] || "").trim()) {
        setError("Заповніть усі обовʼязкові поля.");
        return;
      }
    }

    const mergedGroups = normalizeOptions([
      ...(form.groups || []),
      form.groupInput,
    ]);

    if (!mergedGroups.length) {
      setError("Додайте щонайменше одну групу.");
      return;
    }

    onSave({
      ...form,
      group: mergedGroups[0],
      groups: mergedGroups,
    });
  };

  return (
    <div className="overlay" role="presentation">
      <form className="modal" onSubmit={submit}>
        <h3>{initial?.id ? "Редагувати заняття" : "Додати заняття"}</h3>
        {error ? <p className="error">{error}</p> : null}

        <label>
          Предмет
          <input
            value={form.subject}
            onChange={(e) => set("subject", e.target.value)}
            list="subjects"
          />
          <datalist id="subjects">
            {lookups.subjects.map((x) => (
              <option key={x} value={x} />
            ))}
          </datalist>
        </label>

        <div className="grid2">
          <label>
            День
            <input
              value={form.day}
              onChange={(e) => set("day", e.target.value)}
              list="days"
            />
            <datalist id="days">
              {lookups.days.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>

          <label>
            Час
            <input
              value={form.time}
              onChange={(e) => set("time", e.target.value)}
              list="times"
            />
            <datalist id="times">
              {lookups.times.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
        </div>

        <label>
          Аудиторія
          <input
            value={form.room}
            onChange={(e) => set("room", e.target.value)}
          />
        </label>

        <div>
          <label>
            Група
            <input
              value={form.groupInput || ""}
              onChange={(e) => set("groupInput", e.target.value)}
              onKeyDown={onCreateGroupKeyDown}
              list="groupsCreate"
              placeholder="Введіть групу і натисніть Додати"
            />
            <datalist id="groupsCreate">
              {lookups.groups.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
          <div className="actions">
            <button
              type="button"
              className="secondary"
              onClick={addCreateGroup}
            >
              + Додати групу
            </button>
          </div>
          {form.groups?.length ? (
            <div className="chipList">
              {form.groups.map((x) => (
                <span key={x} className="chip">
                  {x}
                  <button
                    type="button"
                    onClick={() => removeCreateGroup(x)}
                    aria-label={`Видалити групу ${x}`}
                  >
                    ×
                  </button>
                </span>
              ))}
            </div>
          ) : null}
          <small className="mutedSmall"></small>
        </div>

        <div className="grid2">
          <label>
            Тиждень
            <input
              value={form.week}
              onChange={(e) => set("week", e.target.value)}
              list="weeks"
            />
            <datalist id="weeks">
              {lookups.weeks.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>

          <label>
            Викладач
            <input
              value={form.teacher}
              onChange={(e) => set("teacher", e.target.value)}
              list="teachers"
            />
            <datalist id="teachers">
              {lookups.teachers.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
        </div>

        <label>
          Тип
          <select
            value={form.type}
            onChange={(e) => set("type", e.target.value)}
          >
            <option value="">Оберіть</option>
            {TYPES.map((x) => (
              <option key={x} value={x}>
                {x}
              </option>
            ))}
          </select>
        </label>

        <div className="actions right">
          <button type="button" className="secondary" onClick={onClose}>
            Скасувати
          </button>
          <button type="submit">Зберегти</button>
        </div>
      </form>
    </div>
  );
}

function App() {
  const [token, setToken] = useState(
    () => localStorage.getItem("authToken") || "",
  );
  const [user, setUser] = useState(
    () => localStorage.getItem("authUser") || "",
  );
  const [isAdmin, setIsAdmin] = useState(
    () => localStorage.getItem("authIsAdmin") === "1",
  );
  const [authMode, setAuthMode] = useState("login");
  const [authForm, setAuthForm] = useState({ username: "", password: "" });
  const [authError, setAuthError] = useState("");

  const [lookups, setLookups] = useState({
    subjects: [],
    groups: [],
    teachers: [],
    times: TIMES,
    rooms: [],
    days: DAYS,
    weeks: WEEKS,
    types: TYPES,
  });
  const [lessons, setLessons] = useState([]);
  const [filters, setFilters] = useState(EmptyForm());
  const [appliedFilters, setAppliedFilters] = useState(EmptyForm());
  const [status, setStatus] = useState("");
  const [selectedId, setSelectedId] = useState(null);
  const [modalOpen, setModalOpen] = useState(false);
  const [editData, setEditData] = useState(null);

  const groupedLessons = useMemo(() => {
    const map = new Map();
    for (const row of lessons) {
      const key = lessonGroupKey(row);
      if (!map.has(key)) {
        map.set(key, {
          ...row,
          ids: [row.id],
          groups: normalizeOptions([row.group]),
        });
        continue;
      }

      const current = map.get(key);
      current.ids.push(row.id);
      current.groups = normalizeOptions([...(current.groups || []), row.group]);
      current.group = current.groups.join(", ");
      map.set(key, current);
    }

    return [...map.values()];
  }, [lessons]);

  const activeGroupFilters = useMemo(
    () => normalizeOptions(appliedFilters.groups || []),
    [appliedFilters.groups],
  );

  const filteredGroupedLessons = useMemo(() => {
    if (!activeGroupFilters.length) return groupedLessons;

    return groupedLessons.filter((lesson) => {
      const lessonGroups = normalizeOptions(lesson.groups || [lesson.group]);

      if (activeGroupFilters.length === 1) {
        return lessonGroups.includes(activeGroupFilters[0]);
      }

      if (lessonGroups.length !== activeGroupFilters.length) {
        return false;
      }

      return activeGroupFilters.every((group) => lessonGroups.includes(group));
    });
  }, [groupedLessons, activeGroupFilters]);

  const selectedLesson = useMemo(
    () => filteredGroupedLessons.find((x) => x.id === selectedId) || null,
    [filteredGroupedLessons, selectedId],
  );

  const authSubmit = async (e) => {
    e.preventDefault();
    setAuthError("");
    try {
      const data = await api(`/api/auth/${authMode}`, {
        method: "POST",
        body: JSON.stringify(authForm),
      });
      localStorage.setItem("authToken", data.token);
      localStorage.setItem("authUser", data.user.username);
      localStorage.setItem("authIsAdmin", data.user.isAdmin ? "1" : "0");
      setToken(data.token);
      setUser(data.user.username);
      setIsAdmin(Boolean(data.user.isAdmin));
      setAuthForm({ username: "", password: "" });
    } catch (error) {
      setAuthError(error.message);
    }
  };

  const loadLookups = async () => {
    const data = await api("/api/lookups", {}, token);
    setLookups({
      subjects: normalizeOptions(data.subjects),
      groups: normalizeOptions(data.groups),
      teachers: normalizeOptions(data.teachers),
      times: normalizeOptions([...(data.times || []), ...TIMES]),
      rooms: normalizeOptions(data.rooms),
      days: normalizeOptions(data.days?.length ? data.days : DAYS),
      weeks: normalizeOptions(data.weeks?.length ? data.weeks : WEEKS),
      types: normalizeOptions(data.types?.length ? data.types : TYPES),
    });
  };

  const loadLessons = async (customFilters = appliedFilters) => {
    const params = new URLSearchParams();
    const queryFilters = {
      subject: customFilters.subject,
      day: customFilters.day,
      time: customFilters.time,
      room: customFilters.room,
      week: customFilters.week,
      teacher: customFilters.teacher,
      type: customFilters.type,
    };
    Object.entries(queryFilters).forEach(([key, value]) => {
      if (value) params.set(key, value);
    });
    const query = params.toString();
    const data = await api(
      `/api/lessons${query ? `?${query}` : ""}`,
      {},
      token,
    );

    setAppliedFilters({
      ...EmptyForm(),
      ...customFilters,
      groups: normalizeOptions(customFilters.groups || []),
    });
    setLessons(data);
    setSelectedId((prev) => (data.some((x) => x.id === prev) ? prev : null));
  };

  useEffect(() => {
    setStatus(`Показано: ${filteredGroupedLessons.length}`);
  }, [filteredGroupedLessons]);

  useEffect(() => {
    if (!token) return;
    Promise.all([loadLookups(), loadLessons()]).catch((error) => {
      setStatus(`Помилка: ${error.message}`);
    });
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [token]);

  useEffect(() => {
    if (!token) return;

    api("/api/auth/me", {}, token)
      .then((data) => {
        localStorage.setItem("authUser", data.user.username);
        localStorage.setItem("authIsAdmin", data.user.isAdmin ? "1" : "0");
        setUser(data.user.username);
        setIsAdmin(Boolean(data.user.isAdmin));
      })
      .catch(() => {
        localStorage.removeItem("authToken");
        localStorage.removeItem("authUser");
        localStorage.removeItem("authIsAdmin");
        setToken("");
        setUser("");
        setIsAdmin(false);
        setLessons([]);
      });
  }, [token]);

  const logout = () => {
    localStorage.removeItem("authToken");
    localStorage.removeItem("authUser");
    localStorage.removeItem("authIsAdmin");
    setToken("");
    setUser("");
    setIsAdmin(false);
    setLessons([]);
  };

  const addLookup = async (entity, title) => {
    const name = window.prompt(`${title}:`);
    if (!name?.trim()) return;
    await api(
      `/api/lookups/${entity}`,
      {
        method: "POST",
        body: JSON.stringify({ name: name.trim() }),
      },
      token,
    );
    await loadLookups();
  };

  const saveLesson = async (form) => {
    const payload = {
      subject: String(form.subject || "").trim(),
      day: String(form.day || "").trim(),
      time: String(form.time || "").trim(),
      room: String(form.room || "").trim(),
      group: String(form.group || "").trim(),
      groups: normalizeOptions(form.groups || []),
      week: String(form.week || "").trim(),
      teacher: String(form.teacher || "").trim(),
      type: String(form.type || "").trim(),
    };

    if (editData?.id) {
      const idsToReplace = editData.ids?.length ? editData.ids : [editData.id];
      await Promise.all(
        idsToReplace.map((id) =>
          api(`/api/lessons/${id}`, { method: "DELETE" }, token),
        ),
      );
      await api(
        "/api/lessons",
        {
          method: "POST",
          body: JSON.stringify(payload),
        },
        token,
      );
    } else {
      await api(
        "/api/lessons",
        {
          method: "POST",
          body: JSON.stringify(payload),
        },
        token,
      );
    }
    setModalOpen(false);
    setEditData(null);
    await Promise.all([loadLookups(), loadLessons()]);
  };

  const removeLesson = async () => {
    if (!selectedLesson) return;
    if (!window.confirm("Видалити вибраний рядок?")) return;
    const idsToDelete = selectedLesson.ids?.length
      ? selectedLesson.ids
      : [selectedLesson.id];
    await Promise.all(
      idsToDelete.map((id) =>
        api(`/api/lessons/${id}`, { method: "DELETE" }, token),
      ),
    );
    await loadLessons();
  };

  const addFilterGroup = () => {
    setFilters((prev) => ({
      ...prev,
      groups: appendUniqueOption(prev.groups, prev.groupInput),
      groupInput: "",
    }));
  };

  const onFilterGroupKeyDown = (event) => {
    if (event.key !== "Enter") return;
    event.preventDefault();
    addFilterGroup();
  };

  const removeFilterGroup = (value) => {
    setFilters((prev) => ({
      ...prev,
      groups: (prev.groups || []).filter((x) => x !== value),
    }));
  };

  if (!token) {
    return (
      <main className="authWrap">
        <form className="authCard" onSubmit={authSubmit}>
          <h1>Веб-розклад занять</h1>
          <p className="muted">Авторизація користувача</p>

          <div className="segmented">
            <button
              type="button"
              className={authMode === "login" ? "active" : ""}
              onClick={() => setAuthMode("login")}
            >
              Вхід
            </button>
            <button
              type="button"
              className={authMode === "register" ? "active" : ""}
              onClick={() => setAuthMode("register")}
            >
              Реєстрація
            </button>
          </div>

          {authError ? <p className="error">{authError}</p> : null}

          <label>
            Логін
            <input
              value={authForm.username}
              onChange={(e) =>
                setAuthForm((prev) => ({ ...prev, username: e.target.value }))
              }
            />
          </label>
          <label>
            Пароль
            <input
              type="password"
              value={authForm.password}
              onChange={(e) =>
                setAuthForm((prev) => ({ ...prev, password: e.target.value }))
              }
            />
          </label>

          <button type="submit">
            {authMode === "login" ? "Увійти" : "Зареєструватися"}
          </button>
        </form>
      </main>
    );
  }

  return (
    <main className="app">
      <header>
        <h1>Розклад занять</h1>
        <div className="actions">
          <span className="badge">Користувач: {user}</span>
          <span className="badge">
            Роль: {isAdmin ? "Адміністратор" : "Лише перегляд"}
          </span>
          <button className="secondary" onClick={logout}>
            Вийти
          </button>
        </div>
      </header>

      <section className="layout">
        <aside className="panel">
          <h3>Пошук / Фільтр</h3>
          <label>
            Предмет
            <input
              value={filters.subject}
              onChange={(e) =>
                setFilters((prev) => ({ ...prev, subject: e.target.value }))
              }
              list="subjectFilter"
            />
            <datalist id="subjectFilter">
              {lookups.subjects.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
          <label>
            День
            <input
              value={filters.day}
              onChange={(e) =>
                setFilters((prev) => ({ ...prev, day: e.target.value }))
              }
              list="dayFilter"
            />
            <datalist id="dayFilter">
              {lookups.days.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
          <label>
            Час
            <input
              value={filters.time}
              onChange={(e) =>
                setFilters((prev) => ({ ...prev, time: e.target.value }))
              }
              list="timeFilter"
            />
            <datalist id="timeFilter">
              {lookups.times.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
          <label>
            Аудиторія
            <input
              value={filters.room}
              onChange={(e) =>
                setFilters((prev) => ({ ...prev, room: e.target.value }))
              }
              placeholder="Введіть аудиторію"
            />
          </label>
          <label>
            Група
            <input
              value={filters.groupInput || ""}
              onChange={(e) =>
                setFilters((prev) => ({ ...prev, groupInput: e.target.value }))
              }
              onKeyDown={onFilterGroupKeyDown}
              list="groupFilter"
              placeholder="Введіть групу і натисніть Додати"
            />
            <datalist id="groupFilter">
              {lookups.groups.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
          <div className="actions">
            <button
              type="button"
              className="secondary"
              onClick={addFilterGroup}
            >
              + Додати групу у фільтр
            </button>
          </div>
          {filters.groups?.length ? (
            <div className="chipList">
              {filters.groups.map((x) => (
                <span key={x} className="chip">
                  {x}
                  <button
                    type="button"
                    onClick={() => removeFilterGroup(x)}
                    aria-label={`Видалити групу фільтра ${x}`}
                  >
                    ×
                  </button>
                </span>
              ))}
            </div>
          ) : null}
          <label>
            <small className="mutedSmall"></small>
          </label>
          <label>
            Тиждень
            <input
              value={filters.week}
              onChange={(e) =>
                setFilters((prev) => ({ ...prev, week: e.target.value }))
              }
              list="weekFilter"
            />
            <datalist id="weekFilter">
              {lookups.weeks.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
          <label>
            Викладач
            <input
              value={filters.teacher}
              onChange={(e) =>
                setFilters((prev) => ({ ...prev, teacher: e.target.value }))
              }
              list="teacherFilter"
            />
            <datalist id="teacherFilter">
              {lookups.teachers.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>

          <div className="actions stacked">
            <button onClick={() => loadLessons(filters)}>Знайти</button>
            <button
              className="secondary"
              onClick={() => {
                const empty = EmptyForm();
                setFilters(empty);
                loadLessons(empty);
              }}
            >
              Скинути
            </button>
          </div>

          {isAdmin ? (
            <>
              <h4>Довідники</h4>
              <div className="actions stacked">
                <button
                  className="secondary"
                  onClick={() => addLookup("subjects", "Новий предмет")}
                >
                  + Предмет
                </button>
                <button
                  className="secondary"
                  onClick={() => addLookup("groups", "Нова група")}
                >
                  + Група
                </button>
                <button
                  className="secondary"
                  onClick={() => addLookup("teachers", "Новий викладач")}
                >
                  + Викладач
                </button>
              </div>
            </>
          ) : null}
        </aside>

        <section className="tablePanel">
          <SpeechCommander 
            token={token} 
            onCommandExecuted={() => {
              loadLessons();
              loadLookups();
            }}
          />

          {isAdmin ? (
            <div className="actions">
              <button
                onClick={() => {
                  setEditData(null);
                  setModalOpen(true);
                }}
              >
                + Заняття
              </button>
              <button
                className="secondary"
                disabled={!selectedLesson}
                onClick={() => {
                  setEditData(selectedLesson);
                  setModalOpen(true);
                }}
              >
                Редагувати
              </button>
              <button
                className="danger"
                disabled={!selectedLesson}
                onClick={removeLesson}
              >
                Видалити
              </button>
            </div>
          ) : null}

          <div className="tableWrap">
            <table>
              <thead>
                <tr>
                  <th>Предмет</th>
                  <th>День</th>
                  <th>Час</th>
                  <th>Ауд.</th>
                  <th>Група</th>
                  <th>Тиждень</th>
                  <th>Викладач</th>
                  <th>Тип</th>
                </tr>
              </thead>
              <tbody>
                {filteredGroupedLessons.map((row) => (
                  <tr
                    key={row.id}
                    onClick={() => setSelectedId(row.id)}
                    onDoubleClick={() => {
                      if (!isAdmin) return;
                      setEditData(row);
                      setModalOpen(true);
                    }}
                    className={row.id === selectedId ? "selected" : ""}
                  >
                    <td>{row.subject}</td>
                    <td>{row.day}</td>
                    <td>{row.time}</td>
                    <td>{row.room}</td>
                    <td>{row.group}</td>
                    <td>{row.week}</td>
                    <td>{row.teacher}</td>
                    <td>
                      <span className={`pill ${row.type}`}>{row.type}</span>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
          <p className="status">{status}</p>
        </section>
      </section>

      <LessonModal
        open={modalOpen}
        initial={editData}
        lookups={lookups}
        onClose={() => {
          setModalOpen(false);
          setEditData(null);
        }}
        onSave={saveLesson}
      />
    </main>
  );
}

export default App;
