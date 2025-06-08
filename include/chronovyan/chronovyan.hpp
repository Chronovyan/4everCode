#pragma once

/**
 * @file chronovyan.hpp
 * @brief Main header file for the Chronovyan library
 */

#include <chrono>
#include <memory>
#include <string>

// Define a namespace alias for convenience
namespace chrono = std::chrono;

namespace chronovyan {

/**
 * @brief Main class for the Chronovyan library
 */
class Chronovyan {
public:
  /**
   * @brief Create a new Chronovyan instance
   */
  Chronovyan();

  /**
   * @brief Destroy the Chronovyan instance
   */
  ~Chronovyan();

  /**
   * @brief Get the version string
   * @return The version string
   */
  static std::string version() noexcept;

  /**
   * @brief Check if the library is ready
   * @return true if ready, false otherwise
   */
  bool is_ready() const noexcept;

  /**
   * @brief Get the current time point
   * @return A time point representing the current time
   */
  std::chrono::system_clock::time_point now() const;

  /**
   * @brief Format a time point as a string
   * @param time The time point to format
   * @param format The format string (supports strftime format with %f for
   * milliseconds)
   * @return Formatted time string
   */
  std::string
  format_time(const std::chrono::system_clock::time_point &time,
              const std::string &format = "%Y-%m-%d %H:%M:%S") const;

  /**
   * @brief Parse a time string into a time point
   * @param time_str The time string to parse
   * @param format The format string (supports strptime format)
   * @return A time point representing the parsed time
   * @throws std::runtime_error if parsing fails
   */
  std::chrono::system_clock::time_point
  parse_time(const std::string &time_str,
             const std::string &format = "%Y-%m-%d %H:%M:%S") const;

private:
  class Impl;
  std::unique_ptr<Impl> pimpl_;
};

} // namespace chronovyan
