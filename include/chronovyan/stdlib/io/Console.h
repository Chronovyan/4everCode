#pragma once

#include <string>

namespace chronovyan::stdlib::io {

class Console {
public:
    // Log a message to standard output
    static void log(const std::string& message);

    // Log an error message to standard error
    static void error(const std::string& message);

    // Log a warning message to standard error
    static void warn(const std::string& message);
};

}  // namespace chronovyan::stdlib::io
