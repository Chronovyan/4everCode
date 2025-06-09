# Chronovyan Documentation Structure

This document outlines the organization of the Chronovyan documentation to help contributors maintain a consistent structure.

## Core Documentation

- guides/ - Tutorials and how-to guides
  - getting-started/ - Introduction to Chronovyan
  - dvanced/ - Advanced topics and deep dives
  - examples/ - Example-based guides and code samples

- eference/ - API and language reference
  - language/ - Language specification and syntax
  - stdlib/ - Standard library documentation

- development/ - Development resources
  - uilding/ - Build system documentation
  - contributing/ - Contribution guidelines

- design/ - Design documents and architecture decisions
- rchitecture/ - System architecture documentation
- oadmap/ - Project roadmap and future plans
- community/ - Community resources and guidelines

## File Naming Conventions

- Use kebab-case for all filenames (e.g., getting-started.md)
- Use index.md for the main entry point of a directory
- Keep filenames short but descriptive
- Use lowercase for all filenames

## Documentation Standards

- Use Markdown (.md) for all documentation
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
