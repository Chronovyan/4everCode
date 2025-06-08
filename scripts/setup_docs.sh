#!/bin/bash

# Chronovyan Documentation Setup Script
# This script sets up the development environment for building and contributing to Chronovyan's documentation

set -euo pipefail

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

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Start setup
echo -e "\n🚀 ${YELLOW}Setting up Chronovyan documentation environment...${NC}\n"

# Check if running as root
if [ "$EUID" -eq 0 ]; then
    echo -e "${YELLOW}Warning: Running as root is not recommended.${NC}"
    read -p "Continue anyway? [y/N] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Check Python installation
print_section "Checking Python Installation"

if ! command_exists python3; then
    echo -e "${RED}❌ Python is not found in PATH. Please install Python 3.8 or higher and try again.${NC}"
    echo -e "   Download Python: https://www.python.org/downloads/"
    exit 1
fi

# Check Python version
PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:3])))' 2>/dev/null || python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')
REQUIRED_VERSION="3.8.0"

if [ "$(printf '%s\n' "$REQUIRED_VERSION" "$PYTHON_VERSION" | sort -V | head -n1)" != "$REQUIRED_VERSION" ]; then
    echo -e "${RED}❌ Python 3.8 or higher is required. Found Python $PYTHON_VERSION${NC}"
    echo -e "   Please upgrade your Python installation and try again."
    exit 1
fi

echo -e "${GREEN}✓ Python $PYTHON_VERSION is installed${NC}"

# Set up virtual environment
print_section "Setting Up Virtual Environment"

VENV_PATH=".venv"

# Create virtual environment if it doesn't exist
if [ ! -d "$VENV_PATH" ]; then
    echo -e "${GREEN}Creating Python virtual environment...${NC}"
    python3 -m venv "$VENV_PATH"
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ Failed to create virtual environment${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Virtual environment created at $VENV_PATH${NC}"
else
    echo -e "${GREEN}✓ Using existing virtual environment at $VENV_PATH${NC}"
fi

# Activate virtual environment
if [ -f "$VENV_PATH/bin/activate" ]; then
    # Unix/macOS
    source "$VENV_PATH/bin/activate"
    echo -e "${GREEN}✓ Activated virtual environment${NC}"
else
    # Windows (Git Bash)
    if [ -f "$VENV_PATH/Scripts/activate" ]; then
        source "$VENV_PATH/Scripts/activate"
        echo -e "${GREEN}✓ Activated virtual environment${NC}"
    else
        echo -e "${RED}❌ Virtual environment activation script not found${NC}"
        exit 1
    fi
fi

# Upgrade pip and setuptools
print_section "Updating Python Packages"

echo -e "${GREEN}Upgrading pip and setuptools...${NC}"
python -m pip install --upgrade pip setuptools wheel

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Failed to upgrade pip and setuptools${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Successfully upgraded pip and setuptools${NC}"

# Install documentation dependencies
print_section "Installing Documentation Dependencies"

REQUIREMENTS_FILE="$(dirname "$0")/../docs-requirements.txt"

if [ ! -f "$REQUIREMENTS_FILE" ]; then
    echo -e "${RED}❌ Requirements file not found: $REQUIREMENTS_FILE${NC}"
    exit 1
fi

echo -e "${GREEN}Installing dependencies from $REQUIREMENTS_FILE...${NC}"
pip install -r "$REQUIREMENTS_FILE"

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Failed to install documentation dependencies${NC}"
    exit 1
fi

echo -e "${GREEN}✓ Successfully installed documentation dependencies${NC}"

# Install development tools
print_section "Installing Development Tools"

# Install pre-commit if not installed
if ! command_exists pre-commit; then
    echo -e "${GREEN}Installing pre-commit...${NC}"
    pip install pre-commit
    
    if [ $? -ne 0 ]; then
        echo -e "${RED}❌ Failed to install pre-commit${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}✓ Successfully installed pre-commit${NC}"
else
    echo -e "${GREEN}✓ pre-commit is already installed${NC}"
fi

# Set up pre-commit hooks
echo -e "\n${GREEN}Setting up pre-commit hooks...${NC}"
pre-commit install

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ Failed to set up pre-commit hooks${NC}"
    echo -e "${YELLOW}   You can continue without these tools, but they are recommended for development.${NC}"
    read -p "Continue anyway? [y/N] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
else
    echo -e "${GREEN}✓ Successfully set up pre-commit hooks${NC}"
fi

# Final instructions
print_section "Setup Complete!"

echo -e "🎉 ${GREEN}Documentation environment setup is complete!${NC}\n"
echo -e "Next steps:"
echo -e "1. To activate the virtual environment in a new terminal, run:"
echo -e "   ${CYAN}source $VENV_PATH/bin/activate${NC}"
echo -e "   (or ${CYAN}.\\$VENV_PATH\\Scripts\\activate${NC} on Windows)"
echo -e "2. To build the documentation, run:"
echo -e "   ${CYAN}mkdocs serve${NC}"
echo -e "3. Open http://127.0.0.1:8000 in your browser to view the documentation"
echo -e "\nHappy documenting! 🚀${YELLOW}"

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
