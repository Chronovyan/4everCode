#!/usr/bin/env python3
"""
Migrate documentation to the new structure.

This script helps migrate documentation files to the new structure
based on the mapping defined in the migration plan.
"""

import os
import shutil
from pathlib import Path
import yaml

# Configuration
DOCS_DIR = Path(__file__).parent.parent / 'docs'
MIGRATION_MAP = {
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

def ensure_directory(path):
    """Ensure the directory exists."""
    path.parent.mkdir(parents=True, exist_ok=True)

def update_frontmatter(filepath, new_path):
    """Update the frontmatter of a file with new path information."""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
        
        if not content.startswith('---'):
            return False
            
        parts = content.split('---', 2)
        if len(parts) < 3:
            return False
            
        frontmatter = yaml.safe_load(parts[1])
        
        # Update relevant frontmatter fields
        frontmatter['weight'] = get_weight_for_path(new_path)
        
        # Generate new frontmatter
        new_frontmatter = yaml.dump(frontmatter, default_flow_style=False, allow_unicode=True, sort_keys=False)
        new_content = f"---\n{new_frontmatter}---{parts[2]}"
        
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(new_content)
            
        return True
    except Exception as e:
        print(f"Error updating frontmatter for {filepath}: {e}")
        return False

def get_weight_for_path(path):
    """Get a weight based on the file path for consistent ordering."""
    weights = {
        'getting-started': 100,
        'tutorials': 200,
        'language': 300,
        'temporal-programming': 400,
        'advanced': 500,
        'api': 600,
        'development': 700,
        'community': 800,
    }
    
    for prefix, weight in weights.items():
        if path.startswith(prefix):
            return weight + len(path) * 5  # Add some weight based on path depth
    
    return 1000  # Default weight

def migrate_file(src_rel, dest_rel):
    """Migrate a single file to its new location."""
    src = DOCS_DIR / src_rel
    dest = DOCS_DIR / dest_rel
    
    if not src.exists():
        print(f"Source file not found: {src}")
        return False
    
    try:
        # Ensure destination directory exists
        ensure_directory(dest)
        
        # Copy the file
        shutil.copy2(src, dest)
        
        # Update frontmatter
        update_frontmatter(dest, str(dest_rel))
        
        print(f"Migrated: {src_rel} -> {dest_rel}")
        return True
    except Exception as e:
        print(f"Error migrating {src_rel} to {dest_rel}: {e}")
        return False

def main():
    """Main migration function."""
    migrated = 0
    errors = 0
    
    print("Starting documentation migration...\n")
    
    for src_rel, dest_rel in MIGRATION_MAP.items():
        if migrate_file(src_rel, dest_rel):
            migrated += 1
        else:
            errors += 1
    
    print(f"\nMigration complete!")
    print(f"Files migrated: {migrated}")
    print(f"Errors: {errors}")
    
    if migrated > 0:
        print("\nNext steps:")
        print("1. Review the migrated files")
        print("2. Update cross-references and links")
        print("3. Run 'mkdocs serve' to preview the documentation")
        print("4. Commit the changes to version control")

if __name__ == "__main__":
    main()
