# Documentation Review Tool

This tool helps review and analyze the Chronovyan documentation for common issues, generate reports, and ensure consistency across all documentation files.

## Features

- **Frontmatter Validation**: Ensures all documents have required frontmatter fields
- **Link Checking**: Identifies broken internal and external links
- **Style Checking**: Enforces consistent header styles and formatting
- **Code Block Analysis**: Verifies proper syntax highlighting and language specification
- **Statistics Generation**: Provides word counts, header levels, and other metrics
- **Report Generation**: Creates detailed markdown reports of all issues found

## Prerequisites

- Python 3.8 or higher
- Required Python packages (install using `pip install -r requirements-review.txt`)

## Installation

1. Clone the repository (if you haven't already):
   ```bash
   git clone https://github.com/chronovyan/chronovyan.git
   cd chronovyan
   ```

2. Create and activate a virtual environment (recommended):
   ```bash
   # Windows
   python -m venv venv
   .\venv\Scripts\activate
   
   # macOS/Linux
   python3 -m venv venv
   source venv/bin/activate
   ```

3. Install the required packages:
   ```bash
   pip install -r scripts/requirements-review.txt
   ```

## Usage

### Basic Usage

Run the documentation review tool:

```bash
python scripts/review_docs.py
```

This will:
1. Scan all markdown files in the `docs` directory
2. Analyze each file for issues
3. Generate a detailed report in `docs_review_report.md`
4. Print a summary to the console

### Command Line Options

```
usage: review_docs.py [-h] [--output FILE] [--verbose] [--debug]

options:
  -h, --help   show this help message and exit
  --output FILE  Output file for the report (default: docs_review_report.md)
  --verbose      Enable verbose output
  --debug        Enable debug logging
```

### Example Output

```
Documentation Review Summary:
- Documents analyzed: 42
- Issues found: 15
  - Errors: 3
  - Warnings: 12

Report generated: docs_review_report.md
```

## Report Format

The generated report includes:

1. **Summary**: Overall statistics and issue counts
2. **Issues by Category**: Breakdown of issues by type (frontmatter, links, style, etc.)
3. **Detailed Issues**: File-by-file listing of all issues found
4. **Document Statistics**: Word counts, header levels, and other metrics

## Integration with CI/CD

You can integrate the documentation review into your CI/CD pipeline by adding a step like this:

```yaml
- name: Review Documentation
  run: |
    python -m pip install -r scripts/requirements-review.txt
    python scripts/review_docs.py --output docs/review/$(date +%Y%m%d).md
    # Fail the build if there are any errors
    if grep -q "❌" docs/review/$(date +%Y%m%d).md; then
      echo "Documentation contains errors. Please fix them before merging."
      exit 1
    fi
```

## Customization

You can customize the review process by modifying the following:

1. **Configuration**: Adjust constants at the top of `review_docs.py`
2. **Rules**: Modify the check methods in the `DocumentationReviewer` class
3. **Output**: Customize the report format in the `generate_report` method

## Troubleshooting

### Common Issues

1. **Missing Dependencies**:
   - Ensure all required packages are installed: `pip install -r scripts/requirements-review.txt`

2. **Unicode Decode Errors**:
   - Some files may not be UTF-8 encoded. The tool will attempt to read them as Latin-1 as a fallback.

3. **Performance Issues**:
   - For large documentation sets, the review process might be slow. Consider excluding large directories.

## License

This tool is part of the Chronovyan project and is licensed under the [MIT License](../LICENSE).
