import java.util.Scanner;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
void main() {
    Scanner x = new Scanner(System.in);
    System.out.println("Введи перше число");
    int n1;
    n1 = x.nextInt();
    System.out.println("Введи друге число");
    int n2 = x.nextInt();
    System.out.println("Введи операцію між числами");
    Scanner scanner = new Scanner(System.in);
    String y = scanner.nextLine();

    int solution = 0;

    switch (y) {
        case "+" -> solution = n1 + n2;
        case "-" -> solution = n1 - n2;
        case "/" -> solution = n1 / n2;
        default -> System.out.println("Cant be calculated");
    }

    System.out.println("The solution " + solution + "\n");

    scanner.close();
    x.close();

}
