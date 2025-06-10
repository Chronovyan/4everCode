#ifndef CHRONOVYAN_DIAGNOSTIC_H
#define CHRONOVYAN_DIAGNOSTIC_H

#include <iostream>
#include <string>
#include <vector>

#include "compiler/Token.h"

namespace chronovyan {
namespace compiler {

/**
 * DiagnosticSeverity represents the severity level of a diagnostic message.
 */
enum class DiagnosticSeverity { ERROR, WARNING, INFO, HINT };

/**
 * DiagnosticMessage represents a diagnostic message with severity, location,
 * and message.
 */
class DiagnosticMessage {
public:
  DiagnosticMessage(DiagnosticSeverity severity, const SourceLocation &location,
                    std::string message)
      : severity(severity), location(location), message(std::move(message)) {}

  DiagnosticMessage(DiagnosticSeverity severity, const Token &token,
                    std::string message)
      : severity(severity), location(token.getLocation()),
        message(std::move(message)) {}

  // Getters
  DiagnosticSeverity getSeverity() const { return severity; }
  const SourceLocation &getLocation() const { return location; }
  const std::string &getMessage() const { return message; }

  // String representation for output
  std::string toString() const;

private:
  DiagnosticSeverity severity;
  SourceLocation location;
  std::string message;
};

/**
 * DiagnosticReporter is responsible for collecting and reporting diagnostic
 * messages.
 */
class DiagnosticReporter {
public:
  // Add diagnostic messages
  void addError(const SourceLocation &location, const std::string &message);
  void addWarning(const SourceLocation &location, const std::string &message);
  void addInfo(const SourceLocation &location, const std::string &message);
  void addHint(const SourceLocation &location, const std::string &message);

  void addError(const Token &token, const std::string &message);
  void addWarning(const Token &token, const std::string &message);
  void addInfo(const Token &token, const std::string &message);
  void addHint(const Token &token, const std::string &message);

  // Query diagnostic state
  bool hasErrors() const;
  bool hasWarnings() const;
  bool hasDiagnostics() const;

  // Get diagnostics
  const std::vector<DiagnosticMessage> &getDiagnostics() const {
    return diagnostics;
  }

  // Print diagnostics
  void printDiagnostics(std::ostream &os = std::cerr) const;

  // Clear diagnostics
  void clearDiagnostics() { diagnostics.clear(); }

  // Control debug output
  void setDebugOutput(bool enabled) { debugOutput = enabled; }
  bool getDebugOutput() const { return debugOutput; }

private:
  std::vector<DiagnosticMessage> diagnostics;
  bool debugOutput = false;

  void addDiagnostic(DiagnosticSeverity severity,
                     const SourceLocation &location,
                     const std::string &message);
  void addDiagnostic(DiagnosticSeverity severity, const Token &token,
                     const std::string &message);
};

// Output stream operator for DiagnosticMessage
std::ostream &operator<<(std::ostream &os, const DiagnosticMessage &message);

// Helper function to convert DiagnosticSeverity to string
const char *diagnosticSeverityToString(DiagnosticSeverity severity);

} // namespace compiler
} // namespace chronovyan

#endif // CHRONOVYAN_DIAGNOSTIC_H