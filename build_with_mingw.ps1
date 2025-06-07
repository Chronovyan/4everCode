# Simple build script using MinGW

# Set error action preference
$ErrorActionPreference = "Stop"

# Set up build directory
$buildDir = "$PSScriptRoot\build"
if (Test-Path $buildDir) {
    Write-Host "Removing existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Configure with MinGW
Write-Host "Configuring with MinGW..." -ForegroundColor Cyan
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
    cmake --build . --target test --config Debug
    
} catch {
    Write-Host "Error: $_" -ForegroundColor Red
    exit 1
} finally {
    Pop-Location
}

Write-Host "Build completed successfully!" -ForegroundColor Green
