# PowerShell script to rename the root directory from 4ever to 4ever

# Define source and target paths
$sourceDir = "C:\Users\HydraPony\dev\4ever"
$targetDir = "C:\Users\HydraPony\dev\4ever"

# Check if source exists
if (-not (Test-Path $sourceDir)) {
    Write-Error "Source directory does not exist: $sourceDir"
    exit 1
}

# Check if target already exists
if (Test-Path $targetDir) {
    # If target exists, remove it first
    try {
        Remove-Item -Path $targetDir -Recurse -Force
        Write-Host "Removed existing directory: $targetDir"
    }
    catch {
        Write-Error "Failed to remove existing target directory: $_"
        exit 1
    }
}

# Rename the directory
try {
    Rename-Item -Path $sourceDir -NewName (Split-Path $targetDir -Leaf) -Force
    Write-Host "Successfully renamed $sourceDir to $targetDir"
}
catch {
    Write-Error "Failed to rename directory: $_"
    exit 1
}

# Now update all references in files
Write-Host "Updating references in files..."

# Get all files in the new directory
$files = Get-ChildItem -Path $targetDir -Recurse -File | 
    Where-Object { 
        $_.FullName -notlike '*\.git*' -and 
        $_.FullName -notlike '*\node_modules*' -and
        $_.FullName -notlike '*\build*' -and
        $_.FullName -notlike '*\dist*' -and
        $_.FullName -notlike '*\.vs*' -and
        $_.FullName -notlike '*\x64*' -and
        $_.FullName -notlike '*\x86*' -and
        $_.FullName -notlike '*\Debug*' -and
        $_.FullName -notlike '*\Release*' -and
        $_.FullName -notlike '*\CMakeFiles*' -and
        $_.FullName -notlike '*\.idea*' -and
        $_.FullName -notlike '*\__pycache__*' -and
        $_.FullName -notlike '*\.pytest_cache*' -and
        $_.FullName -notlike '*\update_*.ps1' -and
        $_.FullName -notlike '*\rename_*.ps1'
    }

# Define replacements
$replacements = @(
    @{ Pattern = '4ever'; Replacement = '4ever' },
    @{ Pattern = '4ever'; Replacement = '4ever' },
    @{ Pattern = '4ever'; Replacement = '4EVER' }
)

$filesUpdated = 0

foreach ($file in $files) {
    try {
        $content = Get-Content -Path $file.FullName -Raw -ErrorAction Stop
        $newContent = $content
        $contentChanged = $false
        
        # Apply all replacements
        foreach ($replacement in $replacements) {
            if ($newContent -match [regex]::Escape($replacement.Pattern)) {
                $newContent = $newContent -replace [regex]::Escape($replacement.Pattern), $replacement.Replacement
                $contentChanged = $true
            }
        }
        
        # Only update if content actually changed
        if ($contentChanged) {
            $newContent | Set-Content -Path $file.FullName -NoNewline -Force
            $filesUpdated++
            Write-Host "Updated references in: $($file.FullName)"
        }
    }
    catch {
        Write-Warning "Could not process $($file.FullName): $_"
    }
}

Write-Host "\nUpdate complete!" -ForegroundColor Green
Write-Host "Files with references updated: $filesUpdated"
