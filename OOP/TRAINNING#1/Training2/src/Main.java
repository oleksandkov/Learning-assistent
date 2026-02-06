import java.util.Scanner;
//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
/**
 * click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
 */
void main() {
    Scanner scanner = new Scanner(System.in);
    Random random;
    random = new Random();
    int randNum;
    randNum = random.nextInt(100) + 1;

    while (true) {

    System.out.println("Give your suggestion");
    int sugg = scanner.nextInt();

    if(sugg < randNum) {
        System.out.println("Your number is less than random");
    } else if(sugg > randNum) {
        System.out.println("Your number is bigger than random");
    }

    if(sugg == randNum) break;
    }
    System.out.println("You got it");
}
