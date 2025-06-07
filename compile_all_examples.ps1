# PowerShell script to compile all .cvy files in the examples directory

$compiler = ".\bin\chronovyan_compiler.exe"
$examplesDir = "..\examples"

Write-Host "Chronovyan Compiler Batch Processing"
Write-Host "==================================="
Write-Host "Using compiler: $compiler"
Write-Host "Processing directory: $examplesDir"
Write-Host ""

# Get all .cvy files
$cvyFiles = Get-ChildItem -Path $examplesDir -Filter "*.cvy"
$totalFiles = $cvyFiles.Count
$successful = 0
$failed = 0

Write-Host "Found $totalFiles .cvy files to process."
Write-Host ""

# Process each .cvy file
foreach ($file in $cvyFiles) {
    $fileName = $file.Name
    Write-Host "Processing $fileName ..."
    
    try {
        $output = & $compiler $file.FullName -v 2>&1
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "[SUCCESS] $fileName compiled successfully." -ForegroundColor Green
            $successful++
        } else {
            Write-Host "[FAILED] $fileName compilation failed." -ForegroundColor Red
            Write-Host "Error details: $output" -ForegroundColor Red
            $failed++
        }
    } catch {
        Write-Host "[FAILED] $fileName compilation failed with exception." -ForegroundColor Red
        Write-Host "Exception details: $_" -ForegroundColor Red
        $failed++
    }
}

Write-Host ""
Write-Host "==================================="
Write-Host "Compilation Summary:"
Write-Host "- Total files processed: $totalFiles"
Write-Host "- Successfully compiled: $successful" -ForegroundColor Green
Write-Host "- Failed to compile: $failed" -ForegroundColor Red

if ($totalFiles -gt 0) {
    $successRate = [math]::Round(($successful * 100) / $totalFiles, 2)
    Write-Host "- Success rate: $successRate%" -ForegroundColor Cyan
}

Write-Host "===================================" 