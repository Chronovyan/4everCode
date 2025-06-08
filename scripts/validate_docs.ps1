# Documentation Validation Script
# This script checks for common documentation issues and validates the structure

# Set strict mode
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

# Colors for output
$Green = @{ForegroundColor = 'Green'}
$Yellow = @{ForegroundColor = 'Yellow'}
$Cyan = @{ForegroundColor = 'Cyan'}
$Red = @{ForegroundColor = 'Red'}
$White = @{ForegroundColor = 'White'}

# Function to write section headers
function Write-SectionHeader {
    param([string]$Title)
    $divider = '=' * 80
    Write-Host "`n$divider" @Cyan
    Write-Host $Title.PadLeft(($divider.Length + $Title.Length) / 2) @Cyan
    Write-Host "$divider`n" @Cyan
}

# Function to check if a file exists
function Test-FileExists {
    param([string]$Path)
    if (-not (Test-Path $Path)) {
        Write-Host "❌ Missing file: $Path" @Red
        return $false
    }
    return $true
}

# Function to check if a directory exists
function Test-DirectoryExists {
    param([string]$Path)
    if (-not (Test-Path $Path)) {
        Write-Host "❌ Missing directory: $Path" @Red
        return $false
    }
    return $true
}

# Function to check for broken links in a markdown file
function Test-MarkdownLinks {
    param([string]$FilePath)
    
    $content = Get-Content -Path $FilePath -Raw
    $baseDir = Split-Path -Parent $FilePath
    
    # Find all markdown links [text](url)
    $linkPattern = '\[([^\]]+)\]\(([^)]+)\)'
    $matches = [regex]::Matches($content, $linkPattern)
    
    $hasErrors = $false
    
    foreach ($match in $matches) {
        $linkText = $match.Groups[1].Value
        $linkUrl = $match.Groups[2].Value
        
        # Skip external links
        if ($linkUrl -match '^(https?|ftp|mailto):\/\/') {
            continue
        }
        
        # Handle anchor links
        if ($linkUrl -match '^#') {
            # Check if the anchor exists in the same file
            $anchor = $linkUrl.Substring(1)
            if (-not ($content -match "^#+\s+$anchor\b" -or $content -match "id=[\"']$anchor[\"']")) {
                Write-Host "❌ Broken anchor link in $([System.IO.Path]::GetFileName($FilePath)): $linkUrl (text: $linkText)" @Red
                $hasErrors = $true
            }
            continue
        }
        
        # Handle relative links
        $fullPath = [System.IO.Path]::GetFullPath((Join-Path $baseDir $linkUrl))
        
        # Handle links with anchors
        if ($linkUrl -match '^(.+?)(#.+)?$') {
            $filePart = $matches[1]
            $anchorPart = $matches[2]
            
            if ($filePart) {
                $fullPath = [System.IO.Path]::GetFullPath((Join-Path $baseDir $filePart))
                
                # Check if file exists
                if (-not (Test-Path $fullPath)) {
                    Write-Host "❌ Broken link in $([System.IO.Path]::GetFileName($FilePath)): $linkUrl (text: $linkText)" @Red
                    $hasErrors = $true
                    continue
                }
                
                # If there's an anchor, check it exists in the target file
                if ($anchorPart) {
                    $targetContent = Get-Content -Path $fullPath -Raw -ErrorAction SilentlyContinue
                    if (-not $targetContent) {
                        Write-Host "⚠️  Could not check anchor in $linkUrl (file not readable)" @Yellow
                        continue
                    }
                    
                    $anchor = $anchorPart.Substring(1)  # Remove the #
                    if (-not ($targetContent -match "^#+\s+$anchor\b" -or $targetContent -match "id=[\"']$anchor[\"']")) {
                        Write-Host "❌ Broken anchor in $linkUrl: $anchorPart (linked from $([System.IO.Path]::GetFileName($FilePath)))" @Red
                        $hasErrors = $true
                    }
                }
            }
        }
    }
    
    return -not $hasErrors
}

# Main script
Write-Host "`n🔍 Validating Chronovyan Documentation...`n" @Yellow

# Check required files
Write-SectionHeader "Checking Required Files"

$requiredFiles = @(
    "README.md",
    "CONTRIBUTING.md",
    "CODE_OF_CONDUCT.md",
    "LICENSE",
    "mkdocs.yml",
    "docs/index.md",
    "docs/development/contributing.md",
    "docs/development/documentation-guide.md"
)

$allFilesExist = $true
foreach ($file in $requiredFiles) {
    if (-not (Test-FileExists $file)) {
        $allFilesExist = $false
    } else {
        Write-Host "✓ Found: $file" @Green
    }
}

# Check required directories
Write-SectionHeader "Checking Directory Structure"

$requiredDirs = @(
    "docs/guides",
    "docs/reference",
    "docs/development",
    "docs/design",
    "docs/community"
)

$allDirsExist = $true
foreach ($dir in $requiredDirs) {
    if (-not (Test-DirectoryExists $dir)) {
        $allDirsExist = $false
    } else {
        Write-Host "✓ Found directory: $dir" @Green
    }
}

# Check markdown files for issues
Write-SectionHeader "Checking Markdown Files"

$markdownFiles = Get-ChildItem -Path . -Recurse -Include *.md -File | 
    Where-Object { $_.FullName -notlike '*\node_modules\*' -and $_.FullName -notlike '*\venv\*' }

$hasMarkdownIssues = $false

foreach ($file in $markdownFiles) {
    $relativePath = $file.FullName.Substring((Get-Location).Path.Length + 1)
    Write-Host "Checking: $relativePath" @White
    
    # Check for broken links
    if (-not (Test-MarkdownLinks -FilePath $file.FullName)) {
        $hasMarkdownIssues = $true
    }
    
    # Check for TODO/FIXME comments
    $content = Get-Content -Path $file.FullName -Raw
    if ($content -match '(?i)TODO|FIXME|XXX') {
        Write-Host "⚠️  Contains TODO/FIXME/XXX in $relativePath" @Yellow
    }
    
    # Check for trailing whitespace
    if ($content -match ' \r?$') {
        Write-Host "⚠️  Contains trailing whitespace in $relativePath" @Yellow
    }
}

# Summary
Write-SectionHeader "Validation Summary"

if (-not $allFilesExist) {
    Write-Host "❌ Some required files are missing" @Red
} else {
    Write-Host "✓ All required files exist" @Green
}

if (-not $allDirsExist) {
    Write-Host "❌ Some required directories are missing" @Red
} else {
    Write-Host "✓ Directory structure is valid" @Green
}

if ($hasMarkdownIssues) {
    Write-Host "❌ Some markdown files have issues" @Red
} else {
    Write-Host "✓ No issues found in markdown files" @Green
}

if ($allFilesExist -and $allDirsExist -and (-not $hasMarkdownIssues)) {
    Write-Host "`n🎉 Documentation validation passed successfully!" @Green
} else {
    Write-Host "`n❌ Documentation validation found issues that need to be addressed." @Red
    exit 1
}
