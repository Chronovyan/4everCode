# Contributing to 4ever Documentation

Thank you for your interest in contributing to the 4ever programming language documentation! This guide will help you get started with contributing to our documentation.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Documentation Structure](#documentation-structure)
3. [Style Guide](#style-guide)
4. [Writing Guidelines](#writing-guidelines)
5. [Local Development](#local-development)
6. [Submitting Changes](#submitting-changes)
7. [Code of Conduct](#code-of-conduct)

## Getting Started

1. **Fork** the repository on GitHub
2. **Clone** your fork locally
3. Create a new **branch** for your changes:
   ```bash
   git checkout -b docs/your-topic-branch
   ```
4. Make your changes following our style guide
5. **Test** your changes locally
6. **Commit** your changes with a clear message
7. **Push** to your fork and open a **pull request**

## Documentation Structure

The documentation is organized as follows:

```
docs/
├── core/               # Core language concepts and reference
├── guides/             # Tutorials and how-to guides
├── advanced/           # Advanced topics and deep dives
├── reference/          # API and language reference
├── examples/           # Code examples
├── community/          # Community resources
└── assets/             # Images and other static files
```

## Style Guide

### General

- Use **Markdown** for all documentation
- Follow the [Google Developer Documentation Style Guide](https://developers.google.com/style)
- Use **sentence case** for headings
- Write in **clear, concise** language
- Use **active voice**
- Keep paragraphs short (3-4 sentences max)
- Use **lists** for multiple points

### Code Examples

- Include **working, complete** examples
- Add **comments** to explain complex code
- Use **syntax highlighting** for code blocks
- Include **expected output** when relevant

### Images and Diagrams

- Use **SVG** format for diagrams when possible
- Keep images **small and optimized**
- Add **alt text** for accessibility
- Place images in the `assets/images/` directory

### Front Matter

Each Markdown file should start with YAML front matter:

```yaml
---
title: 'Page Title'
description: 'Brief description for search engines'
weight: 100  # Lower numbers appear first in navigation
draft: false  # Set to true for work in progress
---
```

## Writing Guidelines

### Be Consistent

- Use **consistent terminology**
- Follow the **same patterns** for similar content
- Use **parallel structure** in lists and headings

### Be Clear

- Define **technical terms** on first use
- Use **simple language** when possible
- Break down **complex concepts**
- Include **practical examples**

### Be Complete

- Include **prerequisites** at the start of guides
- Link to **related content**
- Provide **next steps** at the end of guides

## Local Development

To preview your changes locally:

1. Install MkDocs and required plugins:
   ```bash
   pip install mkdocs mkdocs-material mkdocs-git-revision-date-localized-plugin
   ```

2. Run the development server:
   ```bash
   mkdocs serve
   ```

3. Open http://localhost:8000 in your browser

## Submitting Changes

1. **Test** your changes locally
2. **Rebase** on the latest `main` branch
3. Write a **clear commit message**:
   ```
   docs: Short description of changes
   
   More detailed explanation of the changes made and why they were made.
   ```
4. **Push** your changes to your fork
5. Open a **pull request** with a clear description

## Code of Conduct

By participating in this project, you agree to abide by our [Code of Conduct](CODE_OF_CONDUCT.md).

## Need Help?

- Join our [community forum](https://github.com/your-org/4ever/discussions)
- Check the [open issues](https://github.com/your-org/4ever/issues)
- Contact the documentation team at docs@4ever.dev

---

*Last updated: 2025-06-09*
