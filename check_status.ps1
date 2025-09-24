Write-Host "==================================="
Write-Host "    Installation Status Check"
Write-Host "==================================="
Write-Host ""

# Check for C++ compilers
Write-Host "Checking for C++ compilers..."
$compilers = @("g++", "gcc", "clang++", "cl")
$foundCompiler = $false

foreach ($compiler in $compilers) {
    try {
        $result = Get-Command $compiler -ErrorAction Stop
        Write-Host "✓ Found: $compiler at $($result.Source)" -ForegroundColor Green
        $foundCompiler = $true
        
        # Test version
        try {
            $version = & $compiler --version 2>&1 | Select-Object -First 1
            Write-Host "  Version: $version" -ForegroundColor Gray
        } catch {
            Write-Host "  (Could not get version)" -ForegroundColor Yellow
        }
    } catch {
        Write-Host "✗ Not found: $compiler" -ForegroundColor Red
    }
}

Write-Host ""

# Check for SFML in common locations
Write-Host "Checking for SFML..."
$sfmlPaths = @(
    "C:\SFML",
    "C:\SFML-2.6.1", 
    "C:\Program Files\SFML",
    "C:\dev-tools\SFML",
    "C:\mingw64\include\SFML",
    "C:\msys64\mingw64\include\SFML"
)

$foundSFML = $false
foreach ($path in $sfmlPaths) {
    if (Test-Path "$path\include\SFML\Graphics.hpp") {
        Write-Host "✓ Found SFML at: $path" -ForegroundColor Green
        $foundSFML = $true
        break
    }
}

if (-not $foundSFML) {
    Write-Host "✗ SFML not found in common locations" -ForegroundColor Red
}

Write-Host ""

# Check development directories
Write-Host "Checking for development tool directories..."
$devDirs = @("C:\mingw64", "C:\msys64", "C:\dev-tools", "C:\TDM-GCC-64")
foreach ($dir in $devDirs) {
    if (Test-Path $dir) {
        Write-Host "✓ Found directory: $dir" -ForegroundColor Green
    }
}

Write-Host ""

# Overall status
Write-Host "==================================="
if ($foundCompiler -and $foundSFML) {
    Write-Host "STATUS: ✓ Ready to compile!" -ForegroundColor Green
    Write-Host "You can now try running: build.bat" -ForegroundColor Green
} elseif ($foundCompiler) {
    Write-Host "STATUS: ⚠ Compiler found, but SFML missing" -ForegroundColor Yellow
    Write-Host "Need to install SFML library" -ForegroundColor Yellow
} elseif ($foundSFML) {
    Write-Host "STATUS: ⚠ SFML found, but no compiler" -ForegroundColor Yellow
    Write-Host "Need to install C++ compiler (MinGW or Visual Studio)" -ForegroundColor Yellow
} else {
    Write-Host "STATUS: ✗ Neither compiler nor SFML found" -ForegroundColor Red
    Write-Host "Need to install both C++ compiler and SFML" -ForegroundColor Red
}

Write-Host ""
Write-Host "Recommendations:"
if (-not $foundCompiler) {
    Write-Host "1. Install MinGW-w64: https://www.mingw-w64.org/downloads/"
    Write-Host "   OR Code::Blocks with MinGW: https://www.codeblocks.org/downloads/"
}
if (-not $foundSFML) {
    Write-Host "2. Install SFML: https://www.sfml-dev.org/download.php"
    Write-Host "   Extract to C:\SFML\ and add C:\SFML\bin to PATH"
}

Write-Host ""
Write-Host "Press any key to continue..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")