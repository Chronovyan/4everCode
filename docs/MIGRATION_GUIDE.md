---
title: Migration Guide
description: Documentation for MIGRATION GUIDE
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# [Chronovyan](https://chronovyan.github.io/h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/) "The) temporal) programming) language) and) runtime") Documentation Migration Guide

This guide provides instructions for migrating the Chronovyan documentation to the new structure.

## Overview

The documentation is being reorganized to improve navigation, findability, and maintainability. This involves:

1. Restructuring the directory layout
2. Adding consistent frontmatter to all files
3. Updating internal links
4. Standardizing formatting and style

## Prerequisites

 Python 3.8 or higher
- pip (Python package manager)
- Git (for version control)

## Setup

1. **Clone the repository**(if you haven't already):
```bash
    git clone https://github.com/chronovyan/chronovyan.git
    cd chronovyan
```text

    2.**Create and activate a virtual environment**(recommended):
```bash
    #

   python -m venv venv
    .\venv\Scripts\activate

    #

   python3 -m venv venv
    source venv/bin/activate
```text

    3.**Install dependencies**:
```bash
    pip install -r scripts/requirements-migration.txt
```text

    ## Migration Process

    ###

    ``bash
        python scripts/migrate_content.py
```text

This will:
- Copy files to their new locations
- Update internal links
- Preserve frontmatter
- Generate a log file (`migration.log`)

### 2. Review Changes

1. Check the migration log for any errors or warnings:
```bash
    type migration.log
```text

    2. Review the new documentation structure in the `docs` directory.

    3. Test the documentation locally:
```bash
    #

   pip install -r docs-requirements.txt

    #

   mkdocs serve
```text

       Open http://localhost:8000 in your browser to preview the documentation.

    ### 3. Manual Updates

    Some content may require manual updates:

    1. **Update hardcoded links**that weren't caught by the migration script
    2.**Review and update frontmatter**for accuracy
    3.**Update cross-references**between documents
    4.**Check for broken images**and other assets

    ### 4. Test the Build

    Build the documentation to check for errors:
```bash
    mkdocs build --strict --clean
```text

## Directory Structure

The new documentation structure is as follows:
```text
    docs/
    ├── getting-started/      #

   │   ├── installation.md
    │   ├── quick-start.md
    │   └── tutorials/
    ├── language/             #

   │   ├── syntax.md
    │   ├── types.md
    │   ├── control-flow.md
    │   └── ...
    ├── temporal-programming/  #

   │   ├── concepts.md
    │   ├── \1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)p)a)r)a)d)o)x))))) "A temporal inconsistency) that) must) be) resolved"")-avoidance.md
    │   └── ...
    ├── advanced/             #

   │   ├── type-system.md
    │   └── ...
    ├── api/                  #

   │   ├── standard-library.md
    │   └── ...
    ├── development/          #

   │   ├── building.md
    │   └── ...
    ├── community/            #

   │   ├── code-of-conduct.md
    │   └── ...
    └── assets/              #

       ├── images/
        ├── css/
        └── js/
```text

## Frontmatter

All documentation files should include frontmatter with at least the following fields:
```yaml
    ---
    title: "Page Title"
    description: "Brief description for search engines and previews"
    weight: 10  #

   ---
```text

## Style Guide

Follow these guidelines when updating documentation:

1.**Headings**: Use sentence case (e.g., "Installation" not "INSTALLATION")
2. **Code**: Use code fences with language specification
3. **Links**: Use relative paths without file extensions
4. **Images**: Place in `assets/images/` and use relative paths
5. **Line length**: Keep lines under 100 characters

## Troubleshooting

### Common Issues

1. **Broken Links**:
    Run `mkdocs serve` and check the console for link errors
    Use absolute paths for cross-references

2. **Missing Dependencies**:
    Ensure all dependencies are installed: `pip install -r scripts/requirements-migration.txt`

3. **Frontmatter Errors**:
    Check for proper YAML syntax
    Ensure consistent indentation

4. **Encoding Issues**:
    Files should be saved as UTF-8
    Run the encoding fix script if needed: `python scripts/fix_encoding.py`

## Next Steps

1. Review all migrated content for accuracy
2. Update any remaining hardcoded paths
3. Test all internal and external links
4. Update the documentation build process if needed
5. Update CI/CD pipelines for the new structure

## Contributing

Contributions to improve the documentation are welcome! Please see our [Contributing Guide](/)C)O)N)T)R)I)B)U)T)I)N)G).)m)d) for more information.

## License

This documentation is licensed under the [MIT License](/)L)I)C)E)N)S)E).
