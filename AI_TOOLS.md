# Інструменти AI-агента для Learning-assistent

Дата перевірки: 20 вересня 2026 року.

Це каталог рекомендацій, а не інсталятор. Він не активує MCP, не встановлює пакети та не змінює особисті налаштування. Оцінка базується на структурі репозиторію, README, конфігураціях збірки, наявних тестах і документації авторів інструментів. Повний аудит коду та практичне тестування зовнішніх інтеграцій не виконувалися.

## 1. Що потрібно саме цьому репозиторію

| Напрям | Виявлений стек | Основна користь від інструментів агента |
| --- | --- | --- |
| Sokoban | C++20, CMake, алгоритми пошуку, Next.js 14.2.5, React 18.3.1, TypeScript | Простежувати помилки від браузера до нативного ядра; перевіряти пам’ять, алгоритми, UI та швидкодію |
| PEOPLE-MACHINE LABA4/5 | React 18, Vite, Express, SQLite, JWT | Перевіряти форми, запити, авторизацію, доступність і поведінку інтерфейсу |
| OOP | Java, JavaFX, Maven; у LABA6 задані Java 25, JavaFX 21.0.6, JUnit 5 | Налагоджувати ООП, ресурси, обробники подій, конфігурацію збірки |
| MATH / ARCHIT | Python, Tkinter, Decimal, unittest | Перевіряти математичну логіку, валідацію та граничні випадки |
| Звіти | Python-генератори документів, PDF і зображень | Створювати й візуально перевіряти навчальні матеріали |

Локальні джерела: [Sokoban package.json](ALG_AND_STR/CURSOVA/SAKOBAN-main/web/package.json), [CMakeLists.txt](ALG_AND_STR/CURSOVA/SAKOBAN-main/CMakeLists.txt), [LABA4 package.json](PEOPLE-MACHINE/LABA4/webapp/package.json), [LABA6 pom.xml](OOP/OOP%20LABA%236/Laba6/pom.xml), [Python-тести](ARCHIT/LAB1/test_converter.py).

**Skill** описує метод роботи агента. **MCP** надає виклик зовнішнього інструмента. **Plugin** пакує skills, MCP та, за потреби, hooks. Це не три незалежні набори, які потрібно встановити повністю: один плагін може вже містити потрібний MCP. [Архітектура плагінів OpenAI](https://developers.openai.com/plugins/concepts/plugins).

## 2. Що вже є

| Інструмент | Перевірений стан | Що робити |
| --- | --- | --- |
| Ponytail 4.10.0 | Плагін увімкнений у персональній конфігурації; навички доступні в сесії | Залишити для мінімальних змін без зайвих залежностей |
| animate | Є [.agents/skills/animate/SKILL.md](.agents/skills/animate/SKILL.md) | Використовувати для анімацій; повторно не встановлювати |
| frontend-design | Є [.agents/skills/frontend-design/SKILL.md](.agents/skills/frontend-design/SKILL.md) | Використовувати для дизайну UI |
| Documents, PDF, Spreadsheets, Presentations | Відповідні навички доступні в поточній сесії | Використовувати для звітів і презентацій |
| Context7 MCP | Є запис `mcp_servers.context7` у персональному `~/.codex/config.toml` | Перевірити підключення перед використанням |
| Playwright MCP | Є запис `mcp_servers.playwright` там само | Перевірити браузерний сценарій |
| Chrome DevTools MCP | Є запис `mcp_servers.chrome-devtools` там само | Перевірити читання console/network |
| GitHub plugin | Знайдений у каталозі; поле `installed` дорівнює `false` | Додавати лише для регулярної роботи з віддаленими PR/issues/CI |

**Запис у конфігурації не доводить працездатність MCP.** Інструменти трьох зазначених серверів не були доступні для прямого виклику в цій сесії; handshake і виконання запитів не перевірені. Є також увімкнені browser/computer-use плагіни в конфігурації, але їхню фактичну браузерну доступність тут не підтверджено.

У робочому дереві вже позначені видалення попередніх skills, зокрема `vercel-react-best-practices`, і зміна `animate`. Ці наявні зміни не відновлювалися та не перезаписувалися. Відсутня папка не означає, що навички ніколи не було.

## 3. Рекомендовані skills

Пріоритет: **P1** — найбільша користь зараз; **P2** — під конкретні задачі; **P3** — лише після адаптації або появи потреби.

| Пріоритет | Skill і джерело | Що робить | Чому потрібна / межі застосування |
| --- | --- | --- | --- |
| P1 | [systematic-debugging — obra](https://github.com/obra/superpowers/tree/main/skills/systematic-debugging) | Відтворення помилки, збір доказів, перевірка гіпотези, виправлення першопричини | Найкорисніша для всіх мов і зв’язку React → API → C++; брати окрему навичку, а не весь workflow Superpowers |
| P1 | [vercel-react-best-practices — Vercel](https://github.com/vercel-labs/agent-skills/tree/main/skills/react-best-practices) | Продуктивність React/Next.js: рендери, запити, завантаження коду | Підходить для обох вебстеків; правила звіряти з React 18 і Next.js 14, не копіювати нові API автоматично |
| P1 | [animate — локальна](.agents/skills/animate/SKILL.md) | Вибір CSS/WAAPI/Motion, переходів і характеру руху | Уже є; застосовувати для станів гри, панелей і взаємодії. Motion додавати лише за потреби |
| P1 | [cpp-pro — Jeffallan](https://github.com/Jeffallan/claude-skills/tree/main/skills/cpp-pro) | C++20/23, RAII, CMake, аналіз пам’яті, профілювання | Особливо корисна Sokoban; зберігати C++20 та наявні тести, не нав’язувати складні шаблони чи SIMD |
| P2 | [java-development — zander-zyx](https://github.com/zander-zyx/java-development-skill) | Java/Maven/JUnit, винятки, null, ресурси, JVM | Краще відповідає звичайній Java, ніж Spring-орієнтовані навички. Не є спеціалізованим посібником із JavaFX |
| P2 | [playwright — OpenAI](https://github.com/openai/skills/tree/main/skills/.curated/playwright) | Керування браузером через CLI, форми, знімки, traces | Альтернатива MCP для агента з терміналом. Не потребує одночасного встановлення ще одного браузерного сервера; shell-wrapper потребує Bash або використання CLI напряму у Windows |
| P2 | [web-design-guidelines — Vercel](https://github.com/vercel-labs/agent-skills/tree/main/skills/web-design-guidelines) | Рев’ю UI за вебрекомендаціями | Доповнює frontend-design перевіркою готового інтерфейсу, зокрема доступності |
| P2 | [security-best-practices — OpenAI](https://github.com/openai/skills/tree/main/skills/.curated/security-best-practices) | Перевірка безпечних практик для підтримуваних стеків | Для Express/JWT/SQLite та Python; заявлені мови Python, JS/TS і Go, не універсальна Java/C++-навичка |
| P3 | [python-pro — Jeffallan](https://github.com/Jeffallan/claude-skills/tree/main/skills/python-pro) | Типізація, stdlib, обробка помилок, pytest, mypy | Оригінал завеликий для лабораторних: вимагає strict mypy та понад 90% покриття. Спершу адаптувати під unittest, Tkinter і невеликі скрипти |

Навички Jeffallan, obra та zander-zyx — сторонні матеріали, а не офіційні навички відповідних мов. Тут оцінено їхній зміст і відповідність репозиторію, а не проведено порівняльний бенчмарк якості.

Для Python зараз достатньо Ponytail + systematic-debugging + наявного unittest. Якщо створювати власну навичку, її вузька задача: відтворювати помилки Tkinter/Decimal, перевіряти граничні значення й запускати чинні тести без зміни фреймворку. Аналогічно JavaFX-специфіку краще додати невеликою локальною інструкцією під ваш проєкт.

## 4. Рекомендовані MCP

| MCP | Рішення для цього репозиторію | Що додає | Як перевірити користь |
| --- | --- | --- | --- |
| [Context7 — Upstash](https://github.com/upstash/context7) | **P1: перевірити вже наявний** | Документацію бібліотек; за наявності відповідної версії допомагає уникати змішування API | Знайти React/Next.js та отримати документацію для потрібної версії. Якщо індекс її не містить — перейти до офіційної документації |
| [Playwright MCP — Microsoft](https://github.com/microsoft/playwright-mcp) | **P1: перевірити вже наявний**, або використати CLI skill | Керування браузером через accessibility snapshots | Відкрити локальний Sokoban, виконати хід, undo і перевірити стан; для розкладу — пройти створення/редагування запису з тестовими даними |
| [Chrome DevTools MCP](https://github.com/ChromeDevTools/chrome-devtools-mcp) | **P2: використовувати для діагностики** | Console, network, performance traces та діагностику браузера | Знайти невдалий запит або помилку консолі; записати trace повільної взаємодії |
| [Serena](https://github.com/oraios/serena) | **P2: спробувати на одному великому підпроєкті** | Семантичний пошук символів, посилань і редагування через LSP/IDE | Знайти виклики методу в Sokoban або Java-проєкті й порівняти користь із rg. Потрібні відповідні мовні сервери; це не runtime debugger |
| [GitHub MCP — GitHub](https://github.com/github/github-mcp-server) | **P3: альтернатива GitHub plugin/gh** | Віддалені репозиторії, PR, issues та Actions | Прочитати PR і лог невдалої CI-перевірки після автентифікації; для локального коду вже є git/gh |

Playwright CLI + skill та Playwright MCP — альтернативні способи браузерної автоматизації. Автори Playwright рекомендують CLI + skills для багатьох задач coding-агентів через менші витрати контексту; MCP корисний для інтерактивної роботи через інструменти. Вибирати за реальним сценарієм. [Порівняння авторів](https://github.com/microsoft/playwright-mcp).

Chrome DevTools варто підключати, коли потрібні саме його засоби діагностики. Не потрібно постійно дублювати однакові браузерні дії через кілька інтеграцій.

## 5. Плагіни

| Плагін | Рекомендація | Обґрунтування |
| --- | --- | --- |
| [Ponytail](https://github.com/DietrichGebert/ponytail) | Залишити наявний | Стримує зайві абстракції та залежності; підходить навчальному репозиторію |
| [Context7 для Codex](https://github.com/upstash/context7/tree/master/plugins/codex/context7) | Опціонально замість окремої конфігурації Context7 MCP | Пакує skill пошуку документації та hosted MCP з OAuth. Уже наявний Context7 MCP не потрібно дублювати |
| GitHub, знайдений через каталог плагінів | Опціонально для регулярних PR/issues/CI | Поєднує connector і CLI workflow. Зараз не встановлений; для локального аналізу достатньо git, для багатьох віддалених задач — gh |
| Documents / PDF / Presentations / Spreadsheets | Використовувати вже доступні | Звіти, таблиці та захист лабораторних; повторна інсталяція не потрібна |
| [Superpowers](https://github.com/obra/superpowers) | Не встановлювати повністю лише заради debugging | Широкий процес розробки має більший обсяг, ніж потрібно для окремих лабораторних; достатньо вибраної навички |

Джерело стану GitHub plugin — пошук у каталозі під час аналізу, а не припущення за наявністю GitHub CLI. Плагін і MCP GitHub не потрібно встановлювати разом лише заради доступу до тих самих PR.

## 6. Що важливіше за нові інтеграції: локальні інструменти

Перевірено лише поточне середовище PowerShell; відсутність у PATH не доводить відсутності на комп’ютері.

| Інструмент | Результат перевірки | Практичне значення |
| --- | --- | --- |
| Node.js | `v24.16.0`, npm доступний | Є основа для запуску Node-based MCP; браузери та самі MCP не перевірені |
| Python | `3.12.11` | Є основа для unittest, pdb та скриптів; сторонні модулі окремо не перевірялися |
| g++, gdb | Знайдені в PATH | Є компілятор і debugger для C++; працездатність конкретної збірки не перевірялася |
| java | `1.8.0_491` | Не відповідає Java 25 у LABA6. Перед налагодженням обрати відповідний JDK/JAVA_HOME |
| javac, mvn, cmake, ninja, clangd | Не знайдені через Get-Command | Перевірити встановлення й PATH. Maven Wrapper є в частині Java-проєктів |
| git, gh | Знайдені | Окремий Git/FileSystem MCP не потрібний для звичайного доступу до репозиторію; GitHub-auth не перевірялася |

Для C++ використовувати Debug-збірку, gdb, попередження компілятора та підтримувані обраним toolchain санітайзери. Для Java — відповідний JDK, stack trace, JUnit та debugger IDE/jdb. Для Python — unittest, traceback і pdb. MCP не замінює цих інструментів.

## 7. Розміщення та налаштування

У корені зберігається цей каталог. Проєктні skills розміщуються в `.agents/skills/<name>/`, конфігурація Codex MCP — у `.codex/config.toml`, якщо потрібна саме проєктна область. Персональні плагіни й MCP можуть залишатися в користувацьких налаштуваннях. Не додавати npm/pip-залежності в корінь заради однієї лабораторної.

Codex підтримує MCP у персональному `~/.codex/config.toml` і проєктному `.codex/config.toml` для довірених проєктів. [Офіційне налаштування MCP](https://developers.openai.com/codex/mcp/).

Нижче лише приклад уже наявних Windows-записів. **Не копіювати повторно без потреби**: конфігурацію автоматично не створено.

```toml
[mcp_servers.context7]
command = "npx.cmd"
args = ["-y", "@upstash/context7-mcp@latest"]
startup_timeout_sec = 60

[mcp_servers.playwright]
command = "npx.cmd"
args = ["-y", "@playwright/mcp@latest"]
startup_timeout_sec = 60

[mcp_servers.chrome-devtools]
command = "npx.cmd"
args = ["-y", "chrome-devtools-mcp@latest"]
startup_timeout_sec = 60
```

Для macOS/Linux команда — `npx`. `@latest` відповідає поточному локальному налаштуванню, але не фіксує версію; для відтворюваного командного середовища варто закріпити версію після перевірки. Credentials залишати поза репозиторієм.

## 8. Перевірка після встановлення або зміни конфігурації

1. Переглянути `codex mcp list`: це перевірка реєстрації серверів, не успішного виконання їхніх інструментів.
2. У новій/перезапущеній сесії відкрити `/mcp` і перевірити підключення; виконати по одному реальному сценарію з таблиці MCP.
3. Для skill перевірити появу у списку навичок і виконати вузьку задачу: наприклад, відтворити помилку тестом та отримати перевірене виправлення через systematic-debugging.
4. Для GitHub перевірити читання PR після входу; для Context7 plugin завершити OAuth, якщо обраний цей спосіб підключення.
5. Запустити релевантні тести з директорії конкретного проєкту. Наявність skill або MCP не є результатом тесту.

Приклади чинних перевірок, а не звіт про їх виконання під час цього аналізу:

```powershell
# З кореня репозиторію:
python ARCHIT/LAB1/test_converter.py

# Із ALG_AND_STR/CURSOVA/SAKOBAN-main, після налаштування CMake/Ninja:
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure

# Із ALG_AND_STR/CURSOVA/SAKOBAN-main/web, після встановлення залежностей:
npm run typecheck
npm run test:native
npm run test:e2e
```

## 9. Порядок дій

1. Усунути невідповідність Java/JDK і перевірити інструменти збірки потрібного проєкту.
2. Перевірити три вже налаштовані MCP, не встановлюючи дублікати.
3. Додати systematic-debugging; React Best Practices повертати лише як свідоме рішення з урахуванням наявних видалень у робочому дереві.
4. Додати cpp-pro та java-development для відповідних великих проєктів.
5. Продовжувати використовувати animate, frontend-design і Ponytail.
6. За потреби додати Playwright CLI skill, UI/security review; Serena випробувати окремо на одному підпроєкті.
7. GitHub plugin/MCP підключати за регулярної потреби у віддалених PR/issues/CI. Python Pro адаптувати перед використанням у малих лабораторних.

Не рекомендовано зараз: окремі filesystem/git/search MCP, що дублюють наявні можливості; Spring-орієнтований java-architect для JavaFX; повні набори десятків skills; хмарні deployment-плагіни без задачі розгортання. Старий `next-best-practices` більше не постачається як окрема навичка — див. [повідомлення Vercel про перенесення](https://github.com/vercel-labs/next-skills); нові Next.js workflow не слід автоматично застосовувати до Next.js 14.
