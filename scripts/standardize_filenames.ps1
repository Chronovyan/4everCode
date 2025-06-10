# Standardize documentation filenames according to our style guide

# Define the directory containing the documentation
$docsDir = Join-Path $PSScriptRoot "..\docs"
$coreDir = Join-Path $docsDir "core"

# Define the renaming mappings
$renameMap = @{
    "Core Documentation - The Art of Temporal Programming.md" = "05_temporal_programming_guide.md"
    "Practical Applications - The Living Narrative of Code.md" = "06_practical_applications.md"
    "Resource Management - The Battle for Temporal Control.md" = "07_resource_management.md"
    "Variables - Temporal Data Management.md" = "08_variables_and_data.md"
}

# Function to update references in files
function Update-FileReferences {
    param (
        [string]$directory,
        [hashtable]$renames
    )
    
    Get-ChildItem -Path $directory -Recurse -File -Filter "*.md" | ForEach-Object {
        $content = Get-Content $_.FullName -Raw
        $updated = $false
        
        $renames.GetEnumerator() | ForEach-Object {
            $oldName = [regex]::Escape($_.Key)
            $newName = $_.Value
            
            if ($content -match $oldName) {
                $content = $content -replace $oldName, $newName
                $updated = $true
            }
        }
        
        if ($updated) {
            Set-Content -Path $_.FullName -Value $content -NoNewline
            Write-Host "Updated references in $($_.Name)"
        }
    }
}

# Update references first
Write-Host "Updating references in markdown files..."
Update-FileReferences -directory $docsDir -renames $renameMap

# Rename files
Write-Host "Renaming files..."
$renameMap.GetEnumerator() | ForEach-Object {
    $oldPath = Join-Path $coreDir $_.Key
    $newPath = Join-Path $coreDir $_.Value
    
    if (Test-Path $oldPath) {
        Rename-Item -Path $oldPath -NewName $_.Value -ErrorAction SilentlyContinue
        Write-Host "Renamed: $($_.Key) -> $($_.Value)"
    }
}

# Update mkdocs.yml if it exists
$mkdocsPath = Join-Path $docsDir "..\mkdocs.yml"
if (Test-Path $mkdocsPath) {
    $content = Get-Content $mkdocsPath -Raw
    $updated = $false
    
    $renameMap.GetEnumerator() | ForEach-Object {
        $oldName = [regex]::Escape($_.Key)
        $newName = $_.Value
        
        if ($content -match $oldName) {
            $content = $content -replace $oldName, $newName
            $updated = $true
        }
    }
    
    if ($updated) {
        Set-Content -Path $mkdocsPath -Value $content -NoNewline
        Write-Host "Updated references in mkdocs.yml"
    }
}

Write-Host "Filename standardization complete!"
