# 📖 Розгорнуті відповіді та рішення — Defense Preparation

> **Призначення:** Детальні розв'язки всіх завдань із `DefensePreparation.md` з поясненнями коду та посиланнями на файли/рядки.
> **Як використовувати:** Відкривайте паралельно з `DefensePreparation.md` — тут готові відповіді з кодом.

---

# 🟢 Рівень 1 — Базовий (готові розв'язки)

## 1.1 Змінити швидкість руху юнітів

**Файл:** `Micro/Unit.java`, рядок 46

```java
// Було:
protected double moveSpeed = 1;

// Стало (приклад — збільшити в 3 рази):
protected double moveSpeed = 3;
```

**Що повториш:** поле з модифікатором `protected` (доступне підкласам — `Warrior`, `Centurio` бачать його), `double` тип, значення за замовчуванням.

**Як перевірити:** запустити гру, виділити юніта, натиснути WASD — він рухатиметься у 3 рази швидше.

**Нюанс:** `moveSpeed` використовується в `moveTo()` (рядок 660–676). Крок руху = `moveSpeed` пікселів за кадр. Збільшення → швидше, але може "перестрибувати" повз ціль (це виправляється перевіркою `if (dist <= moveSpeed)`).

---

## 1.2 Змінити дефолтне HP воїна

**Файл:** `Micro/Warrior.java`, рядок 17

```java
// Було:
private static final double MAX_HEALTH = 100.0;

// Стало (приклад — 200):
private static final double MAX_HEALTH = 200.0;
```

**Що повториш:** `static final` — статична константа (одна на клас, не можна змінити після ініціалізації). Ім'я в UPPER_SNAKE_CASE — стандарт Java для констант.

**Як перевірити:** створити нового воїна через Insert — він матиме 200 HP.

**Нюанс:** `MAX_HEALTH` використовується в:
- конструкторі `Warrior` (рядок 99): `this((int) MAX_HEALTH, ...)`
- методі `promotion()` (рядок 391): створення Centurio з `MAX_HEALTH`

---

## 1.3 Змінити дефолтний Damage

**Файл:** `Micro/Warrior.java`, рядок 99 (no-arg конструктор)

```java
// Було:
public Warrior() {
    this((int) MAX_HEALTH, true, true, 5, false, new ArrayList<>(), 0.0, 0.0);
    //                                                    ^ damage = 5
}

// Стало (damage = 10):
this((int) MAX_HEALTH, true, true, 10, false, new ArrayList<>(), 0.0, 0.0);
```

**Що повториш:** ланцюг конструкторів через `this(...)`, передача параметрів у фінальний конструктор `Unit(...)`.

**Як перевірити:** атакувати ворога — відніматиметься 10 HP замість 5.

---

## 1.4 Змінити cooldown атаки

**Файл:** `Micro/Unit.java`, рядок 50

```java
// Було:
protected final long ATTACK_COOLDOWN = 1000;  // 1 секунда

// Стало (500 мс = 2 атаки/сек):
protected final long ATTACK_COOLDOWN = 500;
```

**Що повториш:** `long` тип для часових міток (мілісекунди з 1970), `final` поле (можна присвоїти лише в конструкторі).

**Як перевірити:** юніт атакуватиме вдвічі частіше.

**Деталь:** `lastAttackTime` (рядок 49) + `ATTACK_COOLDOWN` використовуються в `Warrior.logic()` рядок 471: `if (currentTime - lastAttackTime >= ATTACK_COOLDOWN)`.

---

## 1.5 Змінити cooldown руди

**Файл:** `Micro/Warrior.java`, рядок 20

```java
// Було:
private static final long ORE_COOLDOWN = 1000;

// Стало (2 секунди):
private static final long ORE_COOLDOWN = 2000;
```

**Що повториш:** константа в підкласі, відмінність `ORE_COOLDOWN` (збирання руди) від `ATTACK_COOLDOWN` (бій).

**Як перевірити:** Warrior збиратиме руду повільніше.

---

## 1.6 Змінити розмір світу

**Файл:** `HelloApplication.java`, рядки 38–39

```java
// Було:
public static final double WORLD_WIDTH = 6400.0;
public static final double WORLD_HEIGHT = 4800.0;

// Стало (більший світ):
public static final double WORLD_WIDTH = 8000.0;
public static final double WORLD_HEIGHT = 6000.0;
```

**Що повториш:** `public static final` публічна константа (доступна з будь-якого класу без створення об'єкта), `double` для точності.

**Як перевірити:** юніти можуть ходити далі, міні-карта автоматично розшириться (бо `MiniMapOverlay.recomputeWorldBounds` динамічно додає).

**Нюанс:** камера обмежена `WORLD_WIDTH - viewport` (див. `updateCameraPosition`, рядки 585–597).

---

## 1.7 Змінити масштаб міні-карти

**Файл:** `MiniMapOverlay.java`, рядок 23

```java
// Було:
private static final double SCALE = 0.08;  // 8% від світу

// Стало (12% — більша міні-карта):
private static final double SCALE = 0.12;
```

**Що повториш:** вплив однієї константи на весь UI (міні-карта стане більшою, маркери теж).

**Нюанс:** якщо `SCALE` завеликий — міні-карта може не влізти у вікно. Перевір `positionInCorner` (рядки 147–150).

---

## 1.8 Змінити колір HP-бару

**Файл:** `Micro/Warrior.java`, рядок 111

```java
// Було:
life.setStroke(Color.LIGHTGREEN);

// Стало (червоний):
life.setStroke(Color.RED);
```

Або через `Color.web("#hexcolor")`:
```java
life.setStroke(Color.web("#ff5555"));
```

**Що повториш:** клас `javafx.scene.paint.Color`, статичні поля-кольори (`RED, BLUE, GREEN...`), або `Color.web()` для hex.

**Як перевірити:** запустити — HP-бар воїна тепер червоний.

---

## 1.9 Змінити поріг для просування (100 руди → Centurio)

**Файл:** `Micro/Warrior.java`, рядок 391 (метод `promotion`)

```java
// Було:
@Override
protected void promotion() {
    int ore = (int) this.getOre();
    if (ore >= 100) {  // ← тут
        // ...
    }
}

// Стало (50 руди):
if (ore >= 50) { ... }
```

**Що повториш:** метод `promotion()` викликається з `logic()` (рядок 479), `getOre()` повертає `oreAmount`.

**Нюанс:** `promotion()` створює нового `Centurio` і **видаляє Warrior** (рядок 393–394). Це приклад **декорації** об'єкта в грі (зміна типу під час runtime).

---

## 1.10 Змінити заголовок гри

**Файл:** `HelloApplication.java`, рядок 36

```java
// Було:
private static final String APP_TITLE = "Roman Conquest";

// Стало:
private static final String APP_TITLE = "Roman Empire 2026";
```

**Що повториш:** `String` константа, `setTitle()` (рядок 522).

---

# 🟡 Рівень 2 — Простий код (розв'язки)

## 2.1 Додати нове поле `armor` в `Unit`

### Крок 1: `Micro/Unit.java` — додати поле + getter/setter

```java
private Integer armor;  // ← нове поле

public Integer getArmor() { return armor; }
public void setArmor(Integer armor) { this.armor = armor; }
```

### Крок 2: `Micro/Warrior.java` — ініціалізувати в конструкторі

```java
// Додати параметр у всі 3 конструктори Warrior:
public Warrior(Integer health, Boolean isSpawned, boolean team, Integer damage, 
               Boolean isDead, ArrayList<String> inventor, Integer armor,  // ← нове
               double startX, double startY) {
    super(health, isSpawned, team, damage, isDead, inventor);
    this.armor = armor;
    initGraphics(getName(), startX, startY);
}
```

### Крок 3: `DialogWindows/UnitCreationDialog.java` — додати TextField

```java
private TextField armorField;  // поле класу

// У createDialog():
HBox armorBox = new HBox(8);
armorBox.getChildren().add(new Label("Armor:"));
armorField = new TextField();
armorField.setText("0");
armorField.setTextFormatter(new TextFormatter<>(c -> {
    if (c.getControlNewText().matches("\\d*")) return c;
    return null;
}));
armorBox.getChildren().add(armorField);

// Додати в root.getChildren().addAll(...)

// У handleOK():
int armor = Integer.parseInt(armorField.getText().trim());

// У switch створення:
result = new Warrior(health, isSpawned, team, damage, false, inventor, armor, 100, 100);
```

### Крок 4: `GameSerializer.java` — додати в TEXT формат

```java
// У saveText():
pw.println("armor=" + u.getArmor());

// У loadText():
case "armor=": 
    Integer armor = Integer.parseInt(value);
    currentUnit.setArmor(armor);
    break;
```

### Крок 5: `Micro/Unit.java` — додати в `equals`/`hashCode`/`clone`

```java
// equals():
return Objects.equals(health, unit.health) && ... && Objects.equals(armor, unit.armor);

// hashCode():
return Objects.hash(health, ..., armor);
```

**Що повториш:** повний цикл додавання поля — поле, getter/setter, конструктори, UI, серіалізація, equals/hashCode. Це **найчастіше** завдання на захисті.

---

## 2.2 Валідація в діалозі (HP 1-999, Damage 0-100)

**Файл:** `DialogWindows/UnitCreationDialog.java`, метод `handleOK()` (рядок 168)

```java
private void handleOK() {
    try {
        int health = Integer.parseInt(healthField.getText().trim());
        int damage = Integer.parseInt(damageField.getText().trim());
        
        // ДОДАТИ ВАЛІДАЦІЮ:
        if (health < 1 || health > 999) {
            throw new IllegalArgumentException("Health must be 1-999");
        }
        if (damage < 0 || damage > 100) {
            throw new IllegalArgumentException("Damage must be 0-100");
        }
        
        // ... решта коду
    } catch (NumberFormatException | IllegalArgumentException ex) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("Error");
        alert.setHeaderText("Invalid input");
        alert.setContentText(ex.getMessage());
        alert.showAndWait();
    }
}
```

**Що повториш:** `try-catch` з кількома типами винятків, `Alert.AlertType.ERROR`, повідомлення про помилку.

**Нюанс:** `IllegalArgumentException` — це **unchecked** виняток (extends `RuntimeException`), не потребує `throws`.

---

## 2.3 Змінити колір фону всіх діалогів

### Спосіб А: CSS-файл (рекомендовано)

**Створи `src/main/resources/dialog-style.css`:**
```css
.root {
    -fx-background-color: #2b3a55;
    -fx-font-family: "Segoe UI";
    -fx-font-size: 13px;
}
.label { -fx-text-fill: #f0f0f0; }
.text-field { 
    -fx-background-color: #3b4a6b; 
    -fx-text-fill: white; 
}
.button { 
    -fx-background-color: #4a5d8a; 
    -fx-text-fill: white; 
}
.combo-box, .check-box, .radio-button { 
    -fx-text-fill: #f0f0f0; 
}
```

**У кожному діалозі (UnitCreationDialog, UnitEditDialog, тощо):**
```java
// Замість:
Scene scene = new Scene(root);

// Використай:
Scene scene = new Scene(root);
scene.getStylesheets().add(
    getClass().getResource("/dialog-style.css").toExternalForm()
);
```

### Спосіб Б: хардкод (тільки для одного діалогу)

```java
root.setStyle("-fx-background-color: #2b3a55; -fx-padding: 12;");
typeBox.setStyle("-fx-background-color: #2b3a55;");
healthBox.setStyle("-fx-background-color: #2b3a55;");
// ... для кожного HBox
```

**Що повториш:** CSS-синтаксис для JavaFX, селектори `.label, .button`, властивості `-fx-*`.

---

## 2.4 Додати радіокнопку "Neutral"

**Файл:** `DialogWindows/UnitCreationDialog.java`, рядки 89–97 (`teamBox`)

```java
HBox teamBox = new HBox(15);
teamBox.getChildren().add(new Label("Team:"));
ToggleGroup teamGroup = new ToggleGroup();
allyRadio = new RadioButton("Ally");
allyRadio.setToggleGroup(teamGroup);
allyRadio.setSelected(true);
RadioButton enemyRadio = new RadioButton("Enemy");
enemyRadio.setToggleGroup(teamGroup);
RadioButton neutralRadio = new RadioButton("Neutral");  // ← НОВЕ
neutralRadio.setToggleGroup(teamGroup);
teamBox.getChildren().addAll(allyRadio, enemyRadio, neutralRadio);
```

**У `handleOK()`:**
```java
boolean team;
if (allyRadio.isSelected()) team = true;
else if (enemyRadio.isSelected()) team = false;
else team = false;  // neutral = enemy? або додати enum Team {ALLY, ENEMY, NEUTRAL}
```

**Що повториш:** `ToggleGroup` гарантує, що тільки 1 радіокнопка вибрана. `RadioButton.isSelected()` — перевірка стану.

**Кращий варіант (enum):**
```java
public enum Team { ALLY, ENEMY, NEUTRAL }
private Team team = Team.ALLY;
```

---

## 2.5 Змінити дефолтну зброю в інвентарі

**Файл:** `Micro/Unit.java`, рядок 118 (default constructor)

```java
// Було:
public Unit() {
    this(100, false, true, 5, false, new ArrayList<String>(Arrays.asList("sword")));
}

// Стало (почати з лука):
this(100, false, true, 5, false, new ArrayList<String>(Arrays.asList("bow")));
```

**Що повториш:** `Arrays.asList()` створює список, обернутий у `ArrayList` (бо `asList` повертає фіксований розмір).

**Деталь:** в `inventoryLogic()` (рядки 749–761) кожна зброя має свою логіку:
- `sword` → +5 damage
- `knife` → +3 damage
- `spear` → +4 damage
- `bow` → +2 damage

---

## 2.6 Додати бонус від щита (зменшує damage)

### Крок 1: `Micro/Unit.java`

```java
// Додати поле:
private int shield = 0;

public int getShield() { return shield; }
public void setShield(int shield) { this.shield = shield; }
```

### Крок 2: `Micro/Unit.java` — модифікувати `setHealth()`

```java
public void setHealth(Integer health) {
    if (this.shield > 0 && health < this.health) {  // якщо отримуємо damage
        int absorbed = Math.min(this.shield, this.health - health);
        health += absorbed;  // відновлюємо
        this.shield -= absorbed;  // щит зношується
    }
    this.health = health;
    // ... решта
}
```

### Крок 3: `Micro/Unit.java` — додати в `inventoryLogic()`

```java
if (item.equalsIgnoreCase("shield")) {
    this.shield += 10;
}
```

**Що повториш:** модифікація поведінки через композицію (щит — окреме поле, не змінює health напряму).

---

## 2.7 Змінити форму маркера юніта на міні-карті

**Файл:** `MiniMapOverlay.java`, рядки 290–298 (`updateUnitMarkerShape`)

```java
// Було (ромб):
marker.getPoints().setAll(
    0.0, -half,
    half, 0.0,
    0.0, half,
    -half, 0.0
);

// Стало (квадрат):
marker.getPoints().setAll(
    -half, -half,
    half, -half,
    half, half,
    -half, half
);

// Або (трикутник):
marker.getPoints().setAll(
    0.0, -half,
    half, half,
    -half, half
);
```

**Що повториш:** `Polygon` — багатокутник, `getPoints().setAll(x1, y1, x2, y2, ...)` — задає вершини. Для квадрата 4 вершини, для трикутника 3.

---

## 2.8 Збільшити розмір маркера бази

**Файл:** `MiniMapOverlay.java`, рядки 280–288 (`getBuildingMarkerSize`)

```java
// Було:
if (world instanceof Base) {
    return 8.0;  // розмір маркера
}

// Стало (більше):
if (world instanceof Base) {
    return 12.0;
}
```

**Що повториш:** `instanceof` — перевірка типу під час runtime. Альтернатива — поліморфізм через віртуальний метод `getMarkerSize()` в `World`.

---

## 2.9 Додати клавішу для міні-карти (замість M)

**Файл:** `HelloApplication.java`, рядки 137–144 (обробник `setOnKeyPressed`)

```java
// Було (M):
if (code == KeyCode.M) {
    if (miniMapOverlay != null) {
        miniMapOverlay.toggleVisible();
    }
    return;
}

// Додати ще одну (Tab):
if (code == KeyCode.TAB) {
    if (miniMapOverlay != null) {
        miniMapOverlay.toggleVisible();
    }
    return;
}
```

**Що повториш:** `KeyCode.M`, `KeyCode.TAB` — enum зі стандартними клавішами, listener pattern (JavaFX events).

---

## 2.10 Змінити логіку розштовхування

**Файл:** `Micro/Unit.java`, рядки 615–655 (`locateAndRotateF`)

```java
// Було (рядки 626-627):
double minDistance = 30.0;        
double pushDistance = 50.0;       

// Стало (більша зона):
double minDistance = 60.0;        // штовхати якщо ближче 60px
double pushDistance = 80.0;       // відсувати на 80px
```

**Що повториш:** "magic numbers" → винести в константи (`static final double`).

**Нюанс:** якщо `pushDistance` дуже великий — юніти можуть "вилітати" за межі світу. Перевір `clampBounds` (рядки 559–569).

---

# 🟠 Рівень 3 — Середній (розв'язки)

## 3.1 Додати нового юніта "Archer"

### Крок 1: `Micro/Archer.java` (новий файл)

```java
package org.example.laba5.Unit;

import java.util.ArrayList;
import javafx.scene.control.Label;
import javafx.scene.image.ImageView;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;
import org.example.laba5.HelloApplication;

public class Archer extends Unit {
    private static final double MAX_HEALTH = 80.0;
    private static final int DEFAULT_DAMAGE = 8;
    private double range = 200.0;  // дальність стрільби
    
    public Archer() {
        this((int) MAX_HEALTH, true, true, DEFAULT_DAMAGE, false, new ArrayList<>(), 0.0, 0.0);
    }
    
    public Archer(Integer health, Boolean isSpawned, boolean team, Integer damage, 
                  Boolean isDead, ArrayList<String> inventor, double startX, double startY) {
        super(health, isSpawned, team, damage, isDead, inventor);
        setMaxHealth(MAX_HEALTH);
        this.labelName = new Label("Archer");
        this.life = new Line();
        this.life.setStrokeWidth(5);
        this.life.setStroke(Color.LIGHTBLUE);
        this.image = new ImageView(HelloApplication.imgArcher);
        this.image.setFitWidth(100);
        this.image.setFitHeight(100);
        this.isActive = false;
        this.rectActive = new Rectangle(x - 5, y - 5, 110, 110);
        this.rectActive.setFill(Color.TRANSPARENT);
        this.rectActive.setStrokeWidth(3);
        this.rectActive.setStroke(Color.YELLOW);
        setCoordinates();
    }
    
    @Override
    public void attack() {
        // Стріляє на відстані (не потребує intersects)
        Unit target = findNearestEnemy();
        if (target != null) {
            double dist = Math.hypot(
                target.getX() - this.x,
                target.getY() - this.y
            );
            if (dist <= range) {
                int newHealth = target.getHealth() - this.getDamage();
                target.setHealth(newHealth);
            }
        }
    }
    
    private Unit findNearestEnemy() {
        if (HelloApplication.units == null) return null;
        Unit nearest = null;
        double minDist = Double.MAX_VALUE;
        for (Unit u : HelloApplication.units) {
            if (u == this || u.getTeam() == this.team || u.getDead()) continue;
            double d = Math.hypot(u.getX() - this.x, u.getY() - this.y);
            if (d < minDist) { minDist = d; nearest = u; }
        }
        return nearest;
    }
}
```

### Крок 2: `HelloApplication.java` — додати `imgArcher`

```java
public static Image imgArcher;  // статичне поле

// У start() після завантаження спрайтів:
URL archerUrl = HelloApplication.class.getResource("/archer.png");
imgArcher = new Image(archerUrl.toExternalForm(), 100, 100, false, false);

// Створити Archer:
Archer archer1 = new Archer();
archer1.setTeam(true);
HelloApplication.units.add(archer1);
```

### Крок 3: `DialogWindows/UnitCreationDialog.java` — додати в ComboBox

```java
unitTypeCombo.getItems().addAll("Warrior", "Centurio", "Pretorio", "Archer");

// У switch:
case "Archer":
    result = new Archer(health, isSpawned, team, damage, false, inventor, 100, 100);
    break;
```

### Крок 4: Додати `archer.png` у `src/main/resources/`

**Що повториш:** спадкування, `super()` в конструкторі, override методу, додавання до колекції, реєстрація в ComboBox.

---

## 3.2 Додати будівлю "Barracks"

### `Macro/Barracks.java` (новий)

```java
package org.example.laba5;

import javafx.scene.image.Image;

public class Barracks extends World {
    public Barracks() {
        super();
    }
    
    @Override
    protected void initGraphics(Image image, String name, int numUnits, 
                                int x, int y, double maxHealth, double size) {
        super.initGraphics(image, name, numUnits, x, y, maxHealth, size);
        this.health = 300.0;  // більше HP
    }
    
    // Може мати спеціальну логіку — спавн юнітів
    public Unit spawnUnit(String type) {
        // Створити юніта на позиції Barracks
        // ...
        return null;
    }
}
```

### `HelloApplication.java` — додати Barracks

```java
// Поле:
public static ArrayList<Barracks> barracks = new ArrayList<>();

// Створення:
Barracks barracks1 = new Barracks();
barracks1.setTeam(true);
barracks1.initGraphics(imgBarracks, "barracks1", 0, 1500, 1500, 300.0, 200);
barracks1.resurrectWorld();
buildings.add(barracks1);
```

### `MiniMapOverlay.java` — додати форму для Barracks

```java
private double getBuildingMarkerSize(World world) {
    if (world instanceof Base) return 8.0;
    if (world instanceof Tower) return 6.0;
    if (world instanceof Barracks) return 7.0;  // ← нове
    return 5.6;
}
```

### `GameSerializer.java` — додати в серіалізацію

```java
// У saveText/buildings:
pw.println("type=" + w.getClass().getSimpleName());  // автоматично підхопить "Barracks"

// У loadText:
if (type.equals("Barracks")) {
    building = new Barracks();
}
```

**Що повториш:** спадкування споруд, реєстрація в `buildings`, додавання в серіалізацію.

---

## 3.3 Обертання зброї до ворога

**Файл:** `Micro/Unit.java`, рядки 657–658 (порожній `locateAndRotateE`)

```java
@Override
public void locateAndRotateE() {
    if (mainWeaponImage == null || image == null) return;
    
    // Знайти найближчого ворога
    Unit target = findNearestEnemy();
    if (target == null) return;
    
    // Обчислити кут
    double dx = target.getX() - this.x;
    double dy = target.getY() - this.y;
    double angle = Math.atan2(dy, dx);  // радіани
    
    // Повернути спрайт зброї
    mainWeaponImage.setRotate(Math.toDegrees(angle) + 90);  // +90 бо спрайт "дивиться вгору"
}

private Unit findNearestEnemy() {
    if (HelloApplication.units == null) return null;
    Unit nearest = null;
    double minDist = Double.MAX_VALUE;
    for (Unit u : HelloApplication.units) {
        if (u == this || u.getTeam() == this.team || Boolean.TRUE.equals(u.getDead())) continue;
        double d = Math.hypot(u.getX() - this.x, u.getY() - this.y);
        if (d < minDist) { minDist = d; nearest = u; }
    }
    return nearest;
}
```

**Що повториш:** `Math.atan2(dy, dx)` — кут від осі X до вектора (в радіанах), `Math.toDegrees()` — перетворення в градуси, `setRotate()` — обертання `Node` в JavaFX.

**Нюанс:** `+90` компентує початкову орієнтацію спрайта зброї. Якщо спрайт "дивиться вправо" — без `+90`.

---

## 3.4 AI для Centurio

**Файл:** `Micro/Centurio.java` — додати метод `logic()`

```java
@Override
public void logic() {
    // 1. Знайти ворога поблизу
    Unit enemy = findNearbyEnemy();
    
    if (enemy != null) {
        // 2. Якщо далеко — біжи до нього
        double dist = Math.hypot(enemy.getX() - this.x, enemy.getY() - this.y);
        if (dist > 70) {
            this.moveTo(enemy.getX() - image.getFitWidth()/2, 
                       enemy.getY() - image.getFitHeight()/2);
        } else {
            // 3. Близько — атакуй
            long now = System.currentTimeMillis();
            if (now - lastAttackTime >= ATTACK_COOLDOWN) {
                attack();
                lastAttackTime = now;
            }
        }
    } else {
        // 4. Немає ворогів — патрулюй (рухайся до випадкової точки)
        double randomX = Math.random() * 6000;
        double randomY = Math.random() * 4000;
        if (!isActive()) {
            this.moveTo(randomX, randomY);
        }
    }
}

private Unit findNearbyEnemy() {
    if (HelloApplication.units == null) return null;
    Unit nearest = null;
    double minDist = 200;  // радіус пошуку
    for (Unit u : HelloApplication.units) {
        if (u == this || u.getTeam() == this.team || Boolean.TRUE.equals(u.getDead())) continue;
        double d = Math.hypot(u.getX() - this.x, u.getY() - this.y);
        if (d < minDist) { minDist = d; nearest = u; }
    }
    return nearest;
}
```

**Що повториш:** простий AI (state machine в одному методі), `Math.random()` для випадковості, distance-based поведінка.

---

## 3.5 Юніти не підходять впритул

**Файл:** `Micro/Unit.java` — додати новий метод

```java
public void locateAndRotateG() {
    if (HelloApplication.units == null || image == null) return;
    double selfCenterX = x + image.getFitWidth() / 2.0;
    double selfCenterY = y + image.getFitHeight() / 2.0;
    double minDist = 50.0;
    
    for (Unit unit : HelloApplication.units) {
        if (unit == this || unit.image == null) continue;
        if (unit.getTeam() == this.team) continue;  // тільки ворогів
        double otherCenterX = unit.x + unit.image.getFitWidth() / 2.0;
        double otherCenterY = unit.y + unit.image.getFitHeight() / 2.0;
        double dx = otherCenterX - selfCenterX;
        double dy = otherCenterY - selfCenterY;
        double dist = Math.hypot(dx, dy);
        
        if (dist < minDist && dist > 0.001) {
            // Зупинити юніта, не даючи наблизитись
            double newX = selfCenterX + (dx / dist) * minDist;
            double newY = selfCenterY + (dy / dist) * minDist;
            // Замість moveTo — просто оновити x, y напряму
            this.x = newX - image.getFitWidth() / 2.0;
            this.y = newY - image.getFitHeight() / 2.0;
            this.setCoordinates();
        }
    }
}
```

**Викликати в `move()`:**
```java
public void move(double dx, double dy) {
    x += dx;
    y += dy;
    clampBounds();
    locateAndRotateG();  // ← ДО setCoordinates
    setCoordinates();
    locateAndRotateF();
    locateAndRotateE();
}
```

**Що повториш:** separation behavior (один із steering behaviors у ігровому AI), відстань замість перетину.

---

## 3.6 Damage з урахуванням armor

**Файл:** `Micro/Unit.java`, метод `attack()` (рядки 400–438)

```java
public void attack() {
    // ...
    if (intersects) {
        int targetHealth = unit.getHealth() == null ? 0 : unit.getHealth();
        int rawDamage = this.damage;
        int armor = unit.getArmor() == null ? 0 : unit.getArmor();
        int actualDamage = Math.max(1, rawDamage - armor);  // мінімум 1
        int newHealth = targetHealth - actualDamage;
        unit.setHealth(newHealth);
        // ...
    }
}
```

**Що повториш:** `Math.max(a, b)`, композиція полів (`armor` як окрема характеристика).

---

## 3.7 AI: тікати при low HP

**Файл:** `Micro/Warrior.java`, `logic()` (рядки 444–480)

```java
@Override
public void logic() {
    // ДОДАТИ:
    int health = this.getHealth() == null ? 0 : this.getHealth();
    double maxHP = this.getMaxHealth();
    
    if (health < maxHP / 4) {  // якщо менше 25% HP
        // Тікати до бази
        World base = getTeamBase();
        if (base != null) {
            double bx = base.x + (base.imageView != null ? base.imageView.getFitWidth()/2 : 100);
            double by = base.y + (base.imageView != null ? base.imageView.getFitHeight()/2 : 100);
            this.moveTo(bx, by);
            return;  // не атакувати, не збирати руду
        }
    }
    
    // ... решта logic
}
```

**Що повториш:** state-based AI (стани "FLEE" / "FIGHT" / "MINE"), умовна поведінка.

---

## 3.8 Лічильник kills на спрайті

**Файл:** `Micro/Centurio.java` — аналогічно `Warrior.oreCountLabel`

```java
private Label killCountLabel;

@Override
public void resurrect() {
    super.resurrect();
    if (killCountLabel != null && HelloApplication.group != null) {
        HelloApplication.group.getChildren().add(killCountLabel);
    }
}

@Override
public void setCoordinates() {
    super.setCoordinates();
    if (killCountLabel != null) {
        killCountLabel.setText("Kills: " + killCount);
        killCountLabel.setLayoutX(x + 65);
        killCountLabel.setLayoutY(y - 25);  // вище за HP-бар
    }
}
```

**Що повториш:** `Label` на сцені, оновлення тексту, `setLayoutX/Y`.

---

## 3.9 Звук при атаці

**Файл:** `Micro/Unit.java`

```java
import javafx.scene.media.AudioClip;
import java.net.URL;

// Додати поле:
private AudioClip attackSound;

// У конструкторі:
URL soundUrl = getClass().getResource("/attack.wav");
if (soundUrl != null) {
    attackSound = new AudioClip(soundUrl.toExternalForm());
}

// У attack():
if (intersects) {
    if (attackSound != null) {
        attackSound.play();
    }
    // ... бій
}
```

**Файли:** покласти `attack.wav` у `src/main/resources/`.

**Що повториш:** `AudioClip` (короткі звуки в JavaFX), відтворення через `.play()`.

---

## 3.10 Автозбереження при виході

**Файл:** `HelloApplication.java`, `start()`

```java
stage.setOnCloseRequest(event -> {
    try {
        File autosave = new File("autosave.txt");
        GameSerializer.save(autosave, "TEXT");
    } catch (Exception e) {
        e.printStackTrace();
    }
});
```

**Що повториш:** `WindowEvent` (обробка подій вікна), `File` API, `GameSerializer`.

---

# 🔴 Рівень 4 — Складний (розв'язки)

## 4.1 AI з state machine

**Файл:** `Micro/Warrior.java`

```java
private enum State { IDLE, MINE, DELIVER, FIGHT, FLEE }

private State state = State.IDLE;

@Override
public void logic() {
    int health = getHealth() == null ? 0 : getHealth();
    double maxHP = getMaxHealth();
    
    // State transitions
    Unit enemy = findNearbyEnemyUnit();
    
    if (health < maxHP / 4) {
        state = State.FLEE;
    } else if (enemy != null) {
        state = State.FIGHT;
    } else if (activeOre >= 10) {
        state = State.DELIVER;
    } else {
        state = State.MINE;
    }
    
    // State actions
    switch (state) {
        case IDLE:
            // нічого не робити
            break;
        case MINE:
            doOre();
            break;
        case DELIVER:
            deliverOre();
            break;
        case FIGHT:
            attackEnemy(enemy);
            break;
        case FLEE:
            fleeToBase();
            break;
    }
    
    promotion();
}
```

**Що повториш:** enum, state machine, switch по станах, transitions vs actions.

---

## 4.2 Pathfinding A*

**Файл:** новий `Micro/Pathfinder.java`

```java
package org.example.laba5.Unit;

import java.util.*;

public class Pathfinder {
    private static final int GRID_SIZE = 50;  // клітинка 50x50 пікселів
    
    public static List<double[]> findPath(double startX, double startY, 
                                          double goalX, double goalY,
                                          List<double[]> obstacles) {
        Node start = new Node((int)(startX/GRID_SIZE), (int)(startY/GRID_SIZE));
        Node goal = new Node((int)(goalX/GRID_SIZE), (int)(goalY/GRID_SIZE));
        
        PriorityQueue<Node> open = new PriorityQueue<>(Comparator.comparingDouble(n -> n.f));
        Set<String> closed = new HashSet<>();
        Map<String, Node> allNodes = new HashMap<>();
        
        start.g = 0;
        start.h = heuristic(start, goal);
        start.f = start.g + start.h;
        open.add(start);
        allNodes.put(key(start), start);
        
        while (!open.isEmpty()) {
            Node current = open.poll();
            if (current.x == goal.x && current.y == goal.y) {
                return reconstructPath(current);
            }
            closed.add(key(current));
            
            for (Node neighbor : getNeighbors(current)) {
                if (closed.contains(key(neighbor))) continue;
                if (isObstacle(neighbor, obstacles)) continue;
                
                double tentativeG = current.g + 1;
                if (tentativeG < neighbor.g) {
                    neighbor.parent = current;
                    neighbor.g = tentativeG;
                    neighbor.h = heuristic(neighbor, goal);
                    neighbor.f = neighbor.g + neighbor.h;
                    if (!open.contains(neighbor)) open.add(neighbor);
                }
            }
        }
        return Collections.emptyList();  // шлях не знайдено
    }
    
    private static double heuristic(Node a, Node b) {
        return Math.hypot(a.x - b.x, a.y - b.y);  // Евклідова відстань
    }
    
    // ... допоміжні методи (Node, getNeighbors, isObstacle, reconstructPath)
}
```

**Що повториш:** A* алгоритм, `PriorityQueue` (мін-купа), `Comparator`, евристика.

---

## 4.3 Multiplayer

**Файл:** новий `NetworkClient.java`

```java
import java.io.*;
import java.net.*;

public class NetworkClient {
    private Socket socket;
    private ObjectOutputStream out;
    private ObjectInputStream in;
    
    public void connect(String host, int port) throws IOException {
        socket = new Socket(host, port);
        out = new ObjectOutputStream(socket.getOutputStream());
        in = new ObjectInputStream(socket.getInputStream());
    }
    
    public void sendUnitState(Unit unit) throws IOException {
        out.writeObject(unit);
        out.flush();
    }
    
    public Unit receiveUnitState() throws IOException, ClassNotFoundException {
        return (Unit) in.readObject();
    }
}
```

**Сервер** — окремий потік, слухає `ServerSocket`, синхронізує стани.

**Що повториш:** `Socket`, `ObjectOutputStream` (Java серіалізація поверх TCP), `Serializable`.

---

## 4.4 FXML + Controller (MVC)

**Файл:** `src/main/resources/org/example/laba5/hello-view.fxml`

```xml
<?xml version="1.0" encoding="UTF-8"?>
<?import javafx.scene.layout.Pane?>
<?import javafx.scene.control.Button?>

<Pane xmlns:fx="http://javafx.com/fxml" 
      fx:controller="org.example.laba5.HelloController"
      prefWidth="1920" prefHeight="1080">
    <Button text="Add Warrior" onAction="#onAddWarrior" 
            layoutX="10" layoutY="10"/>
    <Pane fx:id="worldLayer"/>
</Pane>
```

**Файл:** `HelloController.java`

```java
public class HelloController {
    @FXML private Pane worldLayer;
    
    @FXML
    public void onAddWarrior(ActionEvent event) {
        Warrior w = new Warrior();
        // додати в worldLayer
    }
}
```

**У HelloApplication.java:**
```java
FXMLLoader loader = new FXMLLoader(getClass().getResource("hello-view.fxml"));
Parent root = loader.load();
```

**Що повториш:** MVC-патерн, `FXMLLoader`, `@FXML` анотація, `fx:id`/`fx:controller` у FXML.

---

## 4.5 Observer (GameEventBus)

**Файл:** новий `GameEventBus.java`

```java
import java.util.*;

public class GameEventBus {
    private static final Map<String, List<Runnable>> listeners = new HashMap<>();
    
    public static void subscribe(String eventType, Runnable listener) {
        listeners.computeIfAbsent(eventType, k -> new ArrayList<>()).add(listener);
    }
    
    public static void publish(String eventType) {
        List<Runnable> list = listeners.get(eventType);
        if (list != null) for (Runnable r : list) r.run();
    }
}

// Використання:
// Unit помер:
GameEventBus.publish("unit_killed");

// Підписка:
GameEventBus.subscribe("unit_killed", () -> {
    score++;
});
```

**Що повториш:** патерн Observer, decoupling (підписувачі не знають про видавця).

---

## 4.6 JSON серіалізація (Gson)

**pom.xml:**
```xml
<dependency>
    <groupId>com.google.code.gson</groupId>
    <artifactId>gson</artifactId>
    <version>2.10.1</version>
</dependency>
```

**GameSerializer.java:**
```java
import com.google.gson.*;

public static void saveJson(File file) throws IOException {
    Gson gson = new GsonBuilder().setPrettyPrinting().create();
    GameState state = new GameState();
    state.units = HelloApplication.units;
    state.buildings = HelloApplication.buildings;
    try (FileWriter w = new FileWriter(file)) {
        gson.toJson(state, w);
    }
}
```

**Що повториш:** зовнішні бібліотеки (Maven), `Gson` API, серіалізація об'єктів.

---

## 4.7 Гарячі клавіші в діалозі

**Файл:** `DialogWindows/UnitCreationDialog.java`, `createDialog()`

```java
// Додати перед stage.setScene(scene):
root.setOnKeyPressed(event -> {
    if (event.getCode() == KeyCode.ENTER) {
        handleOK();
    } else if (event.getCode() == KeyCode.ESCAPE) {
        handleCancel();
    }
});
```

**Нюанс:** root.focusedProperty має бути true. Або додати `root.requestFocus()` після показу.

**Що повториш:** події клавіатури в JavaFX, `KeyCode.ENTER` / `ESCAPE`.

---

## 4.8 Undo/Redo (Command pattern)

**Файл:** `CommandManager.java`

```java
import java.util.*;

public class CommandManager {
    private Deque<Command> undoStack = new ArrayDeque<>();
    private Deque<Command> redoStack = new ArrayDeque<>();
    
    public void execute(Command c) {
        c.execute();
        undoStack.push(c);
        redoStack.clear();
    }
    
    public void undo() {
        if (!undoStack.isEmpty()) {
            Command c = undoStack.pop();
            c.undo();
            redoStack.push(c);
        }
    }
    
    public void redo() {
        if (!redoStack.isEmpty()) {
            Command c = redoStack.pop();
            c.execute();
            undoStack.push(c);
        }
    }
}

interface Command {
    void execute();
    void undo();
}

// Приклад: створення юніта
class CreateUnitCommand implements Command {
    private Unit unit;
    public void execute() { HelloApplication.units.add(unit); unit.resurrect(); }
    public void undo() { unit.removeUnitFromGame(); }
}
```

**Що повториш:** патерн Command, `Deque` (двостороння черга), LIFO стек.

---

## 4.9 Заміна JavaFX на Swing

**Крок:** визначити інтерфейс `IRenderer` з методами `drawImage, drawLabel, drawLine`, зробити `JavaFXRenderer` та `SwingRenderer`. Вся графіка викликає `renderer.drawImage(...)`.

**Що повториш:** абстракція через інтерфейси, **Dependency Inversion** (SOLID-D).

---

## 4.10 JUnit тести

**pom.xml:**
```xml
<dependency>
    <groupId>org.junit.jupiter</groupId>
    <artifactId>junit-jupiter</artifactId>
    <version>5.10.0</version>
    <scope>test</scope>
</dependency>
```

**`src/test/java/UnitTest.java`:**
```java
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class UnitTest {
    @Test
    public void testMoveTo() {
        Unit u = new Warrior();
        u.setPosition(100, 100);
        u.moveTo(200, 200);
        assertEquals(200, u.getX(), 5);  // допуск 5px
    }
    
    @Test
    public void testTakeDamage() {
        Unit u = new Warrior();
        int hp = u.getHealth();
        u.takeDamage();
        assertEquals(hp - 10, u.getHealth());
    }
    
    @Test
    public void testPromotion() {
        Warrior w = new Warrior();
        w.setOreCount(100);
        w.promotion();
        // Перевірити, що з'явився Centurio
        assertTrue(HelloApplication.units.stream()
            .anyMatch(u -> u instanceof Centurio));
    }
}
```

**Що повториш:** JUnit 5, `assertEquals/assertTrue`, `@Test`, AAA (Arrange-Act-Assert).

---

# ⚫ Рівень 5 — Експертний (розгорнуті відповіді)

## 5.1 Чому `Cloneable`, а не копіюючий конструктор?

**Marker Interface `Cloneable`:**
- не має методів, лише "дозволяє" `super.clone()`
- Java робить **поверхневе** клонування (поля примітивів копіюються, посилання — ті ж)
- `Object.clone()` — `protected`, тому треба override з `public`
- Працює через **механізм рефлексії** JVM

**Копіюючий конструктор:**
```java
public Warrior(Warrior other) {
    super(other);
    this.oreAmount = other.oreAmount;
    // ...
}
```

**Переваги `Cloneable`:** працює через `super.clone()` — поліморфно (Unit → Warrior → Centurio).
**Неділікати:** складно контролювати глибину, не працює з final-полями.

**Висновок для захисту:** у проєкті `Cloneable` обрано для підтримки `cloneActiveUnit()` (рядок 554 у `HelloApplication.java:570`).

---

## 5.2 Command-Query Separation

**`unit.move(dx, dy)` — command:**
- змінює стан (x, y)
- повертає `void`
- побічний ефект

**`World.intersect()` — query:**
- перевіряє зіткнення
- не змінює стан
- повертає boolean/result

**CQS принцип:** метод або змінює стан, або повертає значення, але не обидва. Це полегшує розуміння коду.

**У проєкті:** `attack()` мутує (зменшує HP), `getHealth()` — чистий query. Розділення правильне.

---

## 5.3 Visitor pattern

**Заміна `instanceof` ланцюгів:**

```java
interface UnitVisitor {
    void visit(Warrior w);
    void visit(Centurio c);
    void visit(Pretorio p);
}

abstract class Unit {
    public abstract void accept(UnitVisitor v);
}

class Warrior extends Unit {
    @Override
    public void accept(UnitVisitor v) { v.visit(this); }
}

// Використання:
unit.accept(new UnitVisitor() {
    public void visit(Warrior w) { /* warrior-specific */ }
    public void visit(Centurio c) { /* ... */ }
    public void visit(Pretorio p) { /* ... */ }
});
```

**Переваги:** уникає `instanceof`, додавання нового типу = новий метод visitor, а не новий `if`.

---

## 5.4 Generic колекції

```java
public static <T extends Unit> List<T> filterTeam(List<T> units, boolean team) {
    return units.stream()
        .filter(u -> u.getTeam() == team)
        .collect(Collectors.toList());
}

// Використання:
List<Warrior> allyWarriors = filterTeam(warriors, true);
```

**Що повториш:** generics, bounded types (`<T extends Unit>`), `Stream API`.

---

## 5.5 SOLID — God Object

`HelloApplication.java` має 600+ рядків і робить:
1. Ініціалізацію сцени
2. Ігровий цикл
3. Обробку вводу
4. Управління камерою
5. Створення юнітів
6. Збереження/завантаження
7. Оновлення HUD

**Це порушення SRP** (Single Responsibility Principle).

**Виправлення:** розділити на:
- `GameInitializer` — створення сцени, юнітів
- `GameLoop` — ігровий цикл
- `InputHandler` — клавіатура/миша
- `CameraController` — камера

---

## 5.6 Caching пошуку ворогів

```java
private Map<Unit, Unit> cachedEnemies = new WeakHashMap<>();
private long cacheTime = 0;
private static final long CACHE_DURATION = 100;  // 100мс

private Unit findNearbyEnemyUnit() {
    long now = System.currentTimeMillis();
    if (now - cacheTime < CACHE_DURATION && cachedEnemies.containsKey(this)) {
        return cachedEnemies.get(this);
    }
    // ... звичайний пошук ...
    cachedEnemies.put(this, result);
    cacheTime = now;
    return result;
}
```

**Що повториш:** `WeakHashMap` (автоматичне видалення коли об'єкт більше не використовується), TTL cache.

---

## 5.7 Multi-threaded game loop

```java
ExecutorService executor = Executors.newFixedThreadPool(4);

// AI в окремому потоці:
executor.submit(() -> {
    for (Unit u : units) {
        u.logic();  // ⚠️ НЕ можна змінювати UI тут
    }
});

// UI оновлення — в JavaFX Application Thread:
Platform.runLater(() -> {
    updateHud();
});
```

**Проблеми:** deadlock (UI thread чекає AI), race conditions (список юнітів змінюється під час ітерації).

**Рішення:** immutable data transfer, lock-free structures, `CopyOnWriteArrayList`.

---

## 5.8 ScheduledExecutorService

**Заміна `Timeline` у `HelloApplication.java:259`:**

```java
ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(2);
scheduler.scheduleAtFixedRate(() -> {
    Platform.runLater(() -> tower1.healUnits());
}, 0, 1, TimeUnit.SECONDS);
```

**Переваги:** пул потоків, контрольований lifecycle, може бути скасований (`scheduler.shutdown()`).

---

## 5.9 Plugin System (Reflection)

```java
public Unit createUnit(String className, Object... args) throws Exception {
    Class<?> clazz = Class.forName(className);
    Constructor<?> ctor = clazz.getConstructor(...);
    return (Unit) ctor.newInstance(args);
}

// Використання:
Unit u = createUnit("org.example.laba5.Unit.Archer", 100, true, true, 5, false, inventor, 0.0, 0.0);
```

**Або `ServiceLoader`:**
```java
ServiceLoader<UnitPlugin> loader = ServiceLoader.load(UnitPlugin.class);
for (UnitPlugin p : loader) p.register();
```

**Що повториш:** reflection (`Class.forName`, `Constructor.newInstance`), `ServiceLoader` (SPI механізм Java).

---

## 5.10 Оптимізація рендеру

**Проблема:** 1000+ юнітів = 1000+ `Node` = повільно.

**Рішення 1: Sprite Batching**
- один великий `Image` (атлас спрайтів)
- один `Canvas` замість `ImageView[]`
- `drawImage()` в циклі

**Рішення 2: Dirty Rectangles**
- не перемальовувати все, тільки змінені ділянки

```java
Canvas canvas = new Canvas(WORLD_WIDTH, WORLD_HEIGHT);
GraphicsContext gc = canvas.getGraphicsContext2D();

gameLoop.handle(now) -> {
    gc.clearRect(0, 0, WORLD_WIDTH, WORLD_HEIGHT);
    for (Unit u : units) {
        gc.drawImage(u.getImage(), u.x, u.y, 100, 100);
    }
};
```

**Що повториш:** `Canvas` API, `GraphicsContext`, performance optimization.

---

# 🎓 Загальні поради для захисту

1. **Перед захистом:** пройди по секції 2 (`DefensePreparation.md`) — згадай призначення кожного класу.
2. **Якщо дають завдання:** знайди його у секції 5, згадай які файли і які концепції, виконай у коді.
3. **Під час відповіді:** кажи **"де саме"** в коді (файл + рядок), це показує розуміння.
4. **Якщо не знаєш:** чесно скажи "я б зробив ось так", навіть якщо не впевнений.
5. **Ключові фрази для викладача:**
   - "Поліморфізм тут реалізовано через `extends` та `@Override`"
   - "Інкапсуляція через `private` поля + getter/setter"
   - "Object Pool у `MiniMapOverlay` уникає створення нових `Polygon` кожен кадр"
   - "Snapshot pattern захищає від `ConcurrentModificationException`"

**Успіхів! 🛡️**
