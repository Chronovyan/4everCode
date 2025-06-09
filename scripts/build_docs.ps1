<#
.SYNOPSIS
    Build and test the 4ever documentation.
.DESCRIPTION
    This script automates the process of validating, building, and serving
    the 4ever documentation locally for testing.
.EXAMPLE
    .\scripts\build_docs.ps1 -Serve
    # Validates and serves the documentation locally
.EXAMPLE
    .\scripts\build_docs.ps1 -ValidateOnly
    # Only runs validation checks without building
#>

[CmdletBinding()]
param(
    [switch]$Serve,
    [switch]$ValidateOnly,
    [string]$HostAddress = "127.0.0.1",
    [int]$Port = 8000
)

$ErrorActionPreference = "Stop"

# Configuration
$RootDir = Split-Path -Parent $PSScriptRoot
$SiteDir = Join-Path $RootDir "site"
$MkdocsYml = Join-Path $RootDir "mkdocs.yml"
$Python = "python"

# Check if Python is available
try {
    $pythonVersion = & $Python --version
    Write-Host "Using Python: $pythonVersion" -ForegroundColor Green
} catch {
    Write-Error "Python is required but not found. Please install Python 3.8+ and ensure it's in your PATH."
    exit 1
}

# Function to run a command and check its exit code
function Invoke-CommandCheck {
    param (
        [string]$Command,
        [string]$ErrorMsg = "Command failed"
    )
    
    Write-Host "`n> $Command" -ForegroundColor Cyan
    $global:LASTEXITCODE = 0
    Invoke-Expression $Command
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "$ErrorMsg. Exit code: $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}

# Install Python dependencies
$requirements = Join-Path $PSScriptRoot "requirements-docs.txt"
if (Test-Path $requirements) {
    Write-Host "`nInstalling Python dependencies..." -ForegroundColor Green
    Invoke-CommandCheck "$Python -m pip install -r `"$requirements`"" "Failed to install Python dependencies"
} else {
    Write-Warning "requirements-docs.txt not found. Skipping dependency installation."
}

# Validate documentation
Write-Host "`nValidating documentation..." -ForegroundColor Green
$validateScript = Join-Path $PSScriptRoot "validate_docs.py"
if (Test-Path $validateScript) {
    Invoke-CommandCheck "$Python `"$validateScript`"" "Documentation validation failed"
} else {
    Write-Warning "validate_docs.py not found. Skipping validation."
}

if ($ValidateOnly) {
    Write-Host "`nValidation complete. No issues found!" -ForegroundColor Green
    exit 0
}

# Build documentation
Write-Host "`nBuilding documentation..." -ForegroundColor Green
if (Test-Path $MkdocsYml) {
    Push-Location $RootDir
    try {
        # Clean previous build
        if (Test-Path $SiteDir) {
            Remove-Item -Path $SiteDir -Recurse -Force
        }
        
        # Build docs
        Invoke-CommandCheck "mkdocs build --strict --clean --verbose" "Documentation build failed"
        
        # Check for build output
        $indexHtml = Join-Path $SiteDir "index.html"
        if (-not (Test-Path $indexHtml)) {
            throw "Build completed but index.html not found in $SiteDir"
        }
        
        Write-Host "`nDocumentation built successfully to: $SiteDir" -ForegroundColor Green
        
        # Serve documentation if requested
        if ($Serve) {
            Write-Host "`nStarting documentation server at http://${HostAddress}:${Port}" -ForegroundColor Green
            Write-Host "Press Ctrl+C to stop the server..." -ForegroundColor Yellow
            
            try {
                # Start the MkDocs server
                $mkdocsProcess = Start-Process -FilePath "mkdocs" -ArgumentList "serve --dev-addr ${HostAddress}:${Port}" -NoNewWindow -PassThru
                
                # Open browser
                Start-Process "http://${HostAddress}:${Port}"
                
                # Wait for Ctrl+C
                while ($true) {
                    Start-Sleep -Seconds 1
                }
            } finally {
                if ($mkdocsProcess -and -not $mkdocsProcess.HasExited) {
                    Stop-Process -Id $mkdocsProcess.Id -Force
                }
            }
        }
    } finally {
        Pop-Location
    }
} else {
    Write-Error "mkdocs.yml not found in $RootDir"
    exit 1
}
