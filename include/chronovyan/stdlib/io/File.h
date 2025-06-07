#pragma once

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::io {

class File {
public:
    // Read file contents as a string
    static ValuePtr read(const std::string& path);

    // Write content to a file
    static bool write(const std::string& path, const std::string& content);

    // Check if file exists
    static bool exists(const std::string& path);
};

}  // namespace chronovyan::stdlib::io
