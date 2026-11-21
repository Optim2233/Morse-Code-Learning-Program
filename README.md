# Morse Code Learning Program

A desktop app for learning and practicing Morse code, written in C++ with [SFML](https://www.sfml-dev.org/).
Supports guided lessons, a practice mode, and two-way translation (text ↔ Morse) with audible dot/dash playback.

## Run it

Double-click **`Run Morse Program.bat`** (must stay in the project root — the program loads
`assets\` and `bin\Lessons\` relative to where it is launched).

## Build it

Requires a C++ compiler and SFML 2. See [docs/SETUP_GUIDE.md](docs/SETUP_GUIDE.md).

```bash
g++ -o bin/Morse_Code_Learning_Program_V1.3.exe src/Morse_Code_Learning_Program_V1.3.cpp -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system
```

## Project structure

```
├── src/       Source code
├── assets/    Font and dot/dash sounds
├── bin/       Built exe + DLLs (not committed); Lessons and practice content live here
├── docs/      Setup guide, patch notes, original presentation
└── archive/   Old versions and prototypes (kept for reference)
```

## History

See [docs/Patch Notes.txt](docs/Patch%20Notes.txt) for the version history.
