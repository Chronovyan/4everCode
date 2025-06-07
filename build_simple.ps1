# Simple build script for Chronovyan

# Function to find GTest on Windows
function Find-GTest {
    $commonPaths = @(
        "C:\local\googletest",
        "C:\Program Files\googletest",
        "C:\Program Files (x86)\googletest",
        "$env:ProgramFiles\googletest",
        "${env:ProgramFiles(x86)}\googletest"
    )
    
    foreach ($path in $commonPaths) {
        $includePath = Join-Path $path "include"
        $libPath = Join-Path $path "lib"
        
        $hasInclude = Test-Path (Join-Path $includePath "gtest\gtest.h")
        $hasLib = (Test-Path (Join-Path $libPath "gtest.lib")) -or (Test-Path (Join-Path $libPath "libgtest.a"))
        $hasMainLib = (Test-Path (Join-Path $libPath "gtest_main.lib")) -or (Test-Path (Join-Path $libPath "libgtest_main.a"))
        
        if ($hasInclude -and $hasLib -and $hasMainLib) {
            return @{
                IncludeDir = $includePath
                LibDir = $libPath
            }
        }
    }
    
    return $null
}

# Try to find GTest
$gtestInfo = Find-GTest

# Create build directory if it doesn't exist
$buildDir = "$PSScriptRoot/build"
if (-not (Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Configure with CMake
Write-Host "Configuring build..." -ForegroundColor Cyan
Push-Location $buildDir

# Base CMake arguments
$cmakeArgs = @(
    "-G", "MinGW Makefiles",
    "-DCMAKE_BUILD_TYPE=Debug",
    "-DBUILD_TESTS=ON"
)

# Add MinGW compilers to PATH if not already there
$mingwPath = "C:\msys64\mingw64\bin"
if (Test-Path $mingwPath) {
    $env:Path = "$mingwPath;" + $env:Path
}

# Find MinGW compilers
$gccPath = Get-Command gcc -ErrorAction SilentlyContinue
$gxxPath = Get-Command g++ -ErrorAction SilentlyContinue

if ($gccPath -and $gxxPath) {
    $cmakeArgs += @(
        "-DCMAKE_C_COMPILER=$($gccPath.Source)",
        "-DCMAKE_CXX_COMPILER=$($gxxPath.Source)",
        "-DCMAKE_MAKE_PROGRAM=mingw32-make.exe"
    )
}

# Add GTest paths if found
if ($gtestInfo) {
    Write-Host "Found GTest at: $($gtestInfo.IncludeDir)" -ForegroundColor Green
    $cmakeArgs += @(
        "-DGTEST_INCLUDE_DIR=$($gtestInfo.IncludeDir)",
        "-DGTEST_LIBRARY=$(Join-Path $gtestInfo.LibDir 'gtest.lib')",
        "-DGTEST_MAIN_LIBRARY=$(Join-Path $gtestInfo.LibDir 'gtest_main.lib')"
    )
} else {
    Write-Host "GTest not found in common locations, CMake will try to find it." -ForegroundColor Yellow
}

# Run CMake
cmake $cmakeArgs ..
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed with exit code $LASTEXITCODE"
    Pop-Location
    exit $LASTEXITCODE
}

# Build the project
Write-Host "Building..." -ForegroundColor Cyan
cmake --build . --config Debug
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed with exit code $LASTEXITCODE"
    Pop-Location
    exit $LASTEXITCODE
}

# Run tests
Write-Host "Running tests..." -ForegroundColor Cyan
ctest --output-on-failure
if ($LASTEXITCODE -ne 0) {
    Write-Error "Tests failed with exit code $LASTEXITCODE"
    Pop-Location
    exit $LASTEXITCODE
}

Pop-Location
Write-Host "Build and test completed successfully!" -ForegroundColor Green
