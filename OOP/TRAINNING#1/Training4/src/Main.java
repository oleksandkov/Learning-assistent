import  java.util.HashMap;
import java.util.Scanner;
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
@SuppressWarnings("empty-statement")
void main() {
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

    if (choose == 1) {
        System.out.println("Give the name");
        String name = scanner.nextLine();
        scanner.nextLine();
        System.out.println("Give the number");
        String number = scanner.nextLine();
        scanner.nextLine();
        book.put(name, number);

    } else if (choose == 2) {
        for (java.util.Map.Entry<String, String> entry : book.entrySet()) {
            System.out.println(entry.getKey() + " : " + entry.getValue());
        }
    } else if (choose == 3) {
        String searcher = scanner.nextLine();
        book.get(searcher);
    }  else {
        System.out.println("Bye");
        break;
    }

}
scanner.close();

}
