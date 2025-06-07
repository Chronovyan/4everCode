#!/usr/bin/env python3
"""
Fix malformed links in markdown files, particularly those with broken concept links.
"""
import re
import os
from pathlib import Path
from typing import List, Tuple, Optional, Dict, Any

class MalformedLinkFixer:
    def __init__(self, root_dir: str):
        self.root_dir = Path(root_dir).resolve()
        self.files_processed = 0
        self.links_fixed = 0
        self.patterns = self._get_patterns()
    
    def _get_patterns(self) -> List[Tuple[re.Pattern, str]]:
        """Return a list of (pattern, replacement) tuples for fixing malformed links."""
        return [
            # Fix [text]("concept") where concept is a known term
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\s*\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\s*["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text](concept)")
            (re.compile(r'\[([^\]]+)\]\(\s*(Aethel|Paradox|Weave|Chronon)\s*["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept"")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept"")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")...
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)\.\.\.', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept"...
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\.\.\.', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text](concept...
            (re.compile(r'\[([^\]]+)\]\(\s*(Aethel|Paradox|Weave|Chronon)\.\.\.', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)\s*["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept"")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept"")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept"")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']+["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)["\']\)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("concept")")")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\'](Aethel|Paradox|Weave|Chronon)["\']\)["\']\)["\']\)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text](/core/concepts#concept)")
            (re.compile(r'\[([^\]]+)\]\(\s*/core/concepts#([^"\')]+)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text](/core/concepts#concept)")")
            (re.compile(r'\[([^\]]+)\]\(\s*/core/concepts#([^"\')]+)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("/core/concepts#concept")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\']/core/concepts#([^"\')]+)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("/core/concepts#concept")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\']/core/concepts#([^"\')]+)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("/core/concepts#concept")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\']/core/concepts#([^"\')]+)["\']\)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("/core/concepts#concept")")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\']/core/concepts#([^"\')]+)["\']\)["\']\)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("/core/concepts#concept")")")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\']/core/concepts#([^"\')]+)["\']\)["\']\)["\']\)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
            
            # Fix [text]("/core/concepts#concept")")")")")")")
            (re.compile(r'\[([^\]]+)\]\(\s*["\']/core/concepts#([^"\')]+)["\']\)["\']\)["\']\)["\']\)["\']\)["\']\)["\']\)', re.IGNORECASE),
             r'[\1](/core/concepts#\2)'),
        ]
    
    def fix_malformed_links_in_file(self, file_path: Path) -> int:
        """Fix malformed links in a single file and return the number of fixes applied."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            original_content = content
            fixes_applied = 0
            
            # Apply each pattern
            for pattern, replacement in self.patterns:
                new_content, count = pattern.subn(replacement, content)
                if count > 0:
                    content = new_content
                    fixes_applied += count
            
            # Only write if changes were made
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                self.links_fixed += fixes_applied
                print(f"  - Fixed {fixes_applied} malformed links in {file_path.name}")
                return fixes_applied
            
            return 0
            
        except Exception as e:
            print(f"Error processing {file_path}: {str(e)}")
            return 0
    
    def process_directory(self, directory: Path) -> None:
        """Process all markdown files in the given directory and its subdirectories."""
        for root, _, files in os.walk(directory):
            for file in files:
                if file.endswith('.md'):
                    file_path = Path(root) / file
                    self.files_processed += 1
                    self.fix_malformed_links_in_file(file_path)
        
        print(f"\nMalformed link fixing complete!")
        print(f"Files processed: {self.files_processed}")
        print(f"Total malformed links fixed: {self.links_fixed}")

def main():
    # Get the root directory of the project
    root_dir = Path(__file__).parent.parent.parent
    
    print(f"Fixing malformed links in {root_dir}...\n")
    
    fixer = MalformedLinkFixer(root_dir)
    fixer.process_directory(root_dir / 'docs')

if __name__ == "__main__":
    main()
