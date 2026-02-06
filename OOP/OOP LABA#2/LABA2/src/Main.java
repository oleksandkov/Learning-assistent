import java.util.Scanner;

/**
 * Лабораторна робота #2 - Основи ООП
 * Тема: Класи, об'єкти, конструктори, методи
 * 
 * Приклад: Система управління студентами
 */

// Клас Student - представляє студента
class Student {
    // Поля класу (приватні - інкапсуляція)
    private String name;           // Ім'я студента
    private String surname;        // Прізвище студента
    private int course;            // Курс навчання
    private double averageGrade;   // Середній бал
    private String faculty;        // Факультет
    
    // Конструктор - метод для створення об'єкта
    public Student(String name, String surname, int course, double averageGrade, String faculty) {
        this.name = name;
        this.surname = surname;
        this.course = course;
        this.averageGrade = averageGrade;
        this.faculty = faculty;
    }
    
    // Геттери - методи для отримання значень полів
    public String getName() {
        return name;
    }
    
    public String getSurname() {
        return surname;
    }
    
    public int getCourse() {
        return course;
    }
    
    public double getAverageGrade() {
        return averageGrade;
    }
    
    public String getFaculty() {
        return faculty;
    }
    
    // Сеттери - методи для зміни значень полів
    public void setName(String name) {
        this.name = name;
    }
    
    public void setSurname(String surname) {
        this.surname = surname;
    }
    
    public void setCourse(int course) {
        if (course >= 1 && course <= 6) {
            this.course = course;
        } else {
            System.out.println("Помилка: курс має бути від 1 до 6");
        }
    }
    
    public void setAverageGrade(double averageGrade) {
        if (averageGrade >= 0 && averageGrade <= 100) {
            this.averageGrade = averageGrade;
        } else {
            System.out.println("Помилка: оцінка має бути від 0 до 100");
        }
    }
    
    public void setFaculty(String faculty) {
        this.faculty = faculty;
    }
    
    // Додаткові методи
    
    // Метод для виведення повної інформації про студента
    public void displayInfo() {
        System.out.println("\n=== Інформація про студента ===");
        System.out.println("ПІБ: " + surname + " " + name);
        System.out.println("Факультет: " + faculty);
        System.out.println("Курс: " + course);
        System.out.println("Середній бал: " + averageGrade);
    }
    
    // Метод для визначення статусу стипендії
    public String getScholarshipStatus() {
        if (averageGrade >= 90) {
            return "Підвищена стипендія";
        } else if (averageGrade >= 75) {
            return "Звичайна стипендія";
        } else if (averageGrade >= 60) {
            return "Без стипендії (задовільна успішність)";
        } else {
            return "Академічна заборгованість";
        }
    }
    
    // Метод для переведення на наступний курс
    public void promoteToNextCourse() {
        if (course < 6) {
            course++;
            System.out.println(name + " " + surname + " переведено на " + course + " курс");
        } else {
            System.out.println(name + " " + surname + " вже на останньому курсі");
        }
    }
    
    // Метод для порівняння студентів за середнім балом
    public boolean hasBetterGradeThan(Student other) {
        return this.averageGrade > other.averageGrade;
    }
    
    // Метод для отримання повного імені
    public String getFullName() {
        return surname + " " + name;
    }
}

// Головний клас з методом main
public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        
        System.out.println("╔════════════════════════════════════════════╗");
        System.out.println("║   Лабораторна робота #2 - Основи ООП     ║");
        System.out.println("║   Система управління студентами           ║");
        System.out.println("╚════════════════════════════════════════════╝\n");
        
        // Демонстрація 1: Створення об'єктів за допомогою конструктора
        System.out.println("=== Демонстрація 1: Створення студентів ===\n");
        
        Student student1 = new Student("Олександр", "Коваленко", 2, 85.5, "Комп'ютерні науки");
        Student student2 = new Student("Марія", "Шевченко", 3, 92.0, "Інформаційні технології");
        Student student3 = new Student("Дмитро", "Бойко", 1, 67.5, "Кібербезпека");
        
        // Виведення інформації про всіх студентів
        student1.displayInfo();
        System.out.println("Статус стипендії: " + student1.getScholarshipStatus());
        
        student2.displayInfo();
        System.out.println("Статус стипендії: " + student2.getScholarshipStatus());
        
        student3.displayInfo();
        System.out.println("Статус стипендії: " + student3.getScholarshipStatus());
        
        // Демонстрація 2: Використання геттерів
        System.out.println("\n=== Демонстрація 2: Використання геттерів ===\n");
        System.out.println("Ім'я першого студента: " + student1.getName());
        System.out.println("Прізвище: " + student1.getSurname());
        System.out.println("Курс: " + student1.getCourse());
        System.out.println("Середній бал: " + student1.getAverageGrade());
        System.out.println("Факультет: " + student1.getFaculty());
        
        // Демонстрація 3: Використання сеттерів
        System.out.println("\n=== Демонстрація 3: Зміна даних студента ===\n");
        System.out.println("Оновлюємо середній бал студента " + student1.getFullName());
        System.out.println("Попередній бал: " + student1.getAverageGrade());
        student1.setAverageGrade(88.0);
        System.out.println("Новий бал: " + student1.getAverageGrade());
        System.out.println("Новий статус стипендії: " + student1.getScholarshipStatus());
        
        // Демонстрація 4: Переведення на наступний курс
        System.out.println("\n=== Демонстрація 4: Переведення на наступний курс ===\n");
        student1.promoteToNextCourse();
        System.out.println("Поточний курс: " + student1.getCourse());
        
        // Демонстрація 5: Порівняння студентів
        System.out.println("\n=== Демонстрація 5: Порівняння студентів ===\n");
        if (student1.hasBetterGradeThan(student3)) {
            System.out.println(student1.getFullName() + " має кращий середній бал ніж " + student3.getFullName());
        } else {
            System.out.println(student3.getFullName() + " має кращий середній бал ніж " + student1.getFullName());
        }
        
        // Демонстрація 6: Інтерактивне створення студента
        System.out.println("\n=== Демонстрація 6: Створення власного студента ===\n");
        
        System.out.print("Введіть ім'я студента: ");
        String name = scanner.nextLine();
        
        System.out.print("Введіть прізвище студента: ");
        String surname = scanner.nextLine();
        
        System.out.print("Введіть курс (1-6): ");
        int course = scanner.nextInt();
        
        System.out.print("Введіть середній бал (0-100): ");
        double grade = scanner.nextDouble();
        scanner.nextLine(); // Очищення буфера
        
        System.out.print("Введіть факультет: ");
        String faculty = scanner.nextLine();
        
        // Створення нового студента
        Student customStudent = new Student(name, surname, course, grade, faculty);
        
        // Виведення інформації про створеного студента
        customStudent.displayInfo();
        System.out.println("Статус стипендії: " + customStudent.getScholarshipStatus());
        
        // Демонстрація 7: Статистика
        System.out.println("\n=== Демонстрація 7: Загальна статистика ===\n");
        
        double totalGrade = student1.getAverageGrade() + student2.getAverageGrade() + 
                           student3.getAverageGrade() + customStudent.getAverageGrade();
        double averageOfAll = totalGrade / 4;
        
        System.out.println("Середній бал всіх студентів: " + String.format("%.2f", averageOfAll));
        System.out.println("Кількість студентів: 4");
        
        // Знаходження найкращого студента
        Student bestStudent = student1;
        if (student2.getAverageGrade() > bestStudent.getAverageGrade()) {
            bestStudent = student2;
        }
        if (student3.getAverageGrade() > bestStudent.getAverageGrade()) {
            bestStudent = student3;
        }
        if (customStudent.getAverageGrade() > bestStudent.getAverageGrade()) {
            bestStudent = customStudent;
        }
        
        System.out.println("Найкращий студент: " + bestStudent.getFullName() + 
                          " (бал: " + bestStudent.getAverageGrade() + ")");
        
        System.out.println("\n╔════════════════════════════════════════════╗");
        System.out.println("║   Лабораторна робота завершена!           ║");
        System.out.println("╚════════════════════════════════════════════╝");
        
        scanner.close();
    }
}
