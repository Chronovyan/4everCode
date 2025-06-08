# Script to organize remaining documentation files
# Run this script from the project root

# Create additional directories if they don't exist
$directories = @(
    "docs/design",
    "docs/architecture",
    "docs/roadmap",
    "docs/guides/advanced",
    "docs/guides/examples"
)

foreach ($dir in $directories) {
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
    }
}

# Function to move files with logging
function Move-DocFile {
    param (
        [string]$source,
        [string]$destination
    )
    
    if (Test-Path $source) {
        $destDir = [System.IO.Path]::GetDirectoryName($destination)
        if (-not (Test-Path $destDir)) {
            New-Item -ItemType Directory -Path $destDir -Force | Out-Null
        }
        Move-Item -Path $source -Destination $destination -Force
        Write-Host "Moved: $source -> $destination"
    }
}

# Move design documents
Move-DocFile "docs/advanced_features_design.md" "docs/design/advanced-features.md"
Move-DocFile "docs/lexer_parser_design.md" "docs/design/lexer-parser.md"
Move-DocFile "docs/deja_vu_debugger_design.md" "docs/design/deja-vu-debugger.md"

# Move architecture documents
Move-DocFile "docs/chronicle_compiler_design.md" "docs/architecture/compiler.md"
Move-DocFile "docs/chronicle_ir_design.md" "docs/architecture/intermediate-representation.md"
Move-DocFile "docs/chronicle_bytecode_format.md" "docs/architecture/bytecode-format.md"

# Move roadmap documents
Move-DocFile "docs/ROADMAP.md" "docs/roadmap/index.md"
Move-DocFile "docs/compiler_implementation_roadmap.md" "docs/roadmap/compiler-implementation.md"
Move-DocFile "docs/TUTORIAL_ROADMAP.md" "docs/roadmap/tutorials.md"

# Move advanced guides
Move-DocFile "docs/advanced_optimization_algorithms.md" "docs/guides/advanced/optimization-algorithms.md"
Move-DocFile "docs/chronovyan_language_guide_act2_advanced_mechanics.md" "docs/guides/advanced/advanced-mechanics.md"
Move-DocFile "docs/chronovyan_language_guide_act2_primer.md" "docs/guides/advanced/primer.md"

# Move example guides
Move-DocFile "docs/examples.md" "docs/guides/examples/index.md"

# Create a structure documentation file
@"
# Chronovyan Documentation Structure

This document outlines the organization of the Chronovyan documentation to help contributors maintain a consistent structure.

## Core Documentation

- `guides/` - Tutorials and how-to guides
  - `getting-started/` - Introduction to Chronovyan
  - `advanced/` - Advanced topics and deep dives
  - `examples/` - Example-based guides and code samples

- `reference/` - API and language reference
  - `language/` - Language specification and syntax
  - `stdlib/` - Standard library documentation

- `development/` - Development resources
  - `building/` - Build system documentation
  - `contributing/` - Contribution guidelines

- `design/` - Design documents and architecture decisions
- `architecture/` - System architecture documentation
- `roadmap/` - Project roadmap and future plans
- `community/` - Community resources and guidelines

## File Naming Conventions

- Use kebab-case for all filenames (e.g., `getting-started.md`)
- Use `index.md` for the main entry point of a directory
- Keep filenames short but descriptive
- Use lowercase for all filenames

## Documentation Standards

- Use Markdown (`.md`) for all documentation
- Follow the [Google Developer Documentation Style Guide](https://developers.google.com/style)
- Include front matter for metadata (title, description, etc.)
- Use relative links for internal references

## Adding New Documentation

1. Determine the appropriate directory for your content
2. Create a new Markdown file with a descriptive name
3. Add front matter with title and description
4. Write content following the style guide
5. Add links from relevant index pages
6. Submit a pull request for review
"@ | Out-File -FilePath "docs/STRUCTURE.md" -Encoding utf8

Write-Host "Documentation organization complete!"
