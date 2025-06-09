# PowerShell script to update 4ev to 4ev

# Define the root directory
$rootDir = "$PSScriptRoot\.."

# Get all files that might contain 4ev references
$files = Get-ChildItem -Path $rootDir -Recurse -File | 
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
        $_.FullName -notlike '*\.idea*'
    }

# Counter for tracking changes
$filesUpdated = 0
$referencesUpdated = 0

# Define replacements (simple string replacements)
$replacements = @(
    @{ Pattern = '4ev'; Replacement = '4ev' },
    @{ Pattern = '4ev'; Replacement = '4ev' },
    @{ Pattern = '4ev'; Replacement = '4EV' }
)

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
            $referencesUpdated++
            Write-Host "Updated references in: $($file.FullName)"
        }
    }
    catch {
        Write-Warning "Could not process $($file.FullName): $_"
    }
}

# Now handle file renames
$4evFiles = Get-ChildItem -Path $rootDir -Recurse -File | 
    Where-Object { 
        $_.Name -match '4ev|4ev|4ev' -and 
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
        $_.FullName -notlike '*\.idea*'
    }

foreach ($file in $4evFiles) {
    $newName = $file.FullName
    
    # Apply all replacements to the filename
    foreach ($replacement in $replacements) {
        $newName = $newName -replace [regex]::Escape($replacement.Pattern), $replacement.Replacement
    }
    
    # Only rename if the name actually changed and the target doesn't exist
    if ($newName -ne $file.FullName) {
        try {
            $parentDir = Split-Path -Path $newName -Parent
            $newFileName = Split-Path -Path $newName -Leaf
            
            # Ensure the parent directory exists
            if (-not (Test-Path $parentDir)) {
                New-Item -ItemType Directory -Path $parentDir -Force | Out-Null
            }
            
            # Only rename if the target doesn't exist or is the same file
            if (-not (Test-Path $newName) -or (Resolve-Path $file.FullName) -eq (Resolve-Path $newName)) {
                Rename-Item -Path $file.FullName -NewName $newFileName -Force
                $filesUpdated++
                Write-Host "Renamed: $($file.FullName) -> $newName"
            } else {
                Write-Warning "Skipping $($file.FullName) - $newName already exists and is different"
            }
        }
        catch {
            Write-Warning "Failed to rename $($file.FullName) to $newName : $_"
        }
    }
}

# Handle directory renames (need to process from deepest first)
$4evDirs = Get-ChildItem -Path $rootDir -Recurse -Directory | 
    Where-Object { 
        $_.Name -match '4ev|4ev|4ev' -and 
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
        $_.FullName -notlike '*\.idea*'
    } | 
    Sort-Object { $_.FullName.Length } -Descending

foreach ($dir in $4evDirs) {
    $newName = $dir.FullName
    
    # Apply all replacements to the directory name
    foreach ($replacement in $replacements) {
        $newName = $newName -replace [regex]::Escape($replacement.Pattern), $replacement.Replacement
    }
    
    # Only rename if the name actually changed and the target doesn't exist
    if ($newName -ne $dir.FullName) {
        try {
            # Only rename if the target doesn't exist
            if (-not (Test-Path $newName)) {
                Rename-Item -Path $dir.FullName -NewName (Split-Path $newName -Leaf) -Force
                Write-Host "Renamed directory: $($dir.FullName) -> $newName"
            } else {
                Write-Warning "Skipping directory $($dir.FullName) - $newName already exists"
            }
        }
        catch {
            Write-Warning "Failed to rename directory $($dir.FullName) to $newName : $_"
        }
    }
}

Write-Host "\nUpdate complete!" -ForegroundColor Green
Write-Host "Files renamed: $filesUpdated"
Write-Host "Directories renamed: $($4evDirs.Count)"
Write-Host "Files with references updated: $referencesUpdated"
