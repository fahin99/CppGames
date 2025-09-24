powershell -Command "& {
    Write-Host 'Automatic SFML Setup and Game Compilation'
    Write-Host '======================================='
    
    # Check if g++ is available
    try {
        $version = cmd /c "g++ --version" 2>&1
        Write-Host 'G++ compiler found:' $version[0]
    } catch {
        Write-Host 'ERROR: g++ compiler not found!'
        exit 1
    }
    
    # Try to compile directly first
    Write-Host 'Attempting direct compilation...'
    $result = cmd /c "g++ -std=c++17 *.cpp -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe" 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host 'SUCCESS! Game compiled successfully!'
        Write-Host 'Run with: ./dungeon_crawler.exe'
        exit 0
    }
    
    Write-Host 'Direct compilation failed. Installing SFML...'
    Write-Host $result
    
    # Download SFML
    $sfmlUrl = 'https://www.sfml-dev.org/files/SFML-2.6.1-windows-gcc-13.1.0-mingw-64-bit.zip'
    Write-Host 'Downloading SFML from:' $sfmlUrl
    
    try {
        Invoke-WebRequest -Uri $sfmlUrl -OutFile 'sfml.zip'
        Write-Host 'SFML downloaded successfully'
        
        # Extract SFML
        Write-Host 'Extracting SFML...'
        Expand-Archive -Path 'sfml.zip' -DestinationPath 'C:\' -Force
        Write-Host 'SFML extracted to C:\SFML-2.6.1\'
        
        # Try compilation with explicit paths
        Write-Host 'Compiling with SFML paths...'
        $result2 = cmd /c "g++ -std=c++17 *.cpp -IC:\SFML-2.6.1\include -LC:\SFML-2.6.1\lib -lsfml-graphics -lsfml-window -lsfml-system -o dungeon_crawler.exe" 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host 'SUCCESS! Game compiled with SFML!'
            
            # Copy DLL files
            Write-Host 'Copying required DLL files...'
            Copy-Item 'C:\SFML-2.6.1\bin\*.dll' -Destination '.' -Force
            
            Write-Host 'Game is ready! Run with: ./dungeon_crawler.exe'
        } else {
            Write-Host 'Compilation failed even with SFML paths:'
            Write-Host $result2
        }
        
        Remove-Item 'sfml.zip' -ErrorAction SilentlyContinue
    } catch {
        Write-Host 'Failed to download SFML:' $_.Exception.Message
    }
}"