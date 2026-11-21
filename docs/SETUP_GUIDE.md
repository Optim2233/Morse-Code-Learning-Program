# Morse Code Learning Program - Setup Guide

## Required Programs

### 1. Install SFML Library

**Option A: Using MSYS2 (Recommended for Windows)**

1. Download and install [MSYS2](https://www.msys2.org/)
2. Open MSYS2 MinGW 64-bit terminal
3. Run these commands:

```bash
pacman -Syu
pacman -S mingw-w64-x86_64-sfml mingw-w64-x86_64-gcc
```

4. Add MSYS2 to your PATH:
   - Open System Properties → Environment Variables
   - Add: `C:\msys64\mingw64\bin` to PATH

**Option B: Using Visual Studio**

1. Install [Visual Studio 2022](https://visualstudio.microsoft.com/)
2. During installation, select "Desktop development with C++"
3. SFML will be available through the NuGet package manager

**Option C: Using vcpkg**

1. Install [vcpkg](https://vcpkg.io/en/getting-started)
2. Run: `vcpkg install sfml:x64-windows`

### 2. Compile the Program

**Using MSYS2 GCC:**
```bash
g++ -o "bin/Morse_Code_Learning_Program_V1.3.exe" src/Morse_Code_Learning_Program_V1.3.cpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

**Using Visual Studio:**
```bash
cl /EHsc /Fe:bin\Morse_Code_Learning_Program_V1.3.exe src\Morse_Code_Learning_Program_V1.3.cpp /I"C:\msys64\mingw64\include" /link /LIBPATH:"C:\msys64\mingw64\lib" sfml-graphics.lib sfml-audio.lib sfml-window.lib sfml-system.lib
```

### 3. Run the Program

**Important:** the program loads `assets\` and `bin\Lessons\` relative to the folder it is launched from,
so it must be run from the project root (or use `Run Morse Program.bat` in the project root):

```bash
./bin/Morse_Code_Learning_Program_V1.3.exe
```

## Troubleshooting

**If you get "sfml not found" errors:**
- Make sure SFML is installed in the correct directory
- Check that the include paths are correct
- Verify the library paths in your compiler settings

**If you get linker errors:**
- Make sure you're linking against all SFML libraries (graphics, audio, window, system)
- Check that the library files exist in your library path

## Project Structure

```
C++ Project CD/
├── src/
│   └── Morse_Code_Learning_Program_V1.3.cpp  (Main source file)
├── bin/
│   ├── Lessons/          (Lesson files)
│   └── Practice/         (Practice files)
└── assets/               (Optional: images, sounds)
```
