import java.util.ArrayList;
import java.util.HashMap;

public class Library {
    // Колекція для зберігання книг
    private ArrayList<Book> books;
    
    // HashMap для підрахунку кількості книг кожного автора
    private HashMap<String, Integer> authorBookCount;
    
    // Конструктор
    public Library() {
        books = new ArrayList<>();
        authorBookCount = new HashMap<>();
    }
    
    // Метод для додавання нової книги
    public void addBook(Book book) {
        books.add(book);
        
        // Оновлюємо підрахунок книг автора
        String author = book.getAuthor();
        authorBookCount.put(author, authorBookCount.getOrDefault(author, 0) + 1);
        
        System.out.println("✅ Книгу додано до бібліотеки!");
    }
    
    // Пошук книги за назвою
    public Book findBookByTitle(String title) {
        for (Book book : books) {
            if (book.getTitle().equalsIgnoreCase(title)) {
                return book;
            }
        }
        return null;  // якщо не знайдено
    }
    
    // Пошук книг за автором (може бути кілька книг)
    public ArrayList<Book> findBooksByAuthor(String author) {
        ArrayList<Book> foundBooks = new ArrayList<>();
        for (Book book : books) {
            if (book.getAuthor().equalsIgnoreCase(author)) {
                foundBooks.add(book);
            }
        }
        return foundBooks;
    }
    
    // Видача книги за назвою
    public void borrowBook(String title) {
        Book book = findBookByTitle(title);
        if (book != null) {
            book.borrowBook();
        } else {
            System.out.println("❌ Книгу \"" + title + "\" не знайдено в бібліотеці.");
        }
    }
    
    // Повернення книги за назвою
    public void returnBook(String title) {
        Book book = findBookByTitle(title);
        if (book != null) {
            book.returnBook();
        } else {
            System.out.println("❌ Книгу \"" + title + "\" не знайдено в бібліотеці.");
        }
    }
    
    // Відображення всіх доступних книг
    public void displayAvailableBooks() {
        System.out.println("\n📖 Доступні книги в бібліотеці:");
        System.out.println("================================");
        boolean hasAvailable = false;
        
        for (Book book : books) {
            if (book.isAvailable()) {
                System.out.println(book);
                System.out.println("--------------------------------");
                hasAvailable = true;
            }
        }
        
        if (!hasAvailable) {
            System.out.println("Немає доступних книг.");
        }
    }
    
    // Відображення всіх книг
    public void displayAllBooks() {
        System.out.println("\n📚 Всі книги в бібліотеці:");
        System.out.println("================================");
        
        if (books.isEmpty()) {
            System.out.println("Бібліотека порожня.");
            return;
        }
        
        for (Book book : books) {
            System.out.println(book);
            System.out.println("--------------------------------");
        }
    }
    
    // Підрахунок кількості книг кожного автора
    public void displayAuthorStatistics() {
        System.out.println("\n👤 Статистика по авторам:");
        System.out.println("================================");
        
        if (authorBookCount.isEmpty()) {
            System.out.println("Немає книг у бібліотеці.");
            return;
        }
        
        for (HashMap.Entry<String, Integer> entry : authorBookCount.entrySet()) {
            System.out.println(entry.getKey() + ": " + entry.getValue() + " книг(и)");
        }
    }
    
    // Отримання загальної кількості книг
    public int getTotalBooks() {
        return books.size();
    }
    
    // Отримання кількості доступних книг
    public int getAvailableBooks() {
        int count = 0;
        for (Book book : books) {
            if (book.isAvailable()) {
                count++;
            }
        }
        return count;
    }
}
