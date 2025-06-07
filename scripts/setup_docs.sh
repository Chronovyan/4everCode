#!/bin/bash

# Chronovyan Documentation Setup Script
# This script sets up the development environment for the documentation

set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}Setting up Chronovyan documentation environment...${NC}\n"

# Check if Python is installed
if ! command -v python3 &> /dev/null; then
    echo "Python 3 is required but not installed. Please install Python 3.8 or higher and try again."
    exit 1
fi

# Check Python version
PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')
if [[ "$PYTHON_VERSION" < "3.8" ]]; then
    echo "Python 3.8 or higher is required. Found Python $PYTHON_VERSION"
    exit 1
fi

# Create virtual environment if it doesn't exist
if [ ! -d "venv" ]; then
    echo -e "${GREEN}Creating virtual environment...${NC}"
    python3 -m venv venv
fi

# Activate virtual environment
if [ -f "venv/bin/activate" ]; then
    # Unix/macOS
    source venv/bin/activate
else
    # Windows
    source venv/Scripts/activate
fi

# Upgrade pip
echo -e "\n${GREEN}Upgrading pip...${NC}"
python -m pip install --upgrade pip

# Install documentation dependencies
echo -e "\n${GREEN}Installing documentation dependencies...${NC}"
pip install -r docs-requirements.txt

# Create necessary directories
echo -e "\n${GREEN}Setting up directory structure...${NC}"
mkdir -p docs/assets/{images,js,css}
mkdir -p docs/{getting-started,language,temporal-programming,advanced,api,development,community}

# Create placeholder files
for dir in docs/{getting-started,language,temporal-programming,advanced,api,development,community}; do
    touch "$dir/.gitkeep"
done

# Create .gitignore if it doesn't exist
if [ ! -f ".gitignore" ]; then
    echo -e "\n${GREEN}Creating .gitignore...${NC}"
    cat > .gitignore << EOL
# Virtual environment
venv/
ENV/

# Python cache
__pycache__/
*.py[cod]
*$py.class

# MkDocs build output
site/

# Local development
.DS_Store
Thumbs.db

# IDE specific files
.vscode/
.idea/
*.swp
*.swo

# Environment variables
.env

# Logs
logs/
*.log
EOL
fi

echo -e "\n${GREEN}Documentation setup complete!${NC}"
echo -e "\nTo start the development server, run: ${YELLOW}mkdocs serve${NC}"
echo -e "To build the documentation, run: ${YELLOW}mkdocs build${NC}"
echo -e "\nFor more information, see the documentation at ${YELLOW}http://localhost:8000${NC} when the server is running."
