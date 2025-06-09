#include <fstream>
#include <sstream>

#include "chronovyan/stdlib/io/File.h"

namespace chronovyan::stdlib::io {

ValuePtr File::read(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return String::create(""); // Empty string in Chronovyan
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  return String::create(buffer.str());
}

bool File::write(const std::string &path, const std::string &content) {
  std::ofstream file(path);
  if (!file.is_open()) {
    return false;
  }

  file << content;
  return true;
}

bool File::exists(const std::string &path) {
  std::ifstream file(path);
  return file.good();
}

} // namespace chronovyan::stdlib::io
