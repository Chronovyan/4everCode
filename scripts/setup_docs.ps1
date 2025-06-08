# Chronovyan Documentation Setup Script
# This script sets up the development environment for building and contributing to Chronovyan's documentation

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

# Function to check if a command exists
function Test-CommandExists {
    param([string]$command)
    return (Get-Command $command -ErrorAction SilentlyContinue) -ne $null
}

# Start setup
Write-Host "`n🚀 Setting up Chronovyan documentation environment...`n" @Yellow

# Check if running as administrator
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Host "Warning: Running without administrator privileges. Some operations might require elevation." @Yellow
}

# Check Python installation
Write-SectionHeader "Checking Python Installation"

$python = Get-Command python -ErrorAction SilentlyContinue
if (-not $python) {
    Write-Host "❌ Python is not found in PATH. Please install Python 3.8 or higher and try again." @Red
    Write-Host "   Download Python: https://www.python.org/downloads/" @White
    exit 1
}

# Check Python version
$pythonVersion = & { python -c "import sys; print('.'.join(map(str, sys.version_info[:3])))" } 2>&1
$requiredVersion = [Version]"3.8.0"
$currentVersion = [Version]($pythonVersion -replace '\s')

if ($currentVersion -lt $requiredVersion) {
    Write-Host "❌ Python 3.8 or higher is required. Found Python $pythonVersion" @Red
    Write-Host "   Please upgrade your Python installation and try again." @White
    exit 1
}

Write-Host "✓ Python $pythonVersion is installed" @Green

# Set up virtual environment
Write-SectionHeader "Setting Up Virtual Environment"

$venvPath = ".venv"
$activateScript = "$PSScriptRoot\..\$venvPath\Scripts\Activate.ps1"

# Create virtual environment if it doesn't exist
if (-not (Test-Path $venvPath)) {
    Write-Host "Creating Python virtual environment..." @Green
    python -m venv $venvPath
    
    if (-not $?) {
        Write-Host "❌ Failed to create virtual environment" @Red
        exit 1
    }
    
    Write-Host "✓ Virtual environment created at $venvPath" @Green
} else {
    Write-Host "✓ Using existing virtual environment at $venvPath" @Green
}

# Activate virtual environment
if (Test-Path $activateScript) {
    try {
        & $activateScript
        Write-Host "✓ Activated virtual environment" @Green
    } catch {
        Write-Host "❌ Failed to activate virtual environment: $_" @Red
        exit 1
    }
} else {
    Write-Host "❌ Virtual environment activation script not found: $activateScript" @Red
    exit 1
}

# Upgrade pip and setuptools
Write-SectionHeader "Updating Python Packages"

try {
    Write-Host "Upgrading pip and setuptools..." @Green
    python -m pip install --upgrade pip setuptools wheel
    
    if (-not $?) {
        throw "Failed to upgrade pip and setuptools"
    }
    
    Write-Host "✓ Successfully upgraded pip and setuptools" @Green
} catch {
    Write-Host "❌ Error upgrading pip and setuptools: $_" @Red
    exit 1
}

# Install documentation dependencies
Write-SectionHeader "Installing Documentation Dependencies"

try {
    $requirementsFile = "$PSScriptRoot\..\docs-requirements.txt"
    
    if (-not (Test-Path $requirementsFile)) {
        throw "Requirements file not found: $requirementsFile"
    }
    
    Write-Host "Installing dependencies from $requirementsFile..." @Green
    pip install -r $requirementsFile
    
    if (-not $?) {
        throw "Failed to install documentation dependencies"
    }
    
    Write-Host "✓ Successfully installed documentation dependencies" @Green
} catch {
    Write-Host "❌ Error installing documentation dependencies: $_" @Red
    exit 1
}

# Install development tools
Write-SectionHeader "Installing Development Tools"

try {
    # Install pre-commit if not installed
    if (-not (Test-CommandExists pre-commit)) {
        Write-Host "Installing pre-commit..." @Green
        pip install pre-commit
        
        if (-not $?) {
            throw "Failed to install pre-commit"
        }
        
        Write-Host "✓ Successfully installed pre-commit" @Green
    } else {
        Write-Host "✓ pre-commit is already installed" @Green
    }
    
    # Set up pre-commit hooks
    Write-Host "`nSetting up pre-commit hooks..." @Green
    pre-commit install
    
    if (-not $?) {
        throw "Failed to set up pre-commit hooks"
    }
    
    Write-Host "✓ Successfully set up pre-commit hooks" @Green
    
} catch {
    Write-Host "❌ Error setting up development tools: $_" @Red
    Write-Host "   You can continue without these tools, but they are recommended for development." @Yellow
    $continue = Read-Host "Continue anyway? (y/N)"
    if ($continue -ne 'y') {
        exit 1
    }
}

# Final instructions
Write-SectionHeader "Setup Complete!"

Write-Host "🎉 Documentation environment setup is complete!" @Green
Write-Host "`nNext steps:" @White
Write-Host "1. To activate the virtual environment in a new terminal, run:" @White
Write-Host "   .\$venvPath\Scripts\Activate.ps1" @Cyan
Write-Host "2. To build the documentation, run:" @White
Write-Host "   mkdocs serve" @Cyan
Write-Host "3. Open http://127.0.0.1:8000 in your browser to view the documentation" @White
Write-Host "`nHappy documenting! 🚀" @Yellow

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
