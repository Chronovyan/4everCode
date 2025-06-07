#!/usr/bin/env python3
"""
Fix the remaining broken links in README.md.
"""
import re
from pathlib import Path

def fix_readme_links():
    """Fix broken links in README.md."""
    readme_path = Path('README.md')
    
    try:
        with open(readme_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Fix /core/concepts#[chronon](/... pattern
        content = re.sub(
            r'/core/concepts#\\\[([^\]]+)\\\]\(/core/concepts#([^\s)]+)\)',
            r'/core/concepts#\2',
            content
        )
        
        # Fix /core/concepts#[paradox](/... pattern
        content = re.sub(
            r'/core/concepts#\\\[([^\]]+)\\\]\(/core/concepts#([^\s)]+)\)',
            r'/core/concepts#\2',
            content
        )
        
        # Fix remaining broken links with const
        content = re.sub(
            r'\[([^\]]+)\]\(\s*const\s*\)',
            r'`const`',
            content
        )
        
        # Fix remaining broken links with double
        content = re.sub(
            r'\[([^\]]+)\]\(\s*double\s*\)',
            r'"double"',
            content
        )
        
        # Only write if changes were made
        if content != original_content:
            with open(readme_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print("Fixed broken links in README.md")
        else:
            print("No broken links found in README.md")
            
    except Exception as e:
        print(f"Error processing README.md: {str(e)}")

if __name__ == "__main__":
    fix_readme_links()
