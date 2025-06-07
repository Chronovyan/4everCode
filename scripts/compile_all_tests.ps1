# Compiler path
$compiler = ".\build\src\compiler\chronovyan_compiler.exe"

# Get all .cvy files in the tests directory
$files = Get-ChildItem -Path ".\tests" -Recurse -Filter "*.cvy" | Select-Object -ExpandProperty FullName
$totalFiles = $files.Count

Write-Host "Found $totalFiles .cvy files to compile in tests directory..."
Write-Host ""

# Initialize counters
$successCount = 0
$failCount = 0
$failedFiles = @()

# Compile each file
foreach ($file in $files) {
    Write-Host "Compiling $file..."
    & $compiler $file

    if ($LASTEXITCODE -eq 0) {
        Write-Host "  SUCCESS" -ForegroundColor Green
        $successCount++
    } else {
        Write-Host "  FAILED" -ForegroundColor Red
        $failCount++
        $failedFiles += $file
    }
}

Write-Host ""
Write-Host "Compilation Summary:"
Write-Host "  Total files: $totalFiles"
Write-Host "  Successful: $successCount" -ForegroundColor Green
Write-Host "  Failed: $failCount" -ForegroundColor Red

# List failed files if any
if ($failCount -gt 0) {
    Write-Host ""
    Write-Host "Failed files:" -ForegroundColor Red
    foreach ($file in $failedFiles) {
        Write-Host "  $file" -ForegroundColor Red
    }
} 