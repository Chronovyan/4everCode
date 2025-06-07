#include <iostream>

#include "chronovyan/stdlib/io/Console.h"

namespace chronovyan::stdlib::io {

void Console::log(const std::string& message) { std::cout << message << std::endl; }

void Console::error(const std::string& message) { std::cerr << "Error: " << message << std::endl; }

void Console::warn(const std::string& message) { std::cerr << "Warning: " << message << std::endl; }

}  // namespace chronovyan::stdlib::io
