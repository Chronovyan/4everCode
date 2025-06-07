#ifndef CHRONOVYAN_LOCATION_H
#define CHRONOVYAN_LOCATION_H

#include <string>

namespace chronovyan {

/**
 * Represents a location in source code.
 */
struct Location {
    std::string filename;
    int line;
    int column;

    Location() : filename(""), line(0), column(0) {}

    Location(const std::string& fn, int l, int c) : filename(fn), line(l), column(c) {}

    std::string getFilename() const { return filename; }
    int getLine() const { return line; }
    int getColumn() const { return column; }

    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_LOCATION_H