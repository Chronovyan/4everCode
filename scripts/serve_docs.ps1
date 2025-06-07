<#
.SYNOPSIS
    Serves the Chronovyan documentation locally for preview.
.DESCRIPTION
    This script starts a local MkDocs development server to preview the documentation.
    It handles dependency installation and environment setup automatically.
.PARAMETER Port
    The port number to serve the documentation on (default: 8000).
.PARAMETER Host
    The host address to bind to (default: localhost).
.EXAMPLE
    .\serve_docs.ps1
    Starts the documentation server on http://localhost:8000
.EXAMPLE
    .\serve_docs.ps1 -Port 8080 -Host 0.0.0.0
    Starts the documentation server on http://0.0.0.0:8080 (accessible from other devices on the network)
#>

param(
    [int]$Port = 8000,
    [string]$Host = "localhost"
)

# Colors for output
$Green = @{ForegroundColor = 'Green'}
$Yellow = @{ForegroundColor = 'Yellow'}
$Cyan = @{ForegroundColor = 'Cyan'}
$Red = @{ForegroundColor = 'Red'}

function Show-Header {
    Write-Host "`n"
    Write-Host "  ____  _                      _                    _           _ " @Cyan
    Write-Host " / ___|| |__   __ _ _ __   __| |_ __   __ _ _ __ | |__   __ _| |" @Cyan
    Write-Host "| |    | '_ \\ / _` | '_ \\ / _` | '_ \\ / _` | '_ \| '_ \\ / _` | |" @Cyan
    Write-Host "| |____| | | | (_| | | | | (_| | | | | (_| | | | | | | | (_| | |" @Cyan
    Write-Host " \\____||_| |_|\\__,_|_| |_|\\__,_|_| |_|\\__,_|_| |_|_| |_|\\__,_|_|" @Cyan
    Write-Host "`n                  Documentation Preview Server`n" @Cyan
}

function Test-CommandExists {
    param($command)
    $exists = $null -ne (Get-Command $command -ErrorAction SilentlyContinue)
    return $exists
}

function Install-Dependencies {
    Write-Host "`nChecking dependencies..." @Green
    
    # Check if Python is installed
    if (-not (Test-CommandExists python)) {
        Write-Host "Python is not installed. Please install Python 3.8 or higher and try again." @Red
        exit 1
    }
    
    # Check Python version
    $pythonVersion = & { python -c "import sys; print('.'.join(map(str, sys.version_info[:2])))" }
    $requiredVersion = [Version]"3.8"
    $currentVersion = [Version]$pythonVersion

    if ($currentVersion -lt $requiredVersion) {
        Write-Host "Python 3.8 or higher is required. Found Python $pythonVersion" @Red
        exit 1
    }
    
    # Check if virtual environment exists
    if (-not (Test-Path "venv")) {
        Write-Host "Virtual environment not found. Running setup..." @Yellow
        & ".\scripts\setup_docs.ps1"
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Failed to set up documentation environment." @Red
            exit 1
        }
    }
    
    # Activate virtual environment
    $activateScript = ".\venv\Scripts\Activate.ps1"
    if (Test-Path $activateScript) {
        & $activateScript
    } else {
        Write-Host "Failed to activate virtual environment. Script not found: $activateScript" @Red
        exit 1
    }
    
    # Check if mkdocs is installed
    if (-not (Test-CommandExists mkdocs)) {
        Write-Host "Installing MkDocs and dependencies..." @Green
        pip install -r docs-requirements.txt
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Failed to install documentation dependencies." @Red
            exit 1
        }
    }
}

function Start-DocumentationServer {
    param(
        [int]$Port = 8000,
        [string]$Host = "localhost"
    )
    
    Write-Host "`nStarting documentation server..." @Green
    Write-Host "  - Host: http://${Host}:${Port}" @Cyan
    Write-Host "  - Press Ctrl+C to stop the server`n" @Yellow
    
    try {
        # Start MkDocs server
        mkdocs serve --dev-addr "${Host}:${Port}"
    }
    catch {
        Write-Host "Error starting documentation server: $_" @Red
        exit 1
    }
}

# Main execution
function Main {
    param(
        [int]$Port = 8000,
        [string]$Host = "localhost"
    )
    
    Show-Header
    
    try {
        Install-Dependencies
        Start-DocumentationServer -Port $Port -Host $Host
    }
    catch {
        Write-Host "An error occurred: $_" @Red
        exit 1
    }
}

# Parse command line arguments
$params = @{}
for ($i = 0; $i -lt $args.Length; $i++) {
    $arg = $args[$i]
    if ($arg.StartsWith('-')) {
        $paramName = $arg.TrimStart('-')
        if ($i + 1 -lt $args.Length -and !$args[$i + 1].StartsWith('-')) {
            $paramValue = $args[$i + 1]
            $params[$paramName] = $paramValue
            $i++
        } else {
            $params[$paramName] = $true
        }
    }
}

# Call main with parameters
Main @params
