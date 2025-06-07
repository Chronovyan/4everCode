#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Compiler includes
#include "compiler/include/compiler/AstNode.h"
#include "compiler/include/compiler/BytecodeGenerator.h"
#include "compiler/include/compiler/CompilerDriver.h"
#include "compiler/include/compiler/Diagnostic.h"
#include "compiler/include/compiler/IR.h"
#include "compiler/include/compiler/bytecode/BytecodeModule.h"

// Interpreter includes
#include "interpreter/BytecodeInterpreter.h"

namespace chrono = chronovyan;
namespace fs = std::filesystem;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [args...]" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];

    try {
        // Create compiler driver with default settings
        chrono::compiler::DiagnosticReporter diagnostics;
        chrono::compiler::CompilerDriver driver(diagnostics);

        // Read the source file
        std::ifstream file(inputFile, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << inputFile << std::endl;
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        file.close();

        // Compile the source code
        bool compileSuccess = driver.compileString(source, inputFile);

        // Check for compilation errors
        if (!compileSuccess || diagnostics.hasErrors()) {
            std::cerr << "Compilation failed with errors:" << std::endl;
            for (const auto& diag : diagnostics.getDiagnostics()) {
                std::cerr << inputFile << ":" << diag.getLocation().line << ":"
                          << diag.getLocation().column << ": " << diag.getMessage() << std::endl;
            }
            return 1;
        }

        // Generate output file path
        std::string outputFile = inputFile;
        size_t lastDot = outputFile.find_last_of('.');
        if (lastDot != std::string::npos) {
            outputFile = outputFile.substr(0, lastDot);
        }
        outputFile += ".chron";

        // Set the output file before compiling
        driver.setOutputFile(outputFile);

        // Compile the source code
        if (!driver.compileString(source, inputFile)) {
            std::cerr << "Compilation failed" << std::endl;

            // Print any diagnostics
            for (const auto& diag : diagnostics.getDiagnostics()) {
                std::cerr << inputFile << ":" << diag.getLocation().line << ":"
                          << diag.getLocation().column << ": " << diag.getMessage() << std::endl;
            }
            return 1;
        }

        // Verify the output file was created
        if (!fs::exists(outputFile)) {
            std::cerr << "Error: Expected output file not created: " << outputFile << std::endl;
            return 1;
        }

        // For now, we'll just print a message since we don't have a way to load the module
        // from a file directly. The interpreter would need to be updated to support this.
        std::cout << "Successfully compiled to " << outputFile << std::endl;
        std::cout << "Note: Direct execution of compiled bytecode is not yet implemented."
                  << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
