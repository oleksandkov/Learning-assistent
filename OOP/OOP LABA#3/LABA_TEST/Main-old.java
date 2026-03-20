import java.util.ArrayList;
import java.util.Scanner;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
public class Main-old {
    public static void main(String[] args) {
        System.out.println("THE PROGRAM STARTED");
        Unit unit1;
        Unit unit2;
        Unit unit3 = null;

        unit1 = new Unit(100, false, "Ally", 10, false, new ArrayList<>(java.util.Arrays.asList("Sword")));
        unit2 = new Unit();
        int choice = -1;
        Scanner scanner = new Scanner(System.in);
        while(choice != 0) {
           // System.out.println("\n═══════════════════════════════════");
            System.out.println("Choose next actions");
            System.out.println("1 - Show first Unit object");
            System.out.println("2 - Change first Unit object");
            System.out.println("3 - Show second Unit object");
            System.out.println("4 - Change second Unit object");
            System.out.println("5 - Show third Unit object");
            System.out.println("6 - Change third Unit object");
            System.out.println("7 - Compare units");
            System.out.println("8 - Show object count");
            System.out.println("0 - End program");
            // System.out.println("═══════════════════════════════════");
            System.out.print("Your choice: ");
            choice = scanner.nextInt();
            scanner.nextLine();
            
            if (choice == 1) {
            unit1.print();
        } else if (choice == 2) {
            unit1.runSetters();
        } else if (choice == 3) {
            unit2.print();
        } else if (choice == 4) {
            unit2.runSetters();
        } else if (choice == 5) {
            if (unit3 != null) {
                unit3.print();
            } else {
                System.out.println(" Unit 3 is not initialized!");
                System.out.println("Do you want to create it? (1 - Yes, 0 - No): ");
                int create = scanner.nextInt();
                scanner.nextLine();
                if (create == 1) {
                    unit3 = new Unit();
                    System.out.println(" Unit 3 created!");
                    unit3.print();
                }
            }
        } else if (choice == 6) {
            if (unit3 != null) {
                unit3.runSetters();
            } else {
                System.out.println(" Unit 3 is not initialized! Create it first .");
            }
        } else if (choice == 7) {
            //System.out.println("\n═══════════════════════════════════");
            System.out.println("      COMPARE OBJECTS");
            //System.out.println("═══════════════════════════════════");

            System.out.println("Available objects:");
            System.out.println("1 - Object 1 (Unit 1)");
            System.out.println("2 - Object 2 (Unit 2)");
            if (unit3 != null) {
                System.out.println("3 - Object 3 (Unit 3)");
            } else {
                System.out.println("3 - Object 3 (NOT CREATED)");
            }
            
            System.out.print("\nSelect first object (1-3): ");
            int obj1 = scanner.nextInt();
            scanner.nextLine();
            
            System.out.print("Select second object (1-3): ");
            int obj2 = scanner.nextInt();
            scanner.nextLine();
            
            if ((obj1 == 3 || obj2 == 3) && unit3 == null) {
                System.out.println(" Unit 3 is not created! Create it first ");
            } else {
                Unit x = null;
                Unit y = null;
                
                switch (obj1) {
                    case 1:
                        x = unit1;
                        break;
                    case 2:
                        x = unit2;
                        break;
                    case 3:
                        x = unit3;
                        break;
                    default:
                        System.out.println(" Invalid choice for first object!");
                        break;
                }
                
                switch (obj2) {
                    case 1:
                        y = unit1;
                        break;
                    case 2:
                        y = unit2;
                        break;
                    case 3:
                        y = unit3;
                        break;
                    default:
                        System.out.println(" Invalid choice for second object!");
                        break;
                }
                
                if (x != null && y != null) {
                    boolean result = x.equals(y);
                    
                   // System.out.println("\n═══════════════════════════════════");
                    System.out.println("COMPARISON RESULT:");
                    //System.out.println("═══════════════════════════════════");
                    System.out.println("\nFirst object:");
                    System.out.println(x.toString());
                    System.out.println("\nSecond object:");
                    System.out.println(y.toString());
                  //  System.out.println("\n═══════════════════════════════════");
                    if (result) {
                        System.out.println("Objects are EQUAL");
                    } else {
                        System.out.println("Objects are DIFFERENT");
                    }
                 //   System.out.println("═══════════════════════════════════");
                } else {
                    System.out.println("Invalid object selection!");
                }
            }
        } else if (choice == 8) {
               int numob= Unit.getNumObjects();
                System.out.println("Here the number of objects: " + numob);
        } else if (choice == 0) {
            System.out.println("Ending program...");
        } else {
            System.out.println("Invalid choice! Try again.");
        }
    }
        
        scanner.close();
        System.out.println("\nTHE PROGRAM ENDED");
    }
}