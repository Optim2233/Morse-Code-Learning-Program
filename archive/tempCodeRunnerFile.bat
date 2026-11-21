@echo off
set SRC=src\Morse_Code_Learning_Program_V1.3.cpp
set OUT=bin\Morse_Code_Learning_Program_V1.3.exe

"D:\MinGW730\mingw64\bin\g++.exe" %SRC% -o %OUT% ^
    -ID:/SFML-2.5.1/SFML-2.5.1/include ^
    -LD:/SFML-2.5.1/SFML-2.5.1/lib ^
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio ^
    -std=c++17 
if exist %OUT% (
    echo.
    echo Running %OUT% ... 
    %OUT%
) else ( 
    echo.
    echo Compilation failed.
)

pause