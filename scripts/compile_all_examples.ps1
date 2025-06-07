$compiler = ".\build\src\compiler\chronovyan_compiler.exe"
$files = Get-ChildItem -Path .\examples -Recurse -Filter "*.cvy"

# Limit to first 5 files for testing
$files = $files | Select-Object -First 5

$successCount = 0
$failCount = 0
$failedFiles = @()

Write-Host "Found $($files.Count) .cvy files to compile..."
Write-Host "Using compiler: $compiler"
Write-Host ""

foreach ($file in $files) {
    Write-Host "Compiling $($file.FullName)..." -ForegroundColor Cyan
    Write-Host "Command: $compiler $($file.FullName)" -ForegroundColor DarkGray
    
    # Capture and display the output
    $output = & $compiler $file.FullName 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  SUCCESS" -ForegroundColor Green
        if ($output) {
            Write-Host "  Output: $output" -ForegroundColor DarkGray
        } else {
            Write-Host "  No output from compiler" -ForegroundColor DarkGray
        }
        $successCount++
    } else {
        Write-Host "  FAILED" -ForegroundColor Red
        if ($output) {
            Write-Host "  Error: $output" -ForegroundColor Red
        }
        $failCount++
        $failedFiles += $file.FullName
    }
    
    Write-Host ""
}

Write-Host ""
Write-Host "Compilation Summary:"
Write-Host "  Total files: $($files.Count)"
Write-Host "  Successful: $successCount" -ForegroundColor Green
Write-Host "  Failed: $failCount" -ForegroundColor Red

if ($failCount -gt 0) {
    Write-Host ""
    Write-Host "Failed files:" -ForegroundColor Red
    foreach ($failedFile in $failedFiles) {
        Write-Host "  $failedFile"
    }
} 