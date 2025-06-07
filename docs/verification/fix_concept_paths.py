#!/usr/bin/env python3
"""
Fix broken concept links by updating paths to match the actual file structure.
"""

import re
import os
from pathlib import Path

def fix_concept_paths_in_file(file_path):
    """Fix concept paths in a single file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Fix paths like /core/concepts# to the actual file path
        content = re.sub(
            r'(\[.*?\]\()/core/concepts#(.*?)(\))',
            lambda m: f'{m.group(1)}/core/Core Concepts - The Foundation of Temporal Programming.md#{m.group(2).lower()}{m.group(3)}',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix paths with broken encoding
        content = re.sub(
            r'\[\(\w+)\]\([^)]*?/core/concepts#[^)]*?\)',
            lambda m: f'[{m.group(1)}](/core/Core Concepts - The Foundation of Temporal Programming.md#{m.group(1).lower()})',
            content
        )
        
        if content != original_content:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(content)
            return True
        return False
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return False

def main():
    docs_dir = Path(__file__).parent.parent  # Go up to docs directory
    fixed_files = 0
    
    # Process all markdown files
    for root, _, files in os.walk(docs_dir):
        for file in files:
            if file.endswith(('.md', '.markdown')):
                file_path = os.path.join(root, file)
                if fix_concept_paths_in_file(file_path):
                    print(f"Fixed concept paths in {os.path.relpath(file_path, docs_dir)}")
                    fixed_files += 1
    
    print(f"\nTotal files with fixed concept paths: {fixed_files}")

if __name__ == "__main__":
    main()
