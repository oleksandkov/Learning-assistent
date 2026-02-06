public class Book {
    // Поля класу (змінні екземпляра)
    private String title;           // назва книги
    private String author;          // автор
    private String isbn;            // ISBN код
    private int year;               // рік видання
    private boolean isAvailable;    // чи доступна книга
    
    // Конструктор - спеціальний метод для створення об'єкта
    public Book(String title, String author, String isbn, int year) {
        this.title = title;
        this.author = author;
        this.isbn = isbn;
        this.year = year;
        this.isAvailable = true;  // за замовчуванням книга доступна
    }
    
    // Геттери (методи для отримання значень полів)
    public String getTitle() {
        return title;
    }
    
    public String getAuthor() {
        return author;
    }
    
    public String getIsbn() {
        return isbn;
    }
    
    public int getYear() {
        return year;
    }
    
    public boolean isAvailable() {
        return isAvailable;
    }
    
    // Сеттери (методи для зміни значень полів)
    public void setTitle(String title) {
        this.title = title;
    }
    
    public void setAuthor(String author) {
        this.author = author;
    }
    
    public void setIsbn(String isbn) {
        this.isbn = isbn;
    }
    
    public void setYear(int year) {
        this.year = year;
    }
    
    public void setAvailable(boolean available) {
        isAvailable = available;
    }
    
    // Метод для видачі книги
    public void borrowBook() {
        if (isAvailable) {
            isAvailable = false;
            System.out.println("Книгу \"" + title + "\" видано.");
        } else {
            System.out.println("Книга \"" + title + "\" вже видана.");
        }
    }
    
    // Метод для повернення книги
    public void returnBook() {
        if (!isAvailable) {
            isAvailable = true;
            System.out.println("Книгу \"" + title + "\" повернено.");
        } else {
            System.out.println("Книга \"" + title + "\" вже в бібліотеці.");
        }
    }
    
    // Метод toString() для красивого виводу інформації про книгу
    @Override
    public String toString() {
        String status = isAvailable ? "Доступна" : "Видана";
        return "📚 " + title + "\n" +
               "   Автор: " + author + "\n" +
               "   ISBN: " + isbn + "\n" +
               "   Рік: " + year + "\n" +
               "   Статус: " + status;
    }
}
