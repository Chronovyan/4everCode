#ifndef CHRONOVYAN_SOURCE_LOCATION_H
#define CHRONOVYAN_SOURCE_LOCATION_H

#include <memory>
#include <string>
#include <utility>

namespace chronovyan {

// Forward declaration to avoid circular includes
class SourceFile;

/**
 * @struct SourceLocation
 * @brief Represents a location in the source code (file, line, column)
 *
 * SourceLocation is a fundamental component for error reporting and debugging.
 * It tracks the file, line, column, and character position of code elements,
 * enabling precise error messages and debugging information.
 *
 * When creating a SourceLocation, prefer using actual locations from the source
 * whenever possible. Only use the default constructor for truly unknown locations.
 */
struct SourceLocation {
    /// The source file containing this location
    std::shared_ptr<const SourceFile> sourceFile;

    /// The 1-based line number
    size_t line;

    /// The 1-based column number
    size_t column;

    /// The 0-based character offset in the source string
    size_t position;

    /**
     * @brief Create an invalid/unknown source location
     *
     * Use this constructor only when the source location is genuinely unknown.
     * Prefer creating locations with actual source information whenever possible.
     */
    SourceLocation() : line(0), column(0), position(0) {}

    /**
     * @brief Create a source location with known coordinates
     *
     * @param file The source file
     * @param l The 1-based line number
     * @param c The 1-based column number
     * @param pos The 0-based character offset in the source string
     */
    SourceLocation(std::shared_ptr<const SourceFile> file, size_t l, size_t c, size_t pos)
        : sourceFile(std::move(file)), line(l), column(c), position(pos) {}

    /**
     * @brief Check if this location is valid (has a source file and positive line/column)
     *
     * @return true if the location is valid, false otherwise
     */
    bool isValid() const { return sourceFile != nullptr && line > 0 && column > 0; }

    /**
     * @brief Get a string representation of this location
     *
     * @return A string in the format "file:line:column" or "<unknown location>" if invalid
     */
    std::string toString() const;

    /**
     * @brief Get a detailed string representation with context
     *
     * @param showLineContent Whether to include the actual line of code in the output
     * @return A detailed string representation with the location and optional source line
     */
    std::string toDetailedString(bool showLineContent = true) const;

    /**
     * @brief Create a new location that spans from this location to another
     *
     * This is useful for creating error messages that highlight a range of code.
     *
     * @param end The ending location
     * @return A new location representing the range
     * @throws std::invalid_argument if the locations are in different files
     */
    SourceLocation createRange(const SourceLocation& end) const;

    /**
     * @brief Check if this location is before another location
     *
     * @param other The other location to compare with
     * @return true if this location is before the other, false otherwise
     */
    bool isBefore(const SourceLocation& other) const;

    /**
     * @brief Check if this location is after another location
     *
     * @param other The other location to compare with
     * @return true if this location is after the other, false otherwise
     */
    bool isAfter(const SourceLocation& other) const;

    /**
     * @brief Get the filename from the source file
     *
     * @return The filename or "<unknown>" if the source file is null
     */
    std::string getFilename() const;

    /**
     * @brief Compare two locations for equality
     *
     * @param other The other location to compare with
     * @return true if the locations are equal, false otherwise
     */
    bool operator==(const SourceLocation& other) const;

    /**
     * @brief Compare two locations for inequality
     *
     * @param other The other location to compare with
     * @return true if the locations are not equal, false otherwise
     */
    bool operator!=(const SourceLocation& other) const;
};

/**
 * @brief Create the best possible source location from available information
 *
 * This helper function tries to create a valid source location using the provided
 * parameters, falling back to more general locations if specific information is
 * not available.
 *
 * @param file The source file (can be null)
 * @param line The 1-based line number (0 if unknown)
 * @param column The 1-based column number (0 if unknown)
 * @param position The 0-based character offset (0 if unknown)
 * @return The best possible source location
 */
SourceLocation createBestLocation(std::shared_ptr<const SourceFile> file = nullptr, size_t line = 0,
                                  size_t column = 0, size_t position = 0);

/**
 * @brief Create a merged location that spans from the start to the end location
 *
 * This is useful for creating error messages that highlight a range of code.
 * If either location is invalid, the other is returned. If both are invalid,
 * an invalid location is returned.
 *
 * @param start The starting location
 * @param end The ending location
 * @return A location representing the range, or the valid location if one is invalid
 */
SourceLocation mergeLocations(const SourceLocation& start, const SourceLocation& end);

}  // namespace chronovyan

#endif  // CHRONOVYAN_SOURCE_LOCATION_H