@echo off
echo ================================
echo   Dungeon Crawler Build Script
echo ================================
echo.

REM Check if we have a compiler
echo Checking for available compilers...

REM Check for g++
g++ --version >nul 2>&1
if %errorlevel% == 0 (
    echo [FOUND] g++ compiler available
    set COMPILER=gcc
    goto :build_with_gcc
) else (
    echo [NOT FOUND] g++ compiler not available
)

REM Check for Visual Studio
cl >nul 2>&1
if %errorlevel% == 9009 (
    echo [NOT FOUND] Visual Studio cl compiler not available
) else (
    echo [FOUND] Visual Studio cl compiler available
    set COMPILER=msvc
    goto :setup_msvc
)

echo.
echo ERROR: No suitable C++ compiler found!
echo Please install one of the following:
echo   1. MinGW-w64 (includes g++)
echo   2. Visual Studio with C++ tools
echo   3. Clang
echo.
echo You can install MinGW-w64 from: https://www.mingw-w64.org/
pause
exit /b 1

:build_with_gcc
echo.
echo ================================
echo   Building with GCC
echo ================================
echo.

REM Try to compile without SFML first to check basic setup
echo Step 1: Testing basic compilation...
g++ -std=c++17 -c main.cpp -o main.o 2>compile_errors.txt
if %errorlevel% neq 0 (
    echo ERROR: Compilation failed. Check compile_errors.txt
    type compile_errors.txt
    echo.
    echo This means SFML is not installed or not found.
    echo Please install SFML:
    echo   1. Download SFML from https://www.sfml-dev.org/download.php
    echo   2. Extract to C:\SFML\
    echo   3. Add C:\SFML\bin to your PATH
    pause
    exit /b 1
)

echo Step 2: Compiling all source files...
g++ -std=c++17 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe 2>link_errors.txt
if %errorlevel% neq 0 (
    echo ERROR: Linking failed. SFML libraries not found.
    type link_errors.txt
    echo.
    echo Please install SFML:
    echo   1. Download SFML GCC version from https://www.sfml-dev.org/download.php
    echo   2. Extract to C:\SFML\
    echo   3. Add C:\SFML\bin to your PATH
    echo   4. Try: g++ -std=c++17 *.cpp -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe
    pause
    exit /b 1
)

echo.
echo SUCCESS! Game compiled successfully!
echo Run the game with: dungeon_crawler.exe
echo.
pause
exit /b 0

:setup_msvc
echo.
echo For Visual Studio, please:
echo 1. Install vcpkg package manager
echo 2. Run: vcpkg install sfml:x64-windows
echo 3. Use Visual Studio to build the project
echo.
pause
exit /b 0ding Dungeon Crawler...

REM Check if SFML is available (this is a basic check)
echo Make sure SFML is installed and configured properly.

REM Example compile command for MinGW/GCC
REM Adjust paths as needed for your SFML installation
echo.
echo To compile with GCC/MinGW:
echo g++ -std=c++17 *.cpp -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe
echo.

REM Example for Visual Studio (from VS Developer Command Prompt)
echo To compile with Visual Studio:
echo cl /EHsc /std:c++17 *.cpp /I"C:\SFML\include" /link /LIBPATH:"C:\SFML\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib

echo.
echo Adjust the SFML paths according to your installation.
pause