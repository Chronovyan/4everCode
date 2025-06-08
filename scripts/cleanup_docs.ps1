# Script to clean up remaining documentation files
# Run this script from the project root

# Files to keep in the root of the docs directory
$keepFiles = @(
    "README.md",
    "index.md",
    "CHANGELOG.md",
    "CNAME",
    "STRUCTURE.md"
)

# Directories to keep in the root of the docs directory
$keepDirs = @(
    "guides",
    "reference",
    "development",
    "community",
    "design",
    "architecture",
    "roadmap",
    "assets",
    "images",
    "examples",
    "tutorials",
    "concepts",
    "chronolog"
)

# Get all items in the docs directory
$items = Get-ChildItem -Path "docs" -File | Where-Object { $_.Name -notin $keepFiles }

# Display files that will be moved to an archive directory
if ($items.Count -gt 0) {
    Write-Host "The following files will be moved to docs/archive/:"
    $items | ForEach-Object { Write-Host "- $($_.Name)" }
    
    # Create archive directory if it doesn't exist
    $archiveDir = "docs/archive"
    if (-not (Test-Path $archiveDir)) {
        New-Item -ItemType Directory -Path $archiveDir | Out-Null
    }
    
    # Move files to archive
    $items | ForEach-Object {
        $destination = Join-Path $archiveDir $_.Name
        Move-Item -Path $_.FullName -Destination $destination -Force
        Write-Host "Moved to archive: $($_.Name)"
    }
}

# Get all directories in the docs directory that aren't in the keep list
$dirs = Get-ChildItem -Path "docs" -Directory | 
    Where-Object { $_.Name -notin $keepDirs -and $_.Name -ne "archive" }

# Display directories that will be moved to an archive directory
if ($dirs.Count -gt 0) {
    Write-Host "\nThe following directories will be moved to docs/archive/:"
    $dirs | ForEach-Object { Write-Host "- $($_.Name)" }
    
    # Create archive directory if it doesn't exist
    if (-not (Test-Path $archiveDir)) {
        New-Item -ItemType Directory -Path $archiveDir | Out-Null
    }
    
    # Move directories to archive
    $dirs | ForEach-Object {
        $destination = Join-Path $archiveDir $_.Name
        Move-Item -Path $_.FullName -Destination $destination -Force
        Write-Host "Moved to archive: $($_.Name)"
    }
}

Write-Host "\nCleanup complete! Any files not in the standard structure have been moved to docs/archive/"
