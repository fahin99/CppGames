@echo off
echo Simple Game Build Attempt
echo ==========================

echo.
echo Trying to use any available C++ compiler...

REM Try different compiler names
where g++ >nul 2>&1
if %errorlevel% == 0 (
    echo Found g++ compiler
    set COMPILER=g++
    goto :compile
)

where gcc >nul 2>&1
if %errorlevel% == 0 (
    echo Found gcc compiler
    set COMPILER=gcc
    goto :compile
)

where clang++ >nul 2>&1
if %errorlevel% == 0 (
    echo Found clang++ compiler
    set COMPILER=clang++
    goto :compile
)

where cl >nul 2>&1
if %errorlevel% neq 9009 (
    echo Found Visual Studio cl compiler
    set COMPILER=cl
    goto :compile_msvc
)

echo.
echo ERROR: No C++ compiler found on this system!
echo.
echo Please install one of the following:
echo 1. MinGW-w64: https://www.mingw-w64.org/downloads/
echo 2. Visual Studio: https://visualstudio.microsoft.com/
echo 3. Code::Blocks with MinGW: https://www.codeblocks.org/downloads/
echo 4. Dev-C++: https://www.bloodshed.net/devcpp.html
echo.
echo After installation, make sure the compiler is in your PATH.
goto :end

:compile
echo.
echo Attempting compilation with %COMPILER%...
echo.

REM Try to compile without SFML first (will fail but show what's missing)
%COMPILER% -std=c++17 -c main.cpp 2>compile_log.txt
if %errorlevel% neq 0 (
    echo Compilation failed - SFML headers not found.
    echo Error details:
    type compile_log.txt
    echo.
    echo SFML is required. Please:
    echo 1. Download SFML from https://www.sfml-dev.org/download.php
    echo 2. Extract to C:\SFML\ 
    echo 3. Run this command:
    echo %COMPILER% -std=c++17 *.cpp -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system -o game.exe
    goto :end
)

REM Try full compilation
%COMPILER% -std=c++17 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe 2>link_log.txt
if %errorlevel% == 0 (
    echo.
    echo SUCCESS! Game compiled successfully!
    echo Run with: dungeon_crawler.exe
    del compile_log.txt link_log.txt >nul 2>&1
) else (
    echo.
    echo Linking failed - SFML libraries not found.
    echo Link error details:
    type link_log.txt
    echo.
    echo Please install SFML and try:
    echo %COMPILER% -std=c++17 *.cpp -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system -o game.exe
)
goto :end

:compile_msvc
echo.
echo Visual Studio detected but setup is more complex.
echo Please use Visual Studio IDE or install vcpkg:
echo.
echo vcpkg install sfml:x64-windows
echo.
goto :end

:end
echo.
pause