# Morse Code Learning Program

A desktop app for learning and practicing Morse code, written in C++ with [SFML](https://www.sfml-dev.org/).
Supports guided lessons, a practice mode, and two-way translation (text ↔ Morse) with audible dot/dash playback.

## Run it

Double-click **`bin\Morse_Code_Learning_Program_V1.3.exe`** (or use `Run Morse Program.bat`).
The program locates its own data (`assets\`, lessons, practice files) regardless of where it is launched from.

## Build it

Requires a C++17 compiler and SFML 3. See [docs/SETUP_GUIDE.md](docs/SETUP_GUIDE.md).

```bash
g++ -std=c++17 -o bin/Morse_Code_Learning_Program_V1.3.exe src/*.cpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

Tests: `g++ -std=c++17 -o bin/run_tests.exe src/tests.cpp src/morse.cpp src/content.cpp && bin/run_tests.exe`

## Project structure

```
├── src/       Source code
├── assets/    Font and dot/dash sounds
├── bin/       Built exe + DLLs (not committed); lessons and practice content live here
├── docs/      Setup guide, patch notes, original presentation
└── archive/   Old versions and prototypes (kept for reference)
```

## History

See [docs/CHANGELOG.md](docs/CHANGELOG.md) for the version history.
