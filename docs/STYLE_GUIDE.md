# 4ever Documentation Style Guide

This style guide provides standards for writing clear, consistent, and maintainable documentation for the 4ever programming language.

## Table of Contents

1. [Language and Tone](#language-and-tone)
2. [Formatting](#formatting)
3. [Code Examples](#code-examples)
4. [Terminology](#terminology)
5. [File Structure](#file-structure)
6. [Accessibility](#accessibility)
7. [Localization](#localization)

## Language and Tone

### Voice

- Use **second person** ("you") to address the reader
- Use **active voice** whenever possible
- Be **concise** but **thorough**
- Be **inclusive** and **respectful**

### Tense

- Use **present tense** for most documentation
- Use **future tense** for upcoming features or planned changes
- Use **past tense** only for historical context

### Persona

Write for:
- **Novices** who are new to 4ever
- **Intermediate** users who understand the basics
- **Experts** looking for detailed reference

## Formatting

### Headings

- Use **sentence case** for all headings
- Use **h1** (#) for page titles only
- Use **h2-h6** for section headings
- Leave **one blank line** before and after headings

### Text Formatting

- Use **bold** for UI elements and important terms
- Use *italics* for emphasis (sparingly)
- Use `code font` for code elements, file names, and commands
- Use **>** for notes, tips, and warnings

### Lists

- Use **dash (-)** for unordered lists
- Use **1., 2., 3.** for ordered lists
- Start each item with a **capital letter**
- End each item with **no punctuation** unless it's a complete sentence
- Indent nested lists with **4 spaces**

### Links

- Use **descriptive link text**
- Place links at the **end of sentences** when possible
- Use **relative paths** for internal links
- Add **title attributes** for clarity

## Code Examples

### Formatting

- Use **code fences** (```) for all code blocks
- Specify the **language** after the opening fence
- Keep lines under **80 characters** when possible
- Include **comments** to explain complex logic

### Best Practices

- Show **complete, runnable** examples when possible
- Include **expected output** in comments or separate blocks
- Use **meaningful variable names**
- Follow the **4ever style guide** for code

### Example

```4ever
// Good example with comments and output
fn calculate_fibonacci(n: Int): Int {
    if n <= 1 {
        return n  // Base case
    }
    return calculate_fibonacci(n - 1) + calculate_fibonacci(n - 2)
}

// Example usage
// fibonacci(5)  // Returns: 5
```

## Terminology

### Consistent Terms

| Preferred Term | Avoid |
|---------------|-------|
| function | method |
| parameter | argument |
| temporal variable | time variable |
| conformist variable | constant |
| rebel variable | dynamic variable |

### 4ever-Specific Terms

- **Aethel**: The energy resource in 4ever
- **Chronon**: The time unit in 4ever
- **Temporal Weaving**: The act of writing 4ever code
- **Timestream**: A sequence of related temporal states

## File Structure

### Naming Conventions

- Use **lowercase with hyphens** for file names
- Keep file names **short but descriptive**
- Use **README.md** for directory overviews
- Use **index.md** for section landing pages

### Front Matter

Each Markdown file should include YAML front matter:

```yaml
---
title: 'Page Title'
description: 'Brief description for search engines'
weight: 100  # Determines sort order in navigation
draft: false  # Set to true for work in progress
---
```

## Accessibility

### Images

- Add **alt text** for all images
- Use **descriptive file names**
- Include **captions** for complex diagrams
- Provide **text alternatives** for charts and graphs

### Headings

- Use **proper heading hierarchy** (h1 > h2 > h3, etc.)
- Don't skip heading levels
- Use **descriptive text** for section links

### Color and Contrast

- Ensure **sufficient contrast** between text and background
- Don't rely **only on color** to convey information
- Use **patterns or labels** in addition to color

## Localization

### Guidelines

- Avoid **idioms** and **cultural references**
- Use **simple, clear language**
- Keep **sentences short** and **paragraphs focused**
- Use **standard date and number formats**

### Non-English Content

- Place translations in **language-specific directories** (e.g., `es/`, `ja/`)
- Include **language metadata** in front matter
- Keep **file structure** consistent across languages

---

*Last updated: 2025-06-09*
