import java.util.Scanner;
//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
void main() {
    Scanner scanner = new Scanner(System.in);

    String item;
    double price;
    int quantity;
    String currency = "$";


    System.out.println("What item would you like to buy?");
    item = scanner.nextLine();
    System.out.println("Set a price to this item: ");
    price = scanner.nextDouble();
    System.out.println("How much item would you like to buy?");
    quantity = scanner.nextInt();

    System.out.println("You would like a buy " + quantity + " peaces of " + item + "/s.");
    double total = price * quantity;
    System.out.println("YOu need to have at least " + total + currency);

    scanner.close();
}
