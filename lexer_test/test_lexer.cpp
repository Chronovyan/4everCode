#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "compiler/Diagnostic.h"
#include "compiler/Lexer.h"
#include "compiler/SourceLocation.h"
#include "compiler/Token.h"

using namespace chronovyan::compiler;

struct TestResult {
    int totalTests = 0;
    int passed = 0;
    int failed = 0;
};

void printDivider(const std::string& title = "") {
    const int width = 80;
    if (!title.empty()) {
        int padding = (width - title.length() - 2) / 2;
        std::cout << "\n"
                  << std::string(padding, '=') << " " << title << " "
                  << std::string(width - title.length() - padding - 2, '=') << "\n";
    } else {
        std::cout << "\n" << std::string(width, '=') << "\n";
    }
}

void testLexer(const std::string& source, TestResult& result) {
    result.totalTests++;

    auto reporter = std::make_shared<DiagnosticReporter>();
    Lexer lexer(source, "test_input", reporter);

    std::cout << "Test #" << result.totalTests << ": " << source << "\n";

    bool hasErrors = false;
    std::vector<std::string> tokens;

    // First pass: collect tokens and check for errors
    while (true) {
        Token token = lexer.nextToken();
        std::stringstream ss;
        ss << "  Token: " << tokenTypeToString(token.getType()) << ", Lexeme: '"
           << token.getLexeme() << "'"
           << ", Location: " << token.getLocation().line << ":" << token.getLocation().column;
        tokens.push_back(ss.str());

        if (token.getType() == TokenType::ERROR || token.getType() == TokenType::END_OF_FILE) {
            if (token.getType() == TokenType::ERROR) {
                hasErrors = true;
            }
            break;
        }
    }

    // Check for lexer errors
    if (reporter->hasErrors()) {
        hasErrors = true;
    }

    // Print the tokens if there were no errors, or if we're in verbose mode
    bool verbose = true;  // Set to false to only show failing tests
    if (!hasErrors || verbose) {
        for (const auto& tokenStr : tokens) {
            std::cout << tokenStr << "\n";
        }
    }

    // Print errors if any
    if (hasErrors) {
        std::cerr << "  ❌ FAILED";
        if (reporter->hasErrors()) {
            std::cerr << " - Lexer reported errors";
        }
        std::cerr << "\n";
        result.failed++;
    } else {
        std::cout << "  ✅ PASSED\n";
        result.passed++;
    }

    std::cout << "\n";
}

std::vector<std::string> readTestCases(const std::string& filename) {
    std::vector<std::string> testCases;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open test file: " << filename << "\n";
        return testCases;
    }

    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || (line.size() >= 2 && line[0] == '/' && line[1] == '/')) {
            continue;
        }

        // Remove leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start != std::string::npos) {
            size_t end = line.find_last_not_of(" \t");
            testCases.push_back(line.substr(start, end - start + 1));
        }
    }

    return testCases;
}

// Helper function to get the path to the test cases file
std::string getTestCasesPath() {
    // Try to find the test_cases.txt file in the current directory or parent directories
    std::string path = "test_cases.txt";
    std::ifstream file(path);
    if (file.good()) {
        return path;
    }

    // Try parent directory
    path = "../test_cases.txt";
    file = std::ifstream(path);
    if (file.good()) {
        return path;
    }

    // Try lexer_test directory
    path = "lexer_test/test_cases.txt";
    file = std::ifstream(path);
    if (file.good()) {
        return path;
    }

    return "";
}

int main() {
    TestResult result;

    // Get the path to the test cases file
    std::string testCasesPath = getTestCasesPath();

    // Read test cases from file
    std::vector<std::string> testCases;
    if (!testCasesPath.empty()) {
        std::cout << "Using test cases from: " << testCasesPath << "\n";
        testCases = readTestCases(testCasesPath);
    }

    if (testCases.empty()) {
        std::cerr
            << "No test cases found or error reading test file. Using default test cases.\n\n";

        // Fallback to default test cases if file read fails
        testCases = {
            // Decimal numbers
            "123", "123.456", "1_000_000",
            "1_2_3_",  // Should show error

            // Hexadecimal numbers
            "0x1A3F", "0x1A_3F",
            "0x",  // Should show error

            // Binary numbers
            "0b1010", "0b1010_0101",
            "0b",  // Should show error

            // Octal numbers
            "0123", "0o123", "0o1_2_3",
            "0o",  // Should show error

            // Floating point numbers
            "123.456e7", "1.23e-4", "1_000.000_1",
            "123._456",  // Should be parsed as 123 and ._456

            // Null keyword
            "null", "NULL", "Null",

            // Character literals
            "'a'", "'\\n'", "'\\x1F'", "'\\u1234'", "'\\''", "'\\\\'", "'\\0'",
            "'\\x'",    // Should show error
            "'\\u'",    // Should show error
            "'\\u123'"  // Should show error (incomplete unicode)
        };
    }

    // Run all test cases
    for (const auto& testCase : testCases) {
        // Skip empty test cases
        if (testCase.empty() ||
            (testCase.size() >= 2 && testCase[0] == '/' && testCase[1] == '/')) {
            continue;
        }

        // Print section headers for comments
        if (testCase.size() > 2 && testCase[0] == '/' && testCase[1] == '/') {
            printDivider(testCase.substr(2));
            continue;
        }

        testLexer(testCase, result);
    }

    // Print summary
    printDivider("TEST SUMMARY");
    std::cout << "Total tests: " << result.totalTests << "\n";
    std::cout << "Passed:      " << result.passed << "\n";
    std::cout << "Failed:      " << result.failed << "\n";

    if (result.failed > 0) {
        std::cerr << "\n❌ Some tests failed!\n";
        return 1;
    } else {
        std::cout << "\n✅ All tests passed!\n";
        return 0;
    }

    return 0;
}
