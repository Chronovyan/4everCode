# Script to verify all documentation files exist
$mkdocsPath = Join-Path $PSScriptRoot "..\mkdocs.yml"
$docsDir = Join-Path $PSScriptRoot "..\docs"

# Function to extract file paths from mkdocs.yml
function Get-MkdocsFiles {
    param (
        [Parameter(Mandatory=$true)]
        [string]$MkdocsPath
    )
    
    $content = Get-Content -Path $MkdocsPath -Raw
    $yaml = $content | ConvertFrom-Yaml
    
    $files = @()
    
    # Recursive function to process navigation items
    function Process-Item {
        param($item)
        
        if ($item -is [string]) {
            if ($item -ne 'index.md') {
                $files += $item
            }
        }
        elseif ($item -is [hashtable]) {
            foreach ($key in $item.Keys) {
                $value = $item[$key]
                if ($value -is [string]) {
                    $files += $value
                }
                elseif ($value -is [array]) {
                    foreach ($subItem in $value) {
                        Process-Item -item $subItem
                    }
                }
            }
        }
        elseif ($item -is [array]) {
            foreach ($subItem in $item) {
                Process-Item -item $subItem
            }
        }
    }
    
    # Start processing from the nav section
    if ($yaml.nav) {
        Process-Item -item $yaml.nav
    }
    
    return $files | Where-Object { $_ -ne $null -and $_ -ne '' } | Select-Object -Unique
}

# Check if files exist
$missingFiles = @()
$files = Get-MkdocsFiles -MkdocsPath $mkdocsPath

Write-Host "Verifying $($files.Count) files..."

foreach ($file in $files) {
    $fullPath = Join-Path $docsDir $file
    if (-not (Test-Path $fullPath)) {
        $missingFiles += $file
        Write-Host "[MISSING] $file" -ForegroundColor Red
    }
    else {
        Write-Host "[OK] $file" -ForegroundColor Green
    }
}

# Print summary
if ($missingFiles.Count -gt 0) {
    Write-Host "\nMissing files ($($missingFiles.Count)):" -ForegroundColor Yellow
    $missingFiles | ForEach-Object { Write-Host "- $_" -ForegroundColor Yellow }
    exit 1
}
else {
    Write-Host "\nAll documentation files are present!" -ForegroundColor Green
    exit 0
}
