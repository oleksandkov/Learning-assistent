import java.util.HashMap;
import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        HashMap<Integer, String> list = new HashMap<>(); // Move outside the loop

        while (true) {
            System.out.println("THE LIST OF THE TASKS");
            System.out.println(" ");
            System.out.println("To add new task, click 1");
            System.out.println("To delete a task, click 2");
            System.out.println("To show the list, click 3");
            if (list.isEmpty()) System.out.println("The list is empty!");

            int choose = scanner.nextInt();
            scanner.nextLine(); 

            if (choose == 1) {
                System.out.println("Type the task");
                String task = scanner.nextLine();
                int num = list.size() + 1;
                list.put(num, task);
            } else if (choose == 2) {
                System.out.println("What task you wanna remove?");
                int remove = scanner.nextInt();
                scanner.nextLine(); // Clear buffer
                list.remove(remove);
            } else if (choose == 3) {
                for (java.util.Map.Entry<Integer, String> entry : list.entrySet()) {
                    System.out.println(entry.getKey() + " --- " + entry.getValue());
                }
                scanner.nextLine();
            } else {
                break;
            }
        }
        scanner.close();
    }
    
}
