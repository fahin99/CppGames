@echo off
echo Testing SFML installation...

echo Checking g++ compiler...
g++ --version
if %errorlevel% neq 0 (
    echo ERROR: g++ compiler not found!
    pause
    exit /b 1
)

echo.
echo Attempting to compile with SFML...
g++ -std=c++17 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe 2>error_log.txt

if %errorlevel% == 0 (
    echo SUCCESS! Game compiled successfully!
    echo You can now run: dungeon_crawler.exe
    del error_log.txt 2>nul
) else (
    echo COMPILATION FAILED. Error details:
    type error_log.txt
    echo.
    echo This likely means SFML is not installed.
    echo Let me try to download and install SFML automatically...
    
    echo.
    echo Downloading SFML...
    powershell -Command "Invoke-WebRequest -Uri 'https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip' -OutFile 'sfml.zip'"
    
    if exist sfml.zip (
        echo Extracting SFML...
        powershell -Command "Expand-Archive -Path 'sfml.zip' -DestinationPath 'C:\' -Force"
        echo SFML extracted to C:\SFML-2.6.1\
        
        echo Trying compilation with explicit SFML paths...
        g++ -std=c++17 *.cpp -I"C:\SFML-2.6.1\include" -L"C:\SFML-2.6.1\lib" -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe
        
        if %errorlevel% == 0 (
            echo SUCCESS! Game compiled with downloaded SFML!
            echo Don't forget to add C:\SFML-2.6.1\bin to your PATH
            copy "C:\SFML-2.6.1\bin\*.dll" . >nul 2>&1
            echo DLL files copied to game directory.
        ) else (
            echo Still failed. Please check error_log.txt for details.
        )
        
        del sfml.zip >nul 2>&1
    ) else (
        echo Failed to download SFML. Please install manually.
    )
)

echo.
pause