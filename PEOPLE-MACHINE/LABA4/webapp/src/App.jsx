import { useEffect, useMemo, useState } from 'react';

const DAYS = ['Пн', 'Вт', 'Ср', 'Чт', 'Пт', 'Сб', 'Нд'];
const WEEKS = ['1', '2', 'Обидва'];
const TYPES = ['Лекція', 'Практика', 'Лабораторна'];

function buildTimeOptions(startHour = 8, endHour = 18, intervalMinutes = 15) {
  const result = [];
  for (let hour = startHour; hour <= endHour; hour += 1) {
    for (let minute = 0; minute < 60; minute += intervalMinutes) {
      if (hour === endHour && minute > 0) break;
      const hh = String(hour).padStart(2, '0');
      const mm = String(minute).padStart(2, '0');
      result.push(`${hh}:${mm}`);
    }
  }
  return result;
}

const TIMES = buildTimeOptions(8, 18, 15);

function isValidOptionValue(value) {
  const text = String(value || '').trim();
  return (
    Boolean(text) &&
    !text.includes('?') &&
    !text.includes('�') &&
    !/^webtest(updated)?$/i.test(text)
  );
}

function normalizeOptions(values = []) {
  const unique = [...new Set(values.map((x) => String(x || '').trim()))];
  return unique.filter(isValidOptionValue);
}

function api(path, options = {}, token = '') {
  const headers = {
    'Content-Type': 'application/json',
    ...(options.headers || {})
  };
  if (token) headers.Authorization = `Bearer ${token}`;
  return fetch(path, { ...options, headers }).then(async (res) => {
    const payload = await res.json().catch(() => ({}));
    if (!res.ok) {
      throw new Error(payload.error || 'Request failed');
    }
    return payload;
  });
}

function EmptyForm() {
  return {
    subject: '',
    day: '',
    time: '',
    room: '',
    group: '',
    week: '',
    teacher: '',
    type: ''
  };
}

function LessonModal({ open, initial, lookups, onClose, onSave }) {
  const [form, setForm] = useState(initial || EmptyForm());
  const [error, setError] = useState('');

  useEffect(() => {
    setForm(initial || EmptyForm());
    setError('');
  }, [initial, open]);

  if (!open) return null;

  const set = (field, value) => setForm((prev) => ({ ...prev, [field]: value }));

  const submit = (e) => {
    e.preventDefault();
    const required = ['subject', 'day', 'time', 'room', 'group', 'week', 'teacher', 'type'];
    for (const key of required) {
      if (!String(form[key] || '').trim()) {
        setError('Заповніть усі обовʼязкові поля.');
        return;
      }
    }
    onSave(form);
  };

  return (
    <div className="overlay" role="presentation">
      <form className="modal" onSubmit={submit}>
        <h3>{initial?.id ? 'Редагувати заняття' : 'Додати заняття'}</h3>
        {error ? <p className="error">{error}</p> : null}

        <label>
          Предмет
          <input value={form.subject} onChange={(e) => set('subject', e.target.value)} list="subjects" />
          <datalist id="subjects">
            {lookups.subjects.map((x) => (
              <option key={x} value={x} />
            ))}
          </datalist>
        </label>

        <div className="grid2">
          <label>
            День
            <input value={form.day} onChange={(e) => set('day', e.target.value)} list="days" />
            <datalist id="days">
              {lookups.days.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>

          <label>
            Час
            <input value={form.time} onChange={(e) => set('time', e.target.value)} list="times" />
            <datalist id="times">
              {lookups.times.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
        </div>

        <div className="grid2">
          <label>
            Аудиторія
            <input value={form.room} onChange={(e) => set('room', e.target.value)} />
          </label>

          <label>
            Група
            <input value={form.group} onChange={(e) => set('group', e.target.value)} list="groups" />
            <datalist id="groups">
              {lookups.groups.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
        </div>

        <div className="grid2">
          <label>
            Тиждень
            <input value={form.week} onChange={(e) => set('week', e.target.value)} list="weeks" />
            <datalist id="weeks">
              {lookups.weeks.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>

          <label>
            Викладач
            <input value={form.teacher} onChange={(e) => set('teacher', e.target.value)} list="teachers" />
            <datalist id="teachers">
              {lookups.teachers.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
        </div>

        <label>
          Тип
          <select value={form.type} onChange={(e) => set('type', e.target.value)}>
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
  const [token, setToken] = useState(() => localStorage.getItem('authToken') || '');
  const [user, setUser] = useState(() => localStorage.getItem('authUser') || '');
  const [authMode, setAuthMode] = useState('login');
  const [authForm, setAuthForm] = useState({ username: '', password: '' });
  const [authError, setAuthError] = useState('');

  const [lookups, setLookups] = useState({
    subjects: [],
    groups: [],
    teachers: [],
    times: TIMES,
    rooms: [],
    days: DAYS,
    weeks: WEEKS,
    types: TYPES
  });
  const [lessons, setLessons] = useState([]);
  const [filters, setFilters] = useState(EmptyForm());
  const [status, setStatus] = useState('');
  const [selectedId, setSelectedId] = useState(null);
  const [modalOpen, setModalOpen] = useState(false);
  const [editData, setEditData] = useState(null);

  const selectedLesson = useMemo(
    () => lessons.find((x) => x.id === selectedId) || null,
    [lessons, selectedId]
  );

  const authSubmit = async (e) => {
    e.preventDefault();
    setAuthError('');
    try {
      const data = await api(`/api/auth/${authMode}`, {
        method: 'POST',
        body: JSON.stringify(authForm)
      });
      localStorage.setItem('authToken', data.token);
      localStorage.setItem('authUser', data.user.username);
      setToken(data.token);
      setUser(data.user.username);
      setAuthForm({ username: '', password: '' });
    } catch (error) {
      setAuthError(error.message);
    }
  };

  const loadLookups = async () => {
    const data = await api('/api/lookups', {}, token);
    setLookups({
      subjects: normalizeOptions(data.subjects),
      groups: normalizeOptions(data.groups),
      teachers: normalizeOptions(data.teachers),
      times: normalizeOptions([...(data.times || []), ...TIMES]),
      rooms: normalizeOptions(data.rooms),
      days: normalizeOptions(data.days?.length ? data.days : DAYS),
      weeks: normalizeOptions(data.weeks?.length ? data.weeks : WEEKS),
      types: normalizeOptions(data.types?.length ? data.types : TYPES)
    });
  };

  const loadLessons = async (customFilters = filters) => {
    const params = new URLSearchParams();
    Object.entries(customFilters).forEach(([key, value]) => {
      if (value) params.set(key, value);
    });
    const query = params.toString();
    const data = await api(`/api/lessons${query ? `?${query}` : ''}`, {}, token);
    setLessons(data);
    setSelectedId((prev) => (data.some((x) => x.id === prev) ? prev : null));
    setStatus(`Показано: ${data.length}`);
  };

  useEffect(() => {
    if (!token) return;
    Promise.all([loadLookups(), loadLessons()]).catch((error) => {
      setStatus(`Помилка: ${error.message}`);
    });
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [token]);

  const logout = () => {
    localStorage.removeItem('authToken');
    localStorage.removeItem('authUser');
    setToken('');
    setUser('');
    setLessons([]);
  };

  const addLookup = async (entity, title) => {
    const name = window.prompt(`${title}:`);
    if (!name?.trim()) return;
    await api(
      `/api/lookups/${entity}`,
      {
        method: 'POST',
        body: JSON.stringify({ name: name.trim() })
      },
      token
    );
    await loadLookups();
  };

  const saveLesson = async (form) => {
    if (editData?.id) {
      await api(
        `/api/lessons/${editData.id}`,
        {
          method: 'PUT',
          body: JSON.stringify(form)
        },
        token
      );
    } else {
      await api(
        '/api/lessons',
        {
          method: 'POST',
          body: JSON.stringify(form)
        },
        token
      );
    }
    setModalOpen(false);
    setEditData(null);
    await Promise.all([loadLookups(), loadLessons()]);
  };

  const removeLesson = async () => {
    if (!selectedLesson) return;
    if (!window.confirm('Видалити вибраний рядок?')) return;
    await api(`/api/lessons/${selectedLesson.id}`, { method: 'DELETE' }, token);
    await loadLessons();
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
              className={authMode === 'login' ? 'active' : ''}
              onClick={() => setAuthMode('login')}
            >
              Вхід
            </button>
            <button
              type="button"
              className={authMode === 'register' ? 'active' : ''}
              onClick={() => setAuthMode('register')}
            >
              Реєстрація
            </button>
          </div>

          {authError ? <p className="error">{authError}</p> : null}

          <label>
            Логін
            <input
              value={authForm.username}
              onChange={(e) => setAuthForm((prev) => ({ ...prev, username: e.target.value }))}
            />
          </label>
          <label>
            Пароль
            <input
              type="password"
              value={authForm.password}
              onChange={(e) => setAuthForm((prev) => ({ ...prev, password: e.target.value }))}
            />
          </label>

          <button type="submit">{authMode === 'login' ? 'Увійти' : 'Зареєструватися'}</button>
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
              onChange={(e) => setFilters((prev) => ({ ...prev, subject: e.target.value }))}
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
              onChange={(e) => setFilters((prev) => ({ ...prev, day: e.target.value }))}
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
              onChange={(e) => setFilters((prev) => ({ ...prev, time: e.target.value }))}
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
              onChange={(e) => setFilters((prev) => ({ ...prev, room: e.target.value }))}
              placeholder="Введіть аудиторію"
            />
          </label>
          <label>
            Група
            <input
              value={filters.group}
              onChange={(e) => setFilters((prev) => ({ ...prev, group: e.target.value }))}
              list="groupFilter"
            />
            <datalist id="groupFilter">
              {lookups.groups.map((x) => (
                <option key={x} value={x} />
              ))}
            </datalist>
          </label>
          <label>
            Тиждень
            <input
              value={filters.week}
              onChange={(e) => setFilters((prev) => ({ ...prev, week: e.target.value }))}
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
              onChange={(e) => setFilters((prev) => ({ ...prev, teacher: e.target.value }))}
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

          <h4>Довідники</h4>
          <div className="actions stacked">
            <button className="secondary" onClick={() => addLookup('subjects', 'Новий предмет')}>
              + Предмет
            </button>
            <button className="secondary" onClick={() => addLookup('groups', 'Нова група')}>
              + Група
            </button>
            <button className="secondary" onClick={() => addLookup('teachers', 'Новий викладач')}>
              + Викладач
            </button>
          </div>
        </aside>

        <section className="tablePanel">
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
            <button className="danger" disabled={!selectedLesson} onClick={removeLesson}>
              Видалити
            </button>
          </div>

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
                {lessons.map((row) => (
                  <tr
                    key={row.id}
                    onClick={() => setSelectedId(row.id)}
                    onDoubleClick={() => {
                      setEditData(row);
                      setModalOpen(true);
                    }}
                    className={row.id === selectedId ? 'selected' : ''}
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
