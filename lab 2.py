import os
import sys
import ctypes


def get_line(msg):
    print(msg, end='', flush=True)
    try:
        return input().strip()
    except EOFError:
        return ''

def ask(msg):
    while True:
        v = get_line(msg)
        if v: return v
        print("Поле не може бути порожнім")

def confirm(q):
    while True:
        a = get_line(q + " (т/н): ").lower()
        if a in ('т', 'так', 'y', 'yes'): return True
        if a in ('н', 'ні', 'n', 'no'):   return False
        print("Введіть т або н")

def fmt_size(b):
    if b < 0:           return "N/A"
    if b < 1024:        return f"{b} Б"
    if b < 1024**2:     return f"{b/1024:.2f} КБ"
    if b < 1024**3:     return f"{b/1024**2:.2f} МБ"
    return f"{b/1024**3:.2f} ГБ"

def fix_path(p):
    return p + "\\" if len(p) == 2 and p[1] == ':' else p

def wild_match(pat, name):
    pat, name = pat.lower(), name.lower()
    p = n = 0
    star = match = -1
    while n < len(name):
        if p < len(pat) and (pat[p] == '?' or pat[p] == name[n]):
            p += 1; n += 1
        elif p < len(pat) and pat[p] == '*':
            star = p; p += 1; match = n
        elif star != -1:
            p = star + 1; match += 1; n = match
        else:
            return False
    while p < len(pat) and pat[p] == '*': p += 1
    return p == len(pat)


def cmd_stat(raw):
    path = fix_path(raw)
    if not os.path.exists(path):
        print(f"Помилка: не знайдено: {raw}"); return

    try:
        stat = os.statvfs(path) if hasattr(os, 'statvfs') else None
        if stat:
            total = stat.f_blocks * stat.f_frsize
            free  = stat.f_bfree  * stat.f_frsize
        else:
            import shutil
            usage = shutil.disk_usage(path)
            total, free = usage.total, usage.free
    except Exception as e:
        print(f"Помилка: {e}"); return

    print(f"STAT: {os.path.abspath(path)}")
    print(f"  Всього  : {fmt_size(total)}")
    print(f"  Зайнято : {fmt_size(total - free)}")
    print(f"  Вільно  : {fmt_size(free)}")

def cmd_diskcomp(r1, r2):
    p1, p2 = fix_path(r1), fix_path(r2)
    if not os.path.exists(p1): print(f"Не знайдено: {r1}"); return
    if not os.path.exists(p2): print(f"Не знайдено: {r2}"); return

    print(f"DISKCOMP: {os.path.abspath(p1)}")
    print(f"      vs: {os.path.abspath(p2)}")

    if os.path.isfile(p1) and os.path.isfile(p2):
        s1, s2 = os.path.getsize(p1), os.path.getsize(p2)
        print(f"  Розмір 1: {fmt_size(s1)}")
        print(f"  Розмір 2: {fmt_size(s2)}")
        if s1 != s2:
            print("  Результат: ВІДРІЗНЯЮТЬСЯ (різний розмір)"); return
        with open(p1,'rb') as f1, open(p2,'rb') as f2:
            eq = all(b1 == b2 for b1, b2 in
                     zip(iter(lambda: f1.read(4096), b''),
                         iter(lambda: f2.read(4096), b'')))
        print("  Результат:", "ІДЕНТИЧНІ" if eq else "ВІДРІЗНЯЮТЬСЯ (різний вміст)")

    elif os.path.isdir(p1) and os.path.isdir(p2):
        s1, s2 = set(os.listdir(p1)), set(os.listdir(p2))
        print(f"  Файлів у 1: {len(s1)}")
        print(f"  Файлів у 2: {len(s2)}")
        for f in s1 - s2: print(f"  Тільки в 1: {f}")
        for f in s2 - s1: print(f"  Тільки в 2: {f}")
        print("  Результат:", "Вміст однаковий" if s1 == s2 else "ВІДРІЗНЯЮТЬСЯ")
    else:
        print("Помилка: не можна порівнювати файл з папкою")

def cmd_era(raw):
    path = fix_path(raw)

    if os.path.isfile(path):
        sz = os.path.getsize(path)
        print(f"ERA: {os.path.abspath(path)} ({fmt_size(sz)})")
        if confirm("Видалити?"):
            try: os.remove(path); print("Файл видалено.")
            except Exception as e: print(f"Помилка: {e}")
        else: print("Скасовано.")
        return

    if os.path.isdir(path):
        print("Помилка: ERA не видаляє папки!"); return

    pos = max(raw.rfind('\\'), raw.rfind('/'))
    dir_ = raw[:pos] if pos != -1 else '.'
    name = raw[pos+1:] if pos != -1 else raw

    if '*' not in name and '?' not in name:
        print(f"Файл не знайдено: {raw}"); return

    if not os.path.isdir(dir_):
        print(f"Папку не знайдено: {dir_}"); return

    files = [f for f in os.listdir(dir_)
             if os.path.isfile(os.path.join(dir_, f)) and wild_match(name, f)]

    if not files: print(f"Файлів не знайдено: {name}"); return

    for f in files:
        sz = os.path.getsize(os.path.join(dir_, f))
        print(f"  {f} ({fmt_size(sz)})")

    if not confirm(f"Видалити {len(files)} файл(ів)?"): print("Скасовано."); return

    ok = fail = 0
    for f in files:
        try: os.remove(os.path.join(dir_, f)); ok += 1
        except: print(f"Помилка: {f}"); fail += 1
    print(f"Видалено: {ok}" + (f"  Помилок: {fail}" if fail else ""))


def run_cmd(cmd, a1='', a2=''):
    cmd = cmd.upper()
    if   cmd == 'STAT':     cmd_stat(a1 or ask("Шлях: "))
    elif cmd == 'DISKCOMP': cmd_diskcomp(a1 or ask("Перший шлях: "), a2 or ask("Другий шлях: "))
    elif cmd == 'ERA':      cmd_era(a1 or ask("Файл: "))
    else: print(f"Невідома команда: {cmd}. Введіть ДОПОМОГА.")


def mode_cmd():
    print("Командний режим. STAT / DISKCOMP / ERA / ДОПОМОГА / ВИХІД")
    while True:
        line = get_line("A> ")
        if not line: continue
        parts = line.split()
        cmd = parts[0]
        if cmd.lower() in ('вихід', 'exit', 'back'): return
        if cmd.lower() in ('допомога', 'help'):
            print("STAT [шлях] | DISKCOMP [ш1] [ш2] | ERA [файл]"); continue
        run_cmd(cmd, parts[1] if len(parts)>1 else '', parts[2] if len(parts)>2 else '')

def mode_qa():
    while True:
        print("Q&A: 1-STAT  2-DISKCOMP  3-ERA  0-Назад")
        c = get_line("Вибір: ")
        if   c == '1': cmd_stat(ask("Шлях або диск (C:): ")); return
        elif c == '2': cmd_diskcomp(ask("Перший шлях: "), ask("Другий шлях: ")); return
        elif c == '3': cmd_era(ask("Файл або шаблон: ")); return
        elif c == '0': return
        else: print("Введіть 0-3")

def mode_menu():
    while True:
        print("Меню: 1-STAT  2-DISKCOMP  3-ERA  0-Назад")
        c = get_line("Вибір: ")
        if c == '1':
            import shutil
            drives = []
            for letter in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ':
                d = letter + ":\\"
                if os.path.exists(d): drives.append(d)
            for i, d in enumerate(drives): print(f"  {i+1} - {d}")
            print("  0 - Свій шлях")
            x = get_line("Вибір: ")
            if x == '0': cmd_stat(ask("Шлях: "))
            elif x.isdigit() and 0 < int(x) <= len(drives): cmd_stat(drives[int(x)-1])
            else: print("Невірний вибір")
        elif c == '2': cmd_diskcomp(ask("Перший шлях: "), ask("Другий шлях: "))
        elif c == '3':
            print("ERA: 1-Файл  2-Шаблон  0-Назад")
            m = get_line("Вибір: ")
            if   m == '1': cmd_era(ask("Шлях до файлу: "))
            elif m == '2':
                d = ask("Директорія: "); p = ask("Шаблон (*.tmp): ")
                cmd_era(os.path.join(d, p))
            elif m != '0': print("Введіть 0-2")
        elif c == '0': return
        else: print("Введіть 0-3")

def mode_form():
    while True:
        print("Форма: 1-STAT  2-DISKCOMP  3-ERA  0-Назад")
        c = get_line("Вибір: ")
        if c == '1':
            p = get_line("Шлях [C:]: ") or "C:"
            print(f">> Шлях: {p}")
            if confirm("Виконати?"): cmd_stat(p)
            else: print("Скасовано.")
        elif c == '2':
            p1 = get_line("Шлях 1 [C:]: ") or "C:"
            p2 = get_line("Шлях 2 [D:]: ") or "D:"
            print(f">> {p1} vs {p2}")
            if confirm("Виконати?"): cmd_diskcomp(p1, p2)
            else: print("Скасовано.")
        elif c == '3':
            p = ask("Файл або шаблон: ")
            print(f">> {p}")
            if confirm("Виконати?"): cmd_era(p)
            else: print("Скасовано.")
        elif c == '0': return
        else: print("Введіть 0-3")


def main():
    if sys.platform == 'win32':
        ctypes.windll.kernel32.SetConsoleCP(65001)
        ctypes.windll.kernel32.SetConsoleOutputCP(65001)

    print("Лаб. робота 2 | Варіант 9 | STAT, DISKCOMP, ERA\n")

    if len(sys.argv) > 1:
        a1 = sys.argv[2] if len(sys.argv) > 2 else ''
        a2 = sys.argv[3] if len(sys.argv) > 3 else ''
        run_cmd(sys.argv[1], a1, a2)
        return

    while True:
        print("Тип діалогу: 1-Командний  2-Q&A  3-Меню  4-Форма  0-Вихід")
        c = get_line("Вибір: ")
        if   c == '1': mode_cmd()
        elif c == '2': mode_qa()
        elif c == '3': mode_menu()
        elif c == '4': mode_form()
        elif c == '0': print("end"); break
        else: print("Введіть 0-4")

if __name__ == '__main__':
    main()
