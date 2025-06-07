import os
import re
import yaml
from pathlib import Path
from datetime import datetime
from typing import Dict, Any, Optional, Tuple

def extract_title_from_content(content: str) -> str:
    """Extract title from content if it starts with a heading."""
    lines = content.split('\n')
    for line in lines:
        if line.startswith('# '):
            return line[2:].strip()
    return ''

def clean_front_matter(content: str) -> Tuple[Dict[str, Any], str]:
    """Clean and parse front matter, returning the front matter and the rest of the content."""
    if not content.startswith('---\n'):
        return {}, content
    
    try:
        # Find the end of the front matter
        end_idx = content.index('\n---', 4)  # Skip the first '---' and find the next one
        front_matter_str = content[4:end_idx].strip()
        
        # Clean up markdown links in front matter
        front_matter_str = re.sub(r'\[(.+?)\]\(([^)]+)\)', r'\1', front_matter_str)
        
        # Parse the cleaned YAML
        front_matter = yaml.safe_load(front_matter_str) or {}
        return front_matter, content[end_idx + 5:].lstrip('\n')
    except (yaml.YAMLError, ValueError) as e:
        print(f"Error parsing front matter: {e}")
        return {}, content

def generate_front_matter(file_path: Path, content: str) -> Dict[str, Any]:
    """Generate front matter with sensible defaults."""
    title = file_path.stem.replace('_', ' ').title()
    description = extract_title_from_content(content) or f"Documentation for {title}"
    
    return {
        'title': title,
        'description': description,
        'date_created': datetime.now().strftime('%Y-%m-%d'),
        'status': 'draft',
        'last_updated': datetime.now().strftime('%Y-%m-%d')
    }

def update_front_matter(file_path: Path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Get default front matter
    default_front_matter = generate_front_matter(file_path, content)
    
    # Clean existing front matter if it exists
    front_matter, rest_content = clean_front_matter(content)
    
    # Update front matter with defaults for missing fields
    updated = False
    for key, default_value in default_front_matter.items():
        if key not in front_matter:
            front_matter[key] = default_value
            updated = True
    
    # If we had to update the front matter, write it back
    if updated or content.startswith('---'):
        yaml_content = yaml.dump(front_matter, default_flow_style=False, sort_keys=False)
        new_content = f"---\n{yaml_content}---\n\n{rest_content}"
        
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print(f"Updated front matter in {file_path}")
    else:
        print(f"No updates needed for {file_path}")
    
    # The actual update logic is now in the clean_front_matter and generate_front_matter functions
    # This function now just orchestrates the process

def main():
    root_dir = Path('.').resolve()
    
    # Find all markdown files in the docs directory
    docs_dir = root_dir / 'docs'
    if not docs_dir.exists():
        print("Error: 'docs' directory not found in the current working directory.")
        return
    
    # Process all markdown files in the docs directory and subdirectories
    for file_path in docs_dir.rglob('*.md'):
        # Skip files in certain directories if needed
        if 'node_modules' in str(file_path) or '.git' in str(file_path):
            continue
            
        print(f"Processing {file_path}...")
        try:
            update_front_matter(file_path)
        except Exception as e:
            print(f"Error processing {file_path}: {str(e)}")
    
    print("\nFront matter update complete!")

if __name__ == "__main__":
    main()
