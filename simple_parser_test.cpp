#include <iostream>
#include <string>
#include <vector>

#include "src/compiler/include/compiler/Lexer.h"
#include "src/compiler/include/compiler/Token.h"

// Simple function to test lexing of character literals
void testCharacterLiterals() {
    std::vector<std::string> testCases = {
        "'a'",      // Simple character
        "'\\n'",    // Newline
        "'\\t'",    // Tab
        "'\\\\'",   // Backslash
        "'\\''",    // Single quote
        "'\\x41'",  // Hex value 'A'
        "''",       // Empty (error)
        "'ab'"      // Multi-character (error)
    };

    for (const auto& testCase : testCases) {
        std::cout << "\n=== Testing: " << testCase << " ===\n";

        // Create a lexer with the test case
        chronovyan::compiler::Lexer lexer(testCase, "test.cvy");

        // Get tokens until EOF
        while (true) {
            auto token = lexer.nextToken();
            std::cout << "Token type: " << static_cast<int>(token.getType())
                      << ", Lexeme: " << token.getLexeme() << std::endl;

            if (token.getType() == chronovyan::compiler::TokenType::END_OF_FILE) {
                break;
            }
        }
    }
}

int main() {
    std::cout << "Testing Character Literal Parsing\n";
    std::cout << "================================\n\n";

    testCharacterLiterals();

    std::cout << "\n================================\n";
    std::cout << "Testing Complete\n";

    return 0;
}
