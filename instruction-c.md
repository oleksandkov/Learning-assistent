# How to Compile and Run a .cpp File in Windows

1. **Open your .cpp file in Visual Studio Code or your preferred editor.**

2. **Save your changes.**

3. **Open PowerShell and copy your file to a simple path (recommended):**
   ```powershell
   Copy-Item "<full-path-to-your-file>.cpp" "C:\Temp\<your-file>.cpp" -Force
   cd C:\Temp
   ```
   Replace `<full-path-to-your-file>` and `<your-file>` with your actual file name.

4. **Compile the file using g++:**
   ```powershell
   g++ <your-file>.cpp -o <your-file>.exe
   ```
   This creates an executable named `<your-file>.exe` in the same folder.

5. **Run the compiled program:**
   ```powershell
   .\<your-file>.exe
   ```

---

## Using VS Code Task (Recommended)

1. Open your .cpp file in VS Code.
2. Press `Ctrl+Shift+B`.
3. Select `Copy, Compile, and Run any .cpp file` from the task list.
4. The output will appear in the terminal panel.

This task will automatically copy, compile, and run the currently open .cpp file for you.

---

**Tip:**
- Always save your file before running the task or compiling.
- If you get a 'No such file or directory' error, make sure the file is in the correct folder.
