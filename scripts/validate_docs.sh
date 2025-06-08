#!/bin/bash

# Documentation Validation Script
# This script checks for common documentation issues and validates the structure

set -euo pipefail

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print section headers
print_section() {
    local title="$1"
    local width=80
    local padding=$((($width - ${#title}) / 2))
    
    echo -e "\n${CYAN}${title}${NC}"
    echo -e "${CYAN}$(printf '=%.0s' $(seq 1 $width))${NC}\n"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check if a file exists
file_exists() {
    if [ ! -f "$1" ]; then
        echo -e "${RED}❌ Missing file: $1${NC}"
        return 1
    fi
    return 0
}

# Function to check if a directory exists
dir_exists() {
    if [ ! -d "$1" ]; then
        echo -e "${RED}❌ Missing directory: $1${NC}"
        return 1
    fi
    return 0
}

# Function to check for broken links in a markdown file
check_markdown_links() {
    local file_path="$1"
    local base_dir=$(dirname "$file_path")
    local content=$(cat "$file_path")
    local has_errors=false
    
    # Find all markdown links [text](url)
    local link_pattern='\[([^\]]+)\]\(([^)]+)\)'
    
    # Extract all links using grep and process them
    while IFS= read -r line; do
        # Extract the URL part from the markdown link
        local link_url=$(echo "$line" | sed -E 's/\[.*\]\((.*)\)/\1/' | cut -d' ' -f1 | tr -d '"' | tr -d "'")
        local link_text=$(echo "$line" | sed -E 's/\[(.*)\].*/\1/')
        
        # Skip empty or anchor-only links
        [ -z "$link_url" ] && continue
        
        # Skip external links (http, https, ftp, mailto)
        if [[ "$link_url" =~ ^(https?|ftp|mailto):// ]]; then
            continue
        fi
        
        # Handle anchor links (starting with #)
        if [[ "$link_url" =~ ^# ]]; then
            local anchor=${link_url:1}
            if ! echo "$content" | grep -q -E "^#+\s+$anchor\b" && 
               ! echo "$content" | grep -q -E "id=[\"']$anchor[\"']"; then
                echo -e "${RED}❌ Broken anchor link in $(basename "$file_path"): $link_url (text: $link_text)${NC}"
                has_errors=true
            fi
            continue
        fi
        
        # Handle relative links
        local full_path="$base_dir/$link_url"
        
        # Handle links with anchors (e.g., file.md#section)
        if [[ "$link_url" =~ ^(.+?)(#.+)?$ ]]; then
            local file_part="${BASH_REMATCH[1]}"
            local anchor_part="${BASH_REMATCH[2]:-}"
            
            if [ -n "$file_part" ]; then
                full_path=$(realpath -m "$base_dir/$file_part" 2>/dev/null || echo "")
                
                # Check if file exists
                if [ ! -e "$full_path" ]; then
                    echo -e "${RED}❌ Broken link in $(basename "$file_path"): $link_url (text: $link_text)${NC}"
                    has_errors=true
                    continue
                fi
                
                # If there's an anchor, check it exists in the target file
                if [ -n "$anchor_part" ] && [ -f "$full_path" ]; then
                    local target_content=$(cat "$full_path" 2>/dev/null || echo "")
                    if [ -z "$target_content" ]; then
                        echo -e "${YELLOW}⚠️  Could not check anchor in $link_url (file not readable)${NC}"
                        continue
                    fi
                    
                    local anchor=${anchor_part:1}  # Remove the #
                    if ! echo "$target_content" | grep -q -E "^#+\s+$anchor\b" && 
                       ! echo "$target_content" | grep -q -E "id=[\"']$anchor[\"']"; then
                        echo -e "${RED}❌ Broken anchor in $link_url: $anchor_part (linked from $(basename "$file_path"))${NC}"
                        has_errors=true
                    fi
                fi
            fi
        fi
    done < <(grep -oP '\[.*?\]\(.*?\)' "$file_path" | grep -v '^!\[')
    
    if $has_errors; then
        return 1
    fi
    return 0
}

# Main script
print_section "🔍 Validating Chronovyan Documentation"

# Check required files
print_section "Checking Required Files"

required_files=(
    "README.md"
    "CONTRIBUTING.md"
    "CODE_OF_CONDUCT.md"
    "LICENSE"
    "mkdocs.yml"
    "docs/index.md"
    "docs/development/contributing.md"
    "docs/development/documentation-guide.md"
)

all_files_exist=true
for file in "${required_files[@]}"; do
    if ! file_exists "$file"; then
        all_files_exist=false
    else
        echo -e "${GREEN}✓ Found: $file${NC}"
    fi
done

# Check required directories
print_section "Checking Directory Structure"

required_dirs=(
    "docs/guides"
    "docs/reference"
    "docs/development"
    "docs/design"
    "docs/community"
)

all_dirs_exist=true
for dir in "${required_dirs[@]}"; do
    if ! dir_exists "$dir"; then
        all_dirs_exist=false
    else
        echo -e "${GREEN}✓ Found directory: $dir${NC}"
    fi
done

# Check markdown files for issues
print_section "Checking Markdown Files"

# Find all markdown files, excluding node_modules and venv directories
markdown_files=$(find . -name "*.md" -not -path "*/node_modules/*" -not -path "*/venv/*" -not -path "*/.venv/*" -not -path "*/.git/*")

has_markdown_issues=false

for file in $markdown_files; do
    relative_path=$(realpath --relative-to=. "$file")
    echo -e "${BLUE}Checking: $relative_path${NC}"
    
    # Check for broken links
    if ! check_markdown_links "$file"; then
        has_markdown_issues=true
    fi
    
    # Check for TODO/FIXME comments
    if grep -q -i -E 'TODO|FIXME|XXX' "$file"; then
        echo -e "${YELLOW}⚠️  Contains TODO/FIXME/XXX in $relative_path${NC}"
    fi
    
    # Check for trailing whitespace
    if grep -q -E ' +$' "$file"; then
        echo -e "${YELLOW}⚠️  Contains trailing whitespace in $relative_path${NC}"
    fi
done

# Summary
print_section "Validation Summary"

if $all_files_exist; then
    echo -e "${GREEN}✓ All required files exist${NC}"
else
    echo -e "${RED}❌ Some required files are missing${NC}"
fi

if $all_dirs_exist; then
    echo -e "${GREEN}✓ Directory structure is valid${NC}"
else
    echo -e "${RED}❌ Some required directories are missing${NC}"
fi

if $has_markdown_issues; then
    echo -e "${RED}❌ Some markdown files have issues${NC}"
else
    echo -e "${GREEN}✓ No issues found in markdown files${NC}"
fi

if $all_files_exist && $all_dirs_exist && ! $has_markdown_issues; then
    echo -e "\n🎉 ${GREEN}Documentation validation passed successfully!${NC}"
    exit 0
else
    echo -e "\n❌ ${RED}Documentation validation found issues that need to be addressed.${NC}"
    exit 1
fi
