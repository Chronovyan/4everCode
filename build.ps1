<#
.SYNOPSIS
    Simple wrapper for the Chronovyan build system.
.DESCRIPTION
    This script forwards all arguments to the main build script in the scripts directory.
    It's a convenience wrapper to make building from the project root easier.
.EXAMPLE
    .\build.ps1 -BuildType Release -Clean
    # Forwards to scripts/build.ps1 with the same arguments
#>

param(
    [Parameter(ValueFromRemainingArguments=$true)]
    [string[]]$PassthroughArgs
)

$scriptPath = Join-Path $PSScriptRoot "scripts\build.ps1"

if (-not (Test-Path $scriptPath)) {
    Write-Error "Could not find build script at $scriptPath"
    exit 1
}

# Forward all arguments to the main build script
& $scriptPath @PassthroughArgs

exit $LASTEXITCODE
