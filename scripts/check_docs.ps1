# Simple script to check if documentation files exist
$docsDir = Join-Path $PSScriptRoot "..\docs"
$missingFiles = 0

# List of expected files and directories
$expectedFiles = @(
    "getting-started/installation.md",
    "getting-started/quickstart.md",
    "core/Core Concepts - The Foundation of Temporal Programming.md",
    "core/Core Syntax & Constructs.md",
    "core/Variable System - The Duality of Data.md",
    "core/Resource Management - The Battle for Temporal Control.md",
    "core/Getting Started - The Path to Temporal Mastery.md",
    "core/Coding Philosophies - The Art of Temporal Expression.md",
    "core/The Great Duality - Order and Flux.md",
    "core/Programming as Temporal Weaving.md"
)

Write-Host "Checking documentation files..." -ForegroundColor Cyan

foreach ($file in $expectedFiles) {
    $fullPath = Join-Path $docsDir $file
    if (Test-Path $fullPath) {
        Write-Host "[FOUND] $file" -ForegroundColor Green
    } else {
        Write-Host "[MISSING] $file" -ForegroundColor Red
        $missingFiles++
    }
}

# Check tutorial files
$tutorialFiles = Get-ChildItem -Path (Join-Path $docsDir "tutorials") -Recurse -File -Include *.md
if ($tutorialFiles.Count -gt 0) {
    Write-Host "\nFound $($tutorialFiles.Count) tutorial files" -ForegroundColor Green
} else {
    Write-Host "\nNo tutorial files found!" -ForegroundColor Red
    $missingFiles++
}

# Summary
if ($missingFiles -eq 0) {
    Write-Host "\nAll documentation files are present!" -ForegroundColor Green
} else {
    Write-Host "\nMissing $missingFiles documentation files!" -ForegroundColor Red
}

exit $missingFiles
