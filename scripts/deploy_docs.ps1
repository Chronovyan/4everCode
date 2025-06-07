<#
.SYNOPSIS
    Deploys the Chronovyan documentation to GitHub Pages.
.DESCRIPTION
    This script builds and deploys the documentation to GitHub Pages.
    It handles dependency installation, building, and deployment in one step.
.PARAMETER Message
    The commit message for the deployment (default: "Deploy documentation").
.PARAMETER Branch
    The branch to deploy to (default: gh-pages).
.EXAMPLE
    .\deploy_docs.ps1
    Deploys the documentation with the default commit message.
.EXAMPLE
    .\deploy_docs.ps1 -Message "Update API reference" -Branch "gh-pages"
    Deploys the documentation with a custom commit message to a specific branch.
#>

param(
    [string]$Message = "Deploy documentation",
    [string]$Branch = "gh-pages"
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
    Write-Host "`n                  Documentation Deployment`n" @Cyan
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
    
    # Check if mkdocs and gh-deploy are available
    if (-not (Test-CommandExists mkdocs) -or -not (pip show mkdocs-material | Select-String -Pattern "^Name:")) {
        Write-Host "Installing MkDocs and dependencies..." @Green
        pip install -r docs-requirements.txt
        if ($LASTEXITCODE -ne 0) {
            Write-Host "Failed to install documentation dependencies." @Red
            exit 1
        }
    }
}

function Build-Documentation {
    Write-Host "`nBuilding documentation..." @Green
    
    # Clean the site directory
    if (Test-Path "site") {
        Remove-Item -Recurse -Force "site"
    }
    
    # Build the documentation
    mkdocs build --strict --clean
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Failed to build documentation." @Red
        exit 1
    }
    
    Write-Host "Documentation built successfully." @Green
}

function Deploy-Documentation {
    param(
        [string]$Message,
        [string]$Branch
    )
    
    Write-Host "`nDeploying documentation to GitHub Pages..." @Green
    
    # Check if git is installed
    if (-not (Test-CommandExists git)) {
        Write-Host "Git is not installed. Please install Git and try again." @Red
        exit 1
    }
    
    # Check if this is a git repository
    $gitDir = git rev-parse --git-dir 2>$null
    if (-not $?) {
        Write-Host "This is not a Git repository. Please initialize Git and try again." @Red
        exit 1
    }
    
    # Check for uncommitted changes
    $status = git status --porcelain
    if ($status) {
        Write-Host "Warning: You have uncommitted changes in your working directory." @Yellow
        $response = Read-Host "Do you want to continue? (y/N)"
        if ($response -ne 'y') {
            Write-Host "Deployment cancelled." @Yellow
            exit 0
        }
    }
    
    # Deploy to GitHub Pages
    try {
        $env:MESSAGE = $Message
        mkdocs gh-deploy --force --clean --no-history --message $Message --remote-branch $Branch
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host "`nDocumentation deployed successfully to GitHub Pages!" @Green
            $repoUrl = git config --get remote.origin.url
            $repoUrl = $repoUrl -replace '\.git$', '' -replace '^git@github.com:', 'https://github.com/'
            $pagesUrl = $repoUrl -replace '\.git$', '' -replace '^https://github\.com/', 'https://'
            $pagesUrl = "https://${pagesUrl}/tree/${Branch}"
            Write-Host "View your documentation at: $pagesUrl" @Cyan
        } else {
            Write-Host "Failed to deploy documentation." @Red
            exit 1
        }
    }
    catch {
        Write-Host "Error deploying documentation: $_" @Red
        exit 1
    }
}

# Main execution
function Main {
    param(
        [string]$Message,
        [string]$Branch
    )
    
    Show-Header
    
    try {
        Install-Dependencies
        Build-Documentation
        Deploy-Documentation -Message $Message -Branch $Branch
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

# Set default values if not provided
if (-not $params.ContainsKey('Message')) { $params['Message'] = $Message }
if (-not $params.ContainsKey('Branch')) { $params['Branch'] = $Branch }

# Call main with parameters
Main @params
