# Chronovyan Documentation Setup Script for Windows
# This script sets up the development environment for the documentation

# Colors for output
$Green = @{ForegroundColor = 'Green'}
$Yellow = @{ForegroundColor = 'Yellow'}
$Cyan = @{ForegroundColor = 'Cyan'}
$Red = @{ForegroundColor = 'Red'}

Write-Host "`nSetting up Chronovyan documentation environment...`n" @Yellow

# Check if Python is installed
$python = Get-Command python -ErrorAction SilentlyContinue
if (-not $python) {
    Write-Host "Python is required but not found. Please install Python 3.8 or higher and try again." @Red
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

# Create virtual environment if it doesn't exist
if (-not (Test-Path "venv")) {
    Write-Host "Creating virtual environment..." @Green
    python -m venv venv
}

# Activate virtual environment
$activateScript = ".\venv\Scripts\Activate.ps1"
if (Test-Path $activateScript) {
    & $activateScript
} else {
    Write-Host "Failed to activate virtual environment. Script not found: $activateScript" @Red
    exit 1
}

# Upgrade pip
Write-Host "`nUpgrading pip..." @Green
python -m pip install --upgrade pip

# Install documentation dependencies
Write-Host "`nInstalling documentation dependencies..." @Green
pip install -r docs-requirements.txt

# Create necessary directories
Write-Host "`nSetting up directory structure..." @Green
$directories = @(
    "docs\assets\images",
    "docs\assets\js",
    "docs\assets\css",
    "docs\getting-started",
    "docs\language",
    "docs\temporal-programming",
    "docs\advanced",
    "docs\api",
    "docs\development",
    "docs\community"
)

foreach ($dir in $directories) {
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
    if (-not (Test-Path "$dir\.gitkeep")) {
        New-Item -ItemType File -Path "$dir\.gitkeep" -Force | Out-Null
    }
}

# Create .gitignore if it doesn't exist
if (-not (Test-Path ".gitignore")) {
    Write-Host "`nCreating .gitignore..." @Green
    @"
# Virtual environment
venv/
ENV/

# Python cache
__pycache__/
*.py[cod]
*`$py.class

# MkDocs build output
site/

# Local development
.DS_Store
Thumbs.db

# IDE specific files
.vscode/
.idea/
*.swp
*.swo

# Environment variables
.env

# Logs
logs/
*.log
"@ | Out-File -FilePath ".gitignore" -Encoding utf8
}

Write-Host "`nDocumentation setup complete!" @Green
Write-Host "`nTo start the development server, run:" @Cyan
Write-Host "  mkdocs serve`n" @Yellow
Write-Host "To build the documentation, run:" @Cyan
Write-Host "  mkdocs build`n" @Yellow
Write-Host "For more information, see the documentation at http://localhost:8000 when the server is running." @Cyan
