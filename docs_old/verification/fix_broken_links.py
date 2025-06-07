#!/usr/bin/env python3
"""
Chronovyan Broken Link Fixer

This script fixes common broken link patterns in markdown documentation.
"""

import re
import os
from pathlib import Path
from typing import Dict, List, Optional, Tuple
import yaml

class BrokenLinkFixer:
    def __init__(self, root_dir: str = '.'):
        self.root_dir = Path(root_dir).resolve()
        self.docs_dir = self.root_dir / 'docs'
        self.files_processed = 0
        self.links_fixed = 0
        
        # Define patterns to fix and their replacements
        self.link_fixers = [
            # Fix malformed Chronovyan links
            {
                'pattern': r'(\[Chronovyan\])\(([^)]+)\)',
                'replacement': r'[Chronovyan](https://chronovyan.github.io/\2)',
                'description': 'Fixing malformed Chronovyan links'
            },
            # Fix deeply nested concept links with multiple levels of escaping
            {
                'pattern': r'\[([^\]]+)\]\(/core/concepts#(?:\\\[|\[)([^\]]+)(?:\\\]|\])\(/core/concepts#([^)]+)\)',
                'replacement': r'[\1](/core/concepts#\3)',
                'description': 'Fixing deeply nested concept links'
            },
            # Fix concept links with multiple levels of markdown
            {
                'pattern': r'\[([^\]]+)\]\(/core/concepts#(?:\[([^\]]+)\]\(/core/concepts#)+([^)]+)\)',
                'replacement': r'[\1](/core/concepts#\3)',
                'description': 'Fixing concept links with multiple markdown levels'
            },
            # Fix concept links with repeated patterns
            {
                'pattern': r'\[([^\]]+)\]\(/core/concepts#(?:[^)]*?\[([^\]]+)\]\(/core/concepts#)+([^)]+)\)',
                'replacement': r'[\1](/core/concepts#\3)',
                'description': 'Fixing concept links with repeated patterns'
            },
            # Fix concept links with various patterns
            {
                'pattern': r'(\[^\]]*?\])\(["\']?/(core/concepts#[a-zA-Z0-9_-]+)[^\s\)]*["\']?\)',
                'replacement': r'\1(https://chronovyan.github.io/\2)',
                'description': 'Fixing concept links with optional quotes and text after hash'
            },
            # Fix concept links with missing closing quote or bracket
            {
                'pattern': r'(\[^\]]*?\])\(["\']?/(core/concepts#[a-zA-Z0-9_-]+)[^\s\)]*(?=[\s\)\n])',
                'replacement': r'\1(https://chronovyan.github.io/\2)',
                'description': 'Fixing concept links with missing closing quote/bracket'
            },
            # Fix concept links with special characters or malformed URLs
            {
                'pattern': r'(\[^\]]*?\])\(["\']?/(core/concepts#[a-zA-Z0-9_-]+)[^\s\)]*["\']?[^\s\)]*\)',
                'replacement': r'\1(https://chronovyan.github.io/\2)',
                'description': 'Fixing malformed concept links with special characters'
            },
            # Fix concept links with missing protocol and path
            {
                'pattern': r'(\[^\]]*?\])\(["\']?#([a-zA-Z0-9_-]+)["\']?\)',
                'replacement': r'\1(#\2)',
                'description': 'Fixing fragment-only links'
            },
            # Fix known concept terms that aren't already linked
            {
                'pattern': r'([^\[\]()]|^)(weave|paradox|aethel|chronon)\b(?!\]\()',
                'replacement': r'\1[\2](/core/concepts#\2)',
                'description': 'Adding concept links to known terms',
                'flags': re.IGNORECASE
            },
            # Fix relative links in tutorials
            {
                'pattern': r'(\[.*?\])\(\.\./act(\d+)/README\.md\)',
                'replacement': r'\1(/tutorials/act\2/README.md)',
                'description': 'Fixing relative tutorial links'
            },
            # Fix documentation template link
            {
                'pattern': r'(\[documentation template\])\(\.\./templates/documentation-template\.md\)',
                'replacement': r'[documentation template](/templates/documentation-template.md)',
                'description': 'Fixing documentation template link'
            },
            # Fix broken markdown links with spaces
            {
                'pattern': r'(\[.*?\])\(([^)]+\s+[^)]+)\)',
                'replacement': r'\1(\2)',
                'description': 'Fixing links with spaces'
            },
            # Fix relative links in tutorials
            {
                'pattern': r'(\[.*?\])\(\.\./act(\d+)/README\.md\)',
                'replacement': r'\1(/tutorials/act\2/README.md)',
                'description': 'Fixing relative tutorial links'
            },
            # Fix documentation template link
            {
                'pattern': r'(\[documentation template\])\(\.\./templates/documentation-template\.md\)',
                'replacement': r'[documentation template](/templates/documentation-template.md)',
                'description': 'Fixing documentation template link'
            },
            # Fix double slashes in URLs
            {
                'pattern': r'(\[.*?\])\(//([^/])',
                'replacement': r'\1(/\2',
                'description': 'Fixing double slashes in URLs'
            },
            # Fix relative links that should be absolute
            {
                'pattern': r'(\[.*?\])\((?!http)(\.\./)+([^)]+)\)',
                'replacement': r'\1(https://chronovyan.github.io/\3)',
                'description': 'Fixing relative links'
            },
            # Fix broken markdown links with spaces
            {
                'pattern': r'(\[.*?\])\(([^)]+\s+[^)]+)\)',
                'replacement': r'\1(\2)',
                'description': 'Fixing links with spaces'
            },
            # Fix links with missing closing parenthesis
            {
                'pattern': r'(\[.*?\])\(([^)]*)(?<!\))(?=\s|$)',
                'replacement': r'\1(\2)',
                'description': 'Fixing links with missing closing parenthesis'
            },
            # Fix links with double quotes
            {
                'pattern': r'(\[.*?\])\("([^"]+)"\)',
                'replacement': r'\1(\2)',
                'description': 'Fixing links with double quotes'
            },
            # Fix UI_MOCKUPS.md specific link
            {
                'pattern': r'(\|>)\s*\[([^\]]+)\]\(([^)]+)\)',
                'replacement': r'\1 [\2](\3)',
                'description': 'Fixing UI_MOCKUPS.md specific link format'
            },
            # Fix temporal_synchronizer.md specific link
            {
                'pattern': r'double progress',
                'replacement': r'`double progress`',
                'description': 'Fixing code reference in temporal_synchronizer.md'
            }
        ]
        
        # Compile all patterns
        self.compiled_patterns = []
        for fixer in self.link_fixers:
            self.compiled_patterns.append({
                'pattern': re.compile(fixer['pattern']),
                'replacement': fixer['replacement'],
                'description': fixer['description']
            })
    
    def _fix_links_in_content(self, content: str, file_path: Path) -> Tuple[str, int]:
        """Fix links in the content and return the fixed content and number of fixes."""
        fixes = 0
        
        for fixer in self.compiled_patterns:
            def replace_match(match):
                nonlocal fixes
                original = match.group(0)
                fixed = fixer['pattern'].sub(fixer['replacement'], original)
                if fixed != original:
                    fixes += 1
                return fixed
            
            content = fixer['pattern'].sub(replace_match, content)
        
        return content, fixes
    
    def fix_links_in_file(self, content):
        """Fix broken links in the given content and return the fixed content and number of fixes."""
        original_content = content
        fixes_applied = 0

        def fix_nested_concept_links(text):
            # First, fix links with multiple levels of nesting
            patterns = [
                # Fix [text]("concept") where concept is a known term (Aethel, Paradox, Weave, etc.)
                (r'\[([^\]]+)\]\(\s*"(Aethel|Paradox|Weave|Chronon)"\s*\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept") with extra quotes and parentheses
                (r'\[([^\]]+)\]\(\s*["\']?(Aethel|Paradox|Weave|Chronon)["\']?\s*[)"\']+', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("/core/concepts#concept")")
                (r'\[([^\]]+)\]\(\s*["\']?/core/concepts#([^"\')]+)["\']?\s*[)"\']+', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\s*\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text](/core/concepts#concept)
                (r'\[([^\]]+)\]\(/core/concepts#([^)]+)\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("double")
                (r'\[([^\]]+)\]\(\s*"double"\s*\)', 
                 r'[\1]("double")'),
                
                # Fix [text](concept) where concept is a known term
                (r'\[([^\]]+)\]\(\s*(Aethel|Paradox|Weave|Chronon)\s*\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept")")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\s*["\']\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text](concept)")
                (r'\[([^\]]+)\]\(\s*(Aethel|Paradox|Weave|Chronon)\s*["\']\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept"")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept")")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)["\']\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept"")")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+["\']\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept")")")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)["\']\)["\']\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept"")")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+["\']\)["\']\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix [text]("concept"")")")
                (r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+["\']\)["\']\)["\']\)', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix links with missing closing parenthesis
                (r'(\[([^\]]+)\]\([^)]+)(?!\))', 
                 r'\1)'),
                
                # Fix links with extra quotes and parentheses
                (r'\[([^\]]+)\]\(\s*["\']?/core/concepts#([^"\')]+)["\']?\s*[)"\']*', 
                 r'[\1](/core/concepts#\2)'),
                
                # Fix links with multiple closing parentheses
                (r'(\[([^\]]+)\]\([^)]+)\)+', 
                 r'\1)'),
                
                # Fix links with escaped brackets
                (r'\\\[(.*?)\\\]', 
                 r'[\1]')
            ]
            
            new_text = text
            for pattern, replacement in patterns:
                try:
                    while True:
                        updated_text = re.sub(
                            pattern,
                            replacement,
                            new_text,
                            flags=re.IGNORECASE
                        )
                        if updated_text == new_text:
                            break
                        new_text = updated_text
                        # Print debug info for complex replacements
                        if pattern != patterns[-1][0]:  # Don't print for the last pattern
                            print(f"  - Applied pattern: {pattern[:50]}...")
                            print(f"    Replacement: {replacement}")
                except Exception as e:
                    print(f"  ! Error applying pattern: {str(e)}")
                    print(f"    Pattern: {pattern}")
                    print(f"    Replacement: {replacement}")
                    continue
            return new_text

        # Apply the specialized fixer first
        content = fix_nested_concept_links(content)
        
        # Apply each fixer pattern
        for fixer in self.compiled_patterns:
            try:
                content, count = re.subn(
                    fixer['pattern'],
                    fixer['replacement'],
                    content
                )
                fixes_applied += count
            except Exception as e:
                print(f"  ! Error applying {fixer['description']}: {str(e)}")
                continue

        return content, fixes_applied

    def process_file(self, file_path: Path) -> bool:
        """Process a single file to fix broken links."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()

            original_content = content
            content, fixes_applied = self.fix_links_in_file(content)
            
            # Only write if changes were made
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                self.links_fixed += fixes_applied
                print(f"  - Fixed {fixes_applied} broken links in {file_path.name}")
                return True
            
            return False
            
        except Exception as e:
            print(f"Error processing {file_path}: {str(e)}")
            return False
    
    def run(self):
        """Run the broken link fixer on all markdown files in the docs directory."""
        print("Fixing broken links in documentation files...")
        
        # Find all markdown files
        md_files = list(self.docs_dir.rglob('*.md'))
        
        for file_path in md_files:
            # Skip certain directories
            if 'node_modules' in str(file_path) or '.git' in str(file_path):
                continue
                
            print(f"Processing {file_path.relative_to(self.docs_dir)}...")
            if self.process_file(file_path):
                self.files_processed += 1
        
        print(f"\nBroken link fixing complete!")
        print(f"Files processed: {self.files_processed}")
        print(f"Total links fixed: {self.links_fixed}")


def main():
    """Main entry point for the broken link fixer."""
    fixer = BrokenLinkFixer()
    fixer.run()


if __name__ == "__main__":
    main()
