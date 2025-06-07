#!/usr/bin/env python3
"""
Comprehensive link fixer for Chronovyan documentation.
Fixes various link issues including:
- Broken concept links
- Missing language specifiers in code blocks
- Missing concept links
- Improperly formatted links
"""

import re
import os
import argparse
from pathlib import Path
from typing import List, Dict, Set, Tuple, Optional

class ComprehensiveLinkFixer:
    def __init__(self, root_dir: str):
        self.root_dir = Path(root_dir)
        self.core_concepts_file = self.root_dir / 'core' / 'Core Concepts - The Foundation of Temporal Programming.md'
        self.processed_files: Set[Path] = set()
        
        # Known concepts and their anchors
        self.concept_anchors = {
            'aethel': 'aethel',
            'chronon': 'chronon',
            'weave': 'weave',
            'paradox': 'paradox',
            'anchor': 'anchor',
            'temporal': 'temporal',
            'flux': 'flux',
            'stabilize': 'stabilize',
            'stability': 'stability',
            'const': 'const',
            'double': 'double',
            'static': 'static',
            'conf': 'conformity',
            'reb': 'rebellion',
            'resource': 'resource-system',
            'variable': 'variable-system',
            'loop': 'temporal-loops',
            'quantum': 'quantum-operations'
        }
        
        # Known file extensions for code blocks
        self.code_extensions = {
            'chronoscript', 'python', 'cpp', 'c', 'h', 'hpp', 'js', 'ts',
            'java', 'go', 'rust', 'ruby', 'php', 'bash', 'sh', 'sql', 'json',
            'yaml', 'yml', 'toml', 'ini', 'xml', 'html', 'css', 'scss', 'sass'
        }

    def fix_links_in_file(self, file_path: Path) -> bool:
        """Fix links and code blocks in a single file."""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            original_content = content
            
            # Fix concept links
            for concept, anchor in self.concept_anchors.items():
                # Handle various link formats
                patterns = [
                    # [text](/path#concept)
                    (fr'(\[.*?\]\()/core/concepts#{concept}([^a-zA-Z0-9-]|$)', 
                     fr'\1/core/Core Concepts - The Foundation of Temporal Programming.md#{anchor}\2'),
                    # [text](/path#CONCEPT)
                    (fr'(\[.*?\]\()/core/concepts#{concept.upper()}([^a-zA-Z0-9-]|$)', 
                     fr'\1/core/Core Concepts - The Foundation of Temporal Programming.md#{anchor}\2'),
                    # [CONCEPT]() - missing link
                    (fr'(\[[^\]]*?\b){concept.upper()}\b([^]]*?\]\()(?!http)[^)]*(\))', 
                     fr'\\1{concept.upper()}\\2/core/Core Concepts - The Foundation of Temporal Programming.md#{anchor}\\3'),
                    # [Concept]() - missing link
                    (fr'(\[[^\]]*?\b){concept.capitalize()}\b([^]]*?\]\()(?!http)[^)]*(\))', 
                     fr'\\1{concept.capitalize()}\\2/core/Core Concepts - The Foundation of Temporal Programming.md#{anchor}\\3')
                ]
                
                for pattern, replacement in patterns:
                    content = re.sub(pattern, replacement, content, flags=re.IGNORECASE)
            
            # Fix code blocks without language specifiers
            content = self.fix_code_blocks(content)
            
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                return True
            return False
            
        except Exception as e:
            print(f"Error processing {file_path}: {e}")
            return False
    
    def fix_code_blocks(self, content: str) -> str:
        """Add language specifiers to code blocks where missing."""
        # Split content into lines for easier processing
        lines = content.split('\n')
        i = 0
        
        while i < len(lines):
            line = lines[i].strip()
            
            # Look for code block delimiters
            if line.startswith('```') and len(line) > 3 and not any(ext in line.lower() for ext in self.code_extensions):
                # Found a code block without a language specifier
                # Look at the next non-empty line to guess the language
                j = i + 1
                while j < len(lines) and not lines[j].strip():
                    j += 1
                
                if j < len(lines):
                    first_line = lines[j].strip()
                    lang = self.guess_language(first_line)
                    if lang:
                        lines[i] = f'```{lang}'
            
            i += 1
        
        return '\n'.join(lines)
    
    def guess_language(self, first_line: str) -> str:
        """Guess the programming language based on the first line of code."""
        first_line = first_line.strip()
        
        # Check for Chronoscript patterns
        if any(keyword in first_line.lower() for keyword in ['weave', 'chronon', 'aethel', 'temporal', 'paradox']):
            return 'chronoscript'
            
        # Check for C/C++ patterns
        if any(keyword in first_line for keyword in ['#include', 'std::', 'int main', 'using namespace']):
            return 'cpp'
            
        # Check for Python patterns
        if any(keyword in first_line for keyword in ['def ', 'import ', 'print(', 'class ']):
            return 'python'
            
        # Default to text if we can't determine
        return ''
    
    def process_directory(self, directory: Path):
        """Process all markdown files in the given directory."""
        processed_count = 0
        
        for root, _, files in os.walk(directory):
            for file in files:
                if file.endswith(('.md', '.markdown')):
                    file_path = Path(root) / file
                    if file_path not in self.processed_files and 'node_modules' not in str(file_path):
                        if self.fix_links_in_file(file_path):
                            print(f"Fixed links in {file_path.relative_to(self.root_dir)}")
                            processed_count += 1
        
        return processed_count

def main():
    parser = argparse.ArgumentParser(description='Fix broken links and code blocks in documentation.')
    parser.add_argument('--dir', type=str, default='.', 
                       help='Root directory of the documentation (default: current directory)')
    
    args = parser.parse_args()
    fixer = ComprehensiveLinkFixer(args.dir)
    
    print("Starting comprehensive link and code block fixing...")
    processed = fixer.process_directory(Path(args.dir))
    print(f"\nProcessed {processed} files.")

if __name__ == "__main__":
    main()
