import java.util.*;

public class Main {
    public static void main(String[] args) throws CloneNotSupportedException {
        System.out.println("The program started");
        System.out.println("How many objects do you want to create?");
        int numUnits = new Scanner(System.in).nextInt();
        Random random = new Random();
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
            unit.setHealth(random.nextInt(100) + 10);
        }
        Scanner scanner = new Scanner(System.in);
        


        // Initialization 
        // The uni class
        World world = new World();
        world.setUnits(units);


        // The base macroobject
        Base base = new Base();
        ArrayList<Unit> baseUnits = new ArrayList<>();
        base.setUnits(baseUnits);

        // The tower macroobject
        Tower tower = new Tower();
        ArrayList<Unit> towerUnits = new ArrayList<>();
        tower.setUnits(towerUnits);



        
        int choice;
        while (true) {
            System.out.println("Enter choice (): ");
            System.out.println("1. Вивести всі обє'кти поточоного масиву");
            System.out.println("2. Передивитись i-й об’єкт поточного масиву");
            System.out.println("3. Додати на i-у позицію у поточний масив об’єк");
            System.out.println("4. Видалити i-й об’єкт поточного масиву.");
            System.out.println("5. Відсортувати за певною ознакою об’єкти поточного масиву об’єктів ");
            System.out.println("6. Реалізувати глибоке копіювання для класу мікрооб’єкта");
            System.out.println("7. Реалізувати пошук об’єкта у поточному масиві з допомогою функції 'Arrays.binarySearch' ");
            System.out.println("8. Додати в меню дві команди, кожна з яких виконує один пункт з Таблиці ");
            System.out.println("9. Взаємодія з макрооб'єктами");
            System.out.println("10. End Program");

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
                System.out.println("What characteristics we need to find?");
                Unit copy7Unit = new Unit();
                System.out.println("Set the characteristics of the object you want to find: ");
                System.out.println("Health: ");
                copy7Unit.setHealth(scanner.nextInt());
                scanner.nextLine();
                System.out.println("Team: ");
                copy7Unit.setTeam(scanner.nextLine());
                System.out.println("Damage: ");
                copy7Unit.setDamage(scanner.nextInt());
                scanner.nextLine();
                System.out.print("Inventor: ");
                String inventoryInput = scanner.nextLine().trim();
                copy7Unit.setInventor(new ArrayList<>(Arrays.asList(inventoryInput)));
                System.out.println("Dead status: ");
                copy7Unit.setDead(scanner.nextBoolean());
                scanner.nextLine();
                System.out.println("Spawned status: ");
                copy7Unit.setSpawned(scanner.nextBoolean());
                scanner.nextLine();
                System.out.println("Here the object you want to find: ");
                System.out.println(copy7Unit);
                Comparator<Unit> compareTo47Comparator = Unit::compareTo47;
                ArrayList<Unit> units7copy = new ArrayList<>();
                for (Unit u : units) units7copy.add(u.clone());
                Collections.sort(units7copy, compareTo47Comparator);


                int foundIndex = Collections.binarySearch(units7copy, copy7Unit, compareTo47Comparator);
                if (foundIndex < 0) {
                    System.out.println("Object not found.");
                } else {
                    int left = foundIndex;
                    while (left > 0 && compareTo47Comparator.compare(units7copy.get(left-1), copy7Unit) == 0) left--;
                    int right = foundIndex;
                    while (right < units7copy.size()-1 && compareTo47Comparator.compare(units7copy.get(right+1), copy7Unit) == 0) right++;
                    System.out.println("Found " + (right-left+1) + " matching object(s) in sorted copy. Indexes: ");
                    for (int i = left; i <= right; i++) {
                        System.out.println("Index " + i + ": " + units7copy.get(i));
                        
                    }
                }
                
                Unit.removeUnit();

                
            } else if (choice == 8) {
                System.out.println("Choose the option: ");
                System.out.println("1. INterection with the units");
                System.out.println("2. Competiontion...");
                int Choice8 = scanner.nextInt();
                if (Choice8 == 1) {
                
                    Unit unit8Copy = new Unit();
                    unit8Copy.runSetters();
                    while (true) {

                        System.out.println("Here the result of intercation with the each object: ");
                        System.out.println("1. Show the strongest unit");
                        System.out.println("2. Show if the unit is ally or enemy");
                        System.out.println("3. Exit");
                        int choice8;
                        choice8 = scanner.nextInt();
                        if (choice8 == 1) {
                            for (Unit u : units) {
                                unit8Copy.showTheStrongest(u);
                            }
                        } else if (choice8 == 2) {
                            for (Unit u : units) {
                                if (unit8Copy.isAlly(u)) {
                                    System.out.println("The unit is ally");
                                } else {
                                    System.out.println("The unit is enemy");
                                }
                            }
                        } else if (choice8 == 3) {
                        break;
                        } else {
                            System.out.println("Choose the correct option!");
                        }
                    }
                    Unit.removeUnit();
                } else if (Choice8 == 2) {
                    ArrayList<Unit> units81 = new ArrayList<>();
                    ArrayList<Unit> units82 = new ArrayList<>();
                    for (Unit u : units) {
                        if (u.getTeam().equals("ally")) {
                            units81.add(u);
                        } else {
                            units82.add(u);
                        }
                    }
                    Collections.sort(units81, Comparator.comparing(Unit::getHealth).reversed());
                    Collections.sort(units82, Comparator.comparing(Unit::getHealth).reversed());
                    System.out.println("Here the ally team: ");
                    for (Unit u : units81) {
                        System.out.println(u);
                    }
                    System.out.println("Here the enemy team: ");
                    for (Unit u : units82) {
                        System.out.println(u);
                    }
                    System.out.println("Here the result of competition: ");
                    units81.get(0).showTheStrongest(units82.get(0));
                    
                } else {
                    System.out.println("Choose the correct option!");
                }
            } else if (choice == 9) {
                while (true) { 
                    int idx;
                    System.out.println("Обери дію: ");
                    System.out.println("1. Додати об'єкт");
                    System.out.println("2. Вивести на екран вміст універсального об’єкта та всіх макрооб’єктів.: ");
                    System.out.println("3. Взаємодія мікрооб’єктів. ");
                    System.out.println("4. Взаємодія двох макрооб’єктів. ");
                    System.out.println("5. Підрахунок мікрооб’єктів.  ");
                    System.out.println("6. Видалити мікрооб’єкт.");
                    System.out.println("7. Повернутися до головного меню.");
                    idx = scanner.nextInt();
                    if (idx == 1) {
                            Unit unit9Copy = new Unit();
                            unit9Copy.runSetters();
                            System.out.println("Where do you want to add the object?");
                            System.out.println("1. Tower");
                            System.out.println("2. Base");
                            System.out.println("2. World");
                            int choice9 = scanner.nextInt();
                            if (choice9 == 1) {
                                tower.getUnits().add(unit9Copy);
                            } else if (choice9 == 2) {
                                base.getUnits().add(unit9Copy);
                            } else if (choice9 == 3) {
                                world.insertUnit(world.getUnits().size(), unit9Copy);
                                System.out.println("Object added to the world.");
                            } else {
                                System.out.println("Choose the correct option!");
                            } 

                    } else if (idx == 2) {
                        System.out.println("Here the content of the world: ");
                        for (Unit u : world.getUnits()) {
                            System.out.println(u);
                        }
                        System.out.println("Here the content of the base: ");
                        for (Unit u : base.getUnits()) {
                            if (base.getUnits().size() == 0) {
                                System.out.println("The base is empty.");
                                break;
                            }
                            System.out.println(u);
                        }
                        System.out.println("Here the content of the tower: ");
                        for (Unit u : tower.getUnits()) {
                            if (tower.getUnits().size() == 0) {
                                System.out.println("The tower is empty.");
                                break;
                            }
                            System.out.println(u);
                        }
                    } else if (idx == 4) {
                        
                    } else if (idx == 3) {
                        Unit unit94Copy = new Unit();
                        scanner.nextLine(); 
                        System.out.println("Set the characteristics you want to specify (comma-separated, e.g. health,team,damage): ");
                        ArrayList<String> list = new ArrayList<>();
                        String input = scanner.nextLine().trim();
                        list.addAll(Arrays.asList(input.split("\\s*,\\s*")));
                        unit94Copy.runSettersToComparing(list);
                        System.out.println("Where to get microobjects:");
                        System.out.println("1. Base");
                        System.out.println("2. Tower");
                        System.out.println("3. World");
                        int choice94 = scanner.nextInt();
                        Comparator<Unit> comparatorTo94 = Unit.comparatorFromTemplate(unit94Copy);
                        ArrayList<Unit> units94copy = new ArrayList<>();
                        if (choice94 == 1) {
                            base.getUnits().sort(comparatorTo94);   
                            for (int i = 0; i < base.getUnits().size(); i++) {
                                if (comparatorTo94.compare(base.getUnits().get(i), unit94Copy) == 0) 
                                {
                                    units94copy.add(base.getUnits().get(i).clone());
                                }
                            } 
                        } else if (choice94 == 2) {
                            tower.getUnits().sort(comparatorTo94);
                            for (int i = 0; i < tower.getUnits().size(); i++) {
                                if (comparatorTo94.compare(tower.getUnits().get(i), unit94Copy) == 0) 
                                {
                                    units94copy.add(tower.getUnits().get(i).clone());
                                }
                            }
                            
                        } else if (choice94 == 3) {
                            world.getUnits().sort(comparatorTo94);
                            for (int i = 0; i < world.getUnits().size(); i++) {
                                if (comparatorTo94.compare(world.getUnits().get(i), unit94Copy) == 0) 
                                {
                                    units94copy.add(world.getUnits().get(i).clone());
                                }
                            }
                            
                        } else {
                            System.out.println("Choose the correct option!");
                        }
                        if (units94copy.size() == 0) {
                            System.out.println("No matching objects found.");
                        } else if (units94copy.size() <= 1) {
                            System.out.println("We do not have enought units");
                        } else {
                            System.out.println("Here the units that match the template: ");
                            for (Unit u : units94copy) {
                                System.out.println(u);
                            }
                            System.out.println("Here the result of interaction: ");
                            for (int i = 0; i < units94copy.size() - 1; i++) {
                                units94copy.get(i).showTheStrongest(units94copy.get(i+1));
                            }
                            System.out.println("Here the second interaction: ");
                            for (int i = 0; i < units94copy.size() - 1; i++) {
                                if (units94copy.get(i).isAlly()) {
                                    System.out.println("The units are allies");
                                } else {
                                    System.out.println("The units are enemies");
                                }
                            }

                        }
                    } else if (idx == 5) {
                        
                    } else if (idx == 6) {
                        
                    } else if (idx == 7) {
                        break;
                        
                    } else {
                        System.out.println("Choose the correct option!");
                    }

                }
            } else if (choice == 10) {
                break;
            }
        }
        scanner.close();
    }
}