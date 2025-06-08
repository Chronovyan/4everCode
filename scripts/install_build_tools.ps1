# Install MSYS2 and build tools automatically

# Run as Administrator
if (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) {
    Write-Host "This script requires Administrator privileges. Please run as Administrator." -ForegroundColor Red
    exit 1
}

# Check if MSYS2 is already installed
$msys2Path = "C:\msys64"
$msys2Installed = Test-Path $msys2Path

if (-not $msys2Installed) {
    # Download MSYS2 installer
    Write-Host "Downloading MSYS2 installer..." -ForegroundColor Cyan
    $installerPath = "$env:TEMP\msys2-x86_64-latest.exe"
    Invoke-WebRequest -Uri "https://github.com/msys2/msys2-installer/releases/latest/download/msys2-x86_64.exe" -OutFile $installerPath

    # Install MSYS2 silently
    Write-Host "Installing MSYS2..." -ForegroundColor Cyan
    Start-Process -FilePath $installerPath -ArgumentList 'install','--root','C:\msys64','--confirm-command','--confirm-command','--confirm-command' -Wait
    
    # Add MSYS2 to PATH temporarily
    $env:Path = "C:\msys64\usr\bin;C:\msys64\mingw64\bin;" + $env:Path
    
    # Update MSYS2
    Write-Host "Updating MSYS2..." -ForegroundColor Cyan
    bash -lc 'pacman --noconfirm -Syu'
    bash -lc 'pacman --noconfirm -Su'
} else {
    Write-Host "MSYS2 is already installed at $msys2Path" -ForegroundColor Green
    $env:Path = "C:\msys64\usr\bin;C:\msys64\mingw64\bin;" + $env:Path
}

# Install required tools
Write-Host "Installing build tools..." -ForegroundColor Cyan
bash -lc 'pacman --noconfirm -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja'

# Add to system PATH permanently
$systemPath = [System.Environment]::GetEnvironmentVariable('Path', 'Machine')
$pathsToAdd = @(
    'C:\msys64\mingw64\bin',
    'C:\msys64\usr\bin'
)

$updated = $false
foreach ($path in $pathsToAdd) {
    if ($systemPath -notlike "*$path*") {
        $systemPath = "$path;$systemPath"
        $updated = $true
    }
}

if ($updated) {
    [System.Environment]::SetEnvironmentVariable('Path', $systemPath, 'Machine')
    $env:Path = "$($pathsToAdd -join ';');$env:Path"
    Write-Host "System PATH updated with MSYS2 directories" -ForegroundColor Green
}

# Create a simple build script
$buildScript = @'
# Simple build script for Chronovyan
$ErrorActionPreference = "Stop"

# Create build directory
$buildDir = "$PSScriptRoot\build"
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Configure and build
Push-Location $buildDir
try {
    cmake -G "MinGW Makefiles" `
          -DCMAKE_BUILD_TYPE=Debug `
          -DBUILD_TESTS=ON `
          -DCMAKE_C_COMPILER=gcc `
          -DCMAKE_CXX_COMPILER=g++ `
          ..
    
    if ($LASTEXITCODE -ne 0) { throw "CMake failed" }
    
    cmake --build . --config Debug
    if ($LASTEXITCODE -ne 0) { throw "Build failed" }
    
    # Run tests
    ctest --output-on-failure
} catch {
    Write-Host "Error: $_" -ForegroundColor Red
    exit 1
} finally {
    Pop-Location
}
'@

# Save build script
$buildScriptPath = "$PSScriptRoot\build.ps1"
Set-Content -Path $buildScriptPath -Value $buildScript

Write-Host "`nInstallation complete!" -ForegroundColor Green
Write-Host "To build the project, run:" -ForegroundColor Cyan
Write-Host "  .\build.ps1" -ForegroundColor White
Write-Host "`nYou may need to restart your terminal for PATH changes to take effect." -ForegroundColor Yellow
