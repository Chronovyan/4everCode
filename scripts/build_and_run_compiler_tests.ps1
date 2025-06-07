# PowerShell script to build and run the Chronovyan compiler tests

# Set variables
$BUILD_DIR = "build"
$TEST_BINARY = "bin\compiler_tests.exe"

# Print header
Write-Host "=====================================================" -ForegroundColor Cyan
Write-Host "  Chronovyan Compiler Tests Builder and Runner" -ForegroundColor Cyan
Write-Host "=====================================================" -ForegroundColor Cyan

# Create build directory if it doesn't exist
if (-not (Test-Path $BUILD_DIR)) {
    Write-Host "Creating build directory..." -ForegroundColor Yellow
    New-Item -Path $BUILD_DIR -ItemType Directory | Out-Null
}

# Change to build directory
Set-Location $BUILD_DIR

# Configure the build
Write-Host "Configuring build..." -ForegroundColor Yellow
cmake .. -DBUILD_TESTS=ON

# Check if configuration was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Configuration failed. Please check the errors above." -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Build the compiler tests
Write-Host "Building compiler tests..." -ForegroundColor Yellow
cmake --build . --target compiler_tests -- /maxcpucount

# Check if build was successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed. Please check the errors above." -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host "Build successful." -ForegroundColor Green

# Run the tests
Write-Host "=====================================================" -ForegroundColor Cyan
Write-Host "Running compiler tests..." -ForegroundColor Yellow
Write-Host "=====================================================" -ForegroundColor Cyan
& ".\$TEST_BINARY"

# Check if tests were successful
if ($LASTEXITCODE -ne 0) {
    Write-Host "Tests failed. Please check the errors above." -ForegroundColor Red
    Set-Location ..
    exit 1
}

Write-Host "=====================================================" -ForegroundColor Cyan
Write-Host "All compiler tests passed successfully!" -ForegroundColor Green
Write-Host "=====================================================" -ForegroundColor Cyan

# Return to original directory
Set-Location ..

exit 0 