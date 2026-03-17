import java.util.Scanner;

//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
void main() {
    System.out.println("Test color");
    System.out.println("Test Font");

    Scanner scanner = new Scanner(System.in);

    System.out.println("What my age?");

    int age = scanner.nextInt();

    System.out.println("My age is " + age);

    System.out.println("What is my name?");

    String name = scanner.next();

    System.out.println("My name is " + name);

    System.out.println("How much money do you have?");

    Double money = scanner.nextDouble();

    System.out.println("You are poor");


    scanner.close();
}
