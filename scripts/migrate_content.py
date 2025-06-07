#!/usr/bin/env python3
"""
Migrate content to the new documentation structure.

This script helps migrate content from the old documentation structure
to the new one, updating links and references as needed.
"""

import os
import re
import shutil
import yaml
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Set
import frontmatter
import markdown
from bs4 import BeautifulSoup
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(),
        logging.FileHandler('migration.log')
    ]
)

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
OLD_STRUCTURE = {
    # Core Concepts
    'core/Core Concepts - The Foundation of Temporal Programming.md': 'temporal-programming/concepts.md',
    'core/Getting Started - The Path to Temporal Mastery.md': 'getting-started/quick-start.md',
    'core/Manifesto.md': 'temporal-programming/philosophy.md',
    
    # Tutorials
    'tutorials/act1/chapter1_variables.md': 'getting-started/tutorials/basics/variables.md',
    'tutorials/act1/chapter2_conditionals.md': 'getting-started/tutorials/basics/conditionals.md',
    'tutorials/act1/chapter3_loops.md': 'getting-started/tutorials/basics/loops.md',
    'tutorials/act1/chapter4_functions.md': 'getting-started/tutorials/basics/functions.md',
    'tutorials/act1/chapter5_stabilizing_coras_power_core.md': 'getting-started/tutorials/basics/first-program.md',
    
    # Language Reference
    'core/Core Syntax & Constructs.md': 'language/syntax.md',
    'language_reference/rewind_flow.md': 'language/control-flow.md',
    'specifications/Chronovyan_Data_Types.md': 'language/types.md',
    'specifications/Chronovyan_Formal_Grammar.md': 'language/grammar.md',
    'specifications/Chronovyan_Runtime_Semantics.md': 'language/runtime.md',
    
    # Advanced Topics
    'advanced/Advanced Quantum Operations - The Art of Temporal Manipulation.md': 'advanced/quantum-operations.md',
    'advanced/Development Tools - The Art of Temporal Crafting.md': 'development/tools.md',
    'advanced/Loop Mechanics - The Art of Temporal Iteration.md': 'temporal-programming/loops.md',
    'advanced/error_handling/Advanced Error Handling and Recovery - The Art of Temporal Resilience.md': 'temporal-programming/error-handling.md',
    'advanced/error_handling/Advanced Paradox Management - The Art of Temporal Harmony.md': 'temporal-programming/paradox-avoidance.md',
    
    # API Reference
    'library/Core Utilities - The Foundation of Temporal Operations.md': 'api/standard-library.md',
    'library/Temporal Library - The Art of Time Manipulation.md': 'api/temporal-library.md',
    'specifications/Chronovyan_Variable_Interaction_Rules.md': 'api/variable-interaction.md',
    
    # Community
    'troubleshooting/FAQ - Common Questions and Answers.md': 'community/faq.md',
    'troubleshooting/Troubleshooting - The Art of Temporal Debugging.md': 'community/troubleshooting.md',
}

# Reverse mapping for link updates
NEW_TO_OLD = {v: k for k, v in OLD_STRUCTURE.items()}
OLD_TO_NEW = OLD_STRUCTURE

class Document:
    """Represents a documentation file with its content and metadata."""
    
    def __init__(self, path: Path, content: str = None):
        self.path = path
        self.relative_path = path.relative_to(DOCS_DIR) if path.is_relative_to(DOCS_DIR) else path
        self.content = content or self._read_file()
        self.frontmatter = {}
        self.body = ""
        self._parse_content()
    
    def _read_file(self) -> str:
        """Read the file content with proper encoding handling."""
        try:
            return self.path.read_text(encoding='utf-8')
        except UnicodeDecodeError:
            try:
                return self.path.read_text(encoding='latin-1')
            except Exception as e:
                logging.error(f"Failed to read {self.path}: {e}")
                return ""
    
    def _parse_content(self):
        """Parse the file content into frontmatter and body."""
        if not self.content.strip().startswith('---'):
            self.body = self.content
            return
        
        try:
            # Use frontmatter library to parse YAML frontmatter
            parsed = frontmatter.loads(self.content)
            self.frontmatter = parsed.metadata
            self.body = parsed.content
        except Exception as e:
            logging.warning(f"Failed to parse frontmatter in {self.path}: {e}")
            self.body = self.content
    
    def save(self, path: Path = None):
        """Save the document to the specified path."""
        target_path = path or self.path
        target_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Only write frontmatter if it exists
        if self.frontmatter:
            frontmatter_text = frontmatter.dumps(
                frontmatter.Post(self.body, **self.frontmatter)
            )
            target_path.write_text(frontmatter_text, encoding='utf-8')
        else:
            target_path.write_text(self.body, encoding='utf-8')
        
        logging.info(f"Saved {target_path}")
    
    def update_links(self, link_mapping: Dict[str, str]):
        """Update links in the document based on the provided mapping."""
        if not self.body:
            return
        
        # Convert markdown to HTML to parse links
        html = markdown.markdown(self.body, extensions=['fenced_code', 'tables'])
        soup = BeautifulSoup(html, 'html.parser')
        
        # Update links
        for a in soup.find_all('a', href=True):
            old_href = a['href']
            
            # Skip external links and anchors
            if old_href.startswith(('http://', 'https://', 'mailto:', '#')):
                continue
            
            # Remove leading ./ if present
            if old_href.startswith('./'):
                old_href = old_href[2:]
            
            # Get the relative path
            old_path = self.path.parent / old_href
            rel_path = old_path.relative_to(DOCS_DIR) if old_path.is_relative_to(DOCS_DIR) else Path(old_href)
            
            # Check if this path needs to be updated
            if str(rel_path) in link_mapping:
                new_path = link_mapping[str(rel_path)]
                # Make the path relative to the current file
                rel_new_path = os.path.relpath(
                    DOCS_DIR / new_path,
                    self.path.parent
                ).replace('\\', '/')
                a['href'] = rel_new_path
                logging.debug(f"Updated link in {self.path}: {old_href} -> {rel_new_path}")
        
        # Convert back to markdown (this is a simplification, in practice you might want to preserve more structure)
        self.body = str(soup)

def migrate_file(source: Path, target: Path, link_mapping: Dict[str, str]) -> bool:
    """Migrate a single file to its new location and update its links."""
    try:
        if not source.exists():
            logging.warning(f"Source file not found: {source}")
            return False
        
        # Create target directory if it doesn't exist
        target.parent.mkdir(parents=True, exist_ok=True)
        
        # Read and parse the source file
        doc = Document(source)
        
        # Update links in the document
        doc.update_links(link_mapping)
        
        # Save to the new location
        doc.save(target)
        
        logging.info(f"Migrated {source} -> {target}")
        return True
    except Exception as e:
        logging.error(f"Error migrating {source} to {target}: {e}")
        return False

def generate_link_mapping() -> Dict[str, str]:
    """Generate a mapping of old paths to new paths for link updates."""
    mapping = {}
    
    # Add explicit mappings
    for old_path, new_path in OLD_TO_NEW.items():
        mapping[old_path] = new_path
    
    # Add reverse mappings for directories
    for old_path, new_path in OLD_TO_NEW.items():
        # Add mapping for the directory
        old_dir = str(Path(old_path).parent)
        new_dir = str(Path(new_path).parent)
        if old_dir != '.':
            mapping[old_dir] = new_dir
    
    return mapping

def main():
    """Main migration function."""
    logging.info("Starting documentation migration...")
    
    # Generate link mapping
    link_mapping = generate_link_mapping()
    
    # Process each file in the mapping
    success_count = 0
    error_count = 0
    
    for old_rel_path, new_rel_path in OLD_TO_NEW.items():
        source = DOCS_DIR / old_rel_path
        target = DOCS_DIR / new_rel_path
        
        if migrate_file(source, target, link_mapping):
            success_count += 1
        else:
            error_count += 1
    
    # Summary
    logging.info("\nMigration Summary:")
    logging.info(f"- Successfully migrated: {success_count} files")
    logging.info(f"- Failed to migrate: {error_count} files")
    
    if error_count > 0:
        logging.warning("Some files failed to migrate. Check the log for details.")
    
    logging.info("\nMigration complete!")
    logging.info("\nNext steps:")
    logging.info("1. Review the migrated files")
    logging.info("2. Update any remaining hardcoded links")
    logging.info("3. Run 'mkdocs serve' to preview the documentation")
    logging.info("4. Commit the changes to version control")

if __name__ == "__main__":
    main()
