
import tkinter as tk
from tkinter import ttk, simpledialog, messagebox

# -- Default -------------------------------------------------------------------
groups   = [u'2ПІ-25Б', u'3ПІ-25Б', u'1ПІ-25Б']
teachers = [u'Іваненко І.І.',
            u'Петренко О.В.',
            u'Коваль М.С.',
            u"Ткаченко О.М."]
subjects = [u'Вища Математика', u'ООП', u'Фізика', u'Людино-машинна взаємодія', u"АВ ПЗ"]
lessons  = [
    (u'Вища Математика',
     u'Пн 08:30', '201', u'2ПІ-25Б', '1',
     u'Іваненко І.І.',
     u'Лекція'),
    (u'ООП',
     u'Вт 10:15', '302', u'2ПІ-25Б', u'Обидва',
     u'Петренко О.В.',
     u'Практика'),
    (u'Фізика',
     u'Ср 13:45', '105', u'3ПІ-25Б', '2',
     u'Коваль М.С.',
     u'Лабораторна'),
    (u'АВ ПЗ',
     u'Чт 08:30', '210', u'1ПІ-25Б', '1',
     u'Іваненко І.І.',
     u'Лекція'),
    (u'Людино-машинна взаємодія',
     u'Ср 14:00', '2320', u'2ПІ-25Б', '1',
     u'Ткаченко О.М.',
     u'Лекція')

]

COLS   = (u'Предмет', u'Час',
          u'Аудиторія', u'Група',
          u'Тиждень', u'Викладач',
          u'Тип')
WIDTHS = (180, 110, 80, 80, 70, 160, 110)
LEK    = u'Лекція'
PRAK   = u'Практика'
LAB    = u'Лабораторна'
TYPE_BG = {LEK: '#cce0f5', PRAK: '#ccefd4', LAB: '#fde4b0'}
DAYS   = ['', u'Пн', u'Вт', u'Ср',
          u'Чт', u'Пт', u'Сб', u'Нд']
TYPES  = ['', LEK, PRAK, LAB]
WEEKS  = ['', '1', '2', u'Обидва']


def sync_subjects_from_lessons():
    for r in lessons:
        subj = r[0]
        if subj and subj not in subjects:
            subjects.append(subj)


def split_day_time(value):
    txt = (value or '').strip()
    if not txt:
        return '', ''
    parts = txt.split(None, 1)
    if parts[0] in DAYS[1:]:
        return parts[0], (parts[1] if len(parts) > 1 else '')
    return '', txt


def join_day_time(day, tm):
    day = (day or '').strip()
    tm = (tm or '').strip()
    if day and tm:
        return day + ' ' + tm
    return day or tm


# -- Filter ------------------------------------------------------------------
def apply_filter(tree, fvars, status):
    subj, day, tm, room, grp, week, tchr, typ = (v.get() for v in fvars)

    def norm(text):
        return (text or '').strip().lower()

    subj = norm(subj)
    day = norm(day)
    tm = norm(tm)
    room = norm(room)
    grp = norm(grp)
    week = norm(week)
    tchr = norm(tchr)
    typ = norm(typ)

    res = []
    for r in lessons:
        r_day, r_tm = split_day_time(r[1])
        if (subj in norm(r[0])
            and day in norm(r_day)
            and tm in norm(r_tm)
            and room in norm(r[2])
            and grp in norm(r[3])
            and week in norm(r[4])
            and tchr in norm(r[5])
            and typ in norm(r[6])):
            res.append(r)
    tree.delete(*tree.get_children())
    for r in res:
        tree.insert('', 'end', values=r, tags=(r[6],))
    status.set(u'Показано: %d із %d' % (len(res), len(lessons)))


def reset_filter(fvars, tree, status):
    for v in fvars: v.set('')
    apply_filter(tree, fvars, status)


# -- Lesson dialog (add / edit) ----------------------------------------------
def lesson_dialog(root, title, initial=None):
    if initial:
        init_day, init_tm = split_day_time(initial[1])
        init = (initial[0], init_day, init_tm, initial[2], initial[3], initial[4], initial[5], initial[6])
    else:
        init = (subjects[0] if subjects else '', DAYS[1] if len(DAYS) > 1 else '', '', '',
                groups[0] if groups else '', '1', teachers[0] if teachers else '', LEK)
    dlg = tk.Toplevel(root)
    dlg.title(title)
    dlg.resizable(False, False)
    dlg.grab_set()

    LBLS = [u'Предмет:',
            u'День:',
            u'Час (напр. 08:30):',
            u'Аудиторія:',
            u'Група:', u'Тиждень:',
            u'Викладач:', u'Тип:']
    OPTS = [[''] + subjects, DAYS[1:], None, None,
            [''] + groups, WEEKS, [''] + teachers, [LEK, PRAK, LAB]]
    dvars = []
    for i, (lbl, opt, val) in enumerate(zip(LBLS, OPTS, init)):
        tk.Label(dlg, text=lbl, anchor='w', width=24).grid(
            row=i, column=0, sticky='w', padx=10, pady=4)
        v = tk.StringVar(value=val); dvars.append(v)
        if opt is None:
            tk.Entry(dlg, textvariable=v, width=28).grid(row=i, column=1, padx=10, pady=4)
        else:
            ttk.Combobox(dlg, textvariable=v, values=opt, state='readonly', width=26).grid(
                row=i, column=1, padx=10, pady=4)

    saved = [False]
    def ok(): saved[0] = True; dlg.destroy()
    tk.Button(dlg, text='OK', command=ok, width=12, bg='#4a90d9', fg='white').grid(
        row=8, column=0, pady=12, padx=10)
    tk.Button(dlg, text=u'Скасувати',
              command=dlg.destroy, width=12).grid(row=8, column=1, pady=12, padx=10)

    dlg.update_idletasks()
    x = root.winfo_x() + (root.winfo_width()  - dlg.winfo_width())  // 2
    y = root.winfo_y() + (root.winfo_height() - dlg.winfo_height()) // 2
    dlg.geometry('+%d+%d' % (x, y))
    dlg.wait_window()

    if not saved[0]: return None
    if not dvars[0].get():
        messagebox.showwarning('!', u'Вкажіть предмет',
                               parent=root); return None
    if not dvars[1].get():
        messagebox.showwarning('!', u'Вкажіть день',
                               parent=root); return None
    if not dvars[4].get():
        messagebox.showwarning('!', u'Вкажіть групу',
                               parent=root); return None
    vals = [v.get() for v in dvars]
    return (vals[0], join_day_time(vals[1], vals[2]), vals[3], vals[4], vals[5], vals[6], vals[7])


# -- Actions -----------------------------------------------------------------
def add_group(root, grp_combos):
    name = simpledialog.askstring(
        u'Нова група',
        u'Назва групи:', parent=root) or ''
    if not name: return
    if name in groups: messagebox.showwarning('!', name + u' вже існує', parent=root); return
    groups.append(name)
    for c in grp_combos: c['values'] = [''] + groups


def add_teacher(root, tchr_combos):
    name = simpledialog.askstring(
        u'Новий викладач',
        u'Прізвище та ім’я:',
        parent=root) or ''
    if not name: return
    if name in teachers: messagebox.showwarning('!', name + u' вже існує   ', parent=root); return
    teachers.append(name)
    for c in tchr_combos: c['values'] = [''] + teachers


def add_lesson(root, tree, fvars, status):
    row = lesson_dialog(root, u'Додати заняття')
    if row: lessons.append(row); apply_filter(tree, fvars, status)


def edit_lesson(root, tree, fvars, status):
    sel = tree.selection()
    if not sel:
        messagebox.showwarning('!', u'Оберіть рядок з таблиці',
                               parent=root); return
    vals = tree.item(sel[0], 'values')
    idx  = next((i for i, r in enumerate(lessons) if tuple(r) == tuple(vals)), None)
    if idx is None: return
    row = lesson_dialog(root,
                        u'Редагувати заняття',
                        initial=lessons[idx])
    if row: lessons[idx] = row; apply_filter(tree, fvars, status)


def delete_lesson(root, tree, fvars, status):
    sel = tree.selection()
    if not sel:
        messagebox.showwarning('!', u'Оберіть рядок з таблиці',
                               parent=root); return
    if not messagebox.askyesno('?', u'Видалити вибраний рядок?',
                               parent=root): return
    vals = tree.item(sel[0], 'values')
    idx  = next((i for i, r in enumerate(lessons) if tuple(r) == tuple(vals)), None)
    if idx is not None: lessons.pop(idx)
    apply_filter(tree, fvars, status)

def add_subject(root, subj_combos):
    name = simpledialog.askstring(
        u'Новий предмет',
        u'Назва предмету:', parent=root) or ''
    if not name: return
    if name in subjects: messagebox.showwarning('!', name + u' вже існує', parent=root); return
    subjects.append(name)
    for c in subj_combos: c['values'] = [''] + subjects
# -- Main --------------------------------------------------------------------
def main():
    sync_subjects_from_lessons()

    root = tk.Tk()
    root.title(u'Розклад занять')
    root.geometry('1180x620')
    root.minsize(900, 440)
    status = tk.StringVar()

    bar = tk.Frame(root, bg='#e8e8e8', pady=5)
    bar.pack(fill='x', padx=2)

    fp = tk.LabelFrame(root,
                       text=u'  Пошук / Фільтр  ',
                       padx=8, pady=8, font=('Segoe UI', 9, 'bold'))
    fp.pack(side='left', fill='y', padx=(4, 0), pady=4)

    fvars = []
    def mk_combo(values, width=18):
        v = tk.StringVar()
        c = ttk.Combobox(fp, textvariable=v, values=values, width=width, state='normal')
        return c, v

    tk.Label(fp, text=u'Предмет:', anchor='w',
             font=('Segoe UI', 9)).grid(row=0, column=0, sticky='w', pady=3)
    c_subj, v_subj = mk_combo([''] + subjects)
    c_subj.grid(row=0, column=1, padx=6, pady=3, sticky='ew')
    fvars.append(v_subj)

    filter_cfg = [
        (u'День:',      DAYS,             1),
        (u'Час:',       None,             2),
        (u'Аудиторія:', None,             3),
        (u'Група:',     [''] + groups,    4),
        (u'Тиждень:',   WEEKS,            5),
        (u'Викладач:',  [''] + teachers,  6),
        (u'Тип:',       TYPES,            7),
    ]
    created = {}
    for lbl, opts, r in filter_cfg:
        tk.Label(fp, text=lbl, anchor='w', font=('Segoe UI', 9)).grid(
            row=r, column=0, sticky='w', pady=3)
        if opts is None:
            v = tk.StringVar()
            c = tk.Entry(fp, textvariable=v, width=20)
        else:
            c, v = mk_combo(opts)
        c.grid(row=r, column=1, padx=6, pady=3, sticky='ew')
        fvars.append(v); created[r] = c

    subj_combos = [c_subj]
    grp_combos  = [created[4]]
    tchr_combos = [created[6]]
    tree_ref    = [None]

    tk.Button(fp, text=u'Знайти', bg='#4a90d9', fg='white',
              font=('Segoe UI', 9),
              command=lambda: apply_filter(tree_ref[0], fvars, status)).grid(
              row=8, column=0, columnspan=2, sticky='ew', pady=(12, 3))
    tk.Button(fp, text=u'Скинути', font=('Segoe UI', 9),
              command=lambda: reset_filter(fvars, tree_ref[0], status)).grid(
              row=9, column=0, columnspan=2, sticky='ew', pady=3)

    tk.Label(fp, text=u'Легенда:', anchor='w',
             font=('Segoe UI', 8, 'bold')).grid(row=11, column=0, columnspan=2,
                                                 sticky='w', pady=(10, 2))
    for i, (typ, bg) in enumerate(TYPE_BG.items()):
        tk.Label(fp, text='  ' + typ, bg=bg, font=('Segoe UI', 8),
                 relief='groove', width=17).grid(row=12 + i, column=0, columnspan=2,
                                                  sticky='w', padx=4, pady=1)

    for v in fvars:
        v.trace_add('write', lambda *_: apply_filter(tree_ref[0], fvars, status))

    tv = tk.Frame(root)
    tv.pack(side='left', fill='both', expand=True, padx=4, pady=4)

    tree = ttk.Treeview(tv, columns=COLS, show='headings', selectmode='browse')
    tree_ref[0] = tree

    _sort_rev = {}
    def sort_col(col):
        rev = _sort_rev.get(col, False); idx = list(COLS).index(col)
        lessons.sort(key=lambda r: r[idx], reverse=rev)
        _sort_rev[col] = not rev
        apply_filter(tree, fvars, status)

    for col, w in zip(COLS, WIDTHS):
        tree.heading(col, text=col, command=lambda c=col: sort_col(c))
        tree.column(col, width=w, minwidth=40)
    for tag, bg in TYPE_BG.items():
        tree.tag_configure(tag, background=bg)

    vsb = ttk.Scrollbar(tv, orient='vertical',   command=tree.yview)
    hsb = ttk.Scrollbar(tv, orient='horizontal', command=tree.xview)
    tree.configure(yscrollcommand=vsb.set, xscrollcommand=hsb.set)
    tree.grid(row=0, column=0, sticky='nsew')
    vsb.grid(row=0, column=1, sticky='ns')
    hsb.grid(row=1, column=0, sticky='ew')
    tv.rowconfigure(0, weight=1); tv.columnconfigure(0, weight=1)

    # double-click = edit
    tree.bind('<Double-1>', lambda e: edit_lesson(root, tree, fvars, status))

    # Toolbar
    BTN = dict(padx=9, pady=5, relief='groove', font=('Segoe UI', 9))
    tk.Button(bar, text=u'+ Предмет', bg='#d9edf7', **BTN,
              command=lambda: add_subject(root, subj_combos)).pack(side='left', padx=3)
    tk.Button(bar, text=u'+ Група', bg='#f0f0f0', **BTN,
              command=lambda: add_group(root, grp_combos)).pack(side='left', padx=3)
    tk.Button(bar, text=u'+ Викладач', bg='#f0f0f0', **BTN,
              command=lambda: add_teacher(root, tchr_combos)).pack(side='left', padx=3)
    tk.Button(bar, text=u'+ Заняття', bg='#e6f2ff', **BTN,
              command=lambda: add_lesson(root, tree, fvars, status)).pack(side='left', padx=3)
    tk.Button(bar, text=u'Редагувати',
              bg='#fff8e1', **BTN,
              command=lambda: edit_lesson(root, tree, fvars, status)).pack(side='left', padx=3)
    tk.Button(bar, text=u'Видалити',
              bg='#fdecea', **BTN,
              command=lambda: delete_lesson(root, tree, fvars, status)).pack(side='left', padx=3)

    tk.Label(root, textvariable=status, anchor='w', relief='sunken',
             bg='#e0e0e0', font=('Segoe UI', 8), padx=6).pack(side='bottom', fill='x')

    apply_filter(tree, fvars, status)
    root.mainloop()


if __name__ == '__main__':
    main()
