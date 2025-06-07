#!/usr/bin/env python3
"""
Add frontmatter to markdown files.

This script adds YAML frontmatter to markdown files that don't have it.
"""

import os
import re
import yaml
from pathlib import Path

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
EXCLUDE_DIRS = {'node_modules', '.git', '.github', 'templates', 'scripts', 'images', 'assets'}

def get_title_from_content(content):
    """Extract title from markdown content."""
    # Look for markdown headers
    match = re.search(r'^#\s+(.+?)\s*$', content, re.MULTILINE)
    if match:
        return match.group(1).strip()
    return None

def generate_frontmatter(filepath, title=None, description=None):
    """Generate YAML frontmatter for a file."""
    rel_path = filepath.relative_to(DOCS_DIR)
    
    # Determine the weight based on directory structure
    weight = 100  # Default weight
    
    # Adjust weight based on directory depth and name
    parts = rel_path.parts
    if len(parts) > 1:
        # Add weight based on directory depth
        weight += len(parts) * 10
        
        # Add weight based on directory name
        dir_weights = {
            'getting-started': 0,
            'tutorials': 10,
            'guides': 20,
            'language': 30,
            'reference': 40,
            'api': 50,
            'advanced': 60,
            'community': 1000
        }
        
        for i, part in enumerate(parts[:-1]):
            if part in dir_weights:
                weight += dir_weights[part]
                break
    
    # Generate a description if not provided
    if not description:
        description = f"Documentation for {rel_path}"
    
    # Create frontmatter dictionary
    frontmatter = {
        'title': title or rel_path.stem.replace('-', ' ').title(),
        'description': description,
        'weight': weight,
        'draft': True  # Mark all migrated files as draft initially
    }
    
    return frontmatter

def add_frontmatter_to_file(filepath):
    """Add frontmatter to a single file if it doesn't have it."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read().strip()
        
        # Skip if file already has frontmatter
        if content.startswith('---\n'):
            return False
        
        # Generate frontmatter
        title = get_title_from_content(content)
        frontmatter = generate_frontmatter(filepath, title=title)
        yaml_content = yaml.dump(frontmatter, default_flow_style=False, allow_unicode=True, sort_keys=False)
        
        # Create new content with frontmatter
        new_content = f"---\n{yaml_content}---\n\n{content}"
        
        # Backup the file
        backup_path = f"{filepath}.bak"
        if not os.path.exists(backup_path):
            with open(backup_path, 'w', encoding='utf-8') as f:
                f.write(content)
        
        # Write new content
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
            
        return True
    except Exception as e:
        print(f"Error processing {filepath}: {e}")
        return False

def main():
    """Main function to process all markdown files."""
    processed = 0
    skipped = 0
    errors = 0
    
    for root, dirs, files in os.walk(DOCS_DIR):
        # Skip excluded directories
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]
        
        for file in files:
            if file.lower().endswith('.md'):
                filepath = Path(root) / file
                try:
                    if add_frontmatter_to_file(filepath):
                        print(f"Added frontmatter to {filepath.relative_to(DOCS_DIR)}")
                        processed += 1
                    else:
                        skipped += 1
                except Exception as e:
                    print(f"Error processing {filepath}: {e}")
                    errors += 1
    
    print(f"\nFrontmatter addition complete!")
    print(f"Files processed: {processed}")
    print(f"Files skipped (already had frontmatter): {skipped}")
    print(f"Errors: {errors}")

if __name__ == "__main__":
    main()
