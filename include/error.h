#ifndef CHRONOVYAN_ERROR_H
#define CHRONOVYAN_ERROR_H

#include <stdexcept>
#include <string>

namespace chronovyan {

// Since there's already a ChronovyanError in error_handler.h, we'll use different names

/**
 * @class RuntimeError
 * @brief Base class for all Chronovyan runtime errors
 */
class RuntimeError : public std::runtime_error {
public:
    explicit RuntimeError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @class TypeError
 * @brief Error thrown when a type mismatch occurs
 */
class TypeError : public RuntimeError {
public:
    explicit TypeError(const std::string& message) : RuntimeError(message) {}
};

/**
 * @class ReferenceError
 * @brief Error thrown when a reference to an undefined variable or property is made
 */
class ReferenceError : public RuntimeError {
public:
    explicit ReferenceError(const std::string& message) : RuntimeError(message) {}
};

/**
 * @class SyntaxError
 * @brief Error thrown when a syntax error occurs during parsing
 */
class SyntaxError : public RuntimeError {
public:
    explicit SyntaxError(const std::string& message) : RuntimeError(message) {}
};

/**
 * @class ResourceError
 * @brief Error thrown when a resource allocation/deallocation error occurs
 */
class ResourceError : public RuntimeError {
public:
    explicit ResourceError(const std::string& message) : RuntimeError(message) {}
};

/**
 * @class TemporalError
 * @brief Error thrown when a temporal operation error occurs
 */
class TemporalError : public RuntimeError {
public:
    explicit TemporalError(const std::string& message) : RuntimeError(message) {}
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_ERROR_H