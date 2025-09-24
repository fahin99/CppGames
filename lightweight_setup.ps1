Write-Host "==================================="
Write-Host "  Lightweight C++ Setup (No IDE)"
Write-Host "==================================="
Write-Host ""

Write-Host "Option 1: Direct MinGW Installation"
Write-Host "-----------------------------------"
Write-Host "1. Go to: https://www.mingw-w64.org/downloads/"
Write-Host "2. Click 'MingW-W64-builds'"
Write-Host "3. Download the latest release .zip file"
Write-Host "4. Extract to C:\mingw64\"
Write-Host "5. Add C:\mingw64\bin to Windows PATH"
Write-Host ""

Write-Host "Option 2: MSYS2 (Package Manager)"
Write-Host "--------------------------------"
Write-Host "1. Download MSYS2: https://www.msys2.org/"
Write-Host "2. Install and run these commands:"
Write-Host "   pacman -S mingw-w64-x86_64-gcc"
Write-Host "   pacman -S mingw-w64-x86_64-sfml"
Write-Host "3. Add C:\msys64\mingw64\bin to PATH"
Write-Host ""

Write-Host "Option 3: TDM-GCC (Simplest)"
Write-Host "----------------------------"
Write-Host "1. Download: https://jmeubank.github.io/tdm-gcc/"
Write-Host "2. Install (automatically adds to PATH)"
Write-Host "3. Download SFML separately"
Write-Host ""

Write-Host "Option 4: Visual Studio Build Tools Only"
Write-Host "----------------------------------------"
Write-Host "1. Download 'Build Tools for Visual Studio'"
Write-Host "2. Install C++ build tools"
Write-Host "3. Use vcpkg for SFML: vcpkg install sfml"
Write-Host ""

Write-Host "Option 5: Clang/LLVM"
Write-Host "-------------------"
Write-Host "1. Download: https://llvm.org/"
Write-Host "2. Install Clang compiler"
Write-Host "3. Download SFML"
Write-Host ""

Write-Host "==================================="
Write-Host "After ANY compiler installation:"
Write-Host "==================================="
Write-Host "1. Open Command Prompt or PowerShell"
Write-Host "2. Navigate to your game folder:"
Write-Host "   cd 'D:\OneDrive\Documents\C++_game'"
Write-Host "3. Compile with:"
Write-Host "   g++ -std=c++17 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o game.exe"
Write-Host "4. Run: game.exe"
Write-Host ""

Write-Host "No IDE needed - just compiler + command line!"
Write-Host ""

# Try automated MSYS2 installation
Write-Host "Attempting automatic MSYS2 installation..."
try {
    winget install -e --id=msys2.msys2 --silent
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ MSYS2 installed! Now run these commands in MSYS2 terminal:"
        Write-Host "  pacman -S mingw-w64-x86_64-gcc"
        Write-Host "  pacman -S mingw-w64-x86_64-sfml"
        Write-Host "Then add C:\msys64\mingw64\bin to your PATH"
    }
} catch {
    Write-Host "Automatic installation failed. Please try manual methods above."
}

Write-Host ""
Write-Host "Press any key to continue..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")