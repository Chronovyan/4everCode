# PowerShell Script to Identify Potential Dead Code / TODOs in Chronovyan Project

Write-Host "Starting Potential Dead Code / TODO Identification..."

$rootDir = ".\"
$srcDir = ".\src"
$includeDir = ".\include"
$testsDir = ".\tests" # Add other relevant directories if needed

$todoPattern = "// TODO"
# Regex for C-style block comments /* ... */. PowerShell uses -match with regex.
$blockCommentPattern = "/\*.*?\*/"
# Regex for single-line C++ comment blocks (e.g., // private: // int x;)
$multiLineSingleCommentPattern = "(^\s*//.*$\n)+^\s*//.*$" # This is more complex and might need refinement for accuracy

Write-Host "Searching for '$todoPattern' comments..."
# Search in .cpp and .h files in src and include directories
Get-ChildItem -Path $srcDir, $includeDir -Recurse -Include *.cpp, *.h | Select-String -Pattern $todoPattern -List | ForEach-Object {
    Write-Host "TODO Found: $($_.Path):$($_.LineNumber) - $($_.Line)"
}

Write-Host "Searching for C-style block comments (/* ... */) in source files..."
Write-Host "(Note: This may include license headers or intentional documentation blocks. Manual review is essential.)"
Get-ChildItem -Path $srcDir, $includeDir -Recurse -Include *.cpp, *.h | ForEach-Object {
    $content = Get-Content $_.FullName -Raw
    $matches = $content | Select-String -Pattern $blockCommentPattern -AllMatches
    if ($matches) {
        Write-Host "Potential large commented block in: $($_.FullName)"
        foreach ($match in $matches.Matches) {
            # To avoid extremely long outputs, let's show a snippet
            $snippet = $match.Value.Substring(0, [System.Math]::Min($match.Value.Length, 100)) + "..."
            Write-Host "    Snippet: $snippet"
        }
    }
}

# Add more specific searches if needed, e.g., for commented-out function definitions or class declarations.
# This requires more sophisticated regex and understanding of the C++ syntax.
# Example (very basic, might need refinement):
# $commentedFunctionPattern = "//\s*(void|int|float|double|bool|std::string|auto|const\s+\w+)\s+\w+\s*\(.*\)\s*({|;)"
# Get-ChildItem -Path $srcDir, $includeDir -Recurse -Include *.cpp, *.h | Select-String -Pattern $commentedFunctionPattern -List | ForEach-Object {
#    Write-Host "Potential Commented-Out Function: $($_.Path):$($_.LineNumber) - $($_.Line)"
# }

Write-Host "Identification of Potential Dead Code / TODOs Finished."
Write-Host "Please manually review the findings. This script only identifies potential areas." 