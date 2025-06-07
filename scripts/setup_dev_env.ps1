<#
.SYNOPSIS
    Sets up a development environment for Chronovyan.
.DESCRIPTION
    This script installs the necessary dependencies and sets up the development
    environment for Chronovyan. It supports Windows, macOS, and Linux.
#>

param (
    [switch]$NoPrompt = $false
)

function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Cyan
}

function Write-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARNING] $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
}

function Test-CommandExists {
    param([string]$Command)
    $exists = $null -ne (Get-Command $Command -ErrorAction SilentlyContinue)
    return $exists
}

function Install-WindowsDependencies {
    Write-Info "Installing Windows dependencies..."
    
    # Check if chocolatey is installed
    if (-not (Test-CommandExists "choco")) {
        Write-Info "Installing Chocolatey..."
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    }
    
    # Install required packages
    $packages = @("cmake", "ninja", "llvm")
    foreach ($package in $packages) {
        if (-not (Test-CommandExists $package)) {
            Write-Info "Installing $package..."
            choco install $package -y
        }
    }
    
    # Add LLVM to PATH if not already there
    $llvmPath = "C:\Program Files\LLVM\bin"
    if ($env:PATH -notlike "*$llvmPath*") {
        [System.Environment]::SetEnvironmentVariable("PATH", "$llvmPath;" + [System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::User), [System.EnvironmentVariableTarget]::User)
        $env:PATH = "$llvmPath;" + $env:PATH
        Write-Info "Added LLVM to PATH"
    }
}

function Install-LinuxDependencies {
    Write-Info "Installing Linux dependencies..."
    
    if (Test-CommandExists "apt-get") {
        # Debian/Ubuntu
        sudo apt-get update
        sudo apt-get install -y build-essential cmake ninja-build clang clang-tidy clang-format
    } elseif (Test-CommandExists "yum") {
        # RHEL/CentOS
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y cmake ninja-build clang clang-tools-extra
    } elseif (Test-CommandExists "dnf") {
        # Fedora
        sudo dnf groupinstall -y "Development Tools"
        sudo dnf install -y cmake ninja-build clang clang-tools-extra
    } elseif (Test-CommandExists "pacman") {
        # Arch Linux
        sudo pacman -Syu --noconfirm
        sudo pacman -S --noconfirm base-devel cmake ninja clang
    else
        Write-Error "Unsupported Linux distribution. Please install the following packages manually:"
        Write-Host "  - build-essential / base-devel"
        Write-Host "  - cmake"
        Write-Host "  - ninja-build"
        Write-Host "  - clang"
        exit 1
    fi
}

function Install-MacDependencies {
    Write-Info "Installing macOS dependencies..."
    
    # Check if Homebrew is installed
    if (-not (Test-CommandExists "brew")) {
        Write-Info "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # Add Homebrew to PATH
        $brewPath = "/usr/local/bin"
        if ($env:PATH -notlike "*$brewPath*") {
            [System.Environment]::SetEnvironmentVariable("PATH", "$brewPath;" + [System.Environment]::GetEnvironmentVariable("PATH", [System.EnvironmentVariableTarget]::User), [System.EnvironmentVariableTarget]::User)
            $env:PATH = "$brewPath;" + $env:PATH
        }
    }
    
    # Install required packages
    brew update
    brew install cmake ninja llvm
}

# Main script
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Chronovyan Development Environment Setup  " -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if running as administrator/root
$isAdmin = $false
if ($IsWindows) {
    $isAdmin = ([Security.Principal.WindowsPrincipal] [Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
} else {
    $isAdmin = (id -u) -eq 0
}

if (-not $isAdmin -and -not $NoPrompt) {
    Write-Warning "This script requires administrative privileges to install dependencies."
    $response = Read-Host "Do you want to continue without installing system dependencies? (y/N)"
    if ($response -ne "y") {
        Write-Info "Please run this script as administrator/root to install all dependencies."
        exit 1
    }
}

# Install platform-specific dependencies
if ($IsWindows) {
    Install-WindowsDependencies
} elseif ($IsMacOS) {
    Install-MacDependencies
} else {
    Install-LinuxDependencies
}

# Create build directory
Write-Info "Setting up build directory..."
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Configure CMake
Write-Info "Configuring CMake..."
Set-Location "build"
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON ..

if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed"
    exit 1
}

# Build the project
Write-Info "Building the project..."
cmake --build . --config Debug

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed"
    exit 1
}

# Run tests
Write-Info "Running tests..."
ctest --output-on-failure -C Debug

if ($LASTEXITCODE -ne 0) {
    Write-Warning "Some tests failed. Please check the test output for details."
} else {
    Write-Success "All tests passed!"
}

# Return to the original directory
Set-Location ".."

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "  Development environment setup complete!  " -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "To build the project:"
Write-Host "  cd build"
Write-Host "  cmake --build ."
Write-Host ""
Write-Host "To run tests:"
Write-Host "  cd build"
Write-Host "  ctest -C Debug --output-on-failure"
Write-Host ""
