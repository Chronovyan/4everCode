# Script to validate all .cvy files in the repository
# This helps ensure that our examples are compatible with the current syntax

# Setup variables
$root = $PSScriptRoot
$errorCount = 0
$successCount = 0
$compiler = ".\build\chronovyan_compiler.exe"  # Update path to match new location

# Function to validate a .cvy file
function Validate-CVYFile {
    param(
        [string]$file
    )
    
    Write-Host "Validating $file..."
    
    # Run the compiler with validation mode
    $output = & $compiler $file 2>&1
    
    # Check if there were any errors
    if ($LASTEXITCODE -eq 0 -or $null -eq $LASTEXITCODE) {
        Write-Host "  VALID" -ForegroundColor Green
        return $true
    }
    else {
        Write-Host "  FAILED:" -ForegroundColor Red
        Write-Host $output
        return $false
    }
}

Write-Host "Chronovyan Script Validator" -ForegroundColor Cyan
Write-Host "This script validates all .cvy files in the project." -ForegroundColor Cyan
Write-Host "Using compiler: $compiler"
Write-Host ""

# Find all .cvy files
$files = Get-ChildItem -Path . -Recurse -Filter "*.cvy" | Select-Object -ExpandProperty FullName
$totalFiles = $files.Count

Write-Host "Found $totalFiles .cvy files to validate..."
Write-Host ""

# Initialize counters
$successCount = 0
$failCount = 0
$failedFiles = @()

# Process each file
foreach ($file in $files) {
    Write-Host "Validating $file..." -ForegroundColor DarkGray
    
    # Run the compiler (just validation, not caring about output)
    $output = & $compiler $file 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  VALID" -ForegroundColor Green
        $successCount++
    } else {
        Write-Host "  INVALID" -ForegroundColor Red
        if ($output) {
            Write-Host "  Error: $output" -ForegroundColor Red
        }
        $failCount++
        $failedFiles += $file
    }
}

# Display summary
Write-Host ""
Write-Host "Validation Summary:" -ForegroundColor Cyan
Write-Host "  Total files: $totalFiles"
Write-Host "  Valid: $successCount" -ForegroundColor Green
Write-Host "  Invalid: $failCount" -ForegroundColor Red

# List failed files if any
if ($failCount -gt 0) {
    Write-Host ""
    Write-Host "Invalid files:" -ForegroundColor Red
    foreach ($file in $failedFiles) {
        Write-Host "  $file" -ForegroundColor Red
    }
} 