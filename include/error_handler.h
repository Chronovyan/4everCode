#ifndef CHRONOVYAN_ERROR_HANDLER_H
#define CHRONOVYAN_ERROR_HANDLER_H

#include <chrono>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "source_location.h"

namespace chronovyan {

// Forward declarations
class TemporalRuntime;

/**
 * @enum ErrorSeverity
 * @brief Defines the severity levels for errors
 *
 * Severity levels help determine how errors should be handled:
 * - WARNING: Non-critical issues that don't prevent execution
 * - ERROR: Problems that prevent execution but allow recovery
 * - FATAL: Unrecoverable errors that require termination
 */
enum class ErrorSeverity {
    WARNING,  // Non-critical issues that don't prevent execution
    ERROR,    // Problems that prevent execution but allow recovery
    FATAL     // Unrecoverable errors that require termination
};

/**
 * @enum ParadoxType
 * @brief Types of paradoxes that can occur during temporal operations
 */
enum class ParadoxType {
    TIMELINE_CONFLICT,      // Conflict between timelines
    QUANTUM_CONTRADICTION,  // Contradiction in quantum state
    STABILITY_BREACH,       // Breach in system stability
    RESOURCE_CONFLICT,      // Conflict in resource allocation
    CAUSALITY_VIOLATION     // Violation of causality principles
};

/**
 * @enum GlitchType
 * @brief Types of glitches that can occur during temporal operations
 */
enum class GlitchType {
    VARIABLE_INSTABILITY,  // Unstable variable state
    CHRONON_LEAKAGE,       // Leakage of chronon particles
    AETHEL_FLUCTUATION,    // Unstable aethel flow
    TIMELINE_DISTORTION,   // Timeline shape distortion
    QUANTUM_NOISE          // Quantum noise affecting calculations
};

/**
 * @struct ChronovyanError
 * @brief Represents an error in Chronovyan code
 */
struct ChronovyanError {
    SourceLocation location;  // Where the error occurred
    std::string message;      // The error message
    ErrorSeverity severity;   // The error severity

    ChronovyanError(const SourceLocation& loc, const std::string& msg,
                    ErrorSeverity sev = ErrorSeverity::ERROR)
        : location(loc), message(msg), severity(sev) {}

    /**
     * @brief Get a formatted string representation of this error
     * @return A string representing the error
     */
    std::string toString() const;

    /**
     * @brief Get a detailed string representation of this error with optional line content
     * @param includeLineContent Whether to include the line content in the output
     * @return A detailed string representing the error
     */
    std::string toDetailedString(bool includeLineContent = false) const;

    /**
     * @brief Check if this is a warning (not a critical error)
     * @return True if this is a warning
     */
    bool isWarning() const { return severity == ErrorSeverity::WARNING; }

    /**
     * @brief Check if this is a fatal error
     * @return True if this is a fatal error
     */
    bool isFatal() const { return severity == ErrorSeverity::FATAL; }
};

/**
 * @struct ParadoxEvent
 * @brief Represents a paradox in Chronovyan code
 */
struct ParadoxEvent {
    SourceLocation location;                          // Where the paradox occurred
    std::string message;                              // The paradox message
    ParadoxType type;                                 // The type of paradox
    int paradoxLevel;                                 // Severity level (1-10)
    bool isResolved = false;                          // Whether the paradox has been resolved
    std::chrono::system_clock::time_point timestamp;  // When the paradox occurred

    ParadoxEvent(const SourceLocation& loc, const std::string& msg, ParadoxType typ, int level)
        : location(loc)
        , message(msg)
        , type(typ)
        , paradoxLevel(level)
        , timestamp(std::chrono::system_clock::now()) {}

    /**
     * @brief Get a formatted string representation of this paradox
     * @return A string representing the paradox
     */
    std::string toString() const;

    /**
     * @brief Get a detailed string representation of this paradox with optional line content
     * @param includeLineContent Whether to include the line content in the output
     * @return A detailed string representing the paradox
     */
    std::string toDetailedString(bool includeLineContent = false) const;

    /**
     * @brief Convert a paradox type to a string
     * @param type The paradox type
     * @return A string representation of the paradox type
     */
    static std::string paradoxTypeToString(ParadoxType type);
};

/**
 * @struct GlitchEvent
 * @brief Represents a temporal glitch in Chronovyan code
 */
struct GlitchEvent {
    SourceLocation location;                          // Where the glitch occurred
    std::string message;                              // The glitch message
    GlitchType type;                                  // The type of glitch
    double severity;                                  // Severity level (0.0-1.0)
    bool isFixed = false;                             // Whether the glitch has been fixed
    std::chrono::system_clock::time_point timestamp;  // When the glitch occurred

    GlitchEvent(const SourceLocation& loc, const std::string& msg, GlitchType typ, double sev)
        : location(loc)
        , message(msg)
        , type(typ)
        , severity(sev)
        , timestamp(std::chrono::system_clock::now()) {}

    /**
     * @brief Get a formatted string representation of this glitch
     * @return A string representing the glitch
     */
    std::string toString() const;

    /**
     * @brief Get a detailed string representation of this glitch with optional line content
     * @param includeLineContent Whether to include the line content in the output
     * @return A detailed string representing the glitch
     */
    std::string toDetailedString(bool includeLineContent = false) const;

    /**
     * @brief Convert a glitch type to a string
     * @param type The glitch type
     * @return A string representation of the glitch type
     */
    static std::string glitchTypeToString(GlitchType type);
};

/**
 * @using ErrorCallback
 * @brief Callback type for error handling
 *
 * This function is called when an error occurs, and can be used
 * to implement custom error handling logic.
 *
 * @param error The error that occurred
 * @return true if the error was handled, false otherwise
 */
using ErrorCallback = std::function<bool(const ChronovyanError&)>;

/**
 * @using ParadoxCallback
 * @brief Callback type for paradox handling
 *
 * This function is called when a paradox occurs, and can be used
 * to implement custom paradox handling logic.
 *
 * @param paradox The paradox that occurred
 * @return true if the paradox was handled, false otherwise
 */
using ParadoxCallback = std::function<bool(const ParadoxEvent&)>;

/**
 * @class ErrorHandler
 * @brief Manages errors, warnings, paradoxes, and glitches during interpretation
 *
 * This class provides a centralized system for reporting and handling errors,
 * warnings, paradoxes, and glitches that occur during Chronovyan program execution.
 * It is implemented as a singleton to ensure global access.
 */
class ErrorHandler {
public:
    /**
     * @brief Add an error to the collection
     * @param location The source location where the error occurred
     * @param message The error message
     * @param severity The error severity (default: ERROR)
     * @return The error ID
     */
    int reportError(const SourceLocation& location, const std::string& message,
                    ErrorSeverity severity = ErrorSeverity::ERROR);

    /**
     * @brief Add a warning to the collection
     * @param location The source location where the warning occurred
     * @param message The warning message
     * @return The warning ID
     */
    int reportWarning(const SourceLocation& location, const std::string& message);

    /**
     * @brief Add a fatal error and throw an exception
     * @param location The source location where the error occurred
     * @param message The error message
     * @throws ChronovyanFatalError
     */
    [[noreturn]] void reportFatalError(const SourceLocation& location, const std::string& message);

    /**
     * @brief Report a paradox event
     * @param location The source location where the paradox occurred
     * @param message The paradox message
     * @param type The type of paradox
     * @param level The paradox severity level (1-10)
     * @return The paradox ID
     */
    int reportParadox(const SourceLocation& location, const std::string& message, ParadoxType type,
                      int level);

    /**
     * @brief Report a temporal glitch
     * @param location The source location where the glitch occurred
     * @param message The glitch message
     * @param type The type of glitch
     * @param severity The glitch severity (0.0-1.0)
     * @return The glitch ID
     */
    int reportGlitch(const SourceLocation& location, const std::string& message, GlitchType type,
                     double severity);

    /**
     * @brief Resolve a paradox
     * @param paradoxId The ID of the paradox to resolve
     * @return True if the paradox was resolved, false otherwise
     */
    bool resolveParadox(int paradoxId);

    /**
     * @brief Fix a glitch
     * @param glitchId The ID of the glitch to fix
     * @return True if the glitch was fixed, false otherwise
     */
    bool fixGlitch(int glitchId);

    /**
     * @brief Get the current error count
     * @param includeWarnings Whether to include warnings in the count
     * @return The number of errors
     */
    size_t getErrorCount(bool includeWarnings = false) const;

    /**
     * @brief Get the current paradox count
     * @param includeResolved Whether to include resolved paradoxes in the count
     * @return The number of paradoxes
     */
    size_t getParadoxCount(bool includeResolved = false) const;

    /**
     * @brief Get the current glitch count
     * @param includeFixed Whether to include fixed glitches in the count
     * @return The number of glitches
     */
    size_t getGlitchCount(bool includeFixed = false) const;

    /**
     * @brief Get the list of errors
     * @return A reference to the vector of errors
     */
    const std::vector<ChronovyanError>& getErrors() const { return m_errors; }

    /**
     * @brief Get the list of paradoxes
     * @return A reference to the vector of paradoxes
     */
    const std::vector<ParadoxEvent>& getParadoxes() const { return m_paradoxes; }

    /**
     * @brief Get the list of glitches
     * @return A reference to the vector of glitches
     */
    const std::vector<GlitchEvent>& getGlitches() const { return m_glitches; }

    /**
     * @brief Register a callback for error handling
     * @param callback The callback function
     * @return The callback ID
     */
    int registerErrorCallback(ErrorCallback callback);

    /**
     * @brief Register a callback for paradox handling
     * @param callback The callback function
     * @return The callback ID
     */
    int registerParadoxCallback(ParadoxCallback callback);

    /**
     * @brief Remove an error callback
     * @param callbackId The ID of the callback to remove
     * @return True if the callback was removed, false otherwise
     */
    bool removeErrorCallback(int callbackId);

    /**
     * @brief Remove a paradox callback
     * @param callbackId The ID of the callback to remove
     * @return True if the callback was removed, false otherwise
     */
    bool removeParadoxCallback(int callbackId);

    /**
     * @brief Reset the error handler
     *
     * This clears all errors, paradoxes, glitches, and callbacks.
     */
    void reset();

    /**
     * @brief Get the singleton instance
     * @return A reference to the singleton instance
     */
    static ErrorHandler& getInstance();

    /**
     * @brief Get a string report of all errors, paradoxes, and glitches
     * @param includeWarnings Whether to include warnings in the report
     * @param includeResolved Whether to include resolved paradoxes and fixed glitches
     * @return A string containing the report
     */
    std::string getReport(bool includeWarnings = true, bool includeResolved = false) const;

    /**
     * @brief Set the runtime context for error messages
     * @param context The runtime context
     */
    void setRuntimeContext(const std::string& context) { m_runtimeContext = context; }

    /**
     * @brief Get the runtime context for error messages
     * @return The runtime context
     */
    const std::string& getRuntimeContext() const { return m_runtimeContext; }

    /**
     * @brief Enable or disable console output for errors and warnings
     * @param enabled Whether console output is enabled
     */
    void setConsoleOutput(bool enabled) { m_consoleOutput = enabled; }

    /**
     * @brief Set the minimum severity level for console output
     * @param severity The minimum severity level
     */
    void setMinConsoleSeverity(ErrorSeverity severity) { m_minConsoleSeverity = severity; }

    /**
     * @brief Set the minimum severity level for glitch console output
     * @param severity The minimum severity level (0.0-1.0)
     */
    void setMinGlitchSeverity(double severity) { m_minGlitchSeverity = severity; }

private:
    /**
     * @brief Private constructor for singleton pattern
     */
    ErrorHandler()
        : m_consoleOutput(true)
        , m_minConsoleSeverity(ErrorSeverity::WARNING)
        , m_minGlitchSeverity(0.5) {}

    /**
     * @brief Private copy constructor to prevent copying
     */
    ErrorHandler(const ErrorHandler&) = delete;

    /**
     * @brief Private assignment operator to prevent assignment
     */
    ErrorHandler& operator=(const ErrorHandler&) = delete;

    /**
     * @brief Execute all registered error callbacks
     * @param error The error to process
     * @return True if any callback handled the error, false otherwise
     */
    bool executeErrorCallbacks(const ChronovyanError& error);

    /**
     * @brief Execute all registered paradox callbacks
     * @param paradox The paradox to process
     * @return True if any callback handled the paradox, false otherwise
     */
    bool executeParadoxCallbacks(const ParadoxEvent& paradox);

    std::vector<ChronovyanError> m_errors;  // All errors that have occurred
    std::vector<ParadoxEvent> m_paradoxes;  // All paradoxes that have occurred
    std::vector<GlitchEvent> m_glitches;    // All glitches that have occurred
    std::vector<std::pair<int, ErrorCallback>> m_errorCallbacks;      // Error handling callbacks
    std::vector<std::pair<int, ParadoxCallback>> m_paradoxCallbacks;  // Paradox handling callbacks
    std::string m_runtimeContext;        // Runtime context for error messages
    int m_nextErrorCallbackId = 0;       // ID for the next error callback
    int m_nextParadoxCallbackId = 0;     // ID for the next paradox callback
    bool m_consoleOutput;                // Whether to output errors to the console
    ErrorSeverity m_minConsoleSeverity;  // Minimum severity level for console output
    double m_minGlitchSeverity;          // Minimum severity level for glitch console output
};

/**
 * @class ChronovyanException
 * @brief Base class for all Chronovyan exceptions
 */
class ChronovyanException : public std::runtime_error {
public:
    ChronovyanException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @class ChronovyanParseError
 * @brief Exception for parsing errors
 */
class ChronovyanParseError : public ChronovyanException {
public:
    ChronovyanParseError(const std::string& message, const SourceLocation& location)
        : ChronovyanException(message), m_location(location) {}

    /**
     * @brief Get the source location where the error occurred
     * @return The source location
     */
    const SourceLocation& getLocation() const { return m_location; }

private:
    SourceLocation m_location;
};

/**
 * @class ChronovyanRuntimeError
 * @brief Exception for runtime errors
 */
class ChronovyanRuntimeError : public ChronovyanException {
public:
    ChronovyanRuntimeError(const std::string& message, const SourceLocation& location)
        : ChronovyanException(message), m_location(location) {}

    /**
     * @brief Get the source location where the error occurred
     * @return The source location
     */
    const SourceLocation& getLocation() const { return m_location; }

private:
    SourceLocation m_location;
};

/**
 * @class ChronovyanParadoxError
 * @brief Exception for paradox errors
 */
class ChronovyanParadoxError : public ChronovyanException {
public:
    ChronovyanParadoxError(const std::string& message, const SourceLocation& location,
                           ParadoxType type)
        : ChronovyanException(message), m_location(location), m_type(type) {}

    /**
     * @brief Get the source location where the error occurred
     * @return The source location
     */
    const SourceLocation& getLocation() const { return m_location; }

    /**
     * @brief Get the paradox type
     * @return The paradox type
     */
    ParadoxType getType() const { return m_type; }

private:
    SourceLocation m_location;
    ParadoxType m_type;
};

/**
 * @class ChronovyanGlitchError
 * @brief Exception for glitch errors
 */
class ChronovyanGlitchError : public ChronovyanException {
public:
    ChronovyanGlitchError(const std::string& message, const SourceLocation& location,
                          GlitchType type)
        : ChronovyanException(message), m_location(location), m_type(type) {}

    /**
     * @brief Get the source location where the error occurred
     * @return The source location
     */
    const SourceLocation& getLocation() const { return m_location; }

    /**
     * @brief Get the glitch type
     * @return The glitch type
     */
    GlitchType getType() const { return m_type; }

private:
    SourceLocation m_location;
    GlitchType m_type;
};

/**
 * @brief Format an error message with source location and context
 * @param message The error message
 * @param location The source location
 * @param context The runtime context
 * @param includeLineContent Whether to include the line content in the output
 * @return A formatted error message
 */
std::string formatErrorMessage(const std::string& message, const SourceLocation& location,
                               const std::string& context, bool includeLineContent = false);

/**
 * @brief Check if an error is recoverable
 * @param severity The error severity
 * @return True if the error is recoverable, false otherwise
 */
bool isRecoverableError(ErrorSeverity severity);

}  // namespace chronovyan

#endif  // CHRONOVYAN_ERROR_HANDLER_H