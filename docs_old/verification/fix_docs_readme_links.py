#!/usr/bin/env python3
"""
Fix broken links in docs/README.md.
"""
import re
from pathlib import Path

def fix_docs_readme_links():
    """Fix broken links in docs/README.md."""
    readme_path = Path('docs/README.md')
    
    try:
        with open(readme_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Fix [Aethel](/core/concepts#aethel) pattern
        content = re.sub(
            r'\[Aethel\]\(/core/concepts#\\\[aethel\\\]',
            '[Aethel](/core/concepts#aethel)',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix [Chronon](/core/concepts#chronon) pattern
        content = re.sub(
            r'\[Chronon\]\(/core/concepts#\\\[chronon\\\]',
            '[Chronon](/core/concepts#chronon)',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix [Paradox](/core/concepts#paradox) pattern
        content = re.sub(
            r'\[Paradox\]\(/core/concepts#\\\[paradox\\\]',
            '[Paradox](/core/concepts#paradox)',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix [Weave](/core/concepts#weave) pattern
        content = re.sub(
            r'\[Weave\]\(/core/concepts#\\\[weave\\\]',
            '[Weave](/core/concepts#weave)',
            content,
            flags=re.IGNORECASE
        )
        
        # Only write if changes were made
        if content != original_content:
            with open(readme_path, 'w', encoding='utf-8') as f:
                f.write(content)
            print("Fixed broken links in docs/README.md")
        else:
            print("No broken links found in docs/README.md")
            
    except Exception as e:
        print(f"Error processing docs/README.md: {str(e)}")

if __name__ == "__main__":
    fix_docs_readme_links()
