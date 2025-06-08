# PowerShell script to merge restored documentation with current docs
# This script will copy files from docs_restored to docs_merged, preserving the current structure

# Define source and destination paths
$sourceDir = Join-Path $PSScriptRoot "docs_restored"
$destDir = Join-Path $PSScriptRoot "docs_merged"

# Files and directories to preserve from current docs (won't be overwritten)
$preserveFiles = @(
    "CNAME",
    "CONTRIBUTING.md",
    "index.md",
    "_config.yml",
    "Gemfile",
    ".nojekyll"
)

$preserveDirs = @(
    "css",
    "js"
)

# Function to copy files with logging
function Copy-FileSafe {
    param (
        [string]$source,
        [string]$destination
    )
    
    $destinationDir = [System.IO.Path]::GetDirectoryName($destination)
    
    # Create destination directory if it doesn't exist
    if (-not (Test-Path -Path $destinationDir)) {
        New-Item -ItemType Directory -Path $destinationDir -Force | Out-Null
        Write-Host "Created directory: $destinationDir" -ForegroundColor Green
    }
    
    # Copy the file
    Copy-Item -Path $source -Destination $destination -Force
    Write-Host "Copied: $source -> $destination" -ForegroundColor Cyan
}

# Main copy process
Write-Host "Starting documentation merge..." -ForegroundColor Yellow

# Get all files in the source directory
$allFiles = Get-ChildItem -Path $sourceDir -Recurse -File

foreach ($file in $allFiles) {
    $relativePath = $file.FullName.Substring($sourceDir.Length + 1)
    $destPath = Join-Path $destDir $relativePath
    $shouldCopy = $true
    
    # Check if we should preserve this file
    foreach ($preserve in $preserveFiles) {
        if ($relativePath -eq $preserve) {
            Write-Host "Skipping preserved file: $relativePath" -ForegroundColor Gray
            $shouldCopy = $false
            break
        }
    }
    
    # Check if file is in a preserved directory
    if ($shouldCopy) {
        foreach ($preserveDir in $preserveDirs) {
            if ($relativePath.StartsWith("$preserveDir\")) {
                Write-Host "Skipping file in preserved directory: $relativePath" -ForegroundColor Gray
                $shouldCopy = $false
                break
            }
        }
    }
    
    # Copy the file if needed
    if ($shouldCopy) {
        # Check if destination file exists and is different
        if (Test-Path -Path $destPath) {
            $existingFile = Get-Item -Path $destPath
            if ($file.Length -eq $existingFile.Length -and 
                $file.LastWriteTime -eq $existingFile.LastWriteTime) {
                Write-Host "Skipping unchanged file: $relativePath" -ForegroundColor Gray
                continue
            }
            Write-Host "Updating: $relativePath" -ForegroundColor Yellow
        }
        
        Copy-FileSafe -source $file.FullName -destination $destPath
    }
}

Write-Host "Documentation merge complete!" -ForegroundColor Green
Write-Host "Merged documentation is in: $destDir" -ForegroundColor Green
Write-Host "Review the changes and then copy to 'docs' directory when ready." -ForegroundColor Yellow
