import java.util.*;

public class Main {
    public static void main(String[] args) throws CloneNotSupportedException {
        System.out.println("The program started");
        System.out.println("How many objects do you want to create?");
        int numUnits = new Scanner(System.in).nextInt();
        ArrayList<Unit> units = new ArrayList<>();
        for (int i = 0; i < numUnits; i++) {
            Unit unit = new Unit();
            if (i % 2 == 0 ) {
                unit.setTeam("enemy");
                units.add(unit);                
            } else {
                unit.setTeam("ally");
                units.add(unit);
            }
        }
        Scanner scanner = new Scanner(System.in);

        World world = new World();
        world.setUnits(units);

        int choice;

        while (true) {
            System.out.println("Enter choice (): ");
            System.out.println("1. Вивести всі обє'кти поточоного масиву");
            System.out.println("2. Передивитись i-й об’єкт поточного масиву");
            System.out.println("3. Додати на i-у позицію у поточний масив об’єк");
            System.out.println("4. Видалити i-й об’єкт поточного масиву.");
            System.out.println("5. Відсортувати за певною ознакою об’єкти поточного масиву об’єктів ");
            System.out.println("6. Реалізувати глибоке копіювання для класу мікрооб’єкта");
            System.out.println("7. Реалізувати пошук об’єкта у поточному масиві з допомогою функції 'Arrays.binarySearch' --------- SKKIPED");
            System.out.println("8. Додати в меню дві команди, кожна з яких виконує один пункт з Таблиці ---------- SKKIPED");
            System.out.println("9. End Program");

            choice = scanner.nextInt();

            if (choice == 1) {
                world.printUnits();
            } else if (choice == 2) {
                while (true) {

                System.out.println("Який обє'кт вивести?");
                int idx = scanner.nextInt();
                scanner.nextLine();
                if (idx >= 0 && idx <= world.getUnits().size() - 1) {
                    System.out.println("OBJECT " + idx + ":");
                    System.out.println(units.get(idx));
                    break;
                } else {
                    System.out.println("Невірний індекс.");
                    }
                }
            } else if (choice == 3) {
                System.out.println("На яку позицію помістити обє'кт?");
                int idx = scanner.nextInt();
                if (idx >= 0 && idx <= world.getUnits().size()) {
                    Unit unit = new Unit();
                    unit.runSetters();
                    world.insertUnit(idx, unit);
                    System.out.println("Об'єкт додано.");
                    System.out.println("HERE ALL OBJECTS: ");
                    world.printUnits();
                } else {
                    System.out.println("Невірний індекс.");
                    }
            } else if (choice == 4) {
                while (true) {
                System.out.println("Який обє'кт видалити?");
                int idx = scanner.nextInt();
                scanner.nextLine();
                if (idx >= 0 && idx <= world.getUnits().size() - 1) {
                    units.remove(idx);
                    Unit.removeUnit();
                    System.out.println("Here all objects: ");
                    world.printUnits();
                    break;
                } else {
                    System.out.println("Невірний індекс");
                 }
                }
            } else if (choice == 5 ) {
                if (units.size() < 2) {
                    System.out.println("Потрібно щонайменше 2 об'єкти для порівняння/сортування.");
                    continue;
                }
                System.out.println("Що використати для сортування?");
                System.out.println("1. NEsted class (comporators)");
                System.out.println("2. Посилання на функцію (метод) для сортування");
                int choice52 = scanner.nextInt();
                if (choice52 == 1)
                     {
                    while (true) {
                        
                System.out.println("How you want to sort the masive?");
                System.out.println("1. By health");
                System.out.println("2. By team");
                int choice5 = scanner.nextInt();
                
                if (choice5 == 1) {
                    ArrayList<Unit> units5copy = new ArrayList<>();
                    for (int i = 0; i < units.size(); i++) {
                        units5copy.add(units.get(i).clone());
                    }
                    Collections.sort(units5copy, Collections.<Unit>reverseOrder(new Unit.HealthComparator()));
                    System.out.println("HERE THE SORTED MASSIVE:");
                    for  (int i = 0; i < units5copy.size(); i++) {
                        System.out.println("HEre the UNIT: " + i);
                        System.out.println(units5copy.get(i));
                    }
                    break;
                } else  if (choice5 == 2) { 
                    ArrayList<Unit> units5copy = new ArrayList<>();
                    for (int i = 0; i < units.size(); i++) {
                        units5copy.add(units.get(i).clone());
                    }
                    Collections.sort(units5copy, Collections.<Unit>reverseOrder(new Unit.TeamComparator()));
                    System.out.println("HERE THE SORTED MASSIVE:");
                    for  (int i = 0; i < units5copy.size(); i++)
                    {
                        System.out.println("HEre the UNIT: " + i);
                        System.out.println(units5copy.get(i));
                    }
                    break;
                } else {
                    System.out.println("Choose the correct option!");
                }

                    }
                } else if (choice52 == 2) {
                    while (true) {
                        System.out.println("How you want to sort the masive (method reference)?");
                        System.out.println("1. By health");
                        System.out.println("2. By team");
                        int choice5 = scanner.nextInt();

                        if (choice5 == 1) {
                            ArrayList<Unit> units5copy = new ArrayList<>();
                            for (int i = 0; i < units.size(); i++) {
                                units5copy.add(units.get(i).clone());
                            }
                            units5copy.sort(Comparator.comparing(Unit::getHealth).reversed());
                            System.out.println("HERE THE SORTED MASSIVE:");
                            for (int i = 0; i < units5copy.size(); i++) {
                                System.out.println("HEre the UNIT: " + i);
                                System.out.println(units5copy.get(i));
                            }
                            break;
                        } else if (choice5 == 2) {
                            ArrayList<Unit> units5copy = new ArrayList<>();
                            for (int i = 0; i < units.size(); i++) {
                                units5copy.add(units.get(i).clone());
                            }
                            units5copy.sort(Comparator.comparing(Unit::getTeam).reversed());
                            System.out.println("HERE THE SORTED MASSIVE:");
                            for (int i = 0; i < units5copy.size(); i++) {
                                System.out.println("HEre the UNIT: " + i);
                                System.out.println(units5copy.get(i));
                            }
                            break;
                        } else {
                            System.out.println("Choose the correct option!");
                        }
                    } 
            } else {
                System.err.println("Choose the correct option!");
                System.out.println("Try again!");
            }
         } else if (choice == 6) { 
                while (true) {

                System.out.println("Який обє'кт копіювати?");
                int idx = scanner.nextInt();
                scanner.nextLine();
                if (idx >= 0 && idx <= world.getUnits().size()) {
                    Unit unit = new Unit();
                    unit = units.get(idx).clone();
                    units.get(idx).setInventor(new ArrayList<>(Arrays.asList("BIG SWORD")));
                    units.add(unit);
                    System.out.println("OBJECT " + idx + ":");
                    System.out.println(units.get(idx));
                    System.out.println("COPY");
                    System.out.println(units.get(units.size() - 1));
                    System.out.println("HERE ALL BOJECTS: ");
                    world.printUnits();
                    break;
                } else {
                    System.out.println(" Невірний індекс");
                }
                }
            } else if (choice == 7) {

            } else if (choice == 8) {

            } else if (choice == 9) {
                break;
            }
        }
        scanner.close();
    }
}