# Create build directory if it doesn't exist
$buildDir = "$PSScriptRoot\build"
if (Test-Path -Path $buildDir) {
    Remove-Item -Path $buildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Configure and build
Push-Location $buildDir

# Configure with CMake
cmake -G "Visual Studio 17 2022" -A x64 ..
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed"
    Pop-Location
    exit 1
}

# Build the project
cmake --build . --config Debug
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed"
    Pop-Location
    exit 1
}

# Run tests using CTest
ctest --output-on-failure -C Debug
$testExitCode = $LASTEXITCODE

Pop-Location
exit $testExitCode
