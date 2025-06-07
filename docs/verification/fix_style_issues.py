#!/usr/bin/env python3
"""
Chronovyan Documentation Style Fixer

This script automatically fixes common style issues in markdown documentation.
"""

import re
import os
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import textwrap

class MarkdownStyleFixer:
    def __init__(self, root_dir: str = '.'):
        self.root_dir = Path(root_dir).resolve()
        self.docs_dir = self.root_dir / 'docs'
        self.files_processed = 0
        self.fixes_applied = 0
        
        # Common patterns to fix - ordered by complexity and dependency
        self.patterns = [
            # 1. Fix code blocks first (they should be processed before other patterns)
            (r'^\s{1,3}(```[a-z]*\n[\s\S]*?```)', self._fix_code_block_indent),
            
            # 2. Fix inline code and formatting
            (r'``([^`\n]+)``', r'`\1`'),  # Convert double backticks to single
            (r'`([^`\n]+)\s+`', r'`\1`'),  # Remove spaces around inline code
            
            # 3. Fix bold/italic formatting
            (r'(?<!\*)\*\*(?=\S)([^*]+?)(?<=\S)\*\*(?!\*)', r'**\1**'),
            (r'(?<!_)_(?=\S)([^_]+?)(?<=\S)_(?!_)', r'_\1_'),
            (r'\*\*\s+([^*]+?)\s+\*\*', r'**\1**'),  # Remove spaces inside **bold**
            (r'\*\s+([^*]+?)\s+\*', r'*\1*'),  # Remove spaces inside *italic*
            
            # 4. Fix headings
            (r'^#+([^#\n\s][^\n]*)(?<!#)(?<!\s#)(?<!\s)$', self._fix_heading_style),
            (r'^([#]+)\s*\n+\1\s+', '\1 '),  # Fix headings with extra newlines
            
            # 5. Fix lists
            (r'^(\s*)[-*+]\s+', self._fix_list_indent),
            (r'^(\s*\d+)\.(\s+)(?=\S)', r'\1. '),  # Fix numbered list spacing
            (r'^(\s*)[-*+]\s{0,1}$', ''),  # Remove empty list items
            
            # 6. Fix blockquotes
            (r'^>\s*\n(?!\s*$)', '>\n', re.MULTILINE),
            (r'^>(\s*\n)+', '>\n'),  # Remove extra newlines in blockquotes
            
            # 7. Fix tables
            (r'\|\s*:?-+:?\s*\|', self._fix_table_divider),
            (r'\|\s*\|', '|'),  # Fix empty table cells
            
            # 8. Fix code blocks with missing language
            (r'```\s*\n(\s*[^\s`])', '```text\n\1'),
            (r'```\s*\n\s*\n', '```text\n'),
            
            # 9. Fix spacing and newlines
            (r'([^\n])\n#', '\1\n\n#'),  # Ensure blank before heading
            (r'#([^#\n]+)\n([^\n])', '#\1\n\n\2'),  # Ensure blank after heading
            (r'([^\s])(\n\s*\n)(\s*[-*+])', '\1\n\3'),  # Fix list spacing
            (r'\n{3,}', '\n\n'),  # Limit consecutive newlines
            (r' +\n', '\n'),  # Remove trailing spaces
            (r'\t', '    '),  # Convert tabs to spaces
            
            # 10. Fix markdown links and images
            (r'\[([^\]]+)\]\(([^)]+)\)', self._fix_markdown_links),
            (r'!\[([^\]]*)\]\(([^)]+)\)', self._fix_image_links),
        ]
        
        # Compile patterns for better performance
        self.compiled_patterns = [(re.compile(pattern[0], re.MULTILINE), pattern[1]) 
                                if callable(pattern[1]) else 
                                (re.compile(pattern[0], re.MULTILINE), pattern[1]) 
                                for pattern in self.patterns]
    
    def _fix_code_block_indent(self, match) -> str:
        """Fix indentation in code blocks to use 4 spaces."""
        code_block = match.group(1)
        lines = code_block.split('\n')
        if len(lines) < 2:
            return code_block
            
        # Find minimum indentation (excluding first and last lines which are ```)
        min_indent = float('inf')
        for line in lines[1:-1]:
            if line.strip() == '':
                continue
            indent = len(line) - len(line.lstrip())
            if indent < min_indent:
                min_indent = indent
        
        if min_indent == float('inf'):
            min_indent = 0
        
        # Normalize indentation
        new_lines = [lines[0]]  # Opening ```
        for line in lines[1:-1]:
            if line.strip() == '':
                new_lines.append('')
            else:
                indent = len(line) - len(line.lstrip())
                new_indent = 4 + (indent - min_indent)
                new_lines.append(' ' * new_indent + line.lstrip())
        new_lines.append(lines[-1])  # Closing ```
        
        return '\n'.join(new_lines)
    
    def _fix_heading_style(self, match) -> str:
        """Ensure consistent heading style with space after #."""
        heading = match.group(0)
        if ' ' not in heading.lstrip('#'):
            return heading  # No text after #
        
        # Count # characters
        level = len(heading) - len(heading.lstrip('#'))
        text = heading[level:].strip()
        
        # Rebuild with proper spacing
        return f'{"#" * level} {text}'
    
    def _fix_list_indent(self, match) -> str:
        """Ensure consistent list item indentation (2 spaces)."""
        leading_space = match.group(1)
        if len(leading_space) % 2 != 0:  # Not a multiple of 2
            return f'{leading_space} '  # Add one space
        return match.group(0)
        
    def _fix_table_divider(self, match) -> str:
        """Ensure consistent table dividers."""
        divider = match.group(0).strip()
        if '---' in divider:
            columns = divider.count('|') - 1
            if columns > 0:
                return '| ' + ' | '.join(['---'] * columns) + ' |'
        return match.group(0)
        
    def _fix_markdown_links(self, match) -> str:
        """Fix markdown link formatting."""
        text = match.group(1).strip()
        url = match.group(2).strip()
        
        # Remove any spaces around the URL
        url = url.strip()
        
        # Handle empty links
        if not url:
            return f'[{text}]()'
            
        # Handle reference-style links
        if url.startswith('[') and ']:' in url:
            return match.group(0)
            
        # Handle mailto and other special URLs
        if '://' not in url and not url.startswith(('#', '/', 'mailto:')):
            url = f'/{url}'
            
        return f'[{text}]({url})'
    
    def _fix_image_links(self, match) -> str:
        """Fix image link formatting."""
        alt_text = match.group(1).strip()
        url = match.group(2).strip()
        
        # Remove any spaces around the URL
        url = url.strip()
        
        # Handle empty URLs
        if not url:
            return f'![{alt_text}]()'
            
        # Handle relative paths
        if not url.startswith(('http://', 'https://', '/', 'data:')):
            url = f'/docs/{url}'
            
        return f'![{alt_text}]({url})'
    
    def fix_file(self, file_path: Path) -> bool:
        """Fix style issues in a single file."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            original_content = content
            
            # Apply all patterns
            for pattern, replacement in self.compiled_patterns:
                if callable(replacement):
                    content = pattern.sub(replacement, content)
                else:
                    content = pattern.sub(replacement, content)
            
            # Only write if changes were made
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                self.fixes_applied += 1
                return True
            
            return False
            
        except Exception as e:
            print(f"Error processing {file_path}: {str(e)}")
            return False
    
    def run(self):
        """Run the style fixer on all markdown files in the docs directory."""
        print("Fixing style issues in documentation files...")
        
        # Find all markdown files
        md_files = list(self.docs_dir.rglob('*.md'))
        
        for file_path in md_files:
            # Skip certain directories
            if 'node_modules' in str(file_path) or '.git' in str(file_path):
                continue
                
            print(f"Processing {file_path.relative_to(self.docs_dir)}...")
            if self.fix_file(file_path):
                self.files_processed += 1
        
        print(f"\nStyle fixes complete!")
        print(f"Files processed: {self.files_processed}")
        print(f"Total fixes applied: {self.fixes_applied}")


def main():
    """Main entry point for the style fixer."""
    fixer = MarkdownStyleFixer()
    fixer.run()


if __name__ == "__main__":
    main()
