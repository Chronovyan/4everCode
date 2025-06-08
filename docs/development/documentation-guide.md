# Documentation Contribution Guide

Thank you for your interest in contributing to Chronovyan's documentation! This guide will help you understand how to contribute effectively to our documentation.

## Table of Contents

- [Documentation Structure](#documentation-structure)
- [Writing Guidelines](#writing-guidelines)
- [Style Guide](#style-guide)
- [Markdown Formatting](#markdown-formatting)
- [Code Examples](#code-examples)
- [Building Documentation Locally](#building-documentation-locally)
- [Previewing Changes](#previewing-changes)
- [Documentation Review Process](#documentation-review-process)
- [Common Tasks](#common-tasks)
- [Getting Help](#getting-help)
- [Development Tools](#development-tools)

## Documentation Structure

Chronovyan's documentation is organized into several main sections:

```
docs/
├── guides/           # Tutorials and how-to guides
│   ├── getting-started/  # Beginner's guide to Chronovyan
│   ├── examples/         # Example applications and code snippets
│   └── advanced/         # Advanced topics and concepts
├── reference/        # Technical references
│   ├── language/     # Language specification
│   └── stdlib/       # Standard library documentation
├── development/      # Development guides
│   ├── building/     # Building from source
│   └── contributing/ # Contribution guidelines
├── design/           # Design documents and architecture
│   ├── architecture/ # System architecture
│   └── decisions/    # Architecture Decision Records (ADRs)
└── community/        # Community resources
    ├── code-of-conduct.md
    └── governance.md
```

## Writing Guidelines

### General Principles

1. **Clarity**: Write clearly and concisely
2. **Accuracy**: Ensure all information is accurate and up-to-date
3. **Consistency**: Follow the style guide and existing patterns
4. **Accessibility**: Make content accessible to all users
5. **Modularity**: Keep documents focused and single-purpose

### Document Types

1. **Concepts**: Explain what things are and why they matter
2. **Tasks**: Provide step-by-step instructions
3. **References**: Contain technical details and specifications
4. **Tutorials**: Teach through examples and exercises

## Style Guide

### Language

- Use **second person** ("you") for user documentation
- Use **present tense** for general documentation
- Use **active voice** where possible
- Be **concise** and **direct**
- Avoid **jargon** and **acronyms** without explanation
- Use **sentence case** for headings

### Formatting

- Use **bold** for UI elements and important terms on first mention
- Use `code font` for:
  - File and directory names
  - Commands and code
  - Function and variable names
  - Configuration values

## Markdown Formatting

Chronovyan's documentation uses GitHub Flavored Markdown with some extensions. Here are some common patterns:

### Headings

```markdown
# H1 - Document Title
## H2 - Main Sections
### H3 - Subsections
#### H4 - Sub-subsections (use sparingly)
```

### Links

```markdown
[Link Text](relative/path/or/url)
[Reference-style link][reference]

[reference]: https://example.com
```

### Code Blocks

````markdown
```rust
// Rust code with syntax highlighting
fn main() {
    println!("Hello, Chronovyan!");
}
```

```bash
# Shell commands
$ cargo build
```

```toml
# Configuration example
[package]
name = "my_project"
version = "0.1.0"
```
````

### Admonitions

```markdown
!!! note "Optional Title"
    This is a note with a title.

!!! warning
    This is a warning without a title.

!!! danger "Danger!"
    This is a danger admonition.
```

## Code Examples

1. **Self-contained**: Examples should be complete and runnable
2. **Tested**: All code examples should be tested
3. **Commented**: Add comments to explain non-obvious parts
4. **Consistent**: Follow the project's coding standards

### Example Format

````markdown
Here's how to create a simple Chronovyan program:

```rust
// Import the standard library
use chronovyan::prelude::*;

fn main() {
    // Print a greeting
    println!("Hello, Chronovyan!");
    
    // Get the current time
    let now = time::now();
    println!("Current time: {}", now);
}
```

To run this program:

```bash
$ cargo run --example hello_world
```
````

## Development Tools

### Core Tools

- **MkDocs**: Static site generator for building the documentation
- **Material for MkDocs**: Theme for MkDocs with modern UI components
- **Pre-commit**: Framework for managing and maintaining multi-language pre-commit hooks
- **markdownlint**: Tool to check Markdown files and flag style issues

### Documentation Scripts

We provide several scripts to help with documentation development:

1. **Setup Script** (`scripts/setup_docs.sh` or `scripts/setup_docs.ps1`):
   - Sets up the Python virtual environment
   - Installs all required dependencies
   - Configures pre-commit hooks

2. **Validation Script** (`scripts/validate_docs.sh` or `scripts/validate_docs.ps1`):
   - Validates the documentation structure
   - Checks for broken links and anchors
   - Ensures all required files and directories exist

3. **Documentation Generator** (`scripts/new_doc.sh` or `scripts/new_doc.bat`):
   - Creates new documentation pages with proper front matter
   - Updates the navigation in `mkdocs.yml`
   - Ensures consistent structure across all documentation

### Recommended Editor Extensions

- **Visual Studio Code**:
  - Markdown All in One
  - markdownlint
  - Prettier - Code formatter
  - Code Spell Checker
  - YAML
  - reStructuredText

- **PyCharm/IntelliJ IDEA**:
  - Markdown Navigator
  - Markdown
  - YAML/Ansible support
  - reStructuredText

## Building Documentation Locally

### Prerequisites

- Python 3.8+
- pip
- mkdocs

### Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/Chronovyan/Chronovyan.git
   cd Chronovyan
   ```

2. Run the setup script for your platform:
   - On Windows (PowerShell):
     ```powershell
     .\scripts\setup_docs.ps1
     ```
   - On Unix-like systems (Linux/macOS):
     ```bash
     ./scripts/setup_docs.sh
     ```

3. Activate the virtual environment:
   - On Windows (PowerShell):
     ```powershell
     .\.venv\Scripts\Activate.ps1
     ```
   - On Unix-like systems:
     ```bash
     source .venv/bin/activate
     ```

4. Build the documentation:
   ```bash
   mkdocs build
   ```

5. Serve the documentation locally:
   ```bash
   mkdocs serve
   ```
   Then open http://127.0.0.1:8000 in your browser.

## Previewing Changes

To preview your changes while you work:

```bash
mkdocs serve
```

Then open http://127.0.0.1:8000 in your browser.

## Documentation Review Process

1. **Create a Branch**:
   ```bash
   git checkout -b docs/your-topic
   ```

2. **Make Changes**:
   - Follow the style guide
   - Keep commits focused and atomic
   - Write clear commit messages

3. **Test Your Changes**:
   ```bash
   mkdocs build
   mkdocs serve
   ```

4. **Create a Pull Request**:
   - Reference any related issues
   - Include a summary of changes
   - Request reviews from documentation maintainers

5. **Address Feedback**:
   - Respond to review comments
   - Make necessary updates
   - Update documentation tests if needed

## Common Tasks

### Adding a New Document

1. Create a new Markdown file in the appropriate directory
2. Add front matter at the top:
   ```yaml
   ---
   title: "Document Title"
   description: "Brief description of the document"
   weight: 10  # Used for ordering in navigation
   ---
   ```
3. Add content following the style guide
4. Update the navigation in `mkdocs.yml` if needed

### Updating Navigation

Edit the `nav` section in `mkdocs.yml` to add, remove, or reorder documents.

## Getting Help

If you need help with documentation:

1. Check the [issue tracker](https://github.com/Chronovyan/Chronovyan/issues) for similar questions
2. Join our [Discord server](https://discord.gg/chronovyan)
3. Open a new issue if your question hasn't been answered

## Thank You!

Your contributions help make Chronovyan better for everyone. We appreciate your time and effort!
