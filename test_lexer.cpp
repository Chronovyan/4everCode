#include <iostream>
#include <string>
#include <vector>

#include "compiler/Diagnostic.h"
#include "compiler/Lexer.h"
#include "compiler/SourceFile.h"

using namespace chronovyan::compiler;

void testCharacterLiterals() {
    std::cout << "=== Testing Character Literals ===" << std::endl;

    // Test cases
    std::vector<std::string> testCases = {
        "'a'",     // Simple character
        "'\\n'",   // Newline
        "'\\t'",   // Tab
        "'\\\\'",  // Backslash
        "'\\''",   // Single quote
        "'\\x41'"  // Hex value 'A'
    };

    for (const auto& testCase : testCases) {
        std::cout << "Testing: " << testCase << std::endl;

        // Create a source file with the test case
        auto sourceFile = std::make_shared<SourceFile>(testCase, "test.chron");

        // Create a diagnostic reporter
        auto diagnosticReporter = std::make_shared<DiagnosticReporter>();
        diagnosticReporter->setDebugOutput(true);

        // Create and run the lexer
        Lexer lexer(sourceFile, diagnosticReporter);
        auto tokens = lexer.tokenize();

        // Print the results
        if (tokens.empty()) {
            std::cout << "  No tokens generated!" << std::endl;
            continue;
        }

        std::cout << "  Token type: " << static_cast<int>(tokens[0].type) << ", Lexeme: '"
                  << tokens[0].lexeme << "'" << std::endl;

        if (tokens.size() > 1) {
            std::cout << "  Warning: Generated " << tokens.size() << " tokens, expected 1"
                      << std::endl;
        }

        // Print any diagnostics
        for (const auto& diag : diagnosticReporter->getReports()) {
            std::cout << "  " << (diag.level == DiagnosticLevel::ERROR ? "ERROR: " : "INFO: ")
                      << diag.message << std::endl;
        }

        std::cout << std::endl;
    }
}

int main() {
    testCharacterLiterals();
    return 0;
}
