# Update documentation script for 4ever
# This script updates Chronovyan to 4ever and .cvy to .4e in all documentation files

# Set the root directory
$rootDir = Join-Path $PSScriptRoot ".."
$docsDir = Join-Path $rootDir "docs"

# Get all markdown files
$files = Get-ChildItem -Path $docsDir -Recurse -File -Include *.md

# Process each file
foreach ($file in $files) {
    Write-Host "Updating $($file.FullName)"
    
    # Read file content
    $content = Get-Content -Path $file.FullName -Raw
    $originalContent = $content
    
    # Apply replacements
    $content = $content -replace 'Chronovyan', '4ever'
    $content = $content -replace 'chronovyan', '4ever'
    $content = $content -replace 'CHRONOVYAN', '4EVER'
    $content = $content -replace 'CVY_', '4E_'
    $content = $content -replace 'cvy_', '4e_'
    $content = $content -replace '\.cvy', '.4e'
    
    # Write back if changed
    if ($content -ne $originalContent) {
        Set-Content -Path $file.FullName -Value $content -NoNewline
    }
}

# Rename .cvy files to .4e in the file system
Get-ChildItem -Path $docsDir -Recurse -File -Filter "*.cvy" | ForEach-Object {
    $newName = $_.FullName -replace '\.cvy$', '.4e'
    Write-Host "Renaming $($_.FullName) to $newName"
    Rename-Item -Path $_.FullName -NewName $newName -Force
}

Write-Host "Documentation update complete!"
