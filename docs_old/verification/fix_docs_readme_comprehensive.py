#!/usr/bin/env python3
"""
Comprehensively fix broken links in docs/README.md.
"""
import re
from pathlib import Path

def fix_docs_readme_comprehensive():
    """Fix broken links in docs/README.md using comprehensive patterns."""
    readme_path = Path('docs/README.md')
    
    try:
        with open(readme_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        original_content = content
        
        # Fix [Aethel]()/)c)o)r)e)/)c)o)n)c)e)p)t)s) pattern
        content = re.sub(
            r'\[Aethel\]\(\)/\)c\)o\)r\)e\)/\)c\)o\)n\)c\)e\)p\)t\)s\)#\[\)a\)e\)t\)h\)e\)l\]',
            '[Aethel](/core/concepts#aethel)',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix [Chronon]()/)c)o)r)e)/)c)o)n)c)e)p)t)s) pattern
        content = re.sub(
            r'\[Chronon\]\(\)/\)c\)o\)r\)e\)/\)c\)o\)n\)c\)e\)p\)t\)s\)#\[\)c\)h\)r\)o\)n\)o\)n\]',
            '[Chronon](/core/concepts#chronon)',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix [Paradox]()/)c)o)r)e)/)c)o)n)c)e)p)t)s) pattern
        content = re.sub(
            r'\[Paradox\]\(\)/\)c\)o\)r\)e\)/\)c\)o\)n\)c\)e\)p\)t\)s\)#\[\)p\)a\)r\)a\)d\)o\)x\]',
            '[Paradox](/core/concepts#paradox)',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix [Weave]()/)c)o)r)e)/)c)o)n)c)e)p)t)s) pattern
        content = re.sub(
            r'\[Weave\]\(\)/\)c\)o\)r\)e\)/\)c\)o\)n\)c\)e\)p\)t\)s\)#\[\)w\)e\)a\)v\)e\]',
            '[Weave](/core/concepts#weave)',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix /core/concepts#[chronon](/ pattern
        content = re.sub(
            r'/core/concepts#\\\[chronon\\\]\(/',
            '/core/concepts#chronon',
            content,
            flags=re.IGNORECASE
        )
        
        # Fix /core/concepts#[paradox](/ pattern
        content = re.sub(
            r'/core/concepts#\\\[paradox\\\]\(/',
            '/core/concepts#paradox',
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
    fix_docs_readme_comprehensive()
