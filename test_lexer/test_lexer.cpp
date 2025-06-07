#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "../src/compiler/include/compiler/Diagnostic.h"
#include "../src/compiler/include/compiler/Lexer.h"

using namespace chronovyan::compiler;

int main(int argc, char* argv[]) {
    // Check if a file was provided as an argument
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    // Read the source file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();

    // Create a diagnostic reporter
    auto reporter = std::make_shared<DiagnosticReporter>();

    // Create the lexer
    Lexer lexer(source, argv[1], reporter);

    // Tokenize the input
    std::vector<Token> tokens = lexer.tokenize();

    // Print the tokens
    for (const auto& token : tokens) {
        std::cout << token.toString() << "\n";
    }

    // Print any errors or warnings
    if (reporter->hasErrors()) {
        std::cerr << "\nDiagnostics:\n";
        for (const auto& diag : reporter->getDiagnostics()) {
            std::cerr << diag.toString() << "\n";
        }
    }

    return 0;
}
