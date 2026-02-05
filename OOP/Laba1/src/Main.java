import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        System.out.println("Введіть кількість цілих чисел:");
        int n = scanner.nextInt();
        
        int positive = 0;
        int negative = 0;
        int zeros = 0;
        
        System.out.println("Введіть " + n + " цілих чисел:");
        for (int i = 0; i < n; i++) {
            int number = scanner.nextInt();
            if (number > 0) {
                positive++;
            } else if (number < 0) {
                negative++;
            } else {
                zeros++;
            }
        }
        
        System.out.println("\nРезультати:");
        System.out.println("Додатніх чисел: " + positive);
        System.out.println("Від'ємних чисел: " + negative);
        System.out.println("Нулів: " + zeros);
        
        if (positive > negative && positive > zeros) {
            System.out.println("\nБільше всього додатніх чисел");
        } else if (negative > positive && negative > zeros) {
            System.out.println("\nБільше всього від'ємних чисел");
        } else if (zeros > positive && zeros > negative) {
            System.out.println("\nБільше всього нулів");
        } else {
            System.out.println("\nДекілька типів чисел мають однакову кількість");
        }
        
        scanner.close();
    }
}
