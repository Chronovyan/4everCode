#!/bin/bash

# New Documentation Generator
# This script creates a new documentation page with proper front matter and structure
# SAFETY: This script will create new files and modify mkdocs.yml
# Run with --dry-run to see what would be created without making changes

set -euo pipefail

# Initialize dry run flag
DRY_RUN=false
if [[ "$*" == *"--dry-run"* ]]; then
    DRY_RUN=true
    echo "🔍 DRY RUN MODE - No changes will be made"
fi

# Function to safely create a file
safe_create_file() {
    local file_path="$1"
    local content="$2"
    
    if [ "$DRY_RUN" = true ]; then
        echo "[DRY RUN] Would create file: $file_path"
        echo "--- START OF: $file_path ---"
        echo -e "$content"
        echo -e "--- END OF: $file_path ---\n"
    else
        # Create directory if it doesn't exist
        mkdir -p "$(dirname "$file_path")"
        # Create file with content
        echo -e "$content" > "$file_path"
        echo "✅ Created file: $file_path"
    fi
}

# Function to safely update mkdocs.yml
update_mkdocs_nav() {
    local doc_path="$1"
    local doc_category="$2"
    
    if [ "$DRY_RUN" = true ]; then
        echo "[DRY RUN] Would update mkdocs.yml to include: $doc_category/$doc_path"
        return
    fi
    
    # Backup mkdocs.yml
    if [ ! -f "mkdocs.yml.bak" ]; then
        cp mkdocs.yml mkdocs.yml.bak
        echo "ℹ️  Created backup: mkdocs.yml.bak"
    fi
    
    # Update mkdocs.yml
    if ! grep -q "${doc_category}/${doc_path}" "mkdocs.yml"; then
        sed -i.bak -e "s|  - ${doc_category}:|  - ${doc_category}:\n      - ${doc_category}/${doc_path%.md}|g" "mkdocs.yml"
        echo "✅ Updated mkdocs.yml navigation"
    fi
}

# Function to confirm before proceeding
confirm() {
    if [ "$DRY_RUN" = true ]; then
        return 0
    fi
    
    local message="$1"
    local default="${2:-y}"
    
    read -p "$message [$default] " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$|^$ && "$default" == "y" ]] || \
       [[ $REPLY =~ ^[Nn]$|^$ && "$default" == "n" ]]; then
        return 0
    else
        echo "Operation cancelled by user"
        exit 1
    fi
}

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to print section headers
print_section() {
    local title="$1"
    local width=80
    local padding=$((($width - ${#title}) / 2))
    
    echo -e "\n${CYAN}${title}${NC}"
    echo -e "${CYAN}$(printf '=%.0s' $(seq 1 $width))${NC}\n"
}

# Function to prompt for input with a default value
prompt() {
    local prompt="$1"
    local default="$2"
    local var="$3"
    
    if [ -n "$default" ]; then
        read -p "${prompt} [${default}]: " $var
        eval "${var}=${!var:-$default}"
    else
        while true; do
            read -p "${prompt}: " $var
            if [ -n "${!var}" ]; then
                break
            else
                echo -e "${YELLOW}This field is required.${NC}"
            fi
        done
    fi
}

# Function to generate a slug from a title
generate_slug() {
    echo "$1" | \
    tr '[:upper:]' '[:lower:]' | \
    tr ' ' '-' | \
    tr -cd '[:alnum:]-' | \
    sed 's/--*/-/g' | \
    sed 's/^-//;s/-$//'
}

# Main script
print_section "📄 New Chronovyan Documentation Page"

# Get document details
DOC_TITLE=""
DOC_DESCRIPTION=""
DOC_CATEGORY=""
DOC_WEIGHT=10
DOC_AUTHOR=""

prompt "Document title" "" DOC_TITLE
prompt "Short description" "" DOC_DESCRIPTION
prompt "Category (guides/reference/development/design/community)" "" DOC_CATEGORY
prompt "Navigation weight" "10" DOC_WEIGHT
prompt "Author" "Chronovyan Team" DOC_AUTHOR

# Generate slug from title
DOC_SLUG=$(generate_slug "$DOC_TITLE")
DOC_FILENAME="${DOC_SLUG}.md"

# Determine the directory based on category
case "$DOC_CATEGORY" in
    guides|reference|development|design|community)
        DOC_DIR="docs/$DOC_CATEGORY"
        ;;
    *)
        echo -e "${YELLOW}Invalid category. Using 'docs' as the default directory.${NC}"
        DOC_DIR="docs"
        ;;
esac

# Create the directory if it doesn't exist
mkdir -p "$DOC_DIR"

# Generate the file path
DOC_PATH="${DOC_DIR}/${DOC_FILENAME}"

# Check if file already exists
if [ -f "$DOC_PATH" ]; then
    echo -e "${YELLOW}Warning: File ${DOC_PATH} already exists.${NC}"
    read -p "Overwrite? [y/N] " OVERWRITE
    if [[ ! "$OVERWRITE" =~ ^[Yy]$ ]]; then
        echo -e "${YELLOW}Aborted.${NC}"
        exit 1
    fi
fi

# Get current date in ISO 8601 format
CURRENT_DATE=$(date +"%Y-%m-%d")

# Generate the front matter
FRONT_MATTER="---\n"
FRONT_MATTER+="title: \"${DOC_TITLE}\"\n"
FRONT_MATTER+="description: \"${DOC_DESCRIPTION}\"\n"
FRONT_MATTER+="date: ${CURRENT_DATE}\
"
FRONT_MATTER+="weight: ${DOC_WEIGHT}\
"
FRONT_MATTER+="authors: [\"${DOC_AUTHOR}\"]\n"
FRONT_MATTER+="---\n\n"

# Generate the document content
DOC_CONTENT="${FRONT_MATTER}# ${DOC_TITLE}\n\n"
DOC_CONTENT+="${DOC_DESCRIPTION}\n\n"
DOC_CONTENT+="## Overview\n\n"
DOC_CONTENT+="Add a brief overview of what this document covers.\n\n"
DOC_CONTENT+="## Prerequisites\n\n"
DOC_CONTENT+="- Prerequisite 1\n"
DOC_CONTENT+="- Prerequisite 2\n\n"
DOC_CONTENT+="## Instructions\n\n"
DOC_CONTENT+="1. Step 1\n"
DOC_CONTENT+="2. Step 2\n"
DOC_CONTENT+="3. Step 3\n\n"
DOC_CONTENT+="## Examples\n\n"
DOC_CONTENT+="```rust\n// Example code here\nfn main() {\n    println!(\"Hello, Chronovyan!\");\n}\n```\n\n"
DOC_CONTENT+="## Troubleshooting\n\n"
DOC_CONTENT+="| Issue | Solution |\n"
DOC_CONTENT+="|-------|----------|\n"
DOC_CONTENT+="| Common issue 1 | Solution 1 |\n"
DOC_CONTENT+="| Common issue 2 | Solution 2 |\n\n"
DOC_CONTENT+="## See Also\n\n"
DOC_CONTENT+="- [Related Document 1]()\n"
DOC_CONTENT+="- [Related Document 2]()\n"

# Create the document
safe_create_file "$DOC_PATH" "$DOC_CONTENT"

# Update mkdocs.yml if the category exists
if [ -f "mkdocs.yml" ] && [ -n "$DOC_CATEGORY" ] && [ -d "docs/$DOC_CATEGORY" ]; then
    # Check if the document is already in the nav
    if ! grep -q "${DOC_CATEGORY}/${DOC_FILENAME}" "mkdocs.yml"; then
        confirm "Add '${DOC_CATEGORY}/${DOC_FILENAME}' to mkdocs.yml navigation?" "y"
        update_mkdocs_nav "${DOC_FILENAME%%.md}" "$DOC_CATEGORY"
    fi
fi

if [ "$DRY_RUN" = false ]; then
    echo -e "\n${GREEN}✓ Created new documentation page at: ${DOC_PATH}${NC}"
else
    echo -e "\n${YELLOW}✓ Dry run complete. No files were modified.${NC}"
fi
echo -e "\nNext steps:"
echo -e "1. Edit the file to add your content"
echo -e "2. Preview with: ${CYAN}mkdocs serve${NC}"
echo -e "3. Commit your changes"

exit 0
