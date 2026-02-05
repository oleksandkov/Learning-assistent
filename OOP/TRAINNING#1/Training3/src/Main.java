import java.util.Scanner;
//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
void main() {
    Scanner scanner = new Scanner(System.in);
    System.out.println("How much number you would like to count");
    int num  = scanner.nextInt();

    int[] mass = new int[num];
    for(int i = 0; i < num; i++) {
        int number = scanner.nextInt();
        mass[i] = number;
    }
    System.out.print("THe average number is....");
    int total = 0;
    for(int i = 0; i < num; i++) {
        total += mass[i];
    }
    System.out.print(total / num);

    scanner.close();
}
