public class Main {
    public static void main(String[] args) {
        System.out.println("Welcome to Lecture 2 - Classes and Objects!");
        
        // Example: Working with multiple objects
        Car car1 = new Car("Toyota", "Camry", 2020);
        Car car2 = new Car("Honda", "Accord", 2021);
        
        car1.displayInfo();
        car2.displayInfo();
        
        // Example: Object methods
        car1.startEngine();
        car2.startEngine();
    }
}

class Car {
    private String brand;
    private String model;
    private int year;
    private boolean engineRunning;
    
    public Car(String brand, String model, int year) {
        this.brand = brand;
        this.model = model;
        this.year = year;
        this.engineRunning = false;
    }
    
    public void displayInfo() {
        System.out.println(year + " " + brand + " " + model);
    }
    
    public void startEngine() {
        if (!engineRunning) {
            engineRunning = true;
            System.out.println(brand + " " + model + " engine started!");
        } else {
            System.out.println(brand + " " + model + " engine is already running.");
        }
    }
    
    public void stopEngine() {
        if (engineRunning) {
            engineRunning = false;
            System.out.println(brand + " " + model + " engine stopped.");
        }
    }
}
