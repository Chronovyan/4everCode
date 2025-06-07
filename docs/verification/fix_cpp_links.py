#!/usr/bin/env python3
"""
Fix C++ lambda syntax in markdown files that's being misinterpreted as Markdown links.
"""

import re
import os
from pathlib import Path

def fix_cpp_lambdas_in_file(file_path):
    """Fix C++ lambda syntax in a single file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Fix [](const) -> [] (const) for C++ lambdas
        # This pattern looks for square brackets followed by parentheses with 'const' inside
        pattern = r'\[\]\((\s*const\s*)\)'
        fixed_content, count = re.subn(
            pattern, 
            lambda m: f'[]({m.group(1)})',  # Add space between [] and ()
            content
        )
        
        if count > 0:
            with open(file_path, 'w', encoding='utf-8') as f:
                f.write(fixed_content)
            return count
        return 0
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return 0

def main():
    docs_dir = Path(__file__).parent.parent  # Go up to docs directory
    total_fixes = 0
    
    # Process all markdown files
    for root, _, files in os.walk(docs_dir):
        for file in files:
            if file.endswith(('.md', '.markdown')):
                file_path = os.path.join(root, file)
                fixes = fix_cpp_lambdas_in_file(file_path)
                if fixes > 0:
                    print(f"Fixed {fixes} C++ lambda(s) in {os.path.relpath(file_path, docs_dir)}")
                    total_fixes += fixes
    
    print(f"\nTotal C++ lambda fixes: {total_fixes}")

if __name__ == "__main__":
    main()
