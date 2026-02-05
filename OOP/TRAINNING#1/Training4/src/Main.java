import  java.util.HashMap;
import java.util.Scanner;
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
@SuppressWarnings("empty-statement")
public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        HashMap<String, String > book = new HashMap<>();

        book.put("Alex", "09339");
        book.put("Den", "09312");
        book.put("Bill", "09672");

        while(true) {
            System.out.println("Choose the option to do");
            System.out.println("1 - Add abbonent");
            System.out.println("2 - Give the list of all");
            System.out.println("3 - Search according to the name");
            System.out.println("4 - exit");

            int choose = scanner.nextInt();
            scanner.nextLine(); // consume leftover newline

            if (choose == 1) {
                System.out.println("Give the name");
                String name = scanner.nextLine();
                System.out.println("Give the number");
                String number = scanner.nextLine();
                book.put(name, number);
            } else if (choose == 2) {
                for (java.util.Map.Entry<String, String> entry : book.entrySet()) {
                    System.out.println(entry.getKey() + " : " + entry.getValue());
                }
            } else if (choose == 3) {
                System.out.println("Enter name to search:");
                String searcher = scanner.nextLine();
                String found = book.get(searcher);
                if (found != null) {
                    System.out.println(searcher + " : " + found);
                } else {
                    System.out.println("Not found.");
                }
            }  else {
                System.out.println("Bye");
                break;
            }
        }
        scanner.close();
    }
}
