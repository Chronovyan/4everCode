#!/usr/bin/env python3
"""
Fix broken links in API reference and other documentation files.
"""
import re
import os
from pathlib import Path

def fix_api_links():
    """Fix broken links in API reference and other documentation files."""
    root_dir = Path(__file__).parent.parent
    files_to_fix = [
        root_dir / 'docs' / 'api_reference.md',
        root_dir / 'docs' / 'automatic_resource_optimizer.md',
        root_dir / 'docs' / 'error_handling_system.md',
        root_dir / 'docs' / 'temporal_debt_alert_system.md',
        root_dir / 'docs' / 'temporal_synchronizer.md'
    ]
    
    # Patterns to fix common broken links
    patterns = [
        # Fix [text](const) -> `const`
        (r'\[([^\]]+)\]\(\s*const\s*\)', r'`const`'),
        
        # Fix [text](int) -> `int`
        (r'\[([^\]]+)\]\(\s*int\s*\)', r'`int`'),
        
        # Fix [text](double) -> `double`
        (r'\[([^\]]+)\]\(\s*double\s*\)', r'`double`'),
        
        # Fix [text](TemporalContext&) -> `TemporalContext&`
        (r'\[([^\]]+)\]\(\s*([^\s)]+)\s*\)', r'`\2`')
    ]
    
    total_fixes = 0
    
    for file_path in files_to_fix:
        if not file_path.exists():
            print(f"Skipping non-existent file: {file_path}")
            continue
            
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            original_content = content
            fixes_applied = 0
            
            # Apply each pattern
            for pattern, replacement in patterns:
                content, count = re.subn(pattern, replacement, content)
                fixes_applied += count
            
            # Only write if changes were made
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                print(f"Fixed {fixes_applied} broken links in {file_path.name}")
                total_fixes += fixes_applied
            
        except Exception as e:
            print(f"Error processing {file_path}: {str(e)}")
    
    print(f"\nTotal broken links fixed: {total_fixes}")

if __name__ == "__main__":
    fix_api_links()
