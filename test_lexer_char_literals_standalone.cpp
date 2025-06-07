#include <iostream>
#include <string>
#include <vector>

#include "src/compiler/include/compiler/Lexer.h"
#include "src/compiler/include/compiler/Token.h"

using namespace chronovyan::compiler;

// Helper function to print token information
void printTokenInfo(const Token& token) {
    std::cout << "Token type: " << static_cast<int>(token.getType())
              << ", Lexeme: " << token.getLexeme() << std::endl;
}

// Test character literals
void testCharacterLiterals() {
    std::cout << "=== Testing Character Literals ===\n";

    // Test cases with expected results
    struct TestCase {
        std::string input;
        TokenType expectedType;
        std::string expectedLexeme;
        bool shouldPass;
    };

    std::vector<TestCase> testCases = {
        {"'a'", TokenType::CHARACTER, "'a'", true},
        {"'\\n'", TokenType::CHARACTER, "'\\n'", true},
        {"'\\t'", TokenType::CHARACTER, "'\\t'", true},
        {"'\\\\'", TokenType::CHARACTER, "'\\\\'", true},
        {"'\\''", TokenType::CHARACTER, "'\\''", true},
        {"'\\x41'", TokenType::CHARACTER, "'\\x41'", true},  // 'A'
        {"'\\x1F'", TokenType::CHARACTER, "'\\x1F'", true},  // Unit separator
        {"' '", TokenType::CHARACTER, "' '", true},          // Space character
        {"''", TokenType::ERROR, "''", false},               // Empty character literal (error)
        {"'ab'", TokenType::ERROR, "'ab'", false},           // Multi-character (error)
        {"'a", TokenType::ERROR, "'a", false}                // Unclosed quote (error)
    };

    for (const auto& testCase : testCases) {
        std::cout << "\nTesting: " << testCase.input << std::endl;

        Lexer lexer(testCase.input, "test.cvy");
        Token token = lexer.nextToken();

        printTokenInfo(token);

        if ((token.getType() == testCase.expectedType) == testCase.shouldPass &&
            (token.getLexeme() == testCase.expectedLexeme) == testCase.shouldPass) {
            std::cout << "  \x1B[32mPASS\x1B[0m" << std::endl;
        } else {
            std::cout << "  \x1B[31mFAIL\x1B[0m - Expected: "
                      << static_cast<int>(testCase.expectedType)
                      << ", Got: " << static_cast<int>(token.getType())
                      << " | Lexeme: " << token.getLexeme() << std::endl;
        }

        // Print any additional tokens (should only be EOF for valid cases)
        if (token.getType() != TokenType::END_OF_FILE) {
            token = lexer.nextToken();
            if (token.getType() != TokenType::END_OF_FILE) {
                std::cout << "  Additional token found: ";
                printTokenInfo(token);
            }
        }
    }
}

int main() {
    std::cout << "Chronovyan Lexer Character Literal Tests\n";
    std::cout << "======================================\n\n";

    testCharacterLiterals();

    std::cout << "\n======================================\n";
    std::cout << "Tests Complete\n";

    return 0;
}
