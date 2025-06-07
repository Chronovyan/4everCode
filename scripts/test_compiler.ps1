$compiler = ".\build\src\compiler\chronovyan_compiler.exe"
$testFile = ".\examples\hello_world.cvy"
$outputFile = ".\hello_world_test_output.c"

Write-Host "Testing Chronovyan Compiler on a single file"
Write-Host "Compiler: $compiler"
Write-Host "Input file: $testFile"
Write-Host "Output file: $outputFile"
Write-Host ""

# Run compiler with verbose output and specific output file
Write-Host "Running compiler with verbose output..."
& $compiler $testFile -o $outputFile -v

Write-Host ""
Write-Host "Checking if output file was created..."
if (Test-Path $outputFile) {
    Write-Host "Output file exists: $outputFile" -ForegroundColor Green
    Write-Host "File content preview:" -ForegroundColor Green
    Get-Content $outputFile -TotalCount 10 | ForEach-Object { "  $_" }
    Write-Host "  ..." -ForegroundColor DarkGray
} else {
    Write-Host "Output file not found: $outputFile" -ForegroundColor Red
}

# Check if the default output file was created as well
$defaultOutput = "$testFile.c"
Write-Host ""
Write-Host "Checking if default output file was created..."
if (Test-Path $defaultOutput) {
    Write-Host "Default output file exists: $defaultOutput" -ForegroundColor Green
} else {
    Write-Host "Default output file not found: $defaultOutput" -ForegroundColor Red
} 