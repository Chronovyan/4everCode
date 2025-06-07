#include <iostream>
#include <memory>
#include <string>

#include "src/compiler/include/compiler/Diagnostic.h"
#include "src/compiler/include/compiler/Lexer.h"
#include "src/compiler/include/compiler/Parser.h"

using namespace chronovyan::compiler;

// Helper function to parse a string and print the AST
void parseAndPrint(const std::string& source) {
    auto reporter = std::make_shared<DiagnosticReporter>();
    reporter->setDebugOutput(true);

    std::cout << "\n=== Parsing: " << source << " ===\n";

    Lexer lexer(source, "test.cvy", reporter);
    Parser parser(lexer, reporter);

    auto ast = parser.parse();

    // Print any diagnostics
    reporter->printDiagnostics(std::cout);

    if (ast) {
        std::cout << "Parse successful. AST root type: " << ast->getTypeString() << std::endl;
    } else {
        std::cout << "Parse failed" << std::endl;
    }
}

int main() {
    std::cout << "Testing Parser with Character Literals\n";
    std::cout << "====================================\n\n";

    // Test character literals in different contexts

    // Simple character literal assignment
    parseAndPrint("LET ch = 'a';");

    // Character literal in expression
    parseAndPrint("LET result = 'x' + 1;");

    // Character literal in condition
    parseAndPrint("IF '\n' == '\n' THEN { LET x = 1; }");

    // Character literal with escape sequences
    parseAndPrint("LET tab = '\\t';");
    parseAndPrint("LET quote = '\\'';");
    parseAndPrint("LET backslash = '\\\\';");

    // Character literal in a function call
    parseAndPrint("print('A');");

    // Character literal in a more complex expression
    parseAndPrint("LET isLetter = (ch >= 'a' AND ch <= 'z') OR (ch >= 'A' AND ch <= 'Z');");

    // Test error cases
    std::cout << "\n=== Testing Error Cases ===\n";
    parseAndPrint("LET empty = '';");    // Empty character literal
    parseAndPrint("LET multi = 'ab';");  // Multi-character
    parseAndPrint("LET unclosed = 'A");  // Unclosed quote

    std::cout << "\n====================================\n";
    std::cout << "Testing Complete\n";

    return 0;
}
