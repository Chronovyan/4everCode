#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "ast_nodes.h"
#include "compiler/Lexer.h"
#include "diagnostic_reporter.h"
#include "error_handler.h"
#include "parser.h"
#include "source_file.h"
#include "token.h"

namespace fs = std::filesystem;

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::string inputFile = "test.chron";

    // Use command line argument if provided
    if (argc > 1) {
        inputFile = argv[1];
    }

    // Get absolute path to the input file
    fs::path filePath = fs::absolute(inputFile);

    std::cout << "Testing Chronovyan Parser with file: " << filePath << std::endl;

    try {
        // Read the source file
        std::string source = readFile(filePath.string());

        // Create necessary components
        DiagnosticReporter reporter;
        auto sourceFile = std::make_shared<SourceFile>(filePath.filename().string(), source);
        auto lexer = std::make_shared<Lexer>(sourceFile, reporter);
        auto parser = std::make_unique<Parser>(lexer, reporter);

        std::cout << "Parsing source..." << std::endl;

        // Parse the source
        auto ast = parser->parse();

        // Check for errors
        if (reporter.hasErrors()) {
            std::cerr << "\nParser encountered " << reporter.getErrors().size()
                      << " error(s):" << std::endl;
            for (const auto& error : reporter.getErrors()) {
                std::cerr << "- " << error << std::endl;
            }
            return 1;
        }

        std::cout << "\nSuccessfully parsed the source!" << std::endl;

        // Print AST (simplified)
        if (ast) {
            std::cout << "\nAST Root Type: " << typeid(*ast).name() << std::endl;
            // Print a simple representation of the AST
            std::cout << "\nAST Structure (simplified):" << std::endl;
            // TODO: Add AST pretty printing
        } else {
            std::cout << "\nWarning: AST is null" << std::endl;
        }

        std::cout << "\nParsing completed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
