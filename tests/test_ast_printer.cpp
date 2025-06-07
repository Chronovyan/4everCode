#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "compiler/AstPrinter.h"
#include "compiler/CompilerDriver.h"
#include "compiler/Diagnostic.h"
#include "compiler/Lexer.h"
#include "compiler/Parser.h"

// Enable debug output
#define DEBUG_OUTPUT 1

using namespace chronovyan::compiler;

// Test source code directly embedded
const char* TEST_SOURCE = R"(
// Simple test program for AST printer
let x: int = 42

// Parallel execution
parallel 4 {
    print("Hello from thread")
}

// Branch timeline
branch 3 {
    print("Hello from branch")
}

// Resource statement
resource allocate {
    chronons: 100
    aethel: 50
} {
    print("Using resources")
}
)";

int main(int argc, char* argv[]) {
    // Use current directory for output files
    const std::string outputPath = "ast_printer_output.bin";
    const std::string debugPath = "debug_output.txt";

    // Open output file in binary mode
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return 1;
    }

    // Redirect std::cout to file
    std::streambuf* coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(outFile.rdbuf());

    std::cout << "=== Test Program Started ===\n";
    std::cout << "Current working directory: " << std::filesystem::current_path() << "\n";
    std::cout << "Test source code will be written to: " << outputPath << "\n\n";

    // Write a test pattern to verify binary output
    const char test_pattern[] = "TEST_PATTERN_START_1234567890_END";
    outFile.write(test_pattern, sizeof(test_pattern) - 1);
    outFile.flush();

    // No need to redirect again, we already did it above
    std::cout << "=== Test Program Started ===" << std::endl;
    std::cout << "=== Test Source ===" << std::endl;
    std::string source = TEST_SOURCE;
    std::cout << source << std::endl;
    std::cout << "===================" << std::endl;

    try {
        // Set up compiler components
        std::cout << "Creating lexer..." << std::endl;
        Lexer lexer(source, "test_ast.chron");
        std::cout << "Lexer created successfully." << std::endl;

        std::cout << "Creating parser..." << std::endl;
        Parser parser(lexer);
        std::cout << "Parser created successfully." << std::endl;

#if DEBUG_OUTPUT
        std::cout << "\n=== Source Code ===" << std::endl;
        std::cout << source << std::endl;
        std::cout << "===================" << std::endl;

        std::cout << "\n=== Tokens ===" << std::endl;
        // Print tokens for debugging
        Token token;
        do {
            token = lexer.nextToken();
            std::cout << "Token: " << token.getLexeme()
                      << " (Type: " << static_cast<int>(token.getType())
                      << ", Line: " << token.getLocation().line << ")" << std::endl;
        } while (token.getType() != TokenType::END_OF_FILE);

        // Reset lexer position
        lexer = Lexer(source, "test_ast.chron");
#endif

        std::cout << "\nParsing source..." << std::endl;
        auto ast = parser.parse();

        if (ast) {
            std::cout << "\nParse successful!" << std::endl;

            // Print the AST
            std::cout << "\n=== AST Output ===" << std::endl;
            AstPrinter printer(std::cout);
            ast->accept(printer);
            std::cout << std::endl;

            // Restore original buffers
            std::cout.rdbuf(coutbuf);
            std::cerr.rdbuf(cerrbuf);

            std::ofstream debugFile("debug_output.txt");
            debugFile << "Test completed successfully. Check ast_printer_output.txt for results."
                      << std::endl;
            return 0;
        } else {
            std::cerr << "Failed to parse source" << std::endl;

            // Restore original buffers before exiting
            std::cout.rdbuf(coutbuf);
            std::cerr.rdbuf(cerrbuf);

            std::ofstream debugFile("debug_output.txt");
            debugFile << "Test failed. Check ast_printer_output.txt for details." << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;

        // Restore original cout buffer
        std::cout.rdbuf(coutbuf);

        // Close the output file
        outFile.close();

        std::cout << "Test completed successfully. Check " << outputPath << " for results."
                  << std::endl;
        std::cout << "File size: " << std::filesystem::file_size(outputPath) << " bytes"
                  << std::endl;

        return 0;
    }
}
