#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include "error_handler.h"
#include "temporal_runtime.h"

using namespace std::chrono;

namespace chronovyan {

std::string ChronovyanError::toString() const {
  std::stringstream ss;

  ss << (severity == ErrorSeverity::WARNING ? "Warning"
         : severity == ErrorSeverity::ERROR ? "Error"
                                            : "Fatal Error");

  if (location.isValid()) {
    ss << " at " << location.toString();
  }

  ss << ": " << message;

  // Add timestamp
  auto timeT = system_clock::to_time_t(system_clock::now());
#ifdef _WIN32
  struct tm timeInfo;
  localtime_s(&timeInfo, &timeT);
  ss << " - " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
#else
  ss << " - " << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
#endif

  return ss.str();
}

std::string ChronovyanError::toDetailedString(bool includeLineContent) const {
  std::stringstream ss;

  ss << (severity == ErrorSeverity::WARNING ? "Warning"
         : severity == ErrorSeverity::ERROR ? "Error"
                                            : "Fatal Error");

  if (location.isValid()) {
    ss << " at " << location.toDetailedString(includeLineContent);
  }

  ss << ": " << message;

  // Add timestamp
  auto timeT = system_clock::to_time_t(system_clock::now());
#ifdef _WIN32
  struct tm timeInfo;
  localtime_s(&timeInfo, &timeT);
  ss << " - " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
#else
  ss << " - " << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
#endif

  return ss.str();
}

std::string ParadoxEvent::toString() const {
  std::stringstream ss;

  ss << "Paradox (" << paradoxTypeToString(type) << ", Level " << paradoxLevel
     << ")";

  if (location.isValid()) {
    ss << " at " << location.toString();
  }

  ss << ": " << message;

  if (isResolved) {
    ss << " [RESOLVED]";
  }

  // Add timestamp
  auto timeT = system_clock::to_time_t(timestamp);
#ifdef _WIN32
  struct tm timeInfo;
  localtime_s(&timeInfo, &timeT);
  ss << " - " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
#else
  ss << " - " << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
#endif

  return ss.str();
}

std::string ParadoxEvent::toDetailedString(bool includeLineContent) const {
  std::stringstream ss;

  ss << "Paradox (" << paradoxTypeToString(type) << ", Level " << paradoxLevel
     << ")";

  if (location.isValid()) {
    ss << " at " << location.toDetailedString(includeLineContent);
  }

  ss << ": " << message;

  if (isResolved) {
    ss << " [RESOLVED]";
  }

  // Add timestamp
  auto timeT = system_clock::to_time_t(timestamp);
#ifdef _WIN32
  struct tm timeInfo;
  localtime_s(&timeInfo, &timeT);
  ss << " - " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
#else
  ss << " - " << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
#endif

  return ss.str();
}

std::string ParadoxEvent::paradoxTypeToString(ParadoxType type) {
  switch (type) {
  case ParadoxType::TIMELINE_CONFLICT:
    return "Timeline Conflict";
  case ParadoxType::QUANTUM_CONTRADICTION:
    return "Quantum Contradiction";
  case ParadoxType::STABILITY_BREACH:
    return "Stability Breach";
  case ParadoxType::RESOURCE_CONFLICT:
    return "Resource Conflict";
  case ParadoxType::CAUSALITY_VIOLATION:
    return "Causality Violation";
  default:
    return "Unknown Paradox Type";
  }
}

std::string GlitchEvent::toString() const {
  std::stringstream ss;

  ss << "Glitch (" << glitchTypeToString(type) << ", Severity " << std::fixed
     << std::setprecision(2) << severity << ")";

  if (location.isValid()) {
    ss << " at " << location.toString();
  }

  ss << ": " << message;

  if (isFixed) {
    ss << " [FIXED]";
  }

  // Add timestamp
  auto timeT = system_clock::to_time_t(timestamp);
#ifdef _WIN32
  struct tm timeInfo;
  localtime_s(&timeInfo, &timeT);
  ss << " - " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
#else
  ss << " - " << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
#endif

  return ss.str();
}

std::string GlitchEvent::toDetailedString(bool includeLineContent) const {
  std::stringstream ss;

  ss << "Glitch (" << glitchTypeToString(type) << ", Severity " << std::fixed
     << std::setprecision(2) << severity << ")";

  if (location.isValid()) {
    ss << " at " << location.toDetailedString(includeLineContent);
  }

  ss << ": " << message;

  if (isFixed) {
    ss << " [FIXED]";
  }

  // Add timestamp
  auto timeT = system_clock::to_time_t(timestamp);
#ifdef _WIN32
  struct tm timeInfo;
  localtime_s(&timeInfo, &timeT);
  ss << " - " << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
#else
  ss << " - " << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
#endif

  return ss.str();
}

std::string GlitchEvent::glitchTypeToString(GlitchType type) {
  switch (type) {
  case GlitchType::VARIABLE_INSTABILITY:
    return "Variable Instability";
  case GlitchType::CHRONON_LEAKAGE:
    return "Chronon Leakage";
  case GlitchType::AETHEL_FLUCTUATION:
    return "Aethel Fluctuation";
  case GlitchType::TIMELINE_DISTORTION:
    return "Timeline Distortion";
  case GlitchType::QUANTUM_NOISE:
    return "Quantum Noise";
  default:
    return "Unknown Glitch Type";
  }
}

int ErrorHandler::reportError(const SourceLocation &location,
                              const std::string &message,
                              ErrorSeverity severity) {
  // Create the error
  ChronovyanError error(location, message, severity);

  // Log the error
  if (m_consoleOutput && severity >= m_minConsoleSeverity) {
    std::cerr << error.toString() << std::endl;
  }

  // Execute callbacks
  bool handled = executeErrorCallbacks(error);

  // Add to the collection if not handled
  if (!handled) {
    m_errors.push_back(error);
  }

  // Return the error ID
  return static_cast<int>(m_errors.size() - 1);
}

int ErrorHandler::reportWarning(const SourceLocation &location,
                                const std::string &message) {
  return reportError(location, message, ErrorSeverity::WARNING);
}

[[noreturn]] void ErrorHandler::reportFatalError(const SourceLocation &location,
                                                 const std::string &message) {
  reportError(location, message, ErrorSeverity::FATAL);
  throw ChronovyanException(
      formatErrorMessage(message, location, m_runtimeContext, true));
}

int ErrorHandler::reportParadox(const SourceLocation &location,
                                const std::string &message, ParadoxType type,
                                int level) {
  // Create the paradox
  ParadoxEvent paradox(location, message, type, level);

  // Log the paradox
  if (m_consoleOutput) {
    std::cerr << paradox.toString() << std::endl;
  }

  // Execute callbacks
  bool handled = executeParadoxCallbacks(paradox);

  // Add to the collection if not handled
  if (!handled) {
    m_paradoxes.push_back(paradox);
  }

  // Return the paradox ID
  return static_cast<int>(m_paradoxes.size() - 1);
}

int ErrorHandler::reportGlitch(const SourceLocation &location,
                               const std::string &message, GlitchType type,
                               double severity) {
  // Create the glitch
  GlitchEvent glitch(location, message, type, severity);

  // Log the glitch
  if (m_consoleOutput && severity >= m_minGlitchSeverity) {
    std::cerr << glitch.toString() << std::endl;
  }

  // Add to the collection
  m_glitches.push_back(glitch);

  // Return the glitch ID
  return static_cast<int>(m_glitches.size() - 1);
}

bool ErrorHandler::resolveParadox(int paradoxId) {
  if (paradoxId < 0 || paradoxId >= static_cast<int>(m_paradoxes.size())) {
    return false; // Invalid ID
  }

  auto &paradox = m_paradoxes[paradoxId];
  if (paradox.isResolved) {
    return false; // Already resolved
  }

  paradox.isResolved = true;
  std::cout << "Paradox resolved: "
            << ParadoxEvent::paradoxTypeToString(paradox.type) << std::endl;

  return true;
}

bool ErrorHandler::fixGlitch(int glitchId) {
  if (glitchId < 0 || glitchId >= static_cast<int>(m_glitches.size())) {
    return false; // Invalid ID
  }

  auto &glitch = m_glitches[glitchId];
  if (glitch.isFixed) {
    return false; // Already fixed
  }

  glitch.isFixed = true;
  std::cout << "Glitch fixed: " << GlitchEvent::glitchTypeToString(glitch.type)
            << std::endl;

  return true;
}

size_t ErrorHandler::getErrorCount(bool includeWarnings) const {
  if (includeWarnings) {
    return m_errors.size();
  } else {
    return std::count_if(
        m_errors.begin(), m_errors.end(),
        [](const ChronovyanError &error) { return !error.isWarning(); });
  }
}

size_t ErrorHandler::getParadoxCount(bool includeResolved) const {
  if (includeResolved) {
    return m_paradoxes.size();
  } else {
    return std::count_if(
        m_paradoxes.begin(), m_paradoxes.end(),
        [](const ParadoxEvent &paradox) { return !paradox.isResolved; });
  }
}

size_t ErrorHandler::getGlitchCount(bool includeFixed) const {
  if (includeFixed) {
    return m_glitches.size();
  } else {
    return std::count_if(
        m_glitches.begin(), m_glitches.end(),
        [](const GlitchEvent &glitch) { return !glitch.isFixed; });
  }
}

int ErrorHandler::registerErrorCallback(ErrorCallback callback) {
  int callbackId = m_nextErrorCallbackId++;
  m_errorCallbacks.emplace_back(callbackId, std::move(callback));
  return callbackId;
}

int ErrorHandler::registerParadoxCallback(ParadoxCallback callback) {
  int callbackId = m_nextParadoxCallbackId++;
  m_paradoxCallbacks.emplace_back(callbackId, std::move(callback));
  return callbackId;
}

bool ErrorHandler::removeErrorCallback(int callbackId) {
  auto it = std::find_if(
      m_errorCallbacks.begin(), m_errorCallbacks.end(),
      [callbackId](const auto &pair) { return pair.first == callbackId; });

  if (it != m_errorCallbacks.end()) {
    m_errorCallbacks.erase(it);
    return true;
  }

  return false;
}

bool ErrorHandler::removeParadoxCallback(int callbackId) {
  auto it = std::find_if(
      m_paradoxCallbacks.begin(), m_paradoxCallbacks.end(),
      [callbackId](const auto &pair) { return pair.first == callbackId; });

  if (it != m_paradoxCallbacks.end()) {
    m_paradoxCallbacks.erase(it);
    return true;
  }

  return false;
}

void ErrorHandler::reset() {
  m_errors.clear();
  m_paradoxes.clear();
  m_glitches.clear();
  m_errorCallbacks.clear();
  m_paradoxCallbacks.clear();
  m_runtimeContext.clear();
}

std::string ErrorHandler::getReport(bool includeWarnings,
                                    bool includeResolved) const {
  std::stringstream ss;

  // Report errors
  ss << "=== Errors (" << getErrorCount(includeWarnings) << " total) ===\n";
  for (const auto &error : m_errors) {
    if (!includeWarnings && error.isWarning()) {
      continue;
    }
    ss << error.toDetailedString(true) << "\n\n";
  }

  // Report paradoxes
  ss << "=== Paradoxes (" << getParadoxCount(includeResolved)
     << " total) ===\n";
  for (const auto &paradox : m_paradoxes) {
    if (!includeResolved && paradox.isResolved) {
      continue;
    }
    ss << paradox.toDetailedString(true) << "\n\n";
  }

  // Report glitches
  ss << "=== Glitches (" << getGlitchCount(includeResolved) << " total) ===\n";
  for (const auto &glitch : m_glitches) {
    if (!includeResolved && glitch.isFixed) {
      continue;
    }
    ss << glitch.toDetailedString(true) << "\n\n";
  }

  return ss.str();
}

bool ErrorHandler::executeErrorCallbacks(const ChronovyanError &error) {
  bool handled = false;

  for (const auto &[id, callback] : m_errorCallbacks) {
    if (callback(error)) {
      handled = true;
    }
  }

  return handled;
}

bool ErrorHandler::executeParadoxCallbacks(const ParadoxEvent &paradox) {
  bool handled = false;

  for (const auto &[id, callback] : m_paradoxCallbacks) {
    if (callback(paradox)) {
      handled = true;
    }
  }

  return handled;
}

ErrorHandler &ErrorHandler::getInstance() {
  static ErrorHandler instance;
  return instance;
}

// Helper functions
std::string formatErrorMessage(const std::string &message,
                               const SourceLocation &location,
                               const std::string &context,
                               bool includeLineContent) {
  std::stringstream ss;

  if (location.isValid()) {
    ss << "Error at " << location.toDetailedString(includeLineContent) << ": ";
  } else {
    ss << "Error: ";
  }

  ss << message;

  if (!context.empty()) {
    ss << " (Context: " << context << ")";
  }

  return ss.str();
}

bool isRecoverableError(ErrorSeverity severity) {
  return severity != ErrorSeverity::FATAL;
}

} // namespace chronovyan
