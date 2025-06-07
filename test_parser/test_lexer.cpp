#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "compiler/DiagnosticReporter.h"
#include "compiler/Lexer.h"
#include "compiler/Token.h"

using namespace chronovyan::compiler;

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
        for (const auto& diag : diagnostics_) {
            if (diag.getSeverity() == DiagnosticSeverity::WARNING) {
                return true;
            }
        }
        return false;
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

// Helper function to print token information
void printToken(const Token& token) {
    std::cout << "Token type: " << token.getType() << ", "
              << "Lexeme: \"" << token.getLexeme() << "\", "
              << "Line: " << token.getLocation().line << ", "
              << "Column: " << token.getLocation().column << "\n";

    if (token.getValue().has_value()) {
        if (token.getType() == TokenType::STRING) {
            std::cout << "  String value: \"" << std::any_cast<std::string>(token.getValue())
                      << "\"\n";
        } else if (token.getType() == TokenType::CHARACTER) {
            char c = std::any_cast<char>(token.getValue());
            std::cout << "  Character value: '" << c << "' (" << static_cast<int>(c) << ")\n";
        } else if (token.getType() == TokenType::NUMBER) {
            std::cout << "  Number value: " << std::any_cast<double>(token.getValue()) << "\n";
        } else if (token.getType() == TokenType::IDENTIFIER) {
            std::cout << "  Identifier: " << std::any_cast<std::string>(token.getValue()) << "\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return 1;
    }

    // Read the entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();

    // Create a diagnostic reporter
    auto reporter = std::make_shared<TestDiagnosticReporter>();

    // Create a lexer
    Lexer lexer(source, filename, reporter);

    // Tokenize the input
    std::vector<Token> tokens = lexer.tokenize();

    // Print all tokens
    for (const auto& token : tokens) {
        printToken(token);
    }

    // Print any diagnostics
    reporter->printDiagnostics();

    // Return non-zero if there were any errors
    return reporter->hasErrors() ? 1 : 0;
}
