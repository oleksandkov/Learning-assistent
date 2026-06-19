# 📘 Cursova Defense Preparation — Повний довідник для захисту

> **Проєкт:** Roman Conquest (RTS-гра на JavaFX)
> **Структура:** ~17 Java-класів, серіалізація, GUI-діалоги, міні-карта, ігровий цикл
> **Мета документа:** Повторення всієї структури проєкту та підготовка до можливих завдань викладача

---

## 📑 Зміст

1. [Повна структура проєкту](#1-повна-структура-проєкту)
2. [Коротка карта кожного класу](#2-коротка-карта-кожного-класу)
3. [Ігровий цикл — головний нерв](#3-ігровий-цикл--головний-нерв)
4. [Клавіші керування](#4-клавіші-керування)
5. [Можливі завдання викладача — 5 рівнів складності](#5-можливі-завдання-викладача--5-рівнів-складності)
6. [Усі патерни ООП у проєкті — де шукати](#6-усі-патерни-ооп-у-проєкті--де-шукати)
7. [Шпаргалка: "Викладач питає X → відповідай Y"](#7-шпаргалка-викладач-питає-x--відповідай-y)

---

## 1. Повна структура проєкту

```
OOP/CURSOVA/Cursova/
├── pom.xml
└── src/main/
    ├── java/all/
    │   ├── HelloApplication.java          ← головний клас (Application, сцена, ігровий цикл)
    │   ├── Launcher.java                  ← точка входу (для jar)
    │   ├── GameSerializer.java            ← збереження/завантаження (TEXT/BINARY/XML)
    │   ├── MiniMapOverlay.java            ← міні-карта (права верхня панель)
    │   │
    │   ├── Micro/                         ← рухомі об'єкти (юніти)
    │   │   ├── Unit.java                  ← абстрактна база (health, damage, move, attack)
    │   │   ├── Warrior.java               ← збирає руду, атакує, є логіка зворотного ходу
    │   │   ├── Centurio.java              ← покращений Warrior (після 100 руди)
    │   │   └── Pretorio.java              ← елітний юніт
    │   │
    │   ├── Macro/                         ← нерухомі споруди
    │   │   ├── World.java                 ← база споруд (image, health, team, life bar)
    │   │   ├── Base.java                  ← головна база (HP 200)
    │   │   ├── Tower.java                 ← вежа (лікує союзників щосекунди)
    │   │   └── Source.java                ← джерело руди
    │   │
    │   └── DialogWindows/                 ← модальні вікна JavaFX
    │       ├── UnitCreationDialog.java    ← створення юніта (Insert)
    │       ├── UnitEditDialog.java        ← редагування (ПКМ по юніту)
    │       ├── UnitSearchWindow.java      ← пошук (F)
    │       ├── UnitInventorWindow.java    ← інвентар (I)
    │       └── SerializationDialog.java   ← зберегти/завантажити (Z)
    │
    └── resources/
        ├── warrior.png, centurio.png, pretorio.png
        ├── base2.png, tower2.png, source2.png
        ├── sword.png, knife.png, spear.png, bow.png
        ├── red.png, green.png             ← мітки команд
        └── org/example/laba5/hello-view.fxml
```

**package:** `org.example.laba5` (для Hello/GameSerializer/World) + `org.example.laba5.Unit` (для Micro)

---

## 2. Коротка карта кожного класу

### 🎮 HelloApplication.java
- **package:** `org.example.laba5`
- **extends:** `Application`
- **static-поля (доступні звідусь):** `units`, `warriors`, `centurios`, `pretorios`, `buildings`, `basesA/B`, `towersA/B`, `sourcesA/B`, `imgWarrior/Centurio/Pretorio/Base/Tower/Source`, `WORLD_WIDTH=6400`, `WORLD_HEIGHT=4800`, `VIEWPORT_WIDTH/HEIGHT=1920/1080`, `group`, `scene`
- **instance-поля:** `cameraX/Y`, `cameraViewport`, `worldLayer`, `overlayPane`, `miniMapOverlay`, `unitInventorWindow`, `unitSearchWindow`
- **Головні методи:**
  - `start(Stage)` — створює сцену, юнітів, будівлі, запускає `AnimationTimer`
  - `gameLoop.handle()` — 60 FPS цикл: рух камери → оновлення будівель → логіка юнітів
  - `updateCameraPosition(x, y)` — обмежує `cameraX/Y` межами світу
  - `updateHud()` — оновлює лічильники руди/юнітів
  - `cloneActiveUnit()` — Ctrl+V: клонує виділеного юніта
  - `setOnMouseClicked` — ЛКМ: виділити юніта; ПКМ: відкрити редактор

### 🦸 Unit.java (Micro)
- **package:** `org.example.laba5.Unit`
- **implements:** `Cloneable`
- **Поля:** `health, damage, team, isSpawned, isDead, inventor`, `x, y, isActive`, `image, labelName, life, rectActive, mainWeaponImage`
- **static:** `numObjects` (лічильник створених), `objectedKilled`
- **Константи:** `moveSpeed = 1`, `ATTACK_COOLDOWN = 1000ms`
- **Головні методи:**
  - `moveTo(x, y)` — рух у точку (RTS-стиль, крок `moveSpeed`)
  - `move(dx, dy)` — зсув на вектор
  - `clampBounds()` — не виходити за `WORLD_WIDTH/HEIGHT`
  - `setCoordinates()` — синхронізувати спрайт/HP-бар/lable з `x, y`
  - `attack()` — атакує першого ворожого юніта чи будівлю в зоні `intersects`
  - `locateAndRotateF()` — розштовхує **союзних** юнітів (anti-collision, 30px min, 50px push)
  - `locateAndRotateE()` — **порожній hook** для підкласів (зброя малюється, але не обертається)
  - `resurrect()` — додає спрайти в `HelloApplication.group`
  - `removeUnitFromGame()` — прибирає з гри
  - `inventoryLogic()` — додає бонуси від предметів (sword +5 dmg, health potion +20 maxHP тощо)
  - `clone()` — поверхневе клонування + створення нових Label/ImageView

### ⚔️ Warrior.java (extends Unit)
- **Додаткові поля:** `oreAmount, activeOre (0..10), lastOreTime, collectingOre, deliveringOre, inverseMode, oreCountLabel`
- **Константа:** `COMBAT_PRIORITY_RADIUS = 180`
- **Особливі можливості:**
  - **Збір руди:** `doOre()` — знаходить `Source`, несе на `Base`, цикл
  - **Зворотний хід (V):** `inverseMode = true` — `attackInverse()` додає HP ворогам, `doOreInverse()` краде руду
  - **Просування (promotion):** при 100 руди → перетворюється на `Centurio`
  - `logic()` — AI: ворог близько → біжи + атакуй; інакше → руда
  - `logicInverse()` — те саме, але у зворотному напрямку

### 🛡️ Centurio.java (extends Warrior)
- `attack()` як у `Unit`, але сильніший (`damage` більше в конструкторі)
- перевизначає деталі спрайта/HP

### 🏛️ Pretorio.java (extends Unit)
- елітний юніт, найсильніший

### 🏰 World.java (Macro, база споруд)
- **Поля:** `x, y, image, name, numUnits, health, team, oreAmount, life, imageView, contourView`
- **static:** `allyUnits, enemyUnits`
- **Лічильники юнітів по типу:** `warriorsTeamA/B, centaursTeamA/B, pretionsTeamA/B`
- **Методи:**
  - `setHealth(double)` → `updateLifeBar()`
  - `initGraphics(img, name, ...)` — створює спрайти
  - `resurrectWorld()` — додає в `group`
  - `removeBuildingFromGame()` — зняти з гри
  - `intersect()` — перевірка зіткнень (наслідки для юнітів)

### 🏠 Base.java (extends World)
- `getOre()`, `setOre(oreAmount)` — руда гравця
- HP = 200 (див. `HelloApplication.java:234`)

### 🗼 Tower.java (extends World)
- `healUnits()` — лікує союзних юнітів у радіусі, **запускається щосекунди** через `Timeline`

### ⛏️ Source.java (extends World)
- джерело руди (HP 200)

### 💬 DialogWindows

| Клас | Відкривається | Що містить |
|---|---|---|
| `UnitCreationDialog` | Insert | Type (ComboBox), Health, Damage, Team, Inventory, Ore, Kills |
| `UnitEditDialog` | ПКМ | редагування існуючого юніта |
| `UnitSearchWindow` | F | пошук юніта |
| `UnitInventorWindow` | I | перегляд інвентарю |
| `SerializationDialog` | Z | обрати формат (TEXT/BINARY/XML) і файл |

### 💾 GameSerializer.java
- **package:** `org.example.laba5`
- **Методи:**
  - `save(File, format)` — точка входу, викликає `saveText/saveBinary/saveXml`
  - `load(File, format)` — очищає гру → `loadText/Binary/Xml` → `bringUnitsToFront`
  - `clearGame()` — очищає всі `ArrayList<>` + `group.getChildren()`
  - `bringUnitsToFront()` — піднімає юнітів над будівлями (z-order)
- **Формати:**
  - `TEXT` — `[UNIT]\ntype=Warrior\nx=100\n...` людський формат
  - `BINARY` — `ObjectOutputStream` (серіалізація Java)
  - `XML` — DOM API (`DocumentBuilder`, `Transformer`)

### 🗺️ MiniMapOverlay.java
- **package:** `org.example.laba5`
- `SCALE = 0.08` (8% від світу)
- **Поля:** `unitMarkers, buildingMarkers: IdentityHashMap<>` (Object Pool патерн)
- **Публічні:** `getPane(), bindToScene(scene), toggleVisible(), update(units, buildings), setCameraPosition, setViewportSize, navigateToWorldPoint`
- **Приватні:** `worldToMiniX/Y, miniToWorldX/Y, updateBuildings, updateUnits, updateUnitMarkerShape, updateBuildingMarkerShape`

---

## 3. Ігровий цикл — головний нерв

`HelloApplication.java:390-518` — `AnimationTimer.handle()` викликається кожен кадр (~60 FPS):

```java
handle(long now) {
    // 1. Клавіші руху камери (W/A/S/D, стрілки)
    // 2. Спеціальні клавіші (DELETE, INSERT, ESCAPE, SPACE, I, F, V)
    // 3. Здоров'я будівель: якщо ≤ 0 → removeBuildingFromGame
    // 4. updateHud()
    // 5. world.worldLogic()
    // 6. miniMapOverlay.update(units, buildings)
    // 7. building.intersect() для кожної будівлі
    // 8. for кожного юніта: move() + logic() / logicInverse()
    // 9. unitInventorWindow.updateFromUnits(units)
}
```

**Запам'ятай:** кожен кадр — рух + логіка + рендер.

---

## 4. Клавіші керування

| Клавіша | Дія |
|---|---|
| `W A S D` / стрілки | рух виділених юнітів / камери |
| `ЛКМ` по юніту | виділити (toggle) |
| `ПКМ` по юніту | відкрити `UnitEditDialog` |
| `ESC` | зняти виділення з усіх |
| `DELETE` | видалити виділених |
| `INSERT` | створити нового юніта (`UnitCreationDialog`) |
| `SPACE` | атакувати (`unit.attack()`) |
| `I` | відкрити `UnitInventorWindow` |
| `F` | відкрити `UnitSearchWindow` |
| `V` | toggle inverse mode (Warrior) |
| `Ctrl+V` | клонувати виділеного юніта |
| `Z` | відкрити `SerializationDialog` (save/load) |
| `M` | показати/сховати міні-карту |
| `Tab` | (через MiniMap) |

---

## 5. Можливі завдання викладача — 5 рівнів складності

### 🟢 Рівень 1 — Базовий (5-10 хв на виконання)

| # | Завдання | Файли | Що повториш |
|---|----------|-------|-------------|
| 1.1 | **Змінити швидкість руху юнітів** | `Unit.java:46` (`moveSpeed = 1`) | поля, конструктор, модифікатори |
| 1.2 | **Змінити дефолтне HP воїна** | `Warrior.java:99` (`MAX_HEALTH = 100`) | константи, `static final` |
| 1.3 | **Змінити дефолтний Damage** | `Warrior.java:99` (`damage = 5`) | параметри конструктора |
| 1.4 | **Змінити cooldown атаки** | `Unit.java:50` (`ATTACK_COOLDOWN = 1000`) | константи |
| 1.5 | **Змінити cooldown руди** | `Warrior.java:20` (`ORE_COOLDOWN = 1000`) | підклас, константи |
| 1.6 | **Змінити розмір світу** | `HelloApplication.java:38-39` (`WORLD_WIDTH/HEIGHT`) | static-поля |
| 1.7 | **Змінити масштаб міні-карти** | `MiniMapOverlay.java:23` (`SCALE = 0.08`) | static-константи, вплив на UI |
| 1.8 | **Змінити колір HP-бару** | `Warrior.java:111` (`life.setStroke(Color.LIGHTGREEN)`) | JavaFX `Color` |
| 1.9 | **Змінити дефолтну кількість руди для просування** | `Warrior.java:391` (`if (ore >= 100)`) | метод `promotion()`, getter |
| 1.10 | **Змінити заголовок гри** | `HelloApplication.java:36` (`APP_TITLE`) | static-константи, `Stage.setTitle` |

### 🟡 Рівень 2 — Простий код (10-20 хв)

| # | Завдання | Файли | Що повториш |
|---|----------|-------|-------------|
| 2.1 | **Додати нове поле в `Unit`** (наприклад, `armor: int`) | `Unit.java` (поле + getter/setter), `Warrior.java` (конструктор), `UnitCreationDialog.java` (TextField), `UnitEditDialog.java` (TextField), `GameSerializer.java` (обидва формати), `equals/hashCode/clone` | повний ланцюг додавання поля |
| 2.2 | **Додати перевірку валідності** у діалозі (HP 1-999, Damage 0-100) | `UnitCreationDialog.java:handleOK()` | `try-catch`, `Alert` |
| 2.3 | **Змінити колір фону вікна діалогу** | всі 5 файлів у `DialogWindows/` | CSS або `setStyle`, JavaFX |
| 2.4 | **Додати нову радіокнопку** "Neutral" у вибір команди | `UnitCreationDialog.java` (`teamBox`) | `RadioButton`, `ToggleGroup` |
| 2.5 | **Змінити дефолтну зброю** в `inventoryLogic` | `Unit.java:749-761` | `if/else`, `equalsIgnoreCase` |
| 2.6 | **Додати бонус від щита** (зменшує вхідний damage) | `Unit.java`, `Warrior.java` | нові поля, `inventoryLogic`, `takeDamage` |
| 2.7 | **Змінити форму маркера юніта на міні-карті** | `MiniMapOverlay.java:updateUnitMarkerShape()` | `Polygon.getPoints().setAll()` |
| 2.8 | **Збільшити розмір маркера бази** на міні-карті | `MiniMapOverlay.java:getBuildingMarkerSize()` | `instanceof`, поліморфізм |
| 2.9 | **Додати клавішу** для перемикання показу міні-карти | `HelloApplication.java` (`setOnKeyPressed`) | `KeyCode`, `onNavigate` |
| 2.10 | **Змінити логіку розштовхування** (більша/менша відстань) | `Unit.java:locateAndRotateF()` | `static`-прапорець `isPushing`, рекурсія |

### 🟠 Рівень 3 — Середній (20-40 хв)

| # | Завдання | Файли | Що повториш |
|---|----------|-------|-------------|
| 3.1 | **Додати новий тип юніта** "Archer" (extends `Unit`) | `Micro/Archer.java` (новий), `UnitCreationDialog.java` (ComboBox), `HelloApplication.java` (imgArcher, створення), спрайт | спадкування, абстрактні vs concrete класи |
| 3.2 | **Додати нову будівлю** "Barracks" (extends `World`) | `Macro/Barracks.java`, спрайт, `HelloApplication.java` (розміщення), `GameSerializer.java` | спадкування, серіалізація |
| 3.3 | **Реалізувати обертання зброї** до ворога | `Unit.java:locateAndRotateE()` | `Math.atan2`, `setRotate` |
| 3.4 | **Додати AI для Centurio** (патрулює, атакує, збирає) | `Centurio.java:logic()` | `override`, пошук цілей |
| 3.5 | **Зробити щоб юніти не підходили впритул один до одного** | `Unit.java:новий метод` | separation, `isPushing` |
| 3.6 | **Зробити розрахунок damage з урахуванням armor** | `Unit.java:attack()`, `Warrior.java` | композиція полів |
| 3.7 | **Змінити AI юніта щоб він тікав при low HP** | `Warrior.java:logic()` | `if (health < maxHealth / 4) flee()` |
| 3.8 | **Додати лічильник kills на спрайті** | `Centurio.java` (вже є), `Unit.java:setCoordinates()` | Label, `setText`, `setLayoutX/Y` |
| 3.9 | **Додати звук при атаці** | `Unit.java:attack()` | `AudioClip`, ресурси |
| 3.10 | **Зберегти гру автоматично при виході** | `HelloApplication.java` (`setOnCloseRequest`) | `WindowEvent`, серіалізація |

### 🔴 Рівень 4 — Складний (40-90 хв)

| # | Завдання | Файли | Що повториш |
|---|----------|-------|-------------|
| 4.1 | **Зробити AI з стейт-машиною** (IDLE → MINE → DELIVER → FIGHT → FLEE) | `Warrior.java:logic()` | enum `State`, switch |
| 4.2 | **Додати Pathfinding A*** для обходу перешкод | `World.java` (новий клас `Pathfinder`) | алгоритми, `PriorityQueue` |
| 4.3 | **Додати Multiplayer через мережу** | новий `NetworkClient.java` | сокети, серіалізація, sync |
| 4.4 | **Переробити UI на FXML + Controller** | новий `HelloController.java`, переписати `HelloApplication.java` | MVC, `FXMLLoader` |
| 4.5 | **Додати observer-патерн для подій** (юніт помер → +score) | новий `GameEventBus.java` | патерн Observer, `List<Listener>` |
| 4.6 | **Додати нову серіалізацію — JSON** | `GameSerializer.java:saveJson/loadJson` | бібліотека Gson/Jackson |
| 4.7 | **Додати гарячі клавіші для діалогів** | `UnitCreationDialog.java` | `setOnKeyPressed`, `Enter/Esc` |
| 4.8 | **Зробити Undo/Redo** для створення юніта | новий `CommandManager.java` | патерн Command |
| 4.9 | **Замінити JavaFX на Swing/інше** | весь UI | абстракція через інтерфейси |
| 4.10 | **Додати тести JUnit** | новий `src/test/java/...` | `assertEquals`, Mockito |

### ⚫ Рівень 5 — Експертний (перевірка глибини розуміння)

| # | Завдання | Концепції |
|---|----------|-----------|
| 5.1 | **Пояснити, чому `Unit implements Cloneable`, а не `extends Unit` з конструктором копіювання** | marker interface, `super.clone()` vs explicit |
| 5.2 | **Пояснити, чому `unit.move()` мутує поля, а `World.intersect()` — ні** | command-query separation |
| 5.3 | **Переробити `locateAndRotateF` через `instanceof` + visitor** | патерн Visitor, `accept(Visitor v)` |
| 5.4 | **Зробити generic-колекції типобезпечними** | `<T extends Unit> List<T> filterTeam(boolean team)` |
| 5.5 | **Пояснити SOLID-порушення у `HelloApplication` (600+ рядків)** | SRP — God Object |
| 5.6 | **Додати caching для пошуку ворогів** (`findNearbyEnemyUnit`) | Map<Unit, Enemy>, weak references |
| 5.7 | **Реалізувати multi-threaded game loop** | `Platform.runLater`, `Task`, deadlock prevention |
| 5.8 | **Замінити Timer на `ScheduledExecutorService`** | concurrency, `AtomicInteger` |
| 5.9 | **Зробити plugin system для нових типів юнітів** | reflection, `Class.forName`, ServiceLoader |
| 5.10 | **Оптимізувати рендер для 1000+ юнітів** | sprite batching, dirty rectangles |

---

## 6. Усі патерни ООП у проєкті — де шукати

| Патерн / Концепція | Клас / Метод | Пояснення |
|---|---|---|
| **Спадкування** | `Warrior extends Unit`, `Base extends World` | 3 рівні: `Unit` → `Warrior` → `Centurio` |
| **Інкапсуляція** | `Unit.health (private)`, getter/setter | пряме поле `public x, y` — порушення (пояснити) |
| **Поліморфізм** | `unit.attack()` — різна логіка в підкласах | `unit.logic()` — `Warrior.logic()` ≠ `Centurio.logic()` |
| **Перевизначення (@Override)** | `Warrior.attackInverse()`, `Unit.clone()` | `super.clone()` — поверхневе, потім глибоке |
| **Перевантаження (overload)** | `Warrior(3 конструктори)` | default / повний / з `startX, startY` |
| **Композиція** | `World` має `Image, Line, ImageView, Label` | будівля = контейнер компонентів |
| **Агрегація** | `HelloApplication.units: ArrayList<Unit>` | юніти живуть окремо від `HelloApplication` |
| **Object Pool** | `MiniMapOverlay.unitMarkers: IdentityHashMap<Unit, Polygon>` | Polygon створюється 1 раз, перевикористовується |
| **Template Method** | `Unit.locateAndRotateE() { }` (порожній hook) | підкласи можуть перевизначити |
| **Marker Interface** | `Unit implements Cloneable` | немає методів, лише дозвіл на `clone()` |
| **Static Factory** | `Warrior()` no-arg → `Warrior(MAX, true, true, 5, ...)` | `this(...)` ланцюг |
| **Builder (псевдо)** | `new Warrior(...).setOreCount(ore)` | setter після конструктора |
| **Observer (неявно)** | `scene.setOnKeyPressed(e -> {...})` | JavaFX callback = observer |
| **Strategy (через instance)** | `Warrior.isInverseMode` toggle | `logic()` vs `logicInverse()` |
| **Iterator** | `for (Unit u : units) { }` | Java foreach |
| **Stream API** | `units.stream().filter(...).collect(...)` | `HelloApplication.java:227, 418` |
| **Lambda** | `MiniMapOverlay(..., target -> { ... })` | `Consumer<double[]>` |
| **Null Object (частково)** | `getOreCountLabel() { return null; }` | в `Unit` повертає null, у `Warrior` — реальний label |
| **Snapshot pattern** | `new ArrayList<>(units)` перед ітерацією | захист від `ConcurrentModificationException` |

---

## 7. Шпаргалка: "Викладач питає X → відповідай Y"

### Про структуру
| Питання | Відповідь |
|---|---|
| "Що за гра?" | RTS у римському стилі: дві команди змагаються за руду, юніти збирають руду, вежі лікують, є просування юнітів |
| "Які основні пакети?" | `org.example.laba5` (гра) + `org.example.laba5.Unit` (юніти) |
| "Скільки класів?" | 17 Java-класів + ресурси |
| "Який головний клас?" | `HelloApplication extends Application` — точка входу, `start()`, ігровий цикл |

### Про ООП
| Питання | Відповідь + де показати |
|---|---|
| "Де поліморфізм?" | `for (Unit u : units) { u.attack(); }` — кожен підклас б'є по-своєму. Показати `HelloApplication.java:451-455` |
| "Де спадкування?" | `Warrior extends Unit`, `Base extends World`. Три рівні: Unit → Warrior → Centurio |
| "Де інкапсуляція?" | `Unit.health (private)` + getter/setter; `setHealth` додатково викликає `setCoordinates` |
| "Де абстракція?" | `Unit implements Cloneable` (marker interface); `locateAndRotateE()` — hook для підкласів |
| "Де перевизначення?" | `Warrior.clone()` → `super.clone()` + нові Label/ImageView; `Warrior.attackInverse()` — інвертована атака |
| "Де static-поля?" | `Unit.numObjects` (лічильник), `WORLD_WIDTH = 6400`, `Image imgWarrior` — спільні для всіх |
| "Де `super()`?" | `Warrior(...).super(Unit(...))` у конструкторі — ланцюг до базового |
| "Що таке `this`?" | `this.team = !this.team` у `changeTeam()` — розрізнення поля і параметра |

### Про функціонал
| Питання | Відповідь |
|---|---|
| "Як працює рух?" | `moveTo(x, y)`: якщо dist ≤ moveSpeed → телепорт, інакше крок. Класичний RTS-алгоритм |
| "Як працює атака?" | `attack()`: йде по списку юнітів, якщо `image.intersects(target.image)` — віднімає `damage` |
| "Як збирається руда?" | `Warrior.doOre()`: знаходить найближчий `Source` → несе на `Base` (10 руди максимум) |
| "Як працює серіалізація?" | 3 формати: TEXT (людський), BINARY (`ObjectOutputStream`), XML (DOM API) |
| "Як працює міні-карта?" | `MiniMapOverlay.update()` кожен кадр — перетворює координати світу на міні-координати через `worldToMiniX/Y` |
| "Як юніти не зливаються?" | `locateAndRotateF()` — separation алгоритм, штовхає союзників на 50px якщо ближче 30px |

### Про технічні рішення
| Питання | Відповідь |
|---|---|
| "Чому `static` поля в `HelloApplication`?" | Singleton-подібний доступ звідусь (юніти звертаються до `HelloApplication.units`) — антипатерн, але працює для навчального проєкту |
| "Чому `IdentityHashMap` у `MiniMapOverlay`?" | Бо `Unit.equals()` порівнює поля; два різні юніти з однаковими параметрами — різні об'єкти, потрібен `==` |
| "Чому знімок списків у `update`?" | `attack()` → `removeUnitFromGame()` змінює `units` під час ітерації; без знімка — `ConcurrentModificationException` |
| "Чому `Pane`, а не `VBox`?" | Бо маркери розміщуються вручну через `setLayoutX/Y` (обчислені математично) |
| "Чому два шари (`buildingLayer` + `unitLayer`)?" | Щоб юніти малювались над будівлями — z-order контроль |

---

## 🎯 Як використовувати цей документ перед захистом

1. **Переглянь секцію 1** — структура проєкту (5 хв)
2. **Переглянь секцію 2** — карта класів (10 хв) — згадай призначення кожного
3. **Відкрий кожен рівень завдань** секції 5 і спробуй **виконати в коді** (по 1-2 з кожного рівня)
4. **Перечитай секцію 7** перед самим захистом — це шпаргалка для миттєвих відповідей
5. **Якщо викладач дає завдання** — знайди його у секції 5, згадай які файли і які концепції

**Головне:** викладач перевіряє не "знаєш весь код напам'ять", а **розумієш структуру**, **можеш орієнтуватись** і **знаєш, де що шукати**.

Успіхів на захисті! 🛡️
