#!/usr/bin/env python3
"""
Chronovyan Link Fixer

This script fixes broken links in markdown documentation.
"""

import re
import os
from pathlib import Path
from typing import Dict, List, Optional, Tuple
import yaml

class LinkFixer:
    def __init__(self, root_dir: str = '.'):
        self.root_dir = Path(root_dir).resolve()
        self.docs_dir = self.root_dir / 'docs'
        self.files_processed = 0
        self.links_fixed = 0
        
        # Known concept mappings
        self.concept_links = {
            'CHRONON': '/core/concepts#chronon',
            'WEAVE': '/core/concepts#weave',
            'AETHEL': '/core/concepts#aethel',
            'PARADOX': '/core/concepts#paradox',
            # Add more concept mappings as needed
        }
        
        # Track all markdown files for resolving relative links
        self.markdown_files = {}
        self._build_markdown_index()
    
    def _build_markdown_index(self):
        """Build an index of all markdown files for link resolution."""
        for md_file in self.docs_dir.rglob('*.md'):
            if 'node_modules' in str(md_file) or '.git' in str(md_file):
                continue
            
            # Use relative path without extension as the key
            rel_path = md_file.relative_to(self.docs_dir)
            key = str(rel_path.with_suffix('')).replace('\\', '/')
            self.markdown_files[key.lower()] = f'/{key}.md'
    
    def _resolve_link(self, link: str, source_file: Path) -> str:
        """Resolve a relative link to an absolute path."""
        if not link or link.startswith(('http://', 'https://', 'mailto:', '#')):
            return link
            
        # Handle anchor links
        if link.startswith('#'):
            return link
            
        # Handle concept links
        for concept, url in self.concept_links.items():
            if concept in link.upper():
                return url
        
        # Handle relative paths
        if link.startswith(('./', '../')):
            try:
                # Resolve relative to source file
                abs_path = (source_file.parent / link).resolve()
                rel_path = abs_path.relative_to(self.docs_dir)
                return f'/{rel_path}'.replace('\\', '/')
            except (ValueError, FileNotFoundError):
                # If we can't resolve, return as is
                return link
        
        # Check if it's a direct reference to a markdown file
        if link.lower().endswith('.md'):
            return f'/{link}'
            
        # Try to find a matching markdown file
        link_key = link.lower().replace('\\', '/')
        if link_key in self.markdown_files:
            return self.markdown_files[link_key]
            
        # If we get here, we couldn't resolve the link
        return link
    
    def _fix_links_in_file(self, file_path: Path) -> bool:
        """Fix links in a single file."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            original_content = content
            
            # Fix markdown links [text](url)
            def replace_markdown_link(match):
                text = match.group(1)
                url = match.group(2)
                fixed_url = self._resolve_link(url, file_path)
                return f'[{text}]({fixed_url})'
            
            content = re.sub(r'\[([^\]]+)\]\(([^)]+)\)', replace_markdown_link, content)
            
            # Fix image links ![alt](url)
            def replace_image_link(match):
                alt = match.group(1)
                url = match.group(2)
                fixed_url = self._resolve_link(url, file_path)
                return f'![{alt}]({fixed_url})'
            
            content = re.sub(r'!\[([^\]]*)\]\(([^)]+)\)', replace_image_link, content)
            
            # Fix reference-style links [id]: url
            def replace_ref_link(match):
                ref_id = match.group(1)
                url = match.group(2)
                fixed_url = self._resolve_link(url, file_path)
                return f'[{ref_id}]: {fixed_url}'
            
            content = re.sub(r'^\[([^\]]+)\]:\s*([^\s]+)', replace_ref_link, content, flags=re.MULTILINE)
            
            # Only write if changes were made
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                self.links_fixed += 1
                return True
            
            return False
            
        except Exception as e:
            print(f"Error processing {file_path}: {str(e)}")
            return False
    
    def run(self):
        """Run the link fixer on all markdown files in the docs directory."""
        print("Fixing broken links in documentation files...")
        
        # Find all markdown files
        md_files = list(self.docs_dir.rglob('*.md'))
        
        for file_path in md_files:
            # Skip certain directories
            if 'node_modules' in str(file_path) or '.git' in str(file_path):
                continue
                
            print(f"Processing {file_path.relative_to(self.docs_dir)}...")
            if self._fix_links_in_file(file_path):
                self.files_processed += 1
        
        print(f"\nLink fixing complete!")
        print(f"Files processed: {self.files_processed}")
        print(f"Total links fixed: {self.links_fixed}")


def main():
    """Main entry point for the link fixer."""
    fixer = LinkFixer()
    fixer.run()


if __name__ == "__main__":
    main()
