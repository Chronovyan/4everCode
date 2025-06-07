#!/usr/bin/env python3
"""
Fix missing language specifiers in markdown code blocks.

This script scans markdown files for code blocks without language specifiers
and attempts to automatically detect and add the appropriate language.
"""

import re
import os
import os
import sys
import io
from pathlib import Path
from typing import List, Optional, Set, Dict, Tuple, Pattern, TextIO

# Set up console output to handle Unicode
if sys.stdout.encoding != 'utf-8':
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='utf-8', errors='replace')
if sys.stderr.encoding != 'utf-8':
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding='utf-8', errors='replace')

class LanguageSpecifierFixer:
    def __init__(self, root_dir: str):
        self.root_dir = Path(root_dir).resolve()
        self.processed_files: Set[Path] = set()
        
        # Language detection patterns with confidence scores
        self.language_patterns: Dict[str, List[Tuple[Pattern, float]]] = {
            'chronoscript': [
                (re.compile(r'weave\s*\{'), 0.9),
                (re.compile(r'chronon\s*\{'), 0.9),
                (re.compile(r'aethel\s*\{'), 0.9),
                (re.compile(r'temporal\s+'), 0.8),
                (re.compile(r'stabilize\s*\('), 0.8),
                (re.compile(r'PARADOX_CHECK\s*\('), 0.9),
                (re.compile(r'ANTECEDENCE\s*:'), 0.9),
                (re.compile(r'CONSEQUENCE\s*:'), 0.9),
                (re.compile(r'CHRONOSTREAM\s*\{'), 0.9)
            ],
            'cpp': [
                (re.compile(r'#include\s*[<"].+[">]'), 0.9),
                (re.compile(r'std::'), 0.8),
                (re.compile(r'int\s+main\s*\('), 0.9),
                (re.compile(r'class\s+\w+\s*\{'), 0.8),
                (re.compile(r'using\s+namespace\s+'), 0.9)
            ],
            'python': [
                (re.compile(r'^\s*(def|class)\s+\w+\s*[(:]'), 0.9),
                (re.compile(r'^\s*(import|from)\s+'), 0.9),
                (re.compile(r'print\s*\('), 0.7),
                (re.compile(r'->\s*\w+:'), 0.8),
                (re.compile(r'^\s*@'), 0.8)  # Decorators
            ],
            'bash': [
                (re.compile(r'^\s*\$?\s*(cd|ls|mkdir|echo|cat|grep|find)\s+'), 0.9),
                (re.compile(r'\$\{?\w+\}?'), 0.7),
                (re.compile(r'^\s*#!/.+\b(bash|sh)\b'), 1.0),
                (re.compile(r'\b(if|then|else|fi|for|do|done)\b'), 0.7)
            ],
            'yaml': [
                (re.compile(r'^\s*[a-zA-Z0-9_-]+:'), 0.8),
                (re.compile(r'^---'), 0.9),
                (re.compile(r'^\s+-\s*[a-zA-Z0-9"\']'), 0.8),
                (re.compile(r'^\s*#'), 0.5)  # Common in YAML but not exclusive
            ],
            'json': [
                (re.compile(r'^\s*[{[]'), 0.8),
                (re.compile(r'"[\w-]+"\s*:'), 0.9),
                (re.compile(r'\b(?:true|false|null)\b'), 0.7)
            ]
        }
        
        # Minimum confidence threshold for language detection
        self.min_confidence = 0.7
        
        # Maximum number of lines to analyze for language detection
        self.max_analyze_lines = 20
    
    def guess_language(self, code_block: str) -> Optional[str]:
        """
        Guess the programming language based on code block content.
        
        Args:
            code_block: The code block content to analyze
            
        Returns:
            The detected language as a string, or None if no confident match
        """
        # Check for empty or whitespace-only blocks
        if not code_block.strip():
            return None
            
        # Split into lines and take first few for analysis
        lines = code_block.split('\n')
        sample = '\n'.join(lines[:self.max_analyze_lines])
        
        # Calculate confidence scores for each language
        scores: Dict[str, float] = {}
        
        for lang, patterns in self.language_patterns.items():
            scores[lang] = 0.0
            for pattern, weight in patterns:
                if pattern.search(sample):
                    scores[lang] += weight
            
            # Normalize score by number of patterns
            if patterns:
                scores[lang] /= len(patterns)
        
        # Get the language with highest score above threshold
        best_lang = max(scores.items(), key=lambda x: x[1])
        
        if best_lang[1] >= self.min_confidence:
            return best_lang[0]
            
        return None
    
    def fix_file(self, file_path: Path) -> bool:
        """
        Fix language specifiers in a single markdown file.
        
        Args:
            file_path: Path to the markdown file to process
            
        Returns:
            bool: True if the file was modified, False otherwise
        """
        try:
            # Read file with original line endings
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                
            # Detect original line endings
            lines = content.splitlines(keepends=True)
            if not lines:
                return False
                
            modified = False
            in_code_block = False
            code_block_start = -1
            code_block_lines = []
            output_lines = []
            
            for i, line in enumerate(lines):
                stripped = line.strip()
                
                # Check for code block delimiters
                if stripped.startswith('```'):
                    if in_code_block:
                        # End of code block
                        in_code_block = False
                        
                        # Get the language specifier (if any)
                        lang_specifier = stripped[3:].strip()
                        
                        # If no language specifier, try to guess it
                        if not lang_specifier and code_block_lines:
                            code_content = ''.join(code_block_lines)
                            guessed_lang = self.guess_language(code_content)
                            
                            if guessed_lang:
                                # Replace the opening backticks with language specifier
                                output_lines[code_block_start] = f'```{guessed_lang}\n'
                                modified = True
                        
                        # Reset for next code block
                        code_block_lines = []
                        code_block_start = -1
                    else:
                        # Start of code block
                        in_code_block = True
                        code_block_start = len(output_lines)
                        code_block_lines = []  # Reset code block content
                    
                    output_lines.append(line)
                elif in_code_block:
                    # Inside a code block - collect content for analysis
                    code_block_lines.append(line)
                    output_lines.append(line)
                else:
                    # Outside code block
                    output_lines.append(line)
            
            # Write back only if modified
            if modified:
                # Preserve original line endings
                new_content = ''.join(output_lines)
                with open(file_path, 'w', encoding='utf-8', newline='') as f:
                    f.write(new_content)
                return True
                
            return False
            
        except Exception as e:
            error_msg = f"Error processing {file_path}: {e}"
            try:
                print(error_msg, file=sys.stderr)
            except UnicodeEncodeError:
                # Fallback for environments that can't handle Unicode
                print(error_msg.encode('ascii', 'replace').decode('ascii'), file=sys.stderr)
            return False
    
    def process_directory(self, directory: Path):
        """
        Process all markdown files in the given directory recursively.
        
        Args:
            directory: Directory to search for markdown files
            
        Returns:
            int: Number of files that were modified
        """
        processed_count = 0
        total_files = 0
        
        # First count total files for progress reporting
        for root, _, files in os.walk(directory):
            for file in files:
                if file.endswith(('.md', '.markdown')):
                    file_path = Path(root) / file
                    if 'node_modules' not in str(file_path):
                        total_files += 1
        
        print(f"Scanning {total_files} markdown files...")
        
        # Process files
        for root, _, files in os.walk(directory):
            for file in files:
                if file.endswith(('.md', '.markdown')):
                    file_path = (Path(root) / file).resolve()
                    
                    # Handle relative path for display
                    try:
                        rel_path = file_path.relative_to(self.root_dir)
                    except ValueError:
                        rel_path = file_path
                    
                    # Skip node_modules and already processed files
                    if 'node_modules' in str(file_path) or file_path in self.processed_files:
                        continue
                        
                    self.processed_files.add(file_path)
                    if self.fix_file(file_path):
                        # Use ASCII-friendly checkmark for Windows
                        if os.name == 'nt':
                            print(f"[FIXED] Language specifiers in {rel_path}")
                        else:
                            print(f"✓ Fixed language specifiers in {rel_path}")
                        processed_count += 1
        
        return processed_count


def main():
    import argparse
    
    parser = argparse.ArgumentParser(description='Fix missing language specifiers in markdown code blocks.')
    parser.add_argument('--dir', type=str, default='.', 
                       help='Root directory of the documentation (default: current directory)')
    
    args = parser.parse_args()
    fixer = LanguageSpecifierFixer(args.dir)
    
    print("Starting language specifier fixing...")
    processed = fixer.process_directory(Path(args.dir))
    print(f"\nProcessed {processed} files.")

if __name__ == "__main__":
    main()
