#pragma once

#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace chronovyan {

// Error severity levels
enum class ErrorSeverity {
    NOTE,     // Informational message
    WARNING,  // Warning that doesn't stop execution
    ERROR,    // Recoverable error
    FATAL     // Non-recoverable error
};

// Error categories
enum class ErrorCategory { LEXER, PARSER, SEMANTIC, TYPE_SYSTEM, RUNTIME, INTERNAL, IO, UNKNOWN };

// Error code structure
struct ErrorCode {
    int code;
    ErrorCategory category;
    const char* message;
};

// Common error codes
namespace ErrorCodes {
constexpr ErrorCode UNEXPECTED_TOKEN{100, ErrorCategory::LEXER, "Unexpected token"};
constexpr ErrorCode SYNTAX_ERROR{200, ErrorCategory::PARSER, "Syntax error"};
constexpr ErrorCode TYPE_MISMATCH{300, ErrorCategory::TYPE_SYSTEM, "Type mismatch"};
// Add more error codes as needed
}  // namespace ErrorCodes

// Source location information
struct SourceLocation {
    std::string file;
    int line;
    int column;
    std::string lineText;
};

// Error info structure
struct ErrorInfo {
    ErrorCode code;
    std::string message;
    SourceLocation location;
    ErrorSeverity severity;
    std::vector<std::string> notes;
};

// Main error class
class ChronovyanError : public std::runtime_error {
public:
    ChronovyanError(ErrorInfo info, std::source_location loc = std::source_location::current())
        : std::runtime_error(formatMessage(info, loc)), errorInfo_(std::move(info)) {}

    const ErrorInfo& info() const { return errorInfo_; }
    ErrorSeverity severity() const { return errorInfo_.severity; }
    const ErrorCode& code() const { return errorInfo_.code; }

    // Helper to create error with location
    template <typename... Args>
    static ChronovyanError create(ErrorCode code, const std::string& message, Args&&... args) {
        ErrorInfo info{code, message, {}, ErrorSeverity::ERROR};
        return ChronovyanError(info);
    }

    // Add a note to the error
    void addNote(const std::string& note) { errorInfo_.notes.push_back(note); }

private:
    static std::string formatMessage(const ErrorInfo& info, const std::source_location& loc) {
        std::ostringstream ss;

        // Format: [category:code] message (at file:line:column)
        ss << '[' << categoryToString(info.code.category) << ':' << info.code.code << "] "
           << info.message << " (at " << loc.file_name() << ':' << loc.line() << ')';

        return ss.str();
    }

    static const char* categoryToString(ErrorCategory category) {
        switch (category) {
            case ErrorCategory::LEXER:
                return "lexer";
            case ErrorCategory::PARSER:
                return "parser";
            case ErrorCategory::SEMANTIC:
                return "semantic";
            case ErrorCategory::TYPE_SYSTEM:
                return "type";
            case ErrorCategory::RUNTIME:
                return "runtime";
            case ErrorCategory::INTERNAL:
                return "internal";
            case ErrorCategory::IO:
                return "io";
            default:
                return "unknown";
        }
    }

    ErrorInfo errorInfo_;
};

// Helper macro for throwing errors with source location
#define CHRONOVYAN_ERROR(code, message) \
    throw chronovyan::ChronovyanError::create(code, message, std::source_location::current())

// Helper macro for warnings
#define CHRONOVYAN_WARNING(code, message)                                       \
    if (auto handler = chronovyan::ErrorHandler::getInstance()) {               \
        handler->handleWarning(code, message, std::source_location::current()); \
    }

// Error handler interface
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;

    // Handle an error (can be overridden by derived classes)
    virtual void handleError(const ChronovyanError& error) { throw error; }

    // Handle a warning (can be overridden by derived classes)
    virtual void handleWarning(ErrorCode code, const std::string& message,
                               const std::source_location& loc) {
        std::cerr << "Warning: [" << static_cast<int>(code.code) << "] " << message << " (at "
                  << loc.file_name() << ':' << loc.line() << ')' << std::endl;
    }

    // Get the current error handler
    static ErrorHandler* getInstance() {
        static ErrorHandler defaultHandler;
        return currentHandler_ ? currentHandler_ : &defaultHandler;
    }

    // Set a custom error handler
    static void setHandler(ErrorHandler* handler) { currentHandler_ = handler; }

private:
    static ErrorHandler* currentHandler_;
};

}  // namespace chronovyan
