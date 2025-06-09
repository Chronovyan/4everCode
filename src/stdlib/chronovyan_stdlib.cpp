/**
 * @file chronovyan_stdlib.cpp
 * @brief Implementation of the main Chronovyan Standard Library functions
 *
 * This file contains the implementation of the main entry points for the
 * Chronovyan Standard Library, including initialization and shutdown functions.
 *
 * @copyright Chronovyan Project
 */

#include <iostream>

#include "stdlib/chronovyan_stdlib.h"
#include "stdlib/config.h"

namespace chronovyan {
namespace stdlib {

// Forward declarations for subsystem initialization and shutdown functions
bool initialize_temporal();
bool shutdown_temporal();

bool initialize_resource_management();
bool shutdown_resource_management();

bool initialize_pattern_management();
bool shutdown_pattern_management();

bool initialize_paradox_management();
bool shutdown_paradox_management();

bool initialize_exploration();
bool shutdown_exploration();

// Global state for the standard library
namespace {
bool library_initialized = false;
}

/**
 * @brief Initialize the Chronovyan Standard Library
 *
 * This function initializes all the subsystems of the Chronovyan Standard
 * Library. It must be called before using any standard library functions.
 *
 * @return True if initialization was successful, false otherwise
 */
bool initialize() {
  if (library_initialized) {
    std::cout << "Chronovyan Standard Library already initialized."
              << std::endl;
    return true;
  }

  std::cout << "Initializing Chronovyan Standard Library..." << std::endl;

  // Initialize subsystems in order
  if (!initialize_temporal()) {
    std::cerr << "Failed to initialize temporal manipulation subsystem."
              << std::endl;
    return false;
  }

  if (!initialize_resource_management()) {
    std::cerr << "Failed to initialize resource management subsystem."
              << std::endl;
    shutdown_temporal(); // Clean up already initialized subsystems
    return false;
  }

  if (!initialize_pattern_management()) {
    std::cerr << "Failed to initialize pattern management subsystem."
              << std::endl;
    shutdown_resource_management(); // Clean up already initialized subsystems
    shutdown_temporal();
    return false;
  }

  if (!initialize_paradox_management()) {
    std::cerr << "Failed to initialize paradox management subsystem."
              << std::endl;
    shutdown_pattern_management(); // Clean up already initialized subsystems
    shutdown_resource_management();
    shutdown_temporal();
    return false;
  }

  // For now, we'll just return true for the other subsystems since they're not
  // implemented yet In a full implementation, we would call the actual
  // initialization functions

  if (!initialize_exploration()) {
    std::cerr << "Failed to initialize exploration subsystem." << std::endl;
    shutdown_paradox_management(); // Clean up already initialized subsystems
    shutdown_pattern_management();
    shutdown_resource_management();
    shutdown_temporal();
    return false;
  }

  library_initialized = true;
  std::cout << "Chronovyan Standard Library initialized successfully."
            << std::endl;
  return true;
}

/**
 * @brief Shutdown the Chronovyan Standard Library
 *
 * This function shuts down all the subsystems of the Chronovyan Standard
 * Library. It should be called when the program is done using the library.
 *
 * @return True if shutdown was successful, false otherwise
 */
bool shutdown() {
  if (!library_initialized) {
    std::cout << "Chronovyan Standard Library not initialized." << std::endl;
    return true;
  }

  std::cout << "Shutting down Chronovyan Standard Library..." << std::endl;

  // Shutdown subsystems in reverse order
  bool success = true;

  // For now, we'll just assume the other subsystems shut down successfully
  // In a full implementation, we would call the actual shutdown functions

  if (!shutdown_exploration()) {
    std::cerr << "Failed to shut down exploration subsystem." << std::endl;
    success = false;
  }

  if (!shutdown_paradox_management()) {
    std::cerr << "Failed to shut down paradox management subsystem."
              << std::endl;
    success = false;
  }

  if (!shutdown_pattern_management()) {
    std::cerr << "Failed to shut down pattern management subsystem."
              << std::endl;
    success = false;
  }

  if (!shutdown_resource_management()) {
    std::cerr << "Failed to shut down resource management subsystem."
              << std::endl;
    success = false;
  }

  if (!shutdown_temporal()) {
    std::cerr << "Failed to shut down temporal manipulation subsystem."
              << std::endl;
    success = false;
  }

  library_initialized = false;

  if (success) {
    std::cout << "Chronovyan Standard Library shut down successfully."
              << std::endl;
  } else {
    std::cerr << "Chronovyan Standard Library shut down with errors."
              << std::endl;
  }

  return success;
}

} // namespace stdlib
} // namespace chronovyan