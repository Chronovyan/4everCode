# Chronovyan Documentation Style Guide

This style guide provides guidelines for writing clear, consistent, and maintainable documentation for the Chronovyan project.

## Table of Contents

- [General Principles](#general-principles)
- [Language and Tone](#language-and-tone)
- [Document Structure](#document-structure)
- [Formatting](#formatting)
- [Code Examples](#code-examples)
- [Images and Diagrams](#images-and-diagrams)
- [Accessibility](#accessibility)
- [Localization](#localization)
- [Tools and Linting](#tools-and-linting)

## General Principles

1. **Clarity**: Write clearly and concisely. Prefer simple, direct language.
2. **Consistency**: Follow the patterns and conventions used throughout the documentation.
3. **Accuracy**: Ensure all information is technically accurate and up-to-date.
4. **Completeness**: Provide all necessary information without overwhelming the reader.
5. **Progressive Disclosure**: Start with basic concepts and gradually introduce more complex topics.

## Language and Tone

- **Tone**: Friendly, professional, and approachable. Avoid being overly formal or casual.
- **Voice**: Use active voice where possible. Passive voice is acceptable when the action is more important than the actor.
- **Person**: Use second person ("you") for user documentation. Use first person plural ("we") for the project's perspective.
- **Tense**: Use present tense for general documentation. Use future tense only for features not yet implemented.
- **Mood**: Use imperative mood for instructions (e.g., "Run the following command").
- **Contractions**: Use contractions to sound more natural (e.g., "don't" instead of "do not").
- **Jargon**: Define technical terms on first use. Avoid unnecessary jargon.

## Document Structure

### Document Organization

1. **Title**: Clear and descriptive, using title case
2. **Introduction**: Brief overview of the document's purpose and scope
3. **Table of Contents**: For documents longer than one page
4. **Sections**: Organized logically with clear headings
5. **Examples**: Practical examples to illustrate concepts
6. **See Also**: Links to related documentation
7. **Troubleshooting**: Common issues and solutions (if applicable)

### Headings

- Use ATX-style headings with `#`
- Use sentence case for headings (e.g., "Installation guide" not "Installation Guide")
- Avoid using more than three levels of nesting
- Leave one blank line before and after headings

Example:

```markdown
# Document title

## Section

### Subsection
```

### Lists

- Use ordered lists for sequential steps
- Use unordered lists for non-sequential items
- Use consistent bullet points (`-` or `*`)
- Leave a blank line before and after lists
- Use hanging indents for multi-line list items

Example:

```markdown
To install Chronovyan:

1. Download the installer
2. Run the installation script:
   ```bash
   ./install.sh
   ```
3. Verify the installation

Features include:

- High performance
- Easy to use
- Cross-platform support
```

## Formatting

### Text Formatting

- Use `**bold**` for UI elements, file names, and emphasis
- Use `code font` for commands, functions, variables, and code elements
- Use *italics* for document titles and emphasis
- Use `>` for blockquotes
- Use `---` for horizontal rules between sections

### Links

- Use descriptive link text that indicates the destination
- Place links at the end of the relevant paragraph when possible
- Use reference-style links for better readability in source

Example:

```markdown
For more information, see the [installation guide][install].

[install]: installation-guide.md
```

### Tables

Use tables for comparing options, parameters, or other tabular data.

```markdown
| Option | Description | Default |
|--------|-------------|---------|
| `-v`   | Verbose output | `false` |
| `-f`   | Force operation | `false` |
```

## Code Examples

### General Guidelines

- Include complete, runnable examples
- Show both input and expected output
- Use comments to explain non-obvious parts
- Keep examples focused on the concept being demonstrated
- Use realistic but minimal examples

### Code Blocks

Use fenced code blocks with language specification for syntax highlighting:

````markdown
```rust
// This is a Rust code example
fn main() {
    println!("Hello, Chronovyan!");
}
```

```bash
# This is a shell command
$ cargo build
```

```toml
# This is a TOML configuration
[package]
name = "my_project"
version = "0.1.0"
```
````

### Command Line Examples

- Prefix commands with `$ ` for regular users and `# ` for admin/root
- Include both the command and its output
- Use `...` to indicate omitted output

Example:

```bash
$ chrono --version
chrono 0.1.0
$ chrono run example.chrono
Hello, Chronovyan!
```

## Images and Diagrams

- Place images in the `docs/assets/images/` directory
- Use descriptive file names (e.g., `architecture-overview.svg`)
- Include alt text for accessibility
- Use vector formats (SVG) when possible
- Keep images simple and focused

Example:

```markdown
![Chronovyan Architecture](assets/images/architecture-overview.svg)
```

## Accessibility

- Use semantic HTML when possible (e.g., `**Note:**` instead of `**Note**`)
- Provide alternative text for images
- Use descriptive link text
- Ensure sufficient color contrast
- Use heading levels in order (don't skip levels)

## Localization

- Use simple, clear language
- Avoid idioms and cultural references
- Be consistent with terminology
- Leave room for text expansion in UI elements (30-50% for English to other languages)

## Tools and Linting

### Pre-commit Hooks

We use pre-commit hooks to enforce style guidelines. The following hooks are configured:

- `trailing-whitespace`: Removes trailing whitespace
- `end-of-file-fixer`: Ensures files end with a newline
- `check-yaml`: Validates YAML files
- `markdownlint`: Lints Markdown files
- `validate-docs`: Runs our custom documentation validation

### Linting Rules

- Line length: 100 characters (80 for code blocks)
- Use spaces, not tabs (2 spaces for Markdown, 4 for YAML)
- End files with a single newline character
- Use UTF-8 encoding

### Spell Checking

- Use a spell checker in your editor
- Add technical terms to the project dictionary
- Use US English spelling (e.g., "color" not "colour")

## Documentation Review Checklist

Before submitting documentation changes, verify:

- [ ] Content is accurate and up-to-date
- [ ] Examples work as described
- [ ] Links are not broken
- [ ] Images have alt text
- [ ] Code is properly formatted
- [ ] Style guidelines are followed
- [ ] No spelling or grammar errors

## Resources

- [Google Developer Documentation Style Guide](https://developers.google.com/style)
- [Microsoft Writing Style Guide](https://docs.microsoft.com/en-us/style-guide/welcome/)
- [GitHub Markdown Guide](https://guides.github.com/features/mastering-markdown/)
- [Write the Docs](https://www.writethedocs.org/)

---

*Last updated: June 2025*
