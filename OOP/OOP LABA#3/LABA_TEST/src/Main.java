import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) throws CloneNotSupportedException {
        System.out.println("The program started");
        System.out.println("How many objects do you want to create?");
        int numUnits = new Scanner(System.in).nextInt();
        ArrayList<Unit> units = new ArrayList<>();
        for (int i = 0; i < numUnits; i++) {
            units.add(new Unit());
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
            System.out.println("5. Відсортувати за певною ознакою об’єкти поточного масиву об’єктів ----- SKIPED");
            System.out.println("6. Реалізувати глибоке копіювання для класу мікрооб’єкта");
            System.out.println("7. Реалізувати пошук об’єкта у поточному масиві з допомогою функції 'Arrays.binarySearch' --------- SKKIPED");
            System.out.println("8. Додати в меню дві команди, кожна з яких виконує один пункт з Таблиці ---------- SKKIPED");
            System.out.println("9. End Program");

            choice = scanner.nextInt();

            if (choice == 1) {
                world.printUnits();
            } else if (choice == 2) {
                System.out.println("Який обє'кт вивести?");
                int idx = scanner.nextInt();
                scanner.nextLine();
                if (idx >= 0 && idx <= world.getUnits().size()) {
                    System.out.println("OBJECT " + idx + ":");
                    System.out.println(units.get(idx));
                } else {
                    System.out.println("Невірний індекс.");
                }
            } else if (choice == 3) {
                System.out.println("На яку позицію помістити обє'кт?");
                int idx = scanner.nextInt();
                Unit unit = new Unit();
                unit.runSetters();
                if (idx >= 0 && idx <= world.getUnits().size()) {
                    world.insertUnit(idx, unit);
                    System.out.println("Об'єкт додано.");
                } else {
                    System.out.println("Невірний індекс.");
                }
                world.printUnits();
            } else if (choice == 4) {
                System.out.println("Який обє'кт видалити?");
                int idx = scanner.nextInt();
                scanner.nextLine();
                if (idx >= 0 && idx <= world.getUnits().size()) {
                    units.remove(idx);
                } else {
                    System.out.println("Невірний індекс");
                }
                world.printUnits();
            } else if (choice == 5 ) {

            } else if (choice == 6) {
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
                    world.printUnits();
                } else {
                    System.out.println(" Невірний індекс");
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