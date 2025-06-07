# PowerShell Script to Clean Up Chronovyan Build Artifacts

Write-Host "Starting Chronovyan Build Artifact Cleanup..."

$ErrorActionPreference = "SilentlyContinue"

# Paths to clean
$buildDir = ".\build"
$testingTempDir = ".\Testing\Temporary"
$rootDir = ".\"

# Remove contents of build directory
if (Test-Path $buildDir) {
    Write-Host "Cleaning build directory: $buildDir"
    Get-ChildItem -Path $buildDir -Recurse | Remove-Item -Force -Recurse
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Error cleaning $buildDir"
    } else {
        Write-Host "Successfully cleaned $buildDir"
    }
} else {
    Write-Host "Build directory not found: $buildDir"
}

# Remove contents of Testing/Temporary directory
if (Test-Path $testingTempDir) {
    Write-Host "Cleaning testing temporary directory: $testingTempDir"
    Get-ChildItem -Path $testingTempDir -Recurse | Remove-Item -Force -Recurse
    if ($LASTEXITCODE -ne 0) {
        Write-Warning "Error cleaning $testingTempDir"
    } else {
        Write-Host "Successfully cleaned $testingTempDir"
    }
} else {
    Write-Host "Testing temporary directory not found: $testingTempDir"
}

# Remove .exe files from root directory (be cautious with this)
Write-Host "Searching for and removing .exe files from the root directory: $rootDir"
Get-ChildItem -Path $rootDir -Filter "*.exe" -File | ForEach-Object {
    # Add specific executables to exclude if necessary, e.g.:
    # if ($_.Name -ne "MyEssentialApp.exe") {
    #    Write-Host "Removing $($_.FullName)"
    #    Remove-Item -Path $_.FullName -Force
    #    if ($LASTEXITCODE -ne 0) {
    #        Write-Warning "Error removing $($_.FullName)"
    #    }
    # }

    # For now, let's list them and ask for confirmation for each, or make it more specific.
    # To be safe, this script will just list them for now.
    # To enable deletion, uncomment the Remove-Item line and adjust logic.
    Write-Host "Found executable: $($_.FullName) (manual deletion recommended or make this script more specific)"
} 
# Example of direct removal (use with caution):
# Get-ChildItem -Path $rootDir -Filter "rewind_flow_test.exe" -File | Remove-Item -Force
# Get-ChildItem -Path $rootDir -Filter "resource_viz_demo.exe" -File | Remove-Item -Force
# Get-ChildItem -Path $rootDir -Filter "super_minimal_test.exe" -File | Remove-Item -Force
Write-Host "Manually review and delete specific executables like rewind_flow_test.exe, resource_viz_demo.exe, etc., from the root if they are build artifacts."


Write-Host "Build Artifact Cleanup Finished." 