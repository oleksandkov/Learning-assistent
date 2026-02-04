public class Main {
    public static void main(String[] args) {
        System.out.println("Welcome to Lecture 1 - Introduction to OOP!");
        
        // Example: Creating a simple object
        Student student = new Student("Alex", 20);
        student.introduce();
    }
}

class Student {
    private String name;
    private int age;
    
    public Student(String name, int age) {
        this.name = name;
        this.age = age;
    }
    
    public void introduce() {
        System.out.println("Hi, I'm " + name + " and I'm " + age + " years old.");
    }
}
