#!/usr/bin/env python3
"""
Fix missing concept links in markdown files.
"""

import re
import os
from pathlib import Path

# Core concepts and their corresponding anchor links
CORE_CONCEPTS = {
    'WEAVE': 'weave',
    'CHRONON': 'chronon',
    'AETHEL': 'aethel',
    'PARADOX': 'paradox',
    'ORDER': 'order',
    'FLUX': 'flux',
    'TEMPORAL': 'temporal',
    'ANCHOR': 'anchor',
    'REBEL': 'rebel',
    'SEEKER': 'seeker',
    'STABILIZE': 'stabilize',
    'CHRONOSTREAM': 'chronostream',
    'PARADOX_CHECK': 'paradox-check',
}

def fix_concept_links_in_file(file_path):
    """Fix concept links in a single file."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Fix concept links in the format [CONCEPT] or CONCEPT
        for concept, anchor in CORE_CONCEPTS.items():
            # Replace [CONCEPT] with [CONCEPT](/core/concepts#anchor)
            pattern1 = fr'\[{concept}\]'
            replacement1 = f'[{concept}](/core/concepts#{anchor})'
            content = re.sub(pattern1, replacement1, content)
            
            # Replace standalone CONCEPT with [CONCEPT](/core/concepts#anchor)
            # But only if it's not already part of a link
            pattern2 = fr'(?<!\[){concept}(?!\]\()'
            replacement2 = f'[{concept}](/core/concepts#{anchor})'
            content = re.sub(pattern2, replacement2, content)
        
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
                if fix_concept_links_in_file(file_path):
                    print(f"Fixed concept links in {os.path.relpath(file_path, docs_dir)}")
                    fixed_files += 1
    
    print(f"\nTotal files with fixed concept links: {fixed_files}")

if __name__ == "__main__":
    main()
