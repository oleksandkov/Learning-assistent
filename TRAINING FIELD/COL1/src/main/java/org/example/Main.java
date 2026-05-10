package org.example;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Scanner;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class Main {

    public static void main(String[] args) {
        ArrayList<Train> trains = new ArrayList<>();
        Train train1 = new Train("Train1", new HashMap<>());
        Train train2 = new Train("Train2", new HashMap<>());
        Train train3 = new Train("Train3", new HashMap<>());

        trains.add(train1);
        trains.add(train2);
        trains.add(train3);

        // create passengers
        Passenger passenger1 = new Passenger("Barak", 1);
        Passenger passenger2 = new Passenger("Tramp", 2);
        Passenger passenger3 = new Passenger("Bill", 5);
        Passenger passenger4 = new Passenger("Kennedy", 7);
        Passenger passenger5 = new Passenger("Baiden", 81);
        Passenger passenger6 = new Passenger("Bush", 2);

        // fill trains (note: passenger2 and passenger4 and passenger5 appear on multiple trains to demonstrate query A)
        train1.add(passenger1);
        train1.add(passenger2);
        train1.add(passenger3);

        train2.add(passenger4);
        train2.add(passenger5);
        train2.add(passenger2); // Tramp also on train2

        train3.add(passenger6);
        train3.add(passenger4); // Kennedy also on train3
        train3.add(passenger5); // Baiden also on train3

        // demonstrate deep copy and Map comparison
        Train copyOfTrain1 = new Train(train1);
        System.out.println("Copy of train1 equals original: " + copyOfTrain1.equals(train1));

        Scanner scanner = new Scanner(System.in);
        while (true) {
            System.out.println("\nОбери опцію: ");
            System.out.println("1: Запит A - Пасажири, що одночасно їдуть більше ніж в одному потязі");
            System.out.println("2: Запит B - Знайти пасажира по прізвищу");
            System.out.println("0: Вихід");
            System.out.print(" Ваш вибір: ");

            String line = scanner.nextLine().trim();
            if (line.isEmpty()) continue;
            int choose;
            try {
                choose = Integer.parseInt(line);
            } catch (NumberFormatException e) {
                System.out.println("Невірний ввід, спробуйте ще.");
                continue;
            }

            if (choose == 0) break;

            if (choose == 1) {
                // count occurrences of passenger names across trains
                HashMap<String, Integer> counts = new HashMap<>();
                HashMap<String, ArrayList<String>> where = new HashMap<>();
                for (Train t : trains) {
                    for (String name : t.getPassengers().keySet()) {
                        counts.put(name, counts.getOrDefault(name, 0) + 1);
                        where.computeIfAbsent(name, k -> new ArrayList<>())
                                .add(t.getTitle() + "(seat:" + t.getPassengers().get(name) + ")");
                    }
                }
                boolean found = false;
                System.out.println("Пасажири, що їдуть одночасно більше ніж в одному потязі:");
                for (String name : counts.keySet()) {
                    if (counts.get(name) > 1) {
                        found = true;
                        System.out.println("- " + name + " -> " + where.get(name));
                    }
                }
                if (!found) System.out.println("Такі пасажири відсутні.");
            } else if (choose == 2) {
                System.out.print("Введіть прізвище пасажира: ");
                String target = scanner.nextLine().trim();
                boolean found = false;
                for (Train t : trains) {
                    if (t.getPassengers().containsKey(target)) {
                        int seat = t.getPassengers().get(target);
                        System.out.println(target + " знаходиться в потязі '" + t.getTitle() + "' на місці " + seat);
                        found = true;
                    }
                }
                if (!found) System.out.println("Пасажир з прізвищем '" + target + "' не знайдений в потягах.");
            } else {
                System.out.println("Невідома опція.");
            }
        }

        scanner.close();
        System.out.println("Програма завершила роботу.");
    }
}
