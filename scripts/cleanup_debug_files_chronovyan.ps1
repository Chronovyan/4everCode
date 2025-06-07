# PowerShell Script to Clean Up Chronovyan Debug and Fix Files

Write-Host "Starting Chronovyan Debug and Fix File Cleanup..."

$ErrorActionPreference = "SilentlyContinue"
$rootDir = ".\"

# Patterns for obsolete debug/fix files
$fixFilePatterns = @(
    "*_fix.txt",
    "*_fix_instructions.md",
    "fix_*.md",
    "BUILD-005-*.md",
    "fixed_*_temp.cpp",
    "*_fix.md" # Added this based on our previous session's attempt
)

$filesRemoved = 0
$filesFailed = 0

foreach ($pattern in $fixFilePatterns) {
    Write-Host "Searching for files matching pattern: $pattern in $rootDir"
    $files = Get-ChildItem -Path $rootDir -Filter $pattern -File -Recurse #-Recurse can be removed if only root
    if ($files) {
        foreach ($file in $files) {
            Write-Host "Removing $($file.FullName)"
            Remove-Item -Path $file.FullName -Force
            if ($LASTEXITCODE -ne 0) {
                Write-Warning "Error removing $($file.FullName)"
                $filesFailed++
            } else {
                Write-Host "Successfully removed $($file.FullName)"
                $filesRemoved++
            }
        }
    } else {
        Write-Host "No files found matching pattern: $pattern"
    }
}

Write-Host "Debug and Fix File Cleanup Finished."
Write-Host "Files removed: $filesRemoved"
Write-Host "Files failed to remove: $filesFailed" 