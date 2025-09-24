Write-Host "Setting up lightweight C++ development environment..."
Write-Host "===================================================="

# Check if we can use winget to install a simpler solution
try {
    Write-Host "Installing Code::Blocks with MinGW (includes g++ compiler)..."
    
    # This includes both IDE and MinGW compiler
    $result = winget install -e --id=CodeBlocks.CodeBlocks.MinGW --silent
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Code::Blocks with MinGW installed successfully!"
        
        # Try to find where it was installed
        $possiblePaths = @(
            "C:\Program Files\CodeBlocks\MinGW\bin",
            "C:\Program Files (x86)\CodeBlocks\MinGW\bin",
            "C:\CodeBlocks\MinGW\bin"
        )
        
        foreach ($path in $possiblePaths) {
            if (Test-Path "$path\g++.exe") {
                Write-Host "Found g++ at: $path"
                
                # Add to PATH
                $env:PATH += ";$path"
                
                # Test compiler
                $version = & "$path\g++.exe" --version 2>&1
                Write-Host "G++ version: $($version[0])"
                break
            }
        }
    }
} catch {
    Write-Host "Installation method 1 failed, trying alternative..."
}

# Alternative: Download portable MinGW
Write-Host "`nTrying portable MinGW installation..."

try {
    # Create a local tools directory
    $toolsDir = "C:\dev-tools"
    New-Item -ItemType Directory -Path $toolsDir -Force | Out-Null
    
    # Download a smaller, simpler MinGW distribution
    Write-Host "Downloading portable MinGW..."
    $portableUrl = "https://github.com/brechtsanders/winlibs_mingw/releases/download/13.1.0-16.0.5-11.0.0-msvcrt-r5/winlibs-x86_64-posix-seh-gcc-13.1.0-llvm-16.0.5-mingw-w64msvcrt-11.0.0-r5.zip"
    $mingwFile = "$toolsDir\mingw.zip"
    
    Invoke-WebRequest -Uri $portableUrl -OutFile $mingwFile -UseBasicParsing
    
    # Extract
    Write-Host "Extracting MinGW..."
    Expand-Archive -Path $mingwFile -DestinationPath $toolsDir -Force
    
    # Find the mingw64 folder
    $mingwPath = Get-ChildItem -Path $toolsDir -Name "mingw64" -Directory -Recurse | Select-Object -First 1
    
    if ($mingwPath) {
        $fullMingwPath = Join-Path $toolsDir $mingwPath
        $binPath = Join-Path $fullMingwPath "bin"
        
        Write-Host "MinGW extracted to: $fullMingwPath"
        
        # Add to session PATH
        $env:PATH += ";$binPath"
        
        # Test
        if (Test-Path "$binPath\g++.exe") {
            $version = & "$binPath\g++.exe" --version 2>&1
            Write-Host "G++ installed successfully: $($version[0])"
            
            Write-Host "`nNow downloading SFML..."
            
            # Download SFML
            $sfmlUrl = "https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip"
            $sfmlFile = "$toolsDir\sfml.zip"
            
            Invoke-WebRequest -Uri $sfmlUrl -OutFile $sfmlFile -UseBasicParsing
            
            # Extract SFML
            Expand-Archive -Path $sfmlFile -DestinationPath $toolsDir -Force
            
            # Move SFML to a simpler path
            $sfmlPath = "$toolsDir\SFML"
            if (Test-Path "$toolsDir\SFML-2.6.1") {
                Move-Item "$toolsDir\SFML-2.6.1" $sfmlPath -Force
            }
            
            Write-Host "SFML installed to: $sfmlPath"
            
            # Now try to compile the game
            Write-Host "`nAttempting to compile the game..."
            
            Set-Location "D:\OneDrive\Documents\C++_game"
            
            $compileCmd = "& `"$binPath\g++.exe`" -std=c++17 *.cpp -I`"$sfmlPath\include`" -L`"$sfmlPath\lib`" -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe"
            
            Invoke-Expression $compileCmd
            
            if ($LASTEXITCODE -eq 0) {
                Write-Host "`nSUCCESS! Game compiled successfully!"
                
                # Copy SFML DLLs
                Copy-Item "$sfmlPath\bin\*.dll" -Destination "." -Force
                Write-Host "SFML DLLs copied to game directory"
                
                Write-Host "`nYou can now run the game with: .\dungeon_crawler.exe"
                
                # Test run the game for a few seconds
                Write-Host "Testing the game..."
                Start-Process -FilePath ".\dungeon_crawler.exe" -NoNewWindow
                
            } else {
                Write-Host "Compilation failed. Check error messages above."
            }
            
        }
    }
    
    # Cleanup
    Remove-Item $mingwFile -ErrorAction SilentlyContinue
    Remove-Item $sfmlFile -ErrorAction SilentlyContinue
    
} catch {
    Write-Host "Error: $($_.Exception.Message)"
}

Write-Host "`nSetup complete!"