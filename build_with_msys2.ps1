# Build script using MSYS2 MinGW-w64

# Set error action preference
$ErrorActionPreference = "Stop"

# Check if MSYS2 is installed
$msys2Path = "C:\msys64\mingw64\bin"
if (-not (Test-Path $msys2Path)) {
    Write-Host "MSYS2 MinGW-w64 not found at $msys2Path" -ForegroundColor Red
    Write-Host "Please install MSYS2 from https://www.msys2.org/ and make sure to install the MinGW-w64 toolchain:"
    Write-Host "1. Run 'pacman -S --needed base-devel mingw-w64-x86_64-toolchain' in the MSYS2 terminal"
    exit 1
}

# Add MSYS2 MinGW-w64 to PATH
$env:Path = "$msys2Path;" + $env:Path

# Verify compilers
Write-Host "Using GCC: $(gcc --version | Select-String -Pattern 'gcc' -First 1)" -ForegroundColor Cyan
Write-Host "Using G++: $(g++ --version | Select-String -Pattern 'g\+\+' -First 1)" -ForegroundColor Cyan

# Set up build directory
$buildDir = "$PSScriptRoot\build"
if (Test-Path $buildDir) {
    Write-Host "Removing existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Configure with MSYS2 MinGW-w64
Write-Host "Configuring with MSYS2 MinGW-w64..." -ForegroundColor Cyan
Push-Location $buildDir

try {
    cmake -G "MinGW Makefiles" `
          -DCMAKE_BUILD_TYPE=Debug `
          -DBUILD_TESTS=ON `
          -DCMAKE_C_COMPILER=gcc `
          -DCMAKE_CXX_COMPILER=g++ `
          ..
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code $LASTEXITCODE"
    }
    
    # Build
    Write-Host "Building..." -ForegroundColor Cyan
    cmake --build . --config Debug
    
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }
    
    # Run tests
    Write-Host "Running tests..." -ForegroundColor Cyan
    cmake --build . --target test --config Debug -- ARGS=--output-on-failure
    
} catch {
    Write-Host "Error: $_" -ForegroundColor Red
    exit 1
} finally {
    Pop-Location
}

Write-Host "Build completed successfully!" -Foreground Green
