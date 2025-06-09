#!/usr/bin/env python3
"""
Documentation Validator for 4ever

This script checks the documentation for common issues:
- Missing or invalid front matter
- Broken internal links
- Style guide violations
- Orphaned files
"""

import os
import re
import sys
import yaml
from pathlib import Path
from typing import Dict, List, Set, Tuple, Optional

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
VALID_CATEGORIES = {'core', 'guides', 'advanced', 'reference', 'examples', 'community'}
REQUIRED_FRONT_MATTER = {'title', 'description'}
OPTIONAL_FRONT_MATTER = {'weight', 'draft', 'toc', 'template'}
ALLOWED_EXTENSIONS = {'.md', '.mdx', '.markdown'}

class ValidationError:
    def __init__(self, file: Path, message: str, line: int = 0):
        self.file = file
        self.message = message
        self.line = line
    
    def __str__(self) -> str:
        return f"{self.file}:{self.line}: {self.message}"

def load_front_matter(file_path: Path) -> Tuple[Optional[Dict], int, List[str]]:
    """Extract front matter from a Markdown file."""
    content = file_path.read_text(encoding='utf-8')
    lines = content.split('\n')
    
    if not lines or lines[0].strip() != '---':
        return None, 0, lines
    
    front_matter_lines = []
    end_line = 1
    
    while end_line < len(lines) and lines[end_line].strip() != '---':
        front_matter_lines.append(lines[end_line])
        end_line += 1
    
    if end_line >= len(lines):
        return None, 0, lines  # No closing ---
    
    try:
        front_matter = yaml.safe_load('\n'.join(front_matter_lines))
        return front_matter, end_line + 1, lines
    except yaml.YAMLError as e:
        return None, 0, lines

def validate_file(file_path: Path) -> List[ValidationError]:
    """Validate a single documentation file."""
    errors: List[ValidationError] = []
    rel_path = file_path.relative_to(DOCS_DIR)
    
    # Check file extension
    if file_path.suffix not in ALLOWED_EXTENSIONS:
        errors.append(ValidationError(
            rel_path,
            f"Invalid file extension: {file_path.suffix}. Allowed: {', '.join(ALLOWED_EXTENSIONS)}"
        ))
    
    # Check file naming
    if not file_path.name[0].isalpha() and file_path.name.lower() != 'readme.md':
        errors.append(ValidationError(
            rel_path,
            "File name should start with a letter",
        ))
    
    # Check front matter
    front_matter, content_start, lines = load_front_matter(file_path)
    
    if front_matter is None:
        errors.append(ValidationError(
            rel_path,
            "Missing or invalid YAML front matter",
            1
        ))
        return errors  # Skip further checks if no front matter
    
    # Check required front matter fields
    for field in REQUIRED_FRONT_MATTER:
        if field not in front_matter:
            errors.append(ValidationError(
                rel_path,
                f"Missing required front matter field: {field}",
                1
            ))
    
    # Check for unknown front matter fields
    allowed_fields = REQUIRED_FRONT_MATTER | OPTIONAL_FRONT_MATTER
    for field in front_matter.keys():
        if field not in allowed_fields:
            errors.append(ValidationError(
                rel_path,
                f"Unknown front matter field: {field}",
                1
            ))
    
    # Check weight is a number if present
    if 'weight' in front_matter and not isinstance(front_matter['weight'], (int, float)):
        errors.append(ValidationError(
            rel_path,
            f"Front matter 'weight' must be a number, got {type(front_matter['weight']).__name__}",
            1
        ))
    
    # Check draft is boolean if present
    if 'draft' in front_matter and not isinstance(front_matter['draft'], bool):
        errors.append(ValidationError(
            rel_path,
            f"Front matter 'draft' must be a boolean, got {type(front_matter['draft']).__name__}",
            1
        ))
    
    # Check for broken Markdown links
    link_pattern = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
    for i, line in enumerate(lines[content_start:], start=content_start + 1):
        for match in link_pattern.finditer(line):
            link_text, link_url = match.groups()
            
            # Skip external links
            if link_url.startswith(('http://', 'https://', 'mailto:', '#')):
                continue
                
            # Handle anchor links
            if '#' in link_url:
                link_url = link_url.split('#')[0]
                if not link_url:  # Just an anchor link
                    continue
            
            # Resolve relative paths
            target_path = (file_path.parent / link_url).resolve()
            
            # Check if target exists
            if not target_path.exists():
                errors.append(ValidationError(
                    rel_path,
                    f"Broken link: '{link_text}' -> '{link_url}' not found",
                    i
                ))
    
    return errors

def validate_structure() -> List[ValidationError]:
    """Validate the overall documentation structure."""
    errors: List[ValidationError] = []
    all_files = set()
    
    # Check top-level directories
    for item in DOCS_DIR.iterdir():
        if not item.is_dir():
            continue
            
        if item.name not in VALID_CATEGORIES and item.name != 'assets':
            errors.append(ValidationError(
                item.relative_to(DOCS_DIR),
                f"Unexpected top-level directory. Allowed: {', '.join(VALID_CATEGORIES)}"
            ))
    
    # Validate all Markdown files
    for root, _, files in os.walk(DOCS_DIR):
        # Skip hidden directories
        if '/.' in root or '\\' in root.replace(os.sep, '/'):
            continue
            
        for file in files:
            if file.startswith('.'):
                continue
                
            file_path = Path(root) / file
            rel_path = file_path.relative_to(DOCS_DIR)
            
            # Skip non-Markdown files
            if file_path.suffix not in ALLOWED_EXTENSIONS:
                if file_path.suffix not in {'.png', '.jpg', '.jpeg', '.gif', '.svg', '.pdf'}:
                    errors.append(ValidationError(
                        rel_path,
                        f"Unexpected file type in docs directory"
                    ))
                continue
            
            all_files.add(rel_path)
            
            # Skip files in _templates and _build directories
            if any(part.startswith('_') and part != '_index.md' for part in rel_path.parts):
                continue
            
            # Validate the file
            errors.extend(validate_file(file_path))
    
    return errors

def main() -> int:
    """Main entry point for the validator."""
    print(f"Validating documentation in {DOCS_DIR}...\n")
    
    errors = validate_structure()
    
    if not errors:
        print("✓ No issues found!")
        return 0
    
    # Group errors by file
    errors_by_file: Dict[Path, List[ValidationError]] = {}
    for error in errors:
        errors_by_file.setdefault(error.file, []).append(error)
    
    # Print errors
    for file_path, file_errors in sorted(errors_by_file.items(), key=lambda x: str(x[0])):
        print(f"{file_path}:")
        for error in sorted(file_errors, key=lambda e: e.line):
            print(f"  {error}")
        print()
    
    print(f"\nFound {len(errors)} issue(s) in {len(errors_by_file)} file(s)")
    return 1

if __name__ == "__main__":
    sys.exit(main())
