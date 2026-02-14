import java.util.Scanner;

/**
 * Лабораторна робота N2: Класи та об'єкти
 * Тема: Ball's War
 * Студент: Коваль Олександр Дмитрович
 * Група: 2ПІ-25Б
 */

// Базовий клас мікрооб'єкту
class Unit {
    private String name;
    private int level;
    private double strength;
    private int health;

    // Статичний блок ініціалізації
    static {
        System.out.println("[STATIC BLOCK] Unit класс завантажено в пам'ять");
    }

    // Нестатичний блок ініціалізації
    {
        System.out.println("[INIT BLOCK] Ініціалізація об'єкту класу Unit");
    }

    // Конструктор з параметрами
    public Unit(String name, int level, double strength, int health) {
        this.name = name;
        this.level = level;
        this.strength = strength;
        this.health = health;
        System.out.println("[CONSTRUCTOR] Unit створений: " + name);
    }

    // Конструктор без параметрів (делегування)
    public Unit() {
        this("Unknown Unit", 1, 5.0, 100);
        System.out.println("[DEFAULT CONSTRUCTOR] Використано конструктор без параметрів");
    }

    // Getters та Setters
    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getLevel() {
        return level;
    }

    public void setLevel(int level) {
        this.level = level;
    }

    public double getStrength() {
        return strength;
    }

    public void setStrength(double strength) {
        this.strength = strength;
    }

    public int getHealth() {
        return health;
    }

    public void setHealth(int health) {
        this.health = health;
    }

    // Методи життєдіяльності
    public void work() {
        System.out.println(name + " виконує роботу...");
    }

    public void rest() {
        health = Math.min(health + 20, 100);
        System.out.println(name + " відпочиває. Здоров'я: " + health);
    }

    public void train() {
        strength += 1.5;
        level++;
        System.out.println(name + " тренується. Новий рівень: " + level + ", Сила: " + strength);
    }

    public void takeDamage(double damage) {
        health -= (int) damage;
        if (health < 0) health = 0;
        System.out.println(name + " отримав урон: " + damage + ". Здоров'я: " + health);
    }

    // Метод print
    public void print() {
        System.out.println("═════════════════════════════════════");
        System.out.println("| Unit: " + name);
        System.out.println("| Рівень: " + level);
        System.out.println("| Сила: " + String.format("%.2f", strength));
        System.out.println("| Здоров'я: " + health);
        System.out.println("═════════════════════════════════════");
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Unit unit = (Unit) o;
        return level == unit.level && 
               Double.compare(unit.strength, strength) == 0 && 
               health == unit.health && 
               name.equals(unit.name);
    }

    @Override
    public String toString() {
        return "Unit{" +
                "name='" + name + '\'' +
                ", level=" + level +
                ", strength=" + String.format("%.2f", strength) +
                ", health=" + health +
                '}';
    }
}

// Клас Worker (Шахтар) - рівень 1
class Worker extends Unit {
    private double miningSpeed;

    static {
        System.out.println("[STATIC BLOCK] Worker класс завантажено");
    }

    {
        System.out.println("[INIT BLOCK] Ініціалізація Worker");
    }

    public Worker(String name, int level, double strength, int health, double miningSpeed) {
        super(name, level, strength, health);
        this.miningSpeed = miningSpeed;
        System.out.println("[WORKER CONSTRUCTOR] Шахтар " + name + " готовий до роботи");
    }

    public Worker() {
        this("Шахтар", 1, 3.0, 80, 5.0);
    }

    public double getMiningSpeed() {
        return miningSpeed;
    }

    public void setMiningSpeed(double miningSpeed) {
        this.miningSpeed = miningSpeed;
    }

    public void extractOre() {
        double ore = miningSpeed * getStrength();
        System.out.println(getName() + " добував руду. Кількість: " + String.format("%.2f", ore));
    }

    public void collectResources() {
        System.out.println(getName() + " збирає ресурси зі швидкістю " + miningSpeed);
    }

    @Override
    public void print() {
        System.out.println("═════════════════════════════════════");
        System.out.println("| WORKER - " + getName());
        System.out.println("| Рівень: " + getLevel());
        System.out.println("| Сила: " + String.format("%.2f", getStrength()));
        System.out.println("| Здоров'я: " + getHealth());
        System.out.println("| Швидкість видобування: " + String.format("%.2f", miningSpeed));
        System.out.println("═════════════════════════════════════");
    }

    @Override
    public String toString() {
        return "Worker{" +
                "name='" + getName() + '\'' +
                ", level=" + getLevel() +
                ", miningSpeed=" + String.format("%.2f", miningSpeed) +
                '}';
    }
}

// Клас Warrior (Воїн) - рівень 2
class Warrior extends Unit {
    private double attackDamage;

    static {
        System.out.println("[STATIC BLOCK] Warrior класс завантажено");
    }

    {
        System.out.println("[INIT BLOCK] Ініціалізація Warrior");
    }

    public Warrior(String name, int level, double strength, int health, double attackDamage) {
        super(name, level, strength, health);
        this.attackDamage = attackDamage;
        System.out.println("[WARRIOR CONSTRUCTOR] Воїн " + name + " готовий до бою");
    }

    public Warrior() {
        this("Воїн", 2, 8.0, 120, 15.0);
    }

    public double getAttackDamage() {
        return attackDamage;
    }

    public void setAttackDamage(double attackDamage) {
        this.attackDamage = attackDamage;
    }

    public void attack(Unit target) {
        double totalDamage = attackDamage * (getStrength() / 5.0);
        System.out.println(getName() + " атакує " + target.getName() + " з силою " + String.format("%.2f", totalDamage));
        target.takeDamage(totalDamage);
    }

    public void defend() {
        System.out.println(getName() + " приймає оборонну позицію");
    }

    @Override
    public void print() {
        System.out.println("═════════════════════════════════════");
        System.out.println("| WARRIOR - " + getName());
        System.out.println("| Рівень: " + getLevel());
        System.out.println("| Сила: " + String.format("%.2f", getStrength()));
        System.out.println("| Здоров'я: " + getHealth());
        System.out.println("| Урон атаки: " + String.format("%.2f", attackDamage));
        System.out.println("═════════════════════════════════════");
    }

    @Override
    public String toString() {
        return "Warrior{" +
                "name='" + getName() + '\'' +
                ", level=" + getLevel() +
                ", attackDamage=" + String.format("%.2f", attackDamage) +
                '}';
    }
}

// Клас Centurion (Центуріон - Воїн ІІ) - рівень 3
class Centurion extends Warrior {
    private int commanderSkill;

    static {
        System.out.println("[STATIC BLOCK] Centurion класс завантажено");
    }

    {
        System.out.println("[INIT BLOCK] Ініціалізація Centurion");
    }

    public Centurion(String name, int level, double strength, int health, double attackDamage, int commanderSkill) {
        super(name, level, strength, health, attackDamage);
        this.commanderSkill = commanderSkill;
        System.out.println("[CENTURION CONSTRUCTOR] Центуріон " + name + " готовий до командування");
    }

    public Centurion() {
        this("Центуріон", 3, 12.0, 150, 20.0, 7);
    }

    public int getCommanderSkill() {
        return commanderSkill;
    }

    public void setCommanderSkill(int commanderSkill) {
        this.commanderSkill = commanderSkill;
    }

    public void commandUnit(Unit unit) {
        System.out.println(getName() + " командує " + unit.getName() + " з навичкою командира: " + commanderSkill);
    }

    public void powerAttack(Unit target) {
        double totalDamage = getAttackDamage() * 1.5 * (getStrength() / 5.0);
        System.out.println(getName() + " здійснює потужну атаку на " + target.getName() + " з силою " + String.format("%.2f", totalDamage));
        target.takeDamage(totalDamage);
    }

    @Override
    public void print() {
        System.out.println("═════════════════════════════════════");
        System.out.println("| CENTURION - " + getName());
        System.out.println("| Рівень: " + getLevel());
        System.out.println("| Сила: " + String.format("%.2f", getStrength()));
        System.out.println("| Здоров'я: " + getHealth());
        System.out.println("| Урон атаки: " + String.format("%.2f", getAttackDamage()));
        System.out.println("| Навичка командира: " + commanderSkill);
        System.out.println("═════════════════════════════════════");
    }

    @Override
    public String toString() {
        return "Centurion{" +
                "name='" + getName() + '\'' +
                ", level=" + getLevel() +
                ", commanderSkill=" + commanderSkill +
                '}';
    }
}

public class Main {
    // Об'єкт 1 - статичний елемент класу Main
    static Unit staticUnit = new Worker("老Шахтар Скорид", 2, 4.0, 90, 6.5);

    // Об'єкт 2 - звичайний елемент класу Main
    Unit regularUnit;

    static {
        System.out.println("[MAIN STATIC BLOCK] Статична ініціалізація класу Main");
    }

    {
        System.out.println("[MAIN INIT BLOCK] Нестатична ініціалізація класу Main");
    }

    public static void main(String[] args) {
        System.out.println("\n╔════════════════════════════════════════════════════╗");
        System.out.println("║          Початок роботи програми                   ║");
        System.out.println("║     Лабораторна робота N2: Класи та об'єкти        ║");
        System.out.println("║              Тема: Ball's War                       ║");
        System.out.println("╚════════════════════════════════════════════════════╝\n");

        Main mainInstance = new Main();
        Scanner scanner = new Scanner(System.in);

        // Об'єкт 3 - локальний об'єкт функції main
        Unit localUnit = null;

        // Запит на введення параметрів
        System.out.println("Чи бажаєте ви задавати параметри об'єктів? (y/n): ");
        String choice = scanner.nextLine().trim().toLowerCase();

        if (choice.equals("y")) {
            // Об'єкт 2 з параметрами користувача
            mainInstance.regularUnit = createUnitFromInput(scanner, "Об'єкт 2");
            // Об'єкт 3 з параметрами користувача
            localUnit = createUnitFromInput(scanner, "Об'єкт 3");
        } else {
            // Об'єкти зі значеннями за замовчуванням
            mainInstance.regularUnit = new Warrior("Воїн Мартін", 2, 8.0, 120, 15.0);
            localUnit = new Centurion("Центуріон Августин", 3, 12.0, 150, 20.0, 8);
        }

        // Демонстрація equals() та toString()
        System.out.println("\n[DEMONSTRATION] Тестування equals() та toString():");
        Unit testUnit1 = new Worker("Тест", 1, 3.0, 80, 5.0);
        Unit testUnit2 = new Worker("Тест", 1, 3.0, 80, 5.0);
        System.out.println("testUnit1.equals(testUnit2): " + testUnit1.equals(testUnit2));
        System.out.println("testUnit1.toString(): " + testUnit1.toString());
        System.out.println("testUnit2.toString(): " + testUnit2.toString());

        // Головне меню
        showMenu(mainInstance, localUnit, scanner);

        System.out.println("\n╔════════════════════════════════════════════════════╗");
        System.out.println("║           Кінець роботи програми                   ║");
        System.out.println("╚════════════════════════════════════════════════════╝\n");

        scanner.close();
    }

    private static Unit createUnitFromInput(Scanner scanner, String objectName) {
        System.out.println("\n--- Створення " + objectName + " ---");
        System.out.println("Виберіть тип юніта (1=Worker, 2=Warrior, 3=Centurion): ");
        int type = Integer.parseInt(scanner.nextLine().trim());

        System.out.println("Введіть ім'я: ");
        String name = scanner.nextLine().trim();

        System.out.println("Введіть рівень: ");
        int level = Integer.parseInt(scanner.nextLine().trim());

        System.out.println("Введіть силу (0.0-20.0): ");
        double strength = Double.parseDouble(scanner.nextLine().trim());

        System.out.println("Введіть здоров'я (0-200): ");
        int health = Integer.parseInt(scanner.nextLine().trim());

        switch (type) {
            case 1:
                System.out.println("Введіть швидкість видобування: ");
                double miningSpeed = Double.parseDouble(scanner.nextLine().trim());
                return new Worker(name, level, strength, health, miningSpeed);
            case 2:
                System.out.println("Введіть урон атаки: ");
                double attackDamage = Double.parseDouble(scanner.nextLine().trim());
                return new Warrior(name, level, strength, health, attackDamage);
            case 3:
                System.out.println("Введіть урон атаки: ");
                double attackDamage3 = Double.parseDouble(scanner.nextLine().trim());
                System.out.println("Введіть навичку командира: ");
                int skill = Integer.parseInt(scanner.nextLine().trim());
                return new Centurion(name, level, strength, health, attackDamage3, skill);
            default:
                return new Worker(name, level, strength, health, 5.0);
        }
    }

    private static void showMenu(Main mainInstance, Unit localUnit, Scanner scanner) {
        boolean running = true;

        while (running) {
            System.out.println("\n╔════════════════════════════════════════╗");
            System.out.println("║          ГЛАВНОЕ МЕНЮ                  ║");
            System.out.println("╠════════════════════════════════════════╣");
            System.out.println("║ 1. Вивести Об'єкт 1 (статичний)        ║");
            System.out.println("║ 2. Змінити параметри Об'єкта 1         ║");
            System.out.println("║ 3. Вивести Об'єкт 2 (звичайний)        ║");
            System.out.println("║ 4. Змінити параметри Об'єкта 2         ║");
            System.out.println("║ 5. Вивести Об'єкт 3 (локальний)        ║");
            System.out.println("║ 6. Змінити параметри Об'єкта 3         ║");
            System.out.println("║ 7. Взаємодія між об'єктами             ║");
            System.out.println("║ 8. Завершити програму                  ║");
            System.out.println("╚════════════════════════════════════════╝");
            System.out.println("Виберіть опцію: ");

            String input = scanner.nextLine().trim();
            int option = 0;

            try {
                option = Integer.parseInt(input);
            } catch (NumberFormatException e) {
                System.out.println("❌ Невірний вввід. Спробуйте ще раз.");
                continue;
            }

            switch (option) {
                case 1:
                    staticUnit.print();
                    break;
                case 2:
                    modifyUnit(staticUnit, scanner);
                    break;
                case 3:
                    mainInstance.regularUnit.print();
                    break;
                case 4:
                    modifyUnit(mainInstance.regularUnit, scanner);
                    break;
                case 5:
                    localUnit.print();
                    break;
                case 6:
                    modifyUnit(localUnit, scanner);
                    break;
                case 7:
                    interactUnits(staticUnit, mainInstance.regularUnit, localUnit, scanner);
                    break;
                case 8:
                    running = false;
                    System.out.println("✓ Програма завершується...");
                    break;
                default:
                    System.out.println("❌ Невідома опція. Спробуйте ще раз.");
            }
        }
    }

    private static void modifyUnit(Unit unit, Scanner scanner) {
        System.out.println("\n--- Модифікація юніта: " + unit.getName() + " ---");
        System.out.println("1. Змінити ім'я");
        System.out.println("2. Змінити рівень");
        System.out.println("3. Змінити силу");
        System.out.println("4. Змінити здоров'я");
        System.out.println("5. Повернутися в меню");
        System.out.println("Виберіть: ");

        String choice = scanner.nextLine().trim();

        switch (choice) {
            case "1":
                System.out.println("Нове ім'я: ");
                unit.setName(scanner.nextLine().trim());
                System.out.println("✓ Ім'я змінено");
                break;
            case "2":
                System.out.println("Новий рівень: ");
                unit.setLevel(Integer.parseInt(scanner.nextLine().trim()));
                System.out.println("✓ Рівень змінено");
                break;
            case "3":
                System.out.println("Нова сила: ");
                unit.setStrength(Double.parseDouble(scanner.nextLine().trim()));
                System.out.println("✓ Сила змінена");
                break;
            case "4":
                System.out.println("Нове здоров'я: ");
                unit.setHealth(Integer.parseInt(scanner.nextLine().trim()));
                System.out.println("✓ Здоров'я змінено");
                break;
            case "5":
                return;
            default:
                System.out.println("❌ Невідома опція");
        }
    }

    private static void interactUnits(Unit unit1, Unit unit2, Unit unit3, Scanner scanner) {
        System.out.println("\n--- Взаємодія між об'єктами ---");
        System.out.println("Виберіть першу пару для взаємодії:");
        System.out.println("1. Об'єкт 1 та Об'єкт 2");
        System.out.println("2. Об'єкт 1 та Об'єкт 3");
        System.out.println("3. Об'єкт 2 та Об'єкт 3");
        System.out.println("Виберіть: ");

        String choice = scanner.nextLine().trim();
        Unit first, second;

        switch (choice) {
            case "1":
                first = unit1;
                second = unit2;
                break;
            case "2":
                first = unit1;
                second = unit3;
                break;
            case "3":
                first = unit2;
                second = unit3;
                break;
            default:
                System.out.println("❌ Невідома опція");
                return;
        }

        System.out.println("\nВиберіть дію:");
        System.out.println("1. Тренування");
        System.out.println("2. Відпочинок");
        System.out.println("3. Атака (якщо доступно)");
        System.out.println("Виберіть: ");

        String action = scanner.nextLine().trim();

        switch (action) {
            case "1":
                System.out.println("▼ " + first.getName() + " тренується вместе з " + second.getName());
                first.train();
                second.train();
                break;
            case "2":
                System.out.println("▼ " + first.getName() + " та " + second.getName() + " відпочивають");
                first.rest();
                second.rest();
                break;
            case "3":
                if (first instanceof Warrior && second instanceof Unit) {
                    ((Warrior) first).attack(second);
                } else {
                    System.out.println("❌ Перший об'єкт не може атакувати");
                }
                break;
            default:
                System.out.println("❌ Невідома опція");
        }
    }
}