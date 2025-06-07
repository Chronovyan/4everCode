# PowerShell script to compile and run Chronovyan (.cvy) files

param(
    [Parameter(Mandatory=$true)]
    [string]$InputFile,
    [switch]$VerboseOutput
)

# Define paths to executables
$compiler_path = Join-Path -Path (Get-Location) -ChildPath "build\bin\chronovyan_compiler.exe"

# Attempt to find the interpreter executable
$interpreter_path = $null
$possible_interpreter_paths = @(
    "build\chronovyan_run.exe",
    "build\bin\chronovyan_run.exe",
    "build\src\interpreter\chronovyan_run.exe",
    "build\interpreter\chronovyan_run.exe"
)

foreach ($path in $possible_interpreter_paths) {
    $full_path = Join-Path -Path (Get-Location) -ChildPath $path
    if (Test-Path $full_path) {
        $interpreter_path = $full_path
        break
    }
}

# Verify compiler exists
if (-not (Test-Path $compiler_path)) {
    Write-Host "Error: Compiler not found at $compiler_path" -ForegroundColor Red
    Write-Host "Make sure you've built the Chronovyan compiler." -ForegroundColor Yellow
    exit 1
}

# Verify input file exists
if (-not (Test-Path $InputFile)) {
    Write-Host "Error: Input file not found: $InputFile" -ForegroundColor Red
    exit 1
}

# Set up output file path (same name with .chron extension)
$outputFile = [System.IO.Path]::ChangeExtension($InputFile, ".chron")

# Step 1: Compile the .cvy file to bytecode
Write-Host "Compiling $InputFile to $outputFile..." -ForegroundColor Cyan
if ($VerboseOutput) {
    & $compiler_path -v $InputFile -o $outputFile
} else {
    & $compiler_path $InputFile -o $outputFile
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed with error code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Compilation successful!" -ForegroundColor Green

# Step 2: Run the bytecode file
Write-Host "Running the compiled bytecode..." -ForegroundColor Cyan

# Check if interpreter exists
if ($null -ne $interpreter_path) {
    Write-Host "Using interpreter at: $interpreter_path" -ForegroundColor Green
    if ($VerboseOutput) {
        & $interpreter_path -v $outputFile
    } else {
        & $interpreter_path $outputFile
    }
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Execution failed with error code $LASTEXITCODE" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    
    Write-Host "Execution completed successfully!" -ForegroundColor Green
} else {
    Write-Host "Warning: Interpreter executable not found in any of these locations:" -ForegroundColor Yellow
    foreach ($path in $possible_interpreter_paths) {
        Write-Host "  - $path" -ForegroundColor Yellow
    }
    Write-Host "Skipping execution step. The bytecode file has been generated at: $outputFile" -ForegroundColor Yellow
}

# Display success message
Write-Host "Process completed successfully!" -ForegroundColor Green 