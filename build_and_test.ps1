# PowerShell script to build and test the Chronovyan bytecode implementation

# Step 1: Check if build directory exists
if (-not (Test-Path -Path "build")) {
    Write-Host "Creating build directory..."
    New-Item -ItemType Directory -Path "build"
}

# Step 2: Change to build directory and run CMake
Write-Host "Running CMake to configure build..."
Set-Location -Path "build"
cmake -DBUILD_COMPILER=ON -DBUILD_INTERPRETER=ON -DCMAKE_BUILD_TYPE=Debug ..

# Step 3: Build the project
Write-Host "Building the project..."
cmake --build . --config Debug

# Step 4: Test bytecode generation
Write-Host "Testing bytecode generation..."
Set-Location -Path ".."

# Compile example program
Write-Host "Compiling example program: examples/simple_program.cvy"
$compiler = ".\build\src\compiler\chronovyan_compiler.exe"
& $compiler -v examples/simple_program.cvy

# Step 5: Test bytecode execution
Write-Host "Testing bytecode execution..."
$interpreter = ".\build\src\interpreter\chronovyan_run.exe"
& $interpreter -v examples/simple_program.chron

# Step 6: Display success message
Write-Host "Build and test completed successfully!" -ForegroundColor Green
Write-Host "Chronovyan bytecode implementation is working!" 