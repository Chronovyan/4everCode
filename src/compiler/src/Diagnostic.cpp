#include <sstream>

#include "compiler/Diagnostic.h"

namespace chronovyan {
namespace compiler {

std::string DiagnosticMessage::toString() const {
    std::stringstream ss;

    // Format: [SEVERITY] file:line:column: message
    ss << "[" << diagnosticSeverityToString(severity) << "] ";
    ss << location.filename << ":" << location.line << ":" << location.column << ": ";
    ss << message;

    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const DiagnosticMessage& message) {
    return os << message.toString();
}

const char* diagnosticSeverityToString(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::ERROR:
            return "ERROR";
        case DiagnosticSeverity::WARNING:
            return "WARNING";
        case DiagnosticSeverity::INFO:
            return "INFO";
        case DiagnosticSeverity::HINT:
            return "HINT";
        default:
            return "UNKNOWN";
    }
}

// DiagnosticReporter implementation

void DiagnosticReporter::addDiagnostic(DiagnosticSeverity severity, const SourceLocation& location,
                                       const std::string& message) {
    // Only add INFO and HINT messages if debug output is enabled
    if (severity == DiagnosticSeverity::INFO || severity == DiagnosticSeverity::HINT) {
        if (!debugOutput) {
            return;
        }
        // For debug output, also print to stderr for immediate feedback during tests
        std::cerr << "[DEBUG] " << message << std::endl;
    }
    diagnostics.emplace_back(severity, location, message);
}

void DiagnosticReporter::addDiagnostic(DiagnosticSeverity severity, const Token& token,
                                       const std::string& message) {
    diagnostics.emplace_back(severity, token, message);
}

void DiagnosticReporter::addError(const SourceLocation& location, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::ERROR, location, message);
}

void DiagnosticReporter::addWarning(const SourceLocation& location, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::WARNING, location, message);
}

void DiagnosticReporter::addInfo(const SourceLocation& location, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::INFO, location, message);
}

void DiagnosticReporter::addHint(const SourceLocation& location, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::HINT, location, message);
}

void DiagnosticReporter::addError(const Token& token, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::ERROR, token, message);
}

void DiagnosticReporter::addWarning(const Token& token, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::WARNING, token, message);
}

void DiagnosticReporter::addInfo(const Token& token, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::INFO, token, message);
}

void DiagnosticReporter::addHint(const Token& token, const std::string& message) {
    addDiagnostic(DiagnosticSeverity::HINT, token, message);
}

bool DiagnosticReporter::hasErrors() const {
    for (const auto& diagnostic : diagnostics) {
        if (diagnostic.getSeverity() == DiagnosticSeverity::ERROR) {
            return true;
        }
    }
    return false;
}

bool DiagnosticReporter::hasWarnings() const {
    for (const auto& diagnostic : diagnostics) {
        if (diagnostic.getSeverity() == DiagnosticSeverity::WARNING) {
            return true;
        }
    }
    return false;
}

bool DiagnosticReporter::hasDiagnostics() const { return !diagnostics.empty(); }

void DiagnosticReporter::printDiagnostics(std::ostream& os) const {
    for (const auto& diagnostic : diagnostics) {
        os << diagnostic << std::endl;
    }
}

}  // namespace compiler
}  // namespace chronovyan