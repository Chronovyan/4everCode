#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "source_file.h"
#include "source_location.h"

namespace chronovyan {

std::string SourceLocation::toString() const {
    if (!isValid()) {
        return "<unknown location>";
    }

    std::ostringstream oss;
    oss << sourceFile->getName() << ":" << line << ":" << column;
    return oss.str();
}

std::string SourceLocation::toDetailedString(bool showLineContent) const {
    if (!isValid()) {
        return "<unknown location>";
    }

    std::ostringstream oss;
    oss << sourceFile->getName() << ":" << line << ":" << column;

    if (showLineContent && sourceFile) {
        try {
            std::string lineContent = sourceFile->getLine(line);
            oss << "\n" << lineContent << "\n";

            // Add a pointer to the specific column
            for (size_t i = 1; i < column; ++i) {
                oss << " ";
            }
            oss << "^";
        } catch (const std::out_of_range&) {
            // Line number is invalid, just return the basic information
        }
    }

    return oss.str();
}

SourceLocation SourceLocation::createRange(const SourceLocation& end) const {
    if (!isValid() || !end.isValid()) {
        throw std::invalid_argument("Cannot create range from invalid locations");
    }

    if (sourceFile != end.sourceFile) {
        throw std::invalid_argument("Cannot create range across different files");
    }

    // For simplicity, we'll use the starting location's line and column
    // but store the position range in a way that can be used for highlighting
    return SourceLocation(sourceFile, line, column, end.position - position);
}

bool SourceLocation::isBefore(const SourceLocation& other) const {
    if (!isValid() || !other.isValid() || sourceFile != other.sourceFile) {
        return false;
    }

    return position < other.position;
}

bool SourceLocation::isAfter(const SourceLocation& other) const {
    if (!isValid() || !other.isValid() || sourceFile != other.sourceFile) {
        return false;
    }

    return position > other.position;
}

std::string SourceLocation::getFilename() const {
    if (!sourceFile) {
        return "<unknown>";
    }

    return sourceFile->getName();
}

bool SourceLocation::operator==(const SourceLocation& other) const {
    if (!isValid() && !other.isValid()) {
        return true;  // Two invalid locations are considered equal
    }

    if (!isValid() || !other.isValid()) {
        return false;  // One valid, one invalid
    }

    return sourceFile == other.sourceFile && line == other.line && column == other.column &&
           position == other.position;
}

bool SourceLocation::operator!=(const SourceLocation& other) const { return !(*this == other); }

SourceLocation createBestLocation(std::shared_ptr<const SourceFile> file, size_t line,
                                  size_t column, size_t position) {
    // If we have a file but no position information, try to get the start of the file
    if (file && line == 0 && column == 0 && position == 0) {
        return SourceLocation(file, 1, 1, 0);
    }

    // If we have a file and position but no line/column, try to compute them
    if (file && line == 0 && column == 0 && position > 0) {
        try {
            auto [l, c] = file->getLineAndColumn(position);
            return SourceLocation(file, l, c, position);
        } catch (const std::out_of_range&) {
            // Fall back to the file start
            return SourceLocation(file, 1, 1, 0);
        }
    }

    // If we have line/column but no position, try to compute it
    if (file && line > 0 && column > 0 && position == 0) {
        try {
            size_t pos = file->getPosition(line, column);
            return SourceLocation(file, line, column, pos);
        } catch (const std::out_of_range&) {
            // Fall back to the provided line/column but position 0
            return SourceLocation(file, line, column, 0);
        }
    }

    // Use whatever we have
    return SourceLocation(file, line, column, position);
}

SourceLocation mergeLocations(const SourceLocation& start, const SourceLocation& end) {
    // If either location is invalid, return the other
    if (!start.isValid()) {
        return end;
    }

    if (!end.isValid()) {
        return start;
    }

    // If they're from different files, we can't merge them
    if (start.sourceFile != end.sourceFile) {
        // Return the first one by convention
        return start;
    }

    // Create a location that spans from start to end
    try {
        return start.createRange(end);
    } catch (const std::invalid_argument&) {
        // If we can't create a range, return the start location
        return start;
    }
}

}  // namespace chronovyan