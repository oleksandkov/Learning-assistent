import java.sql.SQLOutput;
import java.util.Scanner;//TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or
// click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
void main() {
    //TIP Press <shortcut actionId="ShowIntentionActions"/> with your caret at the highlighted text
    // to see how IntelliJ IDEA suggests fixing it.
    Scanner scanner = new Scanner (System.in);

    String adjective1;
    String noun1;
    String adjective2;
    String verb1;
    String adjective3;

    System.out.print("Enter the adjective (description): ");
    adjective1 = scanner.nextLine();
    System.out.print("Enter the person or an animal: ");
    noun1 = scanner.nextLine();
    System.out.print("Enter the adjective -ly: ");
    adjective2 = scanner.nextLine();
    System.out.print("Enter the verb: (-ing): ");
    verb1 = scanner.nextLine();
    System.out.println("Enter one more adjective: ");
    adjective3 = scanner.nextLine();

    System.out.println("Today I went to a " + adjective1 + " zoo.");
    System.out.println("In an exhibiit, I saw a " + noun1 + ".");
    System.out.println(noun1 + " was " + adjective2 + " hallarius.");
    System.out.println("While " + noun1 +" "+ verb1 + " I was eating a burger.");
    System.out.println("The " + adjective3 + " dolphin starring at " + noun1);

}
