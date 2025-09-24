Write-Host "Downloading and Installing MinGW-w64 and SFML..."
Write-Host "================================================="

# Create temporary directory
$tempDir = "C:\temp_install"
New-Item -ItemType Directory -Path $tempDir -Force | Out-Null

try {
    # Download MinGW-w64
    Write-Host "Downloading MinGW-w64..."
    $mingwUrl = "https://github.com/niXman/mingw-builds-binaries/releases/download/13.1.0-rt_v11-rev1/x86_64-13.1.0-release-win32-seh-msvcrt-rt_v11-rev1.7z"
    $mingwFile = Join-Path $tempDir "mingw.7z"
    
    Invoke-WebRequest -Uri $mingwUrl -OutFile $mingwFile -UseBasicParsing
    Write-Host "MinGW-w64 downloaded successfully"
    
    # Extract MinGW (assuming 7-Zip is available or using PowerShell expansion)
    Write-Host "Extracting MinGW-w64 to C:\mingw64..."
    
    # Try to use built-in Windows tar command
    $extractResult = Start-Process -FilePath "tar" -ArgumentList "-xf", $mingwFile, "-C", "C:\" -Wait -PassThru
    
    if ($extractResult.ExitCode -eq 0) {
        Write-Host "MinGW-w64 extracted successfully"
        
        # Add to PATH
        $mingwBin = "C:\mingw64\bin"
        $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
        
        if ($currentPath -notlike "*$mingwBin*") {
            $newPath = $currentPath + ";" + $mingwBin
            [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
            Write-Host "Added MinGW to PATH"
        }
        
        # Update current session PATH
        $env:PATH += ";$mingwBin"
        
        # Test g++
        $gppVersion = & "$mingwBin\g++.exe" --version 2>&1
        Write-Host "G++ installed successfully: $($gppVersion[0])"
        
    } else {
        Write-Host "Failed to extract MinGW. Trying alternative method..."
        
        # Try PowerShell Expand-Archive (won't work with .7z but trying anyway)
        try {
            Expand-Archive -Path $mingwFile -DestinationPath "C:\" -Force
            Write-Host "Extraction successful with PowerShell"
        } catch {
            Write-Host "PowerShell extraction failed: $($_.Exception.Message)"
            Write-Host "Please install 7-Zip and run this script again, or install MinGW manually"
            exit 1
        }
    }
    
} catch {
    Write-Host "Error downloading MinGW: $($_.Exception.Message)"
    Write-Host "Trying alternative approach..."
}

# Now download and install SFML
try {
    Write-Host "`nDownloading SFML..."
    $sfmlUrl = "https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip"
    $sfmlFile = Join-Path $tempDir "sfml.zip"
    
    Invoke-WebRequest -Uri $sfmlUrl -OutFile $sfmlFile -UseBasicParsing
    Write-Host "SFML downloaded successfully"
    
    # Extract SFML
    Write-Host "Extracting SFML to C:\SFML..."
    Expand-Archive -Path $sfmlFile -DestinationPath "C:\" -Force
    
    # Rename folder to simpler name
    if (Test-Path "C:\SFML-2.6.1") {
        if (Test-Path "C:\SFML") {
            Remove-Item "C:\SFML" -Recurse -Force
        }
        Rename-Item "C:\SFML-2.6.1" "C:\SFML"
    }
    
    Write-Host "SFML installed to C:\SFML"
    
    # Add SFML bin to PATH
    $sfmlBin = "C:\SFML\bin"
    $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    
    if ($currentPath -notlike "*$sfmlBin*") {
        $newPath = $currentPath + ";" + $sfmlBin
        [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
        Write-Host "Added SFML to PATH"
    }
    
    # Update current session PATH
    $env:PATH += ";$sfmlBin"
    
} catch {
    Write-Host "Error with SFML installation: $($_.Exception.Message)"
}

# Clean up
Remove-Item $tempDir -Recurse -Force -ErrorAction SilentlyContinue

Write-Host "`nInstallation complete!"
Write-Host "You may need to restart your terminal or computer for PATH changes to take effect."
Write-Host "Try running: g++ --version"