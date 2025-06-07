#!/usr/bin/env python3
"""
Update concept links to point to the correct anchors in the core concepts file.
"""

import re
import os
from pathlib import Path

# Mapping of concept names to their anchor IDs
CONCEPT_ANCHORS = {
    'aethel': 'aethel',
    'chronon': 'chronon',
    'weave': 'weave',
    'paradox': 'paradox',
    'anchor': 'anchor',
    'temporal': 'temporal',
    'flux': 'flux',
    'stabilize': 'stabilize',
    'stability': 'stability'
}

def update_links_in_file(file_path):
    """Update concept links in a single file."""
    try:
        with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        original_content = content
        
        # Update links to core concepts
        for concept, anchor in CONCEPT_ANCHORS.items():
            # Handle both upper and lower case variations
            for case in [concept, concept.upper(), concept.capitalize()]:
                # Update links with the old path
                pattern = rf'(\[.*?\]\()/core/concepts#{case}(\)|\s)'
                replacement = rf'\1/core/Core Concepts - The Foundation of Temporal Programming.md#{anchor}\2'
                content = re.sub(pattern, replacement, content, flags=re.IGNORECASE)
                
                # Update links with the new path but wrong anchor
                pattern = rf'(\[.*?\]\()/core/Core Concepts - The Foundation of Temporal Programming\.md#{case}(\s|\))'
                replacement = rf'\1/core/Core Concepts - The Foundation of Temporal Programming.md#{anchor}\2'
                content = re.sub(pattern, replacement, content, flags=re.IGNORECASE)
        
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
                # Skip the core concepts file itself
                if 'Core Concepts - The Foundation of Temporal Programming.md' in file_path:
                    continue
                if update_links_in_file(file_path):
                    print(f"Updated links in {os.path.relpath(file_path, docs_dir)}")
                    fixed_files += 1
    
    print(f"\nTotal files with updated links: {fixed_files}")

if __name__ == "__main__":
    main()
