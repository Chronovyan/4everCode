# Chronovyan Documentation Build Script
# This script builds and validates the documentation

# Colors for output
$Green = @{ForegroundColor = 'Green'}
$Yellow = @{ForegroundColor = 'Yellow'}
$Cyan = @{ForegroundColor = 'Cyan'}
$Red = @{ForegroundColor = 'Red'}

function Show-Header {
    Write-Host "`n"
    Write-Host "  ____  _                      _                    _           _ " @Cyan
    Write-Host " / ___|| |__   __ _ _ __   __| |_ __   __ _ _ __ | |__   __ _| |" @Cyan
    Write-Host "| |    | '_ \\ / _` | '_ \\ / _` | '_ \\ / _` | '_ \\| '_ \\ / _` | |" @Cyan
    Write-Host "| |____| | | | (_| | | | | (_| | | | | (_| | | | | | | | (_| | |" @Cyan
    Write-Host " \\____||_| |_|\\__,_|_| |_|\\__,_|_| |_|\\__,_|_| |_|_| |_|\\__,_|_|" @Cyan
    Write-Host "`n                      Documentation Build System`n" @Cyan
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

function Invoke-Validation {
    Write-Host "`nValidating documentation..." @Green
    python .\scripts\validate_docs.py
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Documentation validation failed. Please fix the issues before building." @Red
        exit 1
    }
}

function Invoke-Build {
    param(
        [switch]$Serve,
        [switch]$Deploy,
        [string]$Env = "local"
    )
    
    Write-Host "`nBuilding documentation..." @Green
    
    # Clean build directory
    if (Test-Path "site") {
        Remove-Item -Recurse -Force "site"
    }
    
    # Build the documentation
    mkdocs build --strict --clean --verbose
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Documentation build failed." @Red
        exit 1
    }
    
    # Check for broken links
    Write-Host "`nChecking for broken links..." @Green
    python -m mkdocs build --strict --clean
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Link check failed." @Red
        exit 1
    }
    
    # Serve the documentation if requested
    if ($Serve) {
        Write-Host "`nStarting development server..." @Green
        Write-Host "Open http://localhost:8000 in your browser to view the documentation." @Cyan
        Write-Host "Press Ctrl+C to stop the server.`n" @Yellow
        mkdocs serve
    }
    
    # Deploy the documentation if requested
    if ($Deploy) {
        Write-Host "`nDeploying documentation..." @Green
        mkdocs gh-deploy --force --clean --no-history
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Deployment failed." @Red
            exit 1
        }
    }
}

function Show-Help {
    Write-Host "`nUsage: .\build_docs.ps1 [options]" @Cyan
    Write-Host "Options:" @Cyan
    Write-Host "  -Serve         Start the development server after building" @Green
    Write-Host "  -Deploy        Deploy the documentation to GitHub Pages" @Green
    Write-Host "  -Env <env>     Environment (local, staging, production)" @Green
    Write-Host "  -Help          Show this help message" @Green
    Write-Host "`nExamples:" @Cyan
    Write-Host "  .\build_docs.ps1 -Serve" @Green
    Write-Host "  .\build_docs.ps1 -Deploy -Env production" @Green
}

# Main execution
function Main {
    param(
        [switch]$Help,
        [switch]$Serve,
        [switch]$Deploy,
        [string]$Env = "local"
    )
    
    Show-Header
    
    if ($Help) {
        Show-Help
        return
    }
    
    try {
        Install-Dependencies
        Invoke-Validation
        Invoke-Build -Serve:$Serve -Deploy:$Deploy -Env $Env
        
        Write-Host "`nDocumentation build completed successfully!" @Green
    }
    catch {
        Write-Host "`nAn error occurred: $_" @Red
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
