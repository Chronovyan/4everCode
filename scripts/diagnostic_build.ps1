# Diagnostic Build Script for Chronovyan
# This script provides detailed diagnostics and multiple build options

# Set error action preference
$ErrorActionPreference = "Stop"

# Function to print section header
function Write-SectionHeader {
    param([string]$Title)
    Write-Host "`n=== $Title ===" -ForegroundColor Cyan
}

# Function to check if a command exists
function Test-CommandExists {
    param([string]$command)
    try {
        $null = Get-Command $command -ErrorAction Stop
        return $true
    } catch {
        return $false
    }
}

# Function to get compiler information
function Get-CompilerInfo {
    param([string]$compiler)
    
    if (-not (Test-CommandExists $compiler)) {
        return "Not found"
    }
    
    try {
        $version = & $compiler --version | Select-Object -First 1
        return $version.Trim()
    } catch {
        return "Error getting version"
    }
}

# Print system information
Write-SectionHeader "System Information"
Write-Host "OS: $([System.Environment]::OSVersion.VersionString)"
Write-Host "PowerShell Version: $($PSVersionTable.PSVersion)"
Write-Host "CMake Version: $(cmake --version | Select-String -Pattern 'cmake version')"

# Check available compilers
Write-SectionHeader "Available Compilers"
$compilers = @{
    "GCC" = "gcc"
    "G++" = "g++"
    "Clang" = "clang"
    "Clang++" = "clang++"
}

foreach ($compiler in $compilers.GetEnumerator()) {
    $info = Get-CompilerInfo $compiler.Value
    Write-Host "$($compiler.Key) ($($compiler.Value)): $info"
}

# Set up build directory
$buildDir = "$PSScriptRoot\build"
if (Test-Path $buildDir) {
    Write-Host "`nRemoving existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force $buildDir
}
New-Item -ItemType Directory -Path $buildDir | Out-Null

# Try different build configurations
$configs = @(
    @{
        Name = "MinGW Makefiles with GCC"
        Generator = "MinGW Makefiles"
        CCompiler = "gcc"
        CxxCompiler = "g++"
    },
    @{
        Name = "Ninja with Clang"
        Generator = "Ninja"
        CCompiler = "clang"
        CxxCompiler = "clang++"
    }
)

foreach ($config in $configs) {
    Write-SectionHeader "Trying build configuration: $($config.Name)"
    
    $cmakeArgs = @(
        "-G", $config.Generator,
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DBUILD_TESTS=ON",
        "-DCMAKE_C_COMPILER=$($config.CCompiler)",
        "-DCMAKE_CXX_COMPILER=$($config.CxxCompiler)"
    )
    
    if ($config.Generator -eq "Ninja") {
        $cmakeArgs += "-DCMAKE_MAKE_PROGRAM=ninja"
    }
    
    Write-Host "Configuring with: cmake $($cmakeArgs -join ' ')" -ForegroundColor DarkGray
    
    try {
        Push-Location $buildDir
        
        # Configure
        $process = Start-Process -FilePath "cmake" 
            -ArgumentList $cmakeArgs "$PSScriptRoot" 
            -NoNewWindow -Wait -PassThru -RedirectStandardOutput "$buildDir\cmake_configure.log" 
            -RedirectStandardError "$buildDir\cmake_configure_errors.log"
            
        if ($process.ExitCode -ne 0) {
            Write-Host "CMake configuration failed with exit code $($process.ExitCode)" -ForegroundColor Red
            Write-Host "Last 20 lines of CMake output:" -ForegroundColor Yellow
            Get-Content "$buildDir\cmake_configure.log" -Tail 20 | ForEach-Object { Write-Host "  $_" }
            Write-Host "`nCheck $buildDir\cmake_configure_errors.log for error details" -ForegroundColor Red
            continue
        }
        
        # Build
        Write-Host "Configuration successful. Building..." -ForegroundColor Green
        $buildArgs = "--build", ".", "--config", "Debug"
        $process = Start-Process -FilePath "cmake" 
            -ArgumentList $buildArgs 
            -NoNewWindow -Wait -PassThru -RedirectStandardOutput "$buildDir\cmake_build.log" 
            -RedirectStandardError "$buildDir\cmake_build_errors.log"
            
        if ($process.ExitCode -ne 0) {
            Write-Host "Build failed with exit code $($process.ExitCode)" -ForegroundColor Red
            Write-Host "Last 20 lines of build output:" -ForegroundColor Yellow
            Get-Content "$buildDir\cmake_build.log" -Tail 20 | ForEach-Object { Write-Host "  $_" }
            Write-Host "`nCheck $buildDir\cmake_build_errors.log for error details" -ForegroundColor Red
        } else {
            Write-Host "Build completed successfully!" -ForegroundColor Green
            
            # Run tests if build was successful
            Write-Host "`nRunning tests..." -ForegroundColor Cyan
            $testArgs = "--build", ".", "--target", "test", "--config", "Debug"
            Start-Process -FilePath "cmake" -ArgumentList $testArgs -NoNewWindow -Wait
            
            # If we got here, we can stop trying other configurations
            break
        }
    } catch {
        Write-Host "Error during build: $_" -ForegroundColor Red
    } finally {
        Pop-Location
    }
}

Write-Host "`nBuild diagnostics complete. Check the log files in the build directory for details." -ForegroundColor Cyan
