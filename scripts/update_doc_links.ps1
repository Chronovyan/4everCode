# Script to update internal documentation links
# Run this script from the project root

# Define the mapping of old paths to new paths
$pathMappings = @{
    # Old path = New path
    "getting_started.md" = "guides/getting-started/index.md"
    "language_specification.md" = "reference/language/specification.md"
    "ROADMAP.md" = "roadmap/index.md"
    "TUTORIAL_ROADMAP.md" = "roadmap/tutorials.md"
    "advanced_features_design.md" = "design/advanced-features.md"
    "lexer_parser_design.md" = "design/lexer-parser.md"
    "deja_vu_debugger_design.md" = "design/deja-vu-debugger.md"
    "chronicle_compiler_design.md" = "architecture/compiler.md"
    "chronicle_ir_design.md" = "architecture/intermediate-representation.md"
    "chronicle_bytecode_format.md" = "architecture/bytecode-format.md"
    "advanced_optimization_algorithms.md" = "guides/advanced/optimization-algorithms.md"
    "chronovyan_language_guide_act2_advanced_mechanics.md" = "guides/advanced/advanced-mechanics.md"
    "chronovyan_language_guide_act2_primer.md" = "guides/advanced/primer.md"
    "examples.md" = "guides/examples/index.md"
}

# Get all markdown files in the docs directory
$markdownFiles = Get-ChildItem -Path "docs" -Recurse -Filter "*.md" -File | 
    Where-Object { $_.FullName -notlike '*\node_modules\*' -and $_.FullName -notlike '*\_site\*' }

foreach ($file in $markdownFiles) {
    $content = Get-Content -Path $file.FullName -Raw
    $originalContent = $content
    $updated = $false
    
    # Update links for each mapping
    foreach ($mapping in $pathMappings.GetEnumerator()) {
        $oldPath = $mapping.Key
        $newPath = $mapping.Value
        
        # Handle various link formats
        $patterns = @(
            "\[([^\]]+)\]\(([^)]*)$($oldPath.Replace('.', '\.') -replace '\[', '\[' -replace '\]', '\]')(#[^)]*)?\)",
            "\[([^\]]+)\]\(([^)]*)$($oldPath.Replace('.', '\.') -replace '\[', '\[' -replace '\]', '\]')(\s+[^)]+)?\)"
        )
        
        foreach ($pattern in $patterns) {
            if ($content -match $pattern) {
                $content = $content -replace $pattern, "[`$1]($($newPath)`$3)"
                $updated = $true
            }
        }
    }
    
    # Save the file if it was updated
    if ($updated) {
        Set-Content -Path $file.FullName -Value $content -NoNewline
        Write-Host "Updated links in: $($file.FullName)"
    }
}

Write-Host "Link update complete!"
