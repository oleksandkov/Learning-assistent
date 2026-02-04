# Lecture 2: Classes and Objects - Detailed Notes

## Overview
In this lecture, we explore the fundamental concepts of classes and objects in Object-Oriented Programming.

## Key Concepts

### 1. What is a Class?
A class is a blueprint or template for creating objects. It defines:
- **Attributes** (fields/properties) - what data the object holds
- **Methods** (functions) - what actions the object can perform

### 2. What is an Object?
An object is an instance of a class. It has:
- **State** - the current values of its attributes
- **Behavior** - the methods it can execute

### 3. Syntax Example

```java
class Car {
    // Attributes (state)
    private String brand;
    private String model;
    private int year;
    
    // Constructor (how to create an object)
    public Car(String brand, String model, int year) {
        this.brand = brand;
        this.model = model;
        this.year = year;
    }
    
    // Method (behavior)
    public void displayInfo() {
        System.out.println(year + " " + brand + " " + model);
    }
}
```

### 4. Creating Objects

```java
// Create two different Car objects
Car car1 = new Car("Toyota", "Camry", 2020);
Car car2 = new Car("Honda", "Accord", 2021);

// Each object has its own state
car1.displayInfo();  // 2020 Toyota Camry
car2.displayInfo();  // 2021 Honda Accord
```

## Important Points

1. **Encapsulation**: Use `private` for attributes to hide internal data
2. **Constructor**: Special method that initializes object state
3. **this keyword**: Refers to the current object instance
4. **Multiple Objects**: Each object is independent with its own state

## Practice Tasks

1. Modify the Car class to add more attributes (color, price)
2. Add methods: `accelerate()`, `brake()`
3. Create a new class `Book` with appropriate attributes and methods
4. Experiment with creating multiple objects and calling their methods

## References
- [Lecture 1 - Introduction to OOP](../Lecture%201/README.md)
- See example code in `src/Main.java`
