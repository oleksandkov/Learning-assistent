# OOP - Object-Oriented Programming Course

This folder contains all lectures and practical work for the OOP course. Each lecture is organized as an independent Java project that can be opened in both VS Code and IntelliJ IDEA.

## Structure

Each lecture folder contains:
- `src/` - Java source files
- `bin/` - Compiled output (VS Code)
- `out/` - Compiled output (IntelliJ IDEA)
- `.idea/` - IntelliJ IDEA project configuration
- `*.iml` - IntelliJ module file
- `note.md` - Lecture notes and documentation
- `README.md` - Lecture-specific information

## How to Run

### In IntelliJ IDEA:
1. Open the lecture folder (e.g., `Lecture 1/`)
2. IntelliJ will automatically recognize it as a Java project
3. Open `src/Main.java`
4. Click the green play button next to the `main` method
5. Or press `Shift + F10`

### In VS Code:
1. Open any `.java` file in the lecture folder
2. Press `Ctrl + Shift + B` (or `Cmd + Shift + B` on Mac)
3. Select "Run Java Project (IntelliJ-style)"
4. The output will appear in the terminal

### Via Terminal:
```bash
cd "Lecture 1"
javac -encoding UTF-8 -d bin src/**/*.java
java -cp bin Main
```

## Lectures

- **Lecture 1** - Introduction to OOP
- **Lecture 2** - Classes and Objects
- _(More lectures will be added)_
