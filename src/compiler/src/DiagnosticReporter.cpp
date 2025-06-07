#include <algorithm>
#include <iostream>

#include "compiler/Diagnostic.h"
#include "compiler/DiagnosticReporter.h"

namespace chronovyan {
namespace compiler {

/**
 * @brief A simple implementation of DiagnosticReporter that outputs to stderr
 */
class StderrDiagnosticReporter : public DiagnosticReporter {
public:
    /**
     * @brief Constructor
     */
    StderrDiagnosticReporter() : diagnostics() {}

    // Add diagnostic messages
    void addError(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::ERROR, location, message);
    }

    void addWarning(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::WARNING, location, message);
    }

    void addInfo(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::INFO, location, message);
    }

    void addHint(const SourceLocation& location, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::HINT, location, message);
    }

    void addError(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::ERROR, token, message);
    }

    void addWarning(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::WARNING, token, message);
    }

    void addInfo(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::INFO, token, message);
    }

    void addHint(const Token& token, const std::string& message) {
        addDiagnostic(DiagnosticSeverity::HINT, token, message);
    }

    // Query diagnostic state
    bool hasErrors() const {
        return std::any_of(diagnostics.begin(), diagnostics.end(),
                           [](const DiagnosticMessage& msg) {
                               return msg.getSeverity() == DiagnosticSeverity::ERROR;
                           });
    }

    bool hasWarnings() const {
        return std::any_of(diagnostics.begin(), diagnostics.end(),
                           [](const DiagnosticMessage& msg) {
                               return msg.getSeverity() == DiagnosticSeverity::WARNING;
                           });
    }

    bool hasDiagnostics() const { return !diagnostics.empty(); }

    // Get diagnostics
    const std::vector<DiagnosticMessage>& getDiagnostics() const { return diagnostics; }

    // Print diagnostics
    void printDiagnostics(std::ostream& os = std::cerr) const {
        for (const auto& diagnostic : diagnostics) {
            os << diagnostic << std::endl;
        }
    }

    // Clear diagnostics
    void clearDiagnostics() { diagnostics.clear(); }

private:
    std::vector<DiagnosticMessage> diagnostics;

    void addDiagnostic(DiagnosticSeverity severity, const SourceLocation& location,
                       const std::string& message) {
        diagnostics.emplace_back(severity, location, message);
    }

    void addDiagnostic(DiagnosticSeverity severity, const Token& token,
                       const std::string& message) {
        diagnostics.emplace_back(severity, token, message);
    }
};

// Factory function to create a default diagnostic reporter
std::unique_ptr<DiagnosticReporter> createDefaultDiagnosticReporter() {
    return std::make_unique<StderrDiagnosticReporter>();
}

}  // namespace compiler
}  // namespace chronovyan