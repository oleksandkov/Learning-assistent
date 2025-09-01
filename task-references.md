# VS Code Task References

This document lists all tasks defined in your `.vscode/tasks.json` file, explains what each task does, and provides usage examples.

---

## 1. Copy, Compile, and Run any .cpp file

**Label:** `Copy, Compile, and Run any .cpp file`

**Type:** `shell`

**What it does:**
- Copies the currently open `.cpp` file to `C:\Temp`.
- Compiles it using `g++`.
- Runs the resulting executable.

**How it works:**
- Uses VS Code's `${file}` variable to get the path of the currently active file.
- The executable will have the same name as your `.cpp` file, but with `.exe` extension.

**Task Definition:**
```json
{
    "label": "Copy, Compile, and Run any .cpp file",
    "type": "shell",
    "command": "Copy-Item \"${file}\" \"C:\\Temp\\${fileBasename}\" -Force; cd C:\\Temp; g++ ${fileBasename} -o ${fileBasenameNoExtension}.exe; .\\${fileBasenameNoExtension}.exe",
    "problemMatcher": [],
    "group": {
        "kind": "build",
        "isDefault": true
    },
    "presentation": {
        "echo": true,
        "reveal": "always",
        "focus": false,
        "panel": "shared"
    }
}
```

**How to use:**
1. Open any `.cpp` file in VS Code.
2. Press `Ctrl+Shift+B` to open the build tasks menu.
3. Select `Copy, Compile, and Run any .cpp file`.
4. The output will appear in the terminal panel.

**Example:**
- If you have a file named `myprogram.cpp` open, running this task will:
    - Copy `myprogram.cpp` to `C:\Temp\myprogram.cpp`
    - Compile it to `C:\Temp\myprogram.exe`
    - Run `C:\Temp\myprogram.exe` and show the output in the terminal.

---

**Tip:**
- You can add more tasks to `.vscode/tasks.json` for other workflows as needed.
