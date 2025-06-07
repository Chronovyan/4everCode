$compiler = ".\build\src\compiler\chronovyan_compiler.exe"
$testFile = ".\examples\hello_world.cvy"
$outputFile = ".\hello_output.c"

# Remove existing output files if they exist
if (Test-Path $outputFile) {
    Remove-Item $outputFile -Force
}

Write-Host "Testing compilation of $testFile" -ForegroundColor Cyan
Write-Host "Using compiler: $compiler"

# Compile with specific output
Write-Host "Compiling to specific output file: $outputFile" -ForegroundColor Yellow
& $compiler $testFile -o $outputFile -v

# Check for output file
if (Test-Path $outputFile) {
    $fileSize = (Get-Item $outputFile).Length
    Write-Host "Output file created: $outputFile"
    Write-Host "File size: $fileSize bytes"
    
    if ($fileSize -gt 0) {
        Write-Host "Content preview:"
        Get-Content $outputFile -TotalCount 20 | ForEach-Object { "  $_" }
    } else {
        Write-Host "File is empty"
    }
} else {
    Write-Host "Output file not created" -ForegroundColor Red
}

# What's happening with the actual .cvy files?
Write-Host ""
Write-Host "Checking for any recently modified .c files in examples directory:" -ForegroundColor Yellow
$recentFiles = Get-ChildItem -Path .\examples -Recurse -Include "*.c" | 
               Where-Object { $_.LastWriteTime -gt (Get-Date).AddMinutes(-5) } |
               Select-Object FullName, LastWriteTime
               
if ($recentFiles) {
    Write-Host "Found recently modified .c files:"
    $recentFiles | Format-Table -AutoSize
} else {
    Write-Host "No recently modified .c files found"
} 