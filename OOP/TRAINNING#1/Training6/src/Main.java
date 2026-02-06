import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        Library library = new Library();
        
        // Додаємо кілька книг для прикладу
        library.addBook(new Book("Кобзар", "Тарас Шевченко", "978-966-03-4208-2", 1840));
        library.addBook(new Book("Тіні забутих предків", "Михайло Коцюбинський", "978-966-03-5123-7", 1911));
        library.addBook(new Book("Захар Беркут", "Іван Франко", "978-966-03-6234-9", 1883));
        library.addBook(new Book("Камінний хрест", "Василь Стефаник", "978-966-03-7345-1", 1900));
        
        // Головне меню
        boolean running = true;
        
        while (running) {
            System.out.println("\n╔════════════════════════════════════════╗");
            System.out.println("║   СИСТЕМА УПРАВЛІННЯ БІБЛІОТЕКОЮ      ║");
            System.out.println("╚════════════════════════════════════════╝");
            System.out.println("1. Додати книгу");
            System.out.println("2. Пошук книги за назвою");
            System.out.println("3. Пошук книг за автором");
            System.out.println("4. Видати книгу");
            System.out.println("5. Повернути книгу");
            System.out.println("6. Показати всі доступні книги");
            System.out.println("7. Показати всі книги");
            System.out.println("8. Статистика по авторам");
            System.out.println("9. Загальна інформація");
            System.out.println("0. Вийти");
            System.out.print("\nОберіть опцію: ");
            
            int choice = scanner.nextInt();
            scanner.nextLine(); // очищення буфера
            
            switch (choice) {
                case 1: // Додати книгу
                    System.out.print("Введіть назву книги: ");
                    String title = scanner.nextLine();
                    System.out.print("Введіть автора: ");
                    String author = scanner.nextLine();
                    System.out.print("Введіть ISBN: ");
                    String isbn = scanner.nextLine();
                    System.out.print("Введіть рік видання: ");
                    int year = scanner.nextInt();
                    scanner.nextLine();
                    
                    library.addBook(new Book(title, author, isbn, year));
                    break;
                    
                case 2: // Пошук за назвою
                    System.out.print("Введіть назву книги: ");
                    String searchTitle = scanner.nextLine();
                    Book foundBook = library.findBookByTitle(searchTitle);
                    
                    if (foundBook != null) {
                        System.out.println("\n✅ Книгу знайдено:");
                        System.out.println(foundBook);
                    } else {
                        System.out.println("❌ Книгу не знайдено.");
                    }
                    break;
                    
                case 3: // Пошук за автором
                    System.out.print("Введіть ім'я автора: ");
                    String searchAuthor = scanner.nextLine();
                    var foundBooks = library.findBooksByAuthor(searchAuthor);
                    
                    if (!foundBooks.isEmpty()) {
                        System.out.println("\n✅ Знайдено " + foundBooks.size() + " книг(и):");
                        for (Book book : foundBooks) {
                            System.out.println(book);
                            System.out.println("--------------------------------");
                        }
                    } else {
                        System.out.println("❌ Книг автора \"" + searchAuthor + "\" не знайдено.");
                    }
                    break;
                    
                case 4: // Видати книгу
                    System.out.print("Введіть назву книги для видачі: ");
                    String borrowTitle = scanner.nextLine();
                    library.borrowBook(borrowTitle);
                    break;
                    
                case 5: // Повернути книгу
                    System.out.print("Введіть назву книги для повернення: ");
                    String returnTitle = scanner.nextLine();
                    library.returnBook(returnTitle);
                    break;
                    
                case 6: // Показати доступні книги
                    library.displayAvailableBooks();
                    break;
                    
                case 7: // Показати всі книги
                    library.displayAllBooks();
                    break;
                    
                case 8: // Статистика по авторам
                    library.displayAuthorStatistics();
                    break;
                    
                case 9: // Загальна інформація
                    System.out.println("\n📊 Загальна інформація:");
                    System.out.println("================================");
                    System.out.println("Всього книг: " + library.getTotalBooks());
                    System.out.println("Доступних книг: " + library.getAvailableBooks());
                    System.out.println("Виданих книг: " + (library.getTotalBooks() - library.getAvailableBooks()));
                    break;
                    
                case 0: // Вийти
                    System.out.println("\n👋 До побачення!");
                    running = false;
                    break;
                    
                default:
                    System.out.println("❌ Невірний вибір. Спробуйте ще раз.");
            }
        }
        
        scanner.close();
    }
}
