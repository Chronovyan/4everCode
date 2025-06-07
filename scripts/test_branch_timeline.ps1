#!/usr/bin/env pwsh
# Script to build the Chronovyan interpreter and run the BRANCH_TIMELINE test

# Create build directory if it doesn't exist
if (-not (Test-Path -Path "build")) {
    Write-Host "Creating build directory..."
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Navigate to build directory and configure
cd build
Write-Host "Configuring build with CMake..."
cmake -G "Visual Studio 17 2022" -A x64 ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

# Build the Chronovyan interpreter
Write-Host "Building Chronovyan interpreter..."
cmake --build .

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

# Navigate back to the project root
cd ..

# Check if the executable exists
$exePath = ".\build\Debug\chronovyan.exe"
if (-not (Test-Path -Path $exePath)) {
    Write-Host "Executable not found at $exePath" -ForegroundColor Red
    exit 1
}

# Run the BRANCH_TIMELINE test
Write-Host "Running BRANCH_TIMELINE test..." -ForegroundColor Cyan
& $exePath ".\test\branch_timeline_test.cvy"

if ($LASTEXITCODE -eq 0) {
    Write-Host "BRANCH_TIMELINE test completed successfully!" -ForegroundColor Green
} else {
    Write-Host "BRANCH_TIMELINE test failed with exit code $LASTEXITCODE" -ForegroundColor Red
}

exit $LASTEXITCODE 