# PowerShell script to test the PARALLEL_EXECUTION implementation

# Navigate to the build directory
if (!(Test-Path "build")) {
    Write-Host "Creating build directory..."
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Build the Chronovyan interpreter
Push-Location "build"
try {
    Write-Host "Configuring build..."
    & cmake ..

    Write-Host "Building Chronovyan interpreter..."
    & cmake --build .
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Build failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    
    Write-Host "Build completed successfully!" -ForegroundColor Green
} finally {
    Pop-Location
}

# Run the PARALLEL_EXECUTION test
Write-Host "Running PARALLEL_EXECUTION test..." -ForegroundColor Cyan
$testFile = ".\test\parallel_execution_test.cvy"
$executable = ".\build\Debug\chronovyan.exe"

if (!(Test-Path $executable)) {
    $executable = ".\build\chronovyan.exe"
}

if (Test-Path $executable) {
    & $executable $testFile
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "PARALLEL_EXECUTION test completed successfully!" -ForegroundColor Green
    } else {
        Write-Host "PARALLEL_EXECUTION test failed with exit code $LASTEXITCODE" -ForegroundColor Red
    }
} else {
    Write-Host "Could not find Chronovyan executable. Please check the build output." -ForegroundColor Red
    exit 1
} 