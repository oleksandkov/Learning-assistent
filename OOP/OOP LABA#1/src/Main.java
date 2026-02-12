import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Введіть кількість цілих чисел: \n ");
        int n;
        n = scanner.nextInt();

        
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
        
        System.out.println("Результати:");
        System.out.println("Додатніх чисел: " + positive);
        System.out.println("Від'ємних чисел: " + negative);
        System.out.println("Нулів: " + zeros);
        
        if (positive > negative && positive > zeros) {
            System.out.println("Більше всього додатніх чисел");
        } else if (negative > positive && negative > zeros) {
            System.out.println("Більше всього від'ємних чисел");
        } else if (zeros > positive && zeros > negative) {
            System.out.println("Більше всього нулів");
        } else {
            System.out.println("Декілька типів чисел мають однакову кількість");
        }
        
        scanner.close();
    }
}
