#pragma once

/**
 * @file chronovyan.hpp
 * @brief Main header file for the Chronovyan library
 */

#include <memory>
#include <string>

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

private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

}  // namespace chronovyan
