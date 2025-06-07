#!/usr/bin/env python3
"""
Chronovyan Documentation Audit Tool

This script analyzes the current documentation structure and generates a report
of what needs to be migrated to the new structure.
"""

import os
import yaml
import sys
from pathlib import Path
from collections import defaultdict

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
REPORT_FILE = DOCS_DIR / 'audit-report.md'

# New structure categories
NEW_CATEGORIES = [
    'getting-started',
    'language',
    'temporal-programming',
    'advanced',
    'api',
    'development',
    'community'
]

def get_file_stats(filepath):
    """Get basic statistics about a file."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        return {
            'size': os.path.getsize(filepath),
            'lines': len(content.splitlines()),
            'words': len(content.split()),
            'has_frontmatter': content.startswith('---\n')
        }
    except Exception as e:
        print(f"Error processing {filepath}: {e}", file=sys.stderr)
        return None

def analyze_docs():
    """Analyze the documentation directory structure."""
    docs = {
        'by_extension': defaultdict(int),
        'files': [],
        'total_size': 0,
        'total_files': 0
    }
    
    for root, _, files in os.walk(DOCS_DIR):
        for file in files:
            if file.startswith('.'):
                continue
                
            filepath = Path(root) / file
            rel_path = filepath.relative_to(DOCS_DIR)
            
            # Skip certain directories
            if any(part.startswith('.') for part in rel_path.parts):
                continue
                
            ext = filepath.suffix.lower()
            stats = get_file_stats(filepath)
            
            if stats:
                docs['by_extension'][ext] += 1
                docs['total_size'] += stats['size']
                docs['total_files'] += 1
                
                file_info = {
                    'path': str(rel_path),
                    'extension': ext,
                    'size': stats['size'],
                    'lines': stats['lines'],
                    'words': stats['words'],
                    'has_frontmatter': stats['has_frontmatter']
                }
                docs['files'].append(file_info)
    
    return docs

def generate_report(docs):
    """Generate a markdown report from the analysis."""
    report = [
        "# Chronovyan Documentation Audit Report",
        "\n## Overview",
        f"- **Total Files**: {docs['total_files']}",
        f"- **Total Size**: {docs['total_size'] / 1024:.2f} KB",
        "\n## File Types",
        "| Extension | Count |",
        "|-----------|-------|",
    ]
    
    for ext, count in sorted(docs['by_extension'].items()):
        report.append(f"| {ext} | {count} |")
    
    report.extend([
        "\n## Documentation Status",
        "| File | Lines | Words | Has Frontmatter |",
        "|------|-------|-------|-----------------|",
    ])
    
    for file_info in sorted(docs['files'], key=lambda x: x['path']):
        report.append(
            f"| {file_info['path']} | {file_info['lines']} | "
            f"{file_info['words']} | {'✅' if file_info['has_frontmatter'] else '❌'} |"
        )
    
    # Add recommendations
    report.extend([
        "\n## Recommendations",
        "1. Add frontmatter to all documentation files",
        "2. Standardize on .md extension for markdown files",
        "3. Organize files according to the new structure",
        "4. Remove or update outdated documentation",
    ])
    
    return '\n'.join(report)

def main():
    print("Analyzing documentation structure...")
    docs = analyze_docs()
    
    print("Generating report...")
    report = generate_report(docs)
    
    with open(REPORT_FILE, 'w', encoding='utf-8') as f:
        f.write(report)
    
    print(f"Report generated: {REPORT_FILE}")
    print(f"\nSummary:")
    print(f"- {docs['total_files']} files analyzed")
    print(f"- {sum(1 for f in docs['files'] if f['has_frontmatter'])}/{docs['total_files']} files have frontmatter")
    print(f"- {len([f for f in docs['files'] if f['extension'] == '.md'])} markdown files found")

if __name__ == "__main__":
    main()
