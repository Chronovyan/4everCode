#!/usr/bin/env python3
"""
Fix file encodings in the documentation.

This script converts files from various encodings to UTF-8 and ensures
consistent line endings.
"""

import os
import codecs
import chardet
from pathlib import Path

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
ENCODINGS = ['utf-8', 'utf-16', 'utf-16-le', 'utf-16-be', 'latin-1']


def detect_encoding(filepath):
    """Detect the encoding of a file."""
    with open(filepath, 'rb') as f:
        raw_data = f.read()
        result = chardet.detect(raw_data)
        return result['encoding']


def convert_to_utf8(filepath, source_encoding):
    """Convert a file to UTF-8."""
    try:
        # Read the file with detected encoding
        with open(filepath, 'rb') as f:
            content = f.read().decode(source_encoding, errors='replace')
        
        # Create a backup
        backup_path = f"{filepath}.bak"
        if not os.path.exists(backup_path):
            os.rename(filepath, backup_path)
        
        # Write back as UTF-8 with LF line endings
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(content)
        
        return True
    except Exception as e:
        print(f"Error converting {filepath}: {e}")
        return False


def process_file(filepath):
    """Process a single file."""
    if filepath.suffix.lower() not in ['.md', '.txt', '']:
        return False
    
    try:
        # First try to detect encoding
        encoding = detect_encoding(filepath)
        
        if encoding and encoding.lower() != 'utf-8':
            print(f"Converting {filepath} from {encoding} to UTF-8")
            return convert_to_utf8(filepath, encoding)
            
    except Exception as e:
        print(f"Error processing {filepath}: {e}")
    
    return False


def main():
    """Main function."""
    processed = 0
    errors = 0
    
    for root, _, files in os.walk(DOCS_DIR):
        for file in files:
            if file.startswith('.'):
                continue
                
            filepath = Path(root) / file
            try:
                if process_file(filepath):
                    processed += 1
            except Exception as e:
                print(f"Failed to process {filepath}: {e}")
                errors += 1
    
    print(f"\nConversion complete!")
    print(f"Files processed: {processed}")
    print(f"Errors: {errors}")


if __name__ == "__main__":
    main()
