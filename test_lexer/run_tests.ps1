param(
    [switch]$build = $false,
    [switch]$clean = $false,
    [string]$filter = "*"
)

$buildDir = "$PSScriptRoot\build"

# Clean build directory if requested
if ($clean -and (Test-Path -Path $buildDir)) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Path $buildDir -Recurse -Force
}

# Create build directory if it doesn't exist
if (!(Test-Path -Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
    $build = $true
}

# Configure and build if requested
if ($build) {
    Push-Location $buildDir
    
    # Configure with CMake
    Write-Host "Configuring with CMake..." -ForegroundColor Cyan
    cmake -G "Visual Studio 17 2022" -A x64 ..
    if ($LASTEXITCODE -ne 0) {
        Write-Error "CMake configuration failed"
        Pop-Location
        exit 1
    }
    
    # Build the project
    Write-Host "Building project..." -ForegroundColor Cyan
    cmake --build . --config Debug
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Build failed"
        Pop-Location
        exit 1
    }
    
    Pop-Location
}

# Run tests with the specified filter
Write-Host "Running tests with filter: $filter" -ForegroundColor Cyan
Push-Location $buildDir

$testCommand = ".\Debug\lexer_tests.exe"
if ($filter -ne "*") {
    $testCommand += " --gtest_filter=$filter"
}

Invoke-Expression $testCommand
$testExitCode = $LASTEXITCODE

Pop-Location
exit $testExitCode
