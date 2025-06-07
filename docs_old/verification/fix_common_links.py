#!/usr/bin/env python3
"""
Fix common broken links in markdown files.
"""

import re
import os
from pathlib import Path

# Common broken link patterns and their fixes
LINK_PATTERNS = [
    # Fix /core/concepts# links
    (r'(\[.*?\]\()/core/concepts#([^)]+)(\))', 
     r'\1/core/Core Concepts - The Foundation of Temporal Programming.md#\2\3'),
    
    # Fix broken const links in code blocks
    (r'\[([^]]+)\]\(([^)]*?const[^)]*?)\)', 
     r'`\2`'),
    
    # Fix broken double links
    (r'\[([^]]+)\]\(([^)]*?double[^)]*?)\)', 
     r'`\2`')
]

def fix_links_in_file(file_path):
    """Fix common broken links in a single file."""
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        original_content = content
        
        for pattern, replacement in LINK_PATTERNS:
            content = re.sub(pattern, replacement, content)
        
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
                if fix_links_in_file(file_path):
                    print(f"Fixed links in {os.path.relpath(file_path, docs_dir)}")
                    fixed_files += 1
    
    print(f"\nTotal files with fixed links: {fixed_files}")

if __name__ == "__main__":
    main()
