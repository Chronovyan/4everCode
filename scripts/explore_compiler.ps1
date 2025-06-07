$compiler = ".\build\src\compiler\chronovyan_compiler.exe"
$testFile = ".\examples\hello_world.cvy"

Write-Host "Exploring Chronovyan Compiler Behavior" -ForegroundColor Cyan
Write-Host "Compiler: $compiler"
Write-Host "Test file: $testFile"
Write-Host ""

# Run with just the file
Write-Host "Test 1: Basic compilation" -ForegroundColor Yellow
Write-Host "Command: $compiler $testFile"
$output = & $compiler $testFile 2>&1
Write-Host "Exit code: $LASTEXITCODE"
Write-Host "Output: $output"
Write-Host ""

# Run with verbose flag
Write-Host "Test 2: Verbose output" -ForegroundColor Yellow
Write-Host "Command: $compiler $testFile -v"
$output = & $compiler $testFile -v 2>&1
Write-Host "Exit code: $LASTEXITCODE"
Write-Host "Output: $output"
Write-Host ""

# Run with specific output file
$outputFile = ".\test_output.c"
Write-Host "Test 3: Specific output file" -ForegroundColor Yellow
Write-Host "Command: $compiler $testFile -o $outputFile"
$output = & $compiler $testFile -o $outputFile 2>&1
Write-Host "Exit code: $LASTEXITCODE"
Write-Host "Output: $output"
if (Test-Path $outputFile) {
    $fileSize = (Get-Item $outputFile).Length
    Write-Host "Output file size: $fileSize bytes"
    if ($fileSize -gt 0) {
        Write-Host "File is not empty"
    } else {
        Write-Host "File is empty"
    }
} else {
    Write-Host "Output file not created"
}
Write-Host ""

# Check for default output file
$defaultOutput = "$testFile.c"
Write-Host "Checking for default output file: $defaultOutput"
if (Test-Path $defaultOutput) {
    $fileSize = (Get-Item $defaultOutput).Length
    Write-Host "Default output file exists, size: $fileSize bytes"
} else {
    Write-Host "Default output file not created"
}

# List all .c files in the examples directory
Write-Host ""
Write-Host "Looking for any .c files in examples directory:" -ForegroundColor Yellow
$cFiles = Get-ChildItem -Path .\examples -Recurse -Filter "*.c" | Select-Object -ExpandProperty FullName
if ($cFiles) {
    Write-Host "Found $($cFiles.Count) .c files:"
    foreach ($file in $cFiles) {
        Write-Host "  $file"
    }
} else {
    Write-Host "No .c files found in examples directory"
} 