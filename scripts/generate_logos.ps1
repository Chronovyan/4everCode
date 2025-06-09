# Generate PNG versions of SVG logos at standard sizes

# Check for required tools
if (-not (Get-Command magick -ErrorAction SilentlyContinue)) {
    Write-Error "ImageMagick is required. Please install it first."
    exit 1
}

# Create output directory if it doesn't exist
$outputDir = "docs/assets/branding/png"
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

# Standard sizes (width in pixels)
$sizes = @(16, 32, 64, 128, 256, 512, 1024)

# Process each SVG file
Get-ChildItem -Path "docs/assets/branding/svg" -Filter "*.svg" | ForEach-Object {
    $baseName = [System.IO.Path]::GetFileNameWithoutExtension($_.Name)
    
    foreach ($size in $sizes) {
        $outputFile = "$outputDir/${baseName}-${size}x${size}.png"
        
        Write-Host "Generating $outputFile..."
        magick convert -background none -resize ${size}x${size} $_.FullName $outputFile
        
        if ($LASTEXITCODE -ne 0) {
            Write-Warning "Failed to generate $outputFile"
        }
    }
    
    # Also generate a high-DPI version (2x) for the largest size
    $outputFile2x = "$outputDir/${baseName}-2048x2048.png"
    Write-Host "Generating $outputFile2x..."
    magick convert -background none -resize 2048x2048 $_.FullName $outputFile2x
}

Write-Host "\nPNG logo generation complete!" -ForegroundColor Green
Write-Host "Generated files are in: $((Resolve-Path $outputDir).Path)"
