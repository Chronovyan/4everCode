#!/bin/bash

# Compiler Demo Script for Temporal Proving Grounds Integration
# This script demonstrates using the Chronovyan compiler with Temporal Proving Grounds validation

echo "=============================================="
echo "Chronovyan Compiler with Proving Grounds Demo"
echo "=============================================="
echo

# Define color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Path to the compiler executable (adjust as needed)
COMPILER="../build/bin/chronovyan_compiler"

# Path to example source files
EXAMPLES_DIR="./sources"
REPORTS_DIR="./validation_reports"

# Create directories if they don't exist
mkdir -p "$REPORTS_DIR"

# Function to run compiler with different options
run_compiler() {
    local source_file="$1"
    local options="$2"
    local description="$3"
    
    echo -e "${BLUE}--- $description ---${NC}"
    echo -e "${YELLOW}Command:${NC} $COMPILER $options $source_file"
    
    $COMPILER $options "$source_file"
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}Compilation successful!${NC}"
    else
        echo -e "${RED}Compilation failed!${NC}"
    fi
    echo
}

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo -e "${RED}Error: Compiler not found at $COMPILER${NC}"
    echo "Please build the compiler first or adjust the path in this script."
    exit 1
fi

# Check if we have example sources
if [ ! -d "$EXAMPLES_DIR" ]; then
    echo -e "${YELLOW}Warning: Examples directory not found.${NC}"
    echo "Creating examples directory with a sample program..."
    mkdir -p "$EXAMPLES_DIR"
    
    # Create a simple example with potential temporal paradoxes
    cat > "$EXAMPLES_DIR/simple_program.cvy" << 'EOF'
// Simple program with potential paradoxes
function main() {
    // Resource block with chronons
    resource(200) {
        // Variable declaration
        var counter = 0;
        
        // Loop that could create timeline branches
        for (var i = 0; i < 10; i++) {
            // Create a timeline branch with potential paradox
            timeline_branch(2) {
                // Modify counter in both branches
                counter += 1;
            }
        }
        
        // This might exceed available resources
        for (var j = 0; j < 100; j++) {
            resource(5) {
                // More nested operations
                timeline_branch(2) {
                    counter += 1;
                }
            }
        }
        
        // Print the final value
        print(counter);
    }
}
EOF
    
    echo -e "${GREEN}Created sample program at $EXAMPLES_DIR/simple_program.cvy${NC}"
    echo
fi

# Demo 1: Basic compilation without validation
run_compiler "$EXAMPLES_DIR/simple_program.cvy" "-o $EXAMPLES_DIR/simple_program.chron" "Basic compilation without validation"

# Demo 2: Compilation with Temporal Proving Grounds validation
run_compiler "$EXAMPLES_DIR/simple_program.cvy" "--validate -o $EXAMPLES_DIR/simple_program_validated.chron" "Compilation with validation enabled"

# Demo 3: Compilation with stricter paradox severity threshold
run_compiler "$EXAMPLES_DIR/simple_program.cvy" "--validate --paradox-severity 3 -o $EXAMPLES_DIR/simple_program_strict.chron" "Compilation with stricter paradox detection"

# Demo 4: Compilation with validation report in HTML format
run_compiler "$EXAMPLES_DIR/simple_program.cvy" "--validate --validation-report $REPORTS_DIR/validation_report.html --report-format html -o $EXAMPLES_DIR/simple_program_with_report.chron" "Compilation with HTML validation report"

# Demo 5: Compilation with validation report in JSON format (for tooling integration)
run_compiler "$EXAMPLES_DIR/simple_program.cvy" "--validate --validation-report $REPORTS_DIR/validation_report.json --report-format json -o $EXAMPLES_DIR/simple_program_json_report.chron" "Compilation with JSON validation report"

echo -e "${BLUE}=============================================${NC}"
echo -e "${GREEN}Demo completed!${NC}"
echo "Validation reports saved to $REPORTS_DIR/"
echo -e "${BLUE}=============================================${NC}" 