@echo off
echo ================================
echo   SFML Game Setup Assistant
echo ================================
echo.
echo This script will help you set up everything needed to build and run the game.
echo.

:menu
echo Please choose your preferred setup method:
echo.
echo 1. Install MinGW-w64 + SFML (Recommended for beginners)
echo 2. Use existing Visual Studio + vcpkg
echo 3. Manual SFML installation guide
echo 4. Check current setup
echo 5. Exit
echo.
set /p choice="Enter your choice (1-5): "

if "%choice%"=="1" goto :install_mingw
if "%choice%"=="2" goto :vcpkg_setup
if "%choice%"=="3" goto :manual_guide
if "%choice%"=="4" goto :check_setup
if "%choice%"=="5" goto :end
echo Invalid choice. Please try again.
goto :menu

:install_mingw
echo.
echo ================================
echo   MinGW-w64 + SFML Installation
echo ================================
echo.
echo Please follow these steps:
echo.
echo 1. Download MinGW-w64:
echo    https://github.com/niXman/mingw-builds-binaries/releases
echo    Choose: x86_64-win32-seh (or x86_64-posix-seh)
echo.
echo 2. Extract to C:\mingw64\
echo.
echo 3. Add C:\mingw64\bin to your Windows PATH
echo.
echo 4. Download SFML:
echo    https://www.sfml-dev.org/download/sfml/2.6.1/
echo    Choose: GCC 13.1.0 MinGW (SEH) - 64-bit
echo.
echo 5. Extract to C:\SFML\
echo.
echo 6. Add C:\SFML\bin to your Windows PATH
echo.
echo Press any key when you've completed these steps...
pause >nul
goto :check_setup

:vcpkg_setup
echo.
echo ================================
echo   Visual Studio + vcpkg Setup
echo ================================
echo.
echo 1. Install Visual Studio 2022 with C++ development tools
echo 2. Clone vcpkg: git clone https://github.com/Microsoft/vcpkg.git
echo 3. Run vcpkg\bootstrap-vcpkg.bat
echo 4. Install SFML: vcpkg install sfml:x64-windows
echo 5. Integrate with VS: vcpkg integrate install
echo.
echo Press any key to continue...
pause >nul
goto :menu

:manual_guide
echo.
echo ================================
echo   Manual SFML Installation
echo ================================
echo.
echo 1. Visit https://www.sfml-dev.org/download.php
echo 2. Download the version matching your compiler:
echo    - For MinGW: GCC version
echo    - For Visual Studio: Visual C++ version
echo 3. Extract to a folder (e.g., C:\SFML\)
echo 4. Add the bin folder to your PATH
echo 5. When compiling, use appropriate include and library paths
echo.
echo Example compile command:
echo g++ -std=c++17 *.cpp -IC:\SFML\include -LC:\SFML\lib -lsfml-graphics -lsfml-window -lsfml-system -o game.exe
echo.
echo Press any key to continue...
pause >nul
goto :menu

:check_setup
echo.
echo ================================
echo   Checking Current Setup
echo ================================
echo.

echo Checking for C++ compilers...
g++ --version >nul 2>&1
if %errorlevel% == 0 (
    echo [OK] g++ found
    g++ --version | head -1
) else (
    echo [MISSING] g++ not found
)

cl >nul 2>&1
if %errorlevel% == 9009 (
    echo [MISSING] Visual Studio cl not found
) else (
    echo [OK] Visual Studio cl found
)

echo.
echo Checking for SFML...
if exist "C:\SFML\include\SFML\Graphics.hpp" (
    echo [OK] SFML headers found in C:\SFML\
) else (
    echo [MISSING] SFML headers not found in C:\SFML\
)

if exist "C:\SFML\lib\libsfml-graphics.a" (
    echo [OK] SFML libraries found (MinGW version)
) else if exist "C:\SFML\lib\sfml-graphics.lib" (
    echo [OK] SFML libraries found (MSVC version)
) else (
    echo [MISSING] SFML libraries not found
)

echo.
echo Now trying to compile a test program...
echo #include ^<SFML/Graphics.hpp^> > test.cpp
echo int main^(^) { return 0; } >> test.cpp

g++ -std=c++17 test.cpp -lsfml-graphics -lsfml-window -lsfml-system -o test.exe >nul 2>&1
if %errorlevel% == 0 (
    echo [SUCCESS] Test compilation successful!
    del test.cpp test.exe >nul 2>&1
    echo.
    echo Your setup looks good! You can now run build.bat to compile the game.
) else (
    echo [FAILED] Test compilation failed
    echo This means SFML is not properly installed or configured.
    del test.cpp >nul 2>&1
    echo.
    echo Please follow the installation steps above.
)

echo.
pause
goto :menu

:end
echo.
echo Setup complete! Run build.bat to compile your game.
echo.