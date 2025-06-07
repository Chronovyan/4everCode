#!/usr/bin/env python3
"""
Validate documentation files for common issues.

This script checks markdown files for common documentation issues
such as broken links, missing frontmatter, and style violations.
"""

import os
import re
import yaml
from pathlib import Path
import sys
from typing import Dict, List, Tuple, Optional, Set
from urllib.parse import urlparse

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
EXCLUDE_DIRS = {'node_modules', '.git', '.github', 'site', 'venv', '__pycache__'}
EXCLUDE_FILES = {'.DS_Store', 'README.md', 'SUMMARY.md'}

# Regular expressions
HEADER_PATTERN = re.compile(r'^(#+)\s+(.+?)(?:\s*\{#([^}]+)\})?\s*$', re.MULTILINE)
LINK_PATTERN = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
CODE_BLOCK_PATTERN = re.compile(r'```[^`]*?```', re.DOTALL)
INLINE_CODE_PATTERN = re.compile(r'`[^`]+`')

class ValidationError:
    def __init__(self, file: Path, message: str, line: int = None, context: str = None):
        self.file = file
        self.message = message
        self.line = line
        self.context = context
    
    def __str__(self):
        location = f"{self.file}"
        if self.line is not None:
            location += f":{self.line}"
        if self.context:
            return f"{location}: {self.message}\n  {self.context}"
        return f"{location}: {self.message}"

def get_files_to_validate() -> List[Path]:
    """Get all markdown files to validate."""
    files = []
    for root, dirs, files in os.walk(DOCS_DIR):
        # Skip excluded directories
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        
        for file in files:
            if file.lower().endswith('.md') and file not in EXCLUDE_FILES:
                files.append(Path(root) / file)
    return files

def validate_frontmatter(file: Path) -> List[ValidationError]:
    """Validate frontmatter in a markdown file."""
    errors = []
    
    try:
        with open(file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Check if file has frontmatter
        if not content.startswith('---'):
            errors.append(ValidationError(file, "Missing frontmatter"))
            return errors
        
        # Extract frontmatter
        parts = content.split('---', 2)
        if len(parts) < 3:
            errors.append(ValidationError(file, "Invalid frontmatter format"))
            return errors
        
        # Parse YAML frontmatter
        try:
            frontmatter = yaml.safe_load(parts[1])
        except yaml.YAMLError as e:
            errors.append(ValidationError(file, f"Invalid YAML in frontmatter: {e}"))
            return errors
        
        # Check required fields
        required_fields = ['title', 'description']
        for field in required_fields:
            if field not in frontmatter or not frontmatter[field]:
                errors.append(ValidationError(file, f"Missing required frontmatter field: {field}"))
        
        # Check title format
        if 'title' in frontmatter and not isinstance(frontmatter['title'], str):
            errors.append(ValidationError(file, "Title must be a string"))
        
        # Check description length
        if 'description' in frontmatter:
            desc = frontmatter['description']
            if not isinstance(desc, str):
                errors.append(ValidationError(file, "Description must be a string"))
            elif len(desc) > 160:  # Recommended max length for meta descriptions
                errors.append(ValidationError(file, "Description is too long (max 160 characters)"))
        
        # Check weight is a number if present
        if 'weight' in frontmatter and not isinstance(frontmatter['weight'], (int, float)):
            errors.append(ValidationError(file, "Weight must be a number"))
    
    except Exception as e:
        errors.append(ValidationError(file, f"Error validating frontmatter: {e}"))
    
    return errors

def validate_headers(file: Path) -> List[ValidationError]:
    """Validate markdown headers."""
    errors = []
    
    try:
        with open(file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Remove code blocks to avoid false positives
        content = CODE_BLOCK_PATTERN.sub('', content)
        
        lines = content.split('\n')
        for i, line in enumerate(lines, 1):
            match = HEADER_PATTERN.match(line)
            if match:
                level = len(match.group(1))
                text = match.group(2).strip()
                
                # Check header levels (only h1-h4 should be used)
                if level > 4:
                    errors.append(ValidationError(
                        file,
                        f"Header level too deep (h{level}): use h1-h4 only",
                        i,
                        line.strip()
                    ))
                
                # Check header text formatting
                if text.endswith(':'):
                    errors.append(ValidationError(
                        file,
                        "Header ends with colon (avoid punctuation in headers)",
                        i,
                        line.strip()
                    ))
    
    except Exception as e:
        errors.append(ValidationError(file, f"Error validating headers: {e}"))
    
    return errors

def validate_links(file: Path) -> List[ValidationError]:
    """Validate links in markdown files."""
    errors = []
    
    try:
        with open(file, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Remove code blocks to avoid false positives
        content = CODE_BLOCK_PATTERN.sub('', content)
        
        # Find all markdown links
        for match in LINK_PATTERN.finditer(content):
            link_text = match.group(1)
            link_url = match.group(2)
            
            # Skip empty links
            if not link_url.strip():
                continue
                
            # Skip web URLs
            parsed_url = urlparse(link_url)
            if parsed_url.scheme in ('http', 'https', 'mailto', 'ftp'):
                continue
                
            # Skip anchor-only links
            if link_url.startswith('#'):
                continue
                
            # Handle relative paths
            if link_url.startswith('/'):
                target_path = DOCS_DIR / link_url[1:]
            else:
                target_path = file.parent / link_url
            
            # Check if target exists
            if not target_path.exists():
                # Check for anchor in the same file
                if '#' in link_url:
                    base_path = link_url.split('#')[0]
                    if base_path:  # Not just an anchor
                        base_target = file.parent / base_path
                        if not base_target.exists():
                            errors.append(ValidationError(
                                file,
                                f"Broken link: {link_url}",
                                None,
                                f"[{link_text}]({link_url})"
                            ))
                else:
                    errors.append(ValidationError(
                        file,
                        f"Broken link: {link_url}",
                        None,
                        f"[{link_text}]({link_url})"
                    ))
    
    except Exception as e:
        errors.append(ValidationError(file, f"Error validating links: {e}"))
    
    return errors

def validate_file(file: Path) -> List[ValidationError]:
    """Run all validations on a single file."""
    errors = []
    
    # Skip excluded files
    if file.name in EXCLUDE_FILES or file.parent.name in EXCLUDE_DIRS:
        return []
    
    # Run validators
    errors.extend(validate_frontmatter(file))
    errors.extend(validate_headers(file))
    errors.extend(validate_links(file))
    
    return errors

def main():
    """Main validation function."""
    print("Validating documentation...\n")
    
    files = get_files_to_validate()
    all_errors = []
    
    for file in files:
        errors = validate_file(file)
        all_errors.extend(errors)
    
    # Print results
    if all_errors:
        print(f"\nFound {len(all_errors)} issues:")
        for error in all_errors:
            print(f"- {error}")
        
        print(f"\nValidation failed with {len(all_errors)} issues.")
        sys.exit(1)
    else:
        print("\nAll documentation files are valid!")
        sys.exit(0)

if __name__ == "__main__":
    main()
