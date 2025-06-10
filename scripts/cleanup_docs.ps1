# Script to clean up old documentation files after restructuring
# Run this script from the root of the repository

$docsDir = "docs"
$backupDir = "$docsDir/archive/old_structure_$(Get-Date -Format 'yyyyMMdd')"

# Files to archive (relative to docs/)
$filesToArchive = @(
    "core/Core Concepts - The Foundation of Temporal Programming.md",
    "core/Core Syntax & Constructs.md",
    "core/Variable System - The Duality of Data.md",
    "core/Variables - The Duality of Data.md",
    "core/Getting Started - The Path to Temporal Mastery.md",
    "core/Coding Philosophies - The Art of Temporal Expression.md",
    "core/The Great Duality - Order and Flux.md",
    "core/Programming as Temporal Weaving.md"
)

# Create backup directory if it doesn't exist
if (-not (Test-Path $backupDir)) {
    New-Item -ItemType Directory -Path $backupDir -Force | Out-Null
}

# Move files to archive
foreach ($file in $filesToArchive) {
    $source = Join-Path $docsDir $file
    $destination = Join-Path $backupDir (Split-Path $file -Leaf)
    
    if (Test-Path $source) {
        Write-Host "Archiving $file..."
        Move-Item -Path $source -Destination $destination -Force
    } else {
        Write-Host "File not found: $file" -ForegroundColor Yellow
    }
}

Write-Host "\nDocumentation cleanup complete!" -ForegroundColor Green
Write-Host "Old files have been moved to: $backupDir"
Write-Host "\nNext steps:"
Write-Host "1. Review the changes in git"
Write-Host "2. Commit the new structure"
Write-Host "3. Update any internal links in other documents"
