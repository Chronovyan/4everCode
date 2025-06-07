#include <iostream>
#include <string>
#include <vector>

#include "src/compiler/include/compiler/Diagnostic.h"
#include "src/compiler/include/compiler/Lexer.h"

using namespace chronovyan::compiler;

void testCharacterLiteral(const std::string& input) {
    auto reporter = std::make_shared<DiagnosticReporter>();
    reporter->setDebugOutput(true);

    std::cout << "Testing: " << input << std::endl;

    Lexer lexer(input, "test.cvy", reporter);

    while (true) {
        Token token = lexer.nextToken();
        std::cout << "  Token type: " << static_cast<int>(token.getType())
                  << ", Lexeme: " << token.getLexeme() << std::endl;

        if (token.getType() == TokenType::END_OF_FILE) {
            break;
        }
    }

    // Print any diagnostics
    reporter->printDiagnostics(std::cerr);
    std::cout << std::endl;
}

int main() {
    std::cout << "Testing Character Literal Parsing\n";
    std::cout << "================================\n\n";

    // Test cases
    testCharacterLiteral("'a'");      // Simple character
    testCharacterLiteral("'\\n'");    // Newline
    testCharacterLiteral("'\\t'");    // Tab
    testCharacterLiteral("'\\\\'");   // Backslash
    testCharacterLiteral("'\\''");    // Single quote
    testCharacterLiteral("'\\x41'");  // Hex value 'A'
    testCharacterLiteral("''");       // Empty (error)
    testCharacterLiteral("'ab'");     // Multi-character (error)

    std::cout << "================================\n";
    std::cout << "Testing Complete\n";

    return 0;
}
