#!/usr/bin/env python3
"""
Chronovyan Concept Linker

This script adds missing concept links to markdown documentation.
"""

import re
from pathlib import Path
from typing import Dict, List, Optional, Tuple

class ConceptLinker:
    def __init__(self, root_dir: str = '.'):
        self.root_dir = Path(root_dir).resolve()
        self.docs_dir = self.root_dir / 'docs'
        self.files_processed = 0
        self.links_added = 0
        
        # Concept definitions with their display names and URLs
        self.concepts = {
            'CHRONON': {
                'name': 'Chronon',
                'url': '/core/concepts#chronon',
                'description': 'The fundamental unit of time in Chronovyan',
                'aliases': ['CHRONONS']
            },
            'WEAVE': {
                'name': 'Weave',
                'url': '/core/concepts#weave',
                'description': 'The fabric of time that connects all events',
                'aliases': ['WEAVING', 'WEAVES']
            },
            'AETHEL': {
                'name': 'Aethel',
                'url': '/core/concepts#aethel',
                'description': 'The energy that powers temporal operations',
                'aliases': ['AETHEL ENERGY']
            },
            'PARADOX': {
                'name': 'Paradox',
                'url': '/core/concepts#paradox',
                'description': 'A temporal inconsistency that must be resolved',
                'aliases': ['PARADOXES', 'PARADOXICAL']
            },
            'CHRONOVYAN': {
                'name': 'Chronovyan',
                'url': '/',
                'description': 'The temporal programming language and runtime',
                'aliases': []
            }
        }
        
        # Build regex patterns for each concept
        self.patterns = {}
        for concept, data in self.concepts.items():
            # Create a pattern that matches the concept and its aliases
            aliases = [concept] + data['aliases']
            # Match whole words only, case-insensitive
            pattern = r'\b(' + '|'.join(re.escape(a) for a in aliases) + r')\b'
            self.patterns[concept] = re.compile(pattern, re.IGNORECASE)
    
    def _add_concept_links(self, content: str) -> str:
        """Add concept links to content."""
        # Track which concepts we've already linked to avoid duplicates
        linked_concepts = set()
        
        def replace_concept(match):
            matched_text = match.group(0)
            
            # Skip if already linked
            if match.start() > 0 and content[match.start()-1] == '[' and '](' in content[match.start()-10:match.start()]:
                return matched_text
                
            # Find which concept was matched
            for concept, pattern in self.patterns.items():
                if pattern.fullmatch(matched_text):
                    # Only link the first occurrence of each concept
                    if concept in linked_concepts:
                        return matched_text
                    
                    linked_concepts.add(concept)
                    concept_data = self.concepts[concept]
                    return f'[{concept_data["name"]}]({concept_data["url"]} "{concept_data["description"]}")'
            
            return matched_text
        
        # Process each concept in the content
        for concept in self.concepts:
            content = self.patterns[concept].sub(replace_concept, content)
        
        return content
    
    def process_file(self, file_path: Path) -> bool:
        """Process a single file to add concept links."""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Skip files that already have concept links
            if any(f'[{concept_data["name"]}](' in content for concept_data in self.concepts.values()):
                return False
            
            original_content = content
            
            # Process the content
            content = self._add_concept_links(content)
            
            # Only write if changes were made
            if content != original_content:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                self.links_added += 1
                return True
            
            return False
            
        except Exception as e:
            print(f"Error processing {file_path}: {str(e)}")
            return False
    
    def run(self):
        """Run the concept linker on all markdown files in the docs directory."""
        print("Adding concept links to documentation files...")
        
        # Find all markdown files
        md_files = list(self.docs_dir.rglob('*.md'))
        
        for file_path in md_files:
            # Skip certain directories
            if 'node_modules' in str(file_path) or '.git' in str(file_path):
                continue
                
            print(f"Processing {file_path.relative_to(self.docs_dir)}...")
            if self.process_file(file_path):
                self.files_processed += 1
        
        print(f"\nConcept linking complete!")
        print(f"Files processed: {self.files_processed}")
        print(f"Total concept links added: {self.links_added}")


def main():
    """Main entry point for the concept linker."""
    linker = ConceptLinker()
    linker.run()


if __name__ == "__main__":
    main()
