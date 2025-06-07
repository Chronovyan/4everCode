#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

// Include only the minimal required headers
#include "compiler/DiagnosticReporter.h"
#include "compiler/Lexer.h"
#include "compiler/Token.h"

using namespace chronovyan::compiler;

// Helper function to convert TokenType to string (renamed to avoid conflict with Token.h)
const char* tokenTypeToReadableString(TokenType type) {
    static const std::map<TokenType, std::string> tokenTypeNames = {
        // Single-character tokens
        {TokenType::LEFT_PAREN, "LEFT_PAREN"},
        {TokenType::RIGHT_PAREN, "RIGHT_PAREN"},
        {TokenType::LEFT_BRACE, "LEFT_BRACE"},
        {TokenType::RIGHT_BRACE, "RIGHT_BRACE"},
        {TokenType::LEFT_BRACKET, "LEFT_BRACKET"},
        {TokenType::RIGHT_BRACKET, "RIGHT_BRACKET"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::DOT, "DOT"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::PLUS, "PLUS"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::STAR, "STAR"},
        {TokenType::PERCENT, "PERCENT"},
        {TokenType::COLON, "COLON"},

        // One or two character tokens
        {TokenType::BANG, "BANG"},
        {TokenType::BANG_EQUAL, "BANG_EQUAL"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::PLUS_PLUS, "PLUS_PLUS"},
        {TokenType::MINUS_MINUS, "MINUS_MINUS"},
        {TokenType::PLUS_EQUAL, "PLUS_EQUAL"},
        {TokenType::MINUS_EQUAL, "MINUS_EQUAL"},
        {TokenType::STAR_EQUAL, "STAR_EQUAL"},
        {TokenType::SLASH_EQUAL, "SLASH_EQUAL"},
        {TokenType::PERCENT_EQUAL, "PERCENT_EQUAL"},
        {TokenType::COLON_COLON, "COLON_COLON"},
        {TokenType::ARROW, "ARROW"},

        // Literals
        {TokenType::IDENTIFIER, "IDENTIFIER"},
        {TokenType::STRING, "STRING"},
        {TokenType::INTEGER, "INTEGER"},
        {TokenType::FLOAT, "FLOAT"},
        {TokenType::CHRONON_LITERAL, "CHRONON_LITERAL"},
        {TokenType::AETHEL_LITERAL, "AETHEL_LITERAL"},
        {TokenType::CHARACTER, "CHARACTER"},

        // Keywords
        {TokenType::LET, "LET"},
        {TokenType::CONST, "CONST"},
        {TokenType::CONF, "CONF"},
        {TokenType::REB, "REB"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::FOR, "FOR"},
        {TokenType::FUNCTION, "FUNCTION"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::IMPORT, "IMPORT"},
        {TokenType::EXPORT, "EXPORT"},
        {TokenType::TRUE, "TRUE"},
        {TokenType::FALSE, "FALSE"},

        // Special tokens
        {TokenType::ERROR, "ERROR"},
        {TokenType::END_OF_FILE, "END_OF_FILE"}};

    auto it = tokenTypeNames.find(type);
    if (it != tokenTypeNames.end()) {
        return it->second.c_str();
    }
    return "UNKNOWN_TOKEN";
}

// Simple diagnostic reporter that prints to stderr
class TestDiagnosticReporter : public DiagnosticReporter {
    std::vector<std::string> errors_;
    std::vector<DiagnosticMessage> diagnostics_;

    void addDiagnostic(DiagnosticSeverity severity, const SourceLocation& location,
                       const std::string& message) {
        diagnostics_.emplace_back(severity, location, message);
        std::string prefix;
        switch (severity) {
            case DiagnosticSeverity::ERROR:
                prefix = "Error";
                break;
            case DiagnosticSeverity::WARNING:
                prefix = "Warning";
                break;
            case DiagnosticSeverity::INFO:
                prefix = "Info";
                break;
            case DiagnosticSeverity::HINT:
                prefix = "Hint";
                break;
        }
        std::cerr << prefix << ": " << message << " at " << location.filename << ":"
                  << location.line << ":" << location.column << std::endl;

        if (severity == DiagnosticSeverity::ERROR) {
            errors_.push_back(message);
        }
    }

    void addDiagnostic(DiagnosticSeverity severity, const Token& token,
                       const std::string& message) {
        addDiagnostic(severity, token.getLocation(), message);
    }

public:
    // Implement all required methods from DiagnosticReporter
    virtual void addError(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::ERROR, location, message);
    }

    virtual void addWarning(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::WARNING, location, message);
    }

    virtual void addInfo(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::INFO, location, message);
    }

    virtual void addHint(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::HINT, location, message);
    }

    virtual void addError(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::ERROR, token, message);
    }

    virtual void addWarning(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::WARNING, token, message);
    }

    virtual void addInfo(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::INFO, token, message);
    }

    virtual void addHint(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::HINT, token, message);
    }

    virtual bool hasErrors() const { return !errors_.empty(); }

    virtual bool hasWarnings() const {
        return std::any_of(diagnostics_.begin(), diagnostics_.end(), [](const auto& d) {
            return d.getSeverity() == DiagnosticSeverity::WARNING;
        });
    }

    virtual bool hasDiagnostics() const { return !diagnostics_.empty(); }

    virtual void printDiagnostics(std::ostream& os = std::cerr) const {
        for (const auto& diag : diagnostics_) {
            os << diag.toString() << std::endl;
        }
    }

    // Additional helper methods for testing
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<DiagnosticMessage>& getDiagnostics() const { return diagnostics_; }
};

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    try {
        // Check command line arguments
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <source_file>" << std::endl;
            return 1;
        }

        // Read the source file
        fs::path filePath = argv[1];
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filePath << std::endl;
            return 1;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        std::cout << "Lexing file: " << filePath.filename() << std::endl;

        // Create necessary components
        TestDiagnosticReporter reporter;

        try {
            // Create lexer with source
            Lexer lexer(source, filePath.string());

            // Print tokens
            std::cout << "\n=== Tokens ===" << std::endl;
            Token token = lexer.nextToken();
            int tokenCount = 0;

            // Print a header
            std::cout << "\n" << std::string(80, '=') << "\n";
            std::cout << "TOKEN STREAM\n";
            std::cout << std::string(80, '=') << "\n";
            std::cout << std::left << std::setw(20) << "TOKEN TYPE" << std::setw(30) << "LEXEME"
                      << "POSITION\n";
            std::cout << std::string(80, '-') << "\n";

            while (token.getType() != TokenType::END_OF_FILE) {
                const auto& loc = token.getLocation();
                std::string tokenType = tokenTypeToReadableString(token.getType());
                std::string lexeme = token.getLexeme();

                // Truncate long lexemes for display
                if (lexeme.length() > 25) {
                    lexeme = lexeme.substr(0, 22) + "...";
                }

                // Escape special characters in lexeme for display
                for (size_t i = 0; i < lexeme.length(); ++i) {
                    if (lexeme[i] == '\n')
                        lexeme.replace(i, 1, "\\n");
                    else if (lexeme[i] == '\t')
                        lexeme.replace(i, 1, "\\t");
                    else if (lexeme[i] == '\r')
                        lexeme.replace(i, 1, "\\r");
                }

                std::string position =
                    "Line: " + std::to_string(loc.line) + ", Col: " + std::to_string(loc.column);

                // Color coding for different token types
                std::string colorCode;
                if (token.getType() >= TokenType::LET && token.getType() <= TokenType::FALSE) {
                    colorCode = "\033[1;34m";  // Blue for keywords
                } else if (token.getType() == TokenType::IDENTIFIER) {
                    colorCode = "\033[1;36m";  // Cyan for identifiers
                } else if (token.getType() == TokenType::INTEGER ||
                           token.getType() == TokenType::FLOAT) {
                    colorCode = "\033[1;33m";  // Yellow for numbers
                } else if (token.getType() == TokenType::STRING) {
                    colorCode = "\033[1;32m";  // Green for strings
                } else if (token.getType() == TokenType::CHARACTER) {
                    colorCode = "\033[1;35m";  // Magenta for characters
                } else if (token.getType() == TokenType::ERROR) {
                    colorCode = "\033[1;31m";  // Red for errors
                } else {
                    colorCode = "\033[0m";  // Default color
                }

                std::cout << colorCode << std::left << std::setw(20) << tokenType << std::setw(30)
                          << lexeme << position << "\033[0m" << std::endl;

                token = lexer.nextToken();
                tokenCount++;
            }

            std::cout << "\nLexing completed successfully! Found " << tokenCount << " tokens."
                      << std::endl;

            if (reporter.hasErrors()) {
                std::cerr << "\nEncountered " << reporter.getErrors().size()
                          << " error(s):" << std::endl;
                for (const auto& error : reporter.getErrors()) {
                    std::cerr << "- " << error << std::endl;
                }
                return 1;
            }

        } catch (const std::exception& e) {
            std::cerr << "\nError: " << e.what() << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
