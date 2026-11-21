# Morse Code Learning Program - Setup Guide

## Required Programs

### 1. Install SFML Library

**Option A: Using MSYS2 (Recommended for Windows)**

1. Download and install [MSYS2](https://www.msys2.org/)
2. Open MSYS2 UCRT64 terminal and run these commands:

```bash
pacman -Syu
pacman -S mingw-w64-ucrt-x86_64-sfml mingw-w64-ucrt-x86_64-gcc
```

3. Optional (only if you want `g++` available everywhere): add `C:\msys64\ucrt64\bin` to PATH.
   Inside the MSYS2 UCRT64 shell it already works.

**Option B: Using Visual Studio**

1. Install [Visual Studio 2022](https://visualstudio.microsoft.com/)
2. During installation, select "Desktop development with C++"
3. SFML will be available through the NuGet package manager

**Option C: Using vcpkg**

1. Install [vcpkg](https://vcpkg.io/en/getting-started)
2. Run: `vcpkg install sfml:x64-windows`

### 2. Compile the Program

The code uses SFML 3 (requires C++17). From the project root:

**Using MSYS2 GCC (UCRT64 shell):**
```bash
g++ -std=c++17 -o bin/Morse_Code_Learning_Program_V1.3.exe src/*.cpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

The SFML 3 DLLs the exe needs are already in `bin\` (copied from `C:\msys64\ucrt64\bin`).

### 3. Run the Program

The program switches to the project root on startup, so it can be launched from anywhere:

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
