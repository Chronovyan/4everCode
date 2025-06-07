#include <iomanip>
#include <iostream>
#include <sstream>

#include "chronovyan/Error.h"

namespace chronovyan {

// Initialize static member
ErrorHandler* ErrorHandler::currentHandler_ = nullptr;

namespace {  // Anonymous namespace for helper functions

// Format an error message with source location
std::string formatErrorMessage(const ErrorInfo& info, const std::source_location& loc) {
    std::ostringstream ss;

    // Format: [category:code] message (at file:line:column)
    ss << '[' << categoryToString(info.code.category) << ':' << info.code.code << "] "
       << info.message << " (at " << (loc.file_name() ? loc.file_name() : "") << ':' << loc.line()
       << ':' << loc.column() << ')';

    // Add notes if any
    for (const auto& note : info.notes) {
        ss << "\n  note: " << note;
    }

    return ss.str();
}

// Convert error category to string
const char* categoryToString(ErrorCategory category) {
    using EC = ErrorCategory;
    switch (category) {
        case EC::LEXER:
            return "lexer";
        case EC::PARSER:
            return "parser";
        case EC::SEMANTIC:
            return "semantic";
        case EC::TYPE_SYSTEM:
            return "type";
        case EC::RUNTIME:
            return "runtime";
        case EC::INTERNAL:
            return "internal";
        case EC::IO:
            return "io";
        default:
            return "unknown";
    }
}

}  // anonymous namespace

// ChronovyanError implementation
ChronovyanError::ChronovyanError(ErrorInfo info, std::source_location loc)
    : std::runtime_error(formatErrorMessage(info, loc)), errorInfo_(std::move(info)) {
    // Set the location in the error info
    errorInfo_.location = {
        loc.file_name() ? loc.file_name() : "", static_cast<int>(loc.line()),
        static_cast<int>(loc.column()),
        ""  // Line text would need to be populated from the source file
    };
}

// Create a new error with the given code and message
ChronovyanError ChronovyanError::create(ErrorCode code, const std::string& message,
                                        const std::source_location& loc) {
    return ChronovyanError({code, message, {}, ErrorSeverity::ERROR}, loc);
}

// Add a note to the error
void ChronovyanError::addNote(const std::string& note) { errorInfo_.notes.push_back(note); }

// Default error handler implementation
void ErrorHandler::handleError(const ChronovyanError& error) { throw error; }

// Default warning handler implementation
void ErrorHandler::handleWarning(ErrorCode code, const std::string& message,
                                 const std::source_location& loc) {
    std::cerr << "Warning: [" << categoryToString(code.category) << ":" << code.code << "] "
              << message << " (at " << (loc.file_name() ? loc.file_name() : "") << ':' << loc.line()
              << ':' << loc.column() << ')' << std::endl;
}

// Set a custom error handler
void ErrorHandler::setHandler(ErrorHandler* handler) { currentHandler_ = handler; }

// Get the current error handler
ErrorHandler* ErrorHandler::getInstance() {
    static ErrorHandler defaultHandler;
    return currentHandler_ ? currentHandler_ : &defaultHandler;
}

}  // namespace chronovyan
