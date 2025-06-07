#ifndef CHRONOVYAN_COMPILER_SOURCE_LOCATION_H
#define CHRONOVYAN_COMPILER_SOURCE_LOCATION_H

#include <string>

namespace chronovyan {
namespace compiler {

/**
 * Represents a location in the source code.
 */
struct SourceLocation {
    std::string filename;
    int line;
    int column;

    SourceLocation() : line(0), column(0) {}

    SourceLocation(std::string filename, int line, int column)
        : filename(std::move(filename)), line(line), column(column) {}

    bool isValid() const { return line > 0; }
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_SOURCE_LOCATION_H