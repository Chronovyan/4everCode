<#
.SYNOPSIS
    Build script for Chronovyan
.DESCRIPTION
    This script provides a consistent way to build Chronovyan across different platforms.
    It handles configuration, building, and testing in a single command.
.PARAMETER BuildType
    The build type (Debug, Release, RelWithDebInfo, MinSizeRel). Default is Release.
.PARAMETER BuildDir
    The build directory. Default is 'build' in the project root.
.PARAMETER Generator
    The CMake generator to use. Auto-detected if not specified.
.PARAMETER Target
    The build target (e.g., 'all', 'install', 'test'). Default is 'all'.
.PARAMETER Clean
    Clean the build directory before building.
.PARAMETER EnableTests
    Enable building tests. Default is true.
.PARAMETER EnableExamples
    Enable building examples. Default is true.
.PARAMETER EnableDocs
    Enable building documentation. Default is false.
.PARAMETER UseVcpkg
    Use vcpkg for dependency management. Auto-detected if not specified.
.EXAMPLE
    .\scripts\build.ps1 -BuildType Release -Clean
    # Cleans the build directory and performs a clean Release build
.EXAMPLE
    .\scripts\build.ps1 -BuildType Debug -Target test
    # Builds in Debug mode and runs tests
#>

param(
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$BuildType = 'Release',
    
    [string]$BuildDir = "$PSScriptRoot/../build",
    
    [string]$Generator = "",
    
    [string]$Target = "all",
    
    [switch]$Clean,
    
    [bool]$EnableTests = $true,
    
    [bool]$EnableExamples = $true,
    
    [bool]$EnableDocs = $false,
    
    [string]$UseVcpkg = "AUTO"
)

# Convert to absolute path
$BuildDir = [System.IO.Path]::GetFullPath($BuildDir)
$ProjectRoot = [System.IO.Path]::GetFullPath("$PSScriptRoot/..")

# Detect OS
$IsMacOS = $false
$IsLinux = $false
$IsWindows = $true  # Default to Windows for this script

# Try to detect non-Windows platforms if needed
if ($PSVersionTable.PSVersion.Major -gt 6) {
    # PowerShell Core 7+ has built-in variables
    $IsMacOS = $IsMacOS
    $IsLinux = $IsLinux
    $IsWindows = -not ($IsMacOS -or $IsLinux)
} else {
    # For Windows PowerShell, assume Windows unless we detect otherwise
    try {
        $osInfo = Get-CimInstance -ClassName Win32_OperatingSystem -ErrorAction Stop
        $IsWindows = $true
    } catch {
        # If we can't get OS info, assume Windows
        $IsWindows = $true
    }
}

# Set default generator if not specified
if ([string]::IsNullOrEmpty($Generator)) {
    if ($IsWindows) {
        # Default to Visual Studio generator on Windows if available
        if (Get-Command "msbuild" -ErrorAction SilentlyContinue) {
            $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
            if (Test-Path $vsWhere) {
                $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
                if ($vsPath) {
                    $Generator = "Visual Studio 17 2022"
                    $cmakePlatform = "-A x64"
                }
            }
        }
        
        # Fall back to MinGW if VS not found
        if (-not $Generator) {
            $Generator = "MinGW Makefiles"
            # Make sure MinGW is in PATH
            $mingwPath = "C:\MinGW\bin"
            if (Test-Path $mingwPath) {
                $env:Path = "$mingwPath;$env:Path"
            }
        }
    } elseif ($IsMacOS) {
        $Generator = "Unix Makefiles"
    } elseif ($IsLinux) {
        $Generator = "Unix Makefiles"
    }
}

# Clean build directory if requested
if ($Clean -and (Test-Path $BuildDir)) {
    Write-Host "Cleaning build directory: $BuildDir" -ForegroundColor Yellow
    Remove-Item -Recurse -Force $BuildDir -ErrorAction SilentlyContinue
}

# Create build directory if it doesn't exist
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Common CMake arguments
$CMakeArgs = @(
    "-S", "$PSScriptRoot/..",
    "-B", $BuildDir,
    "-G", $Generator,
    "-DCMAKE_BUILD_TYPE=$BuildType",
    "-DBUILD_TESTING=$($EnableTests.ToString())",
    "-DBUILD_EXAMPLES=$($EnableExamples.ToString())",
    "-DBUILD_DOCS=$($EnableDocs.ToString())",
    "-DUSE_VCPKG=$UseVcpkg"
)

# Add platform-specific arguments
if ($IsWindows -and $Generator -like "Visual Studio*") {
    $CMakeArgs += @("-A", "x64")
}

# Add vcpkg toolchain if available
$vcpkgRoot = $env:VCPKG_ROOT
if ([string]::IsNullOrEmpty($vcpkgRoot)) {
    $vcpkgRoot = "$env:USERPROFILE\vcpkg"
}

$vcpkgToolchain = "$vcpkgRoot/scripts/buildsystems/vcpkg.cmake"
if (($UseVcpkg -eq "AUTO" -or $UseVcpkg -eq "ON") -and (Test-Path $vcpkgToolchain)) {
    $CMakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$vcpkgToolchain"
}

# Configure step
Write-Host "Configuring Chronovyan with CMake..." -ForegroundColor Cyan
Set-Location $ProjectRoot
$cmakeCmd = "cmake", "-B", "$BuildDir" + $CMakeArgs + "$ProjectRoot"
& cmake -B $BuildDir $CMakeArgs $ProjectRoot

if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}

# Build step
Write-Host "Building Chronovyan..." -ForegroundColor Cyan
$buildArgs = @("--build", "$BuildDir", "--config", $BuildType)

# Add parallel build option if not using Visual Studio
if (-not $IsWindows -or $Generator -eq "Ninja") {
    $cpuCount = (Get-CimInstance -ClassName Win32_ComputerSystem).NumberOfLogicalProcessors
    if ($IsMacOS -or $IsLinux) {
        $cpuCount = sysctl -n hw.ncpu | Out-String
    }
    $buildArgs += "--", "-j$cpuCount"
}

& cmake $buildArgs

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed with exit code $LASTEXITCODE"
    exit $LASTEXITCODE
}

# Run tests if requested
if ($Target -eq "test" -and $EnableTests) {
    Write-Host "Running tests..." -ForegroundColor Cyan
    Set-Location $BuildDir
    $testArgs = @("--output-on-failure")
    if ($IsWindows) {
        $testArgs += "-C", $BuildType
    }
    & ctest $testArgs
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Tests failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}

# Install if requested
if ($Target -eq "install") {
    Write-Host "Installing Chronovyan..." -ForegroundColor Cyan
    Set-Location $BuildDir
    & cmake --build . --config $BuildType --target install
    
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Installation failed with exit code $LASTEXITCODE"
        exit $LASTEXITCODE
    }
}

Write-Host "Build completed successfully!" -ForegroundColor Green
