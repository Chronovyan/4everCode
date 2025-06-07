/**
 * @file paradox_management.h
 * @brief Paradox management types and functions for the Chronovyan Standard Library
 *
 * This file contains definitions for paradox management, which helps detect and
 * prevent temporal paradoxes in Chronovyan programs.
 *
 * @copyright Chronovyan Project
 */

#ifndef CHRONOVYAN_STDLIB_PARADOX_MANAGEMENT_H
#define CHRONOVYAN_STDLIB_PARADOX_MANAGEMENT_H

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "resource_management.h"

namespace chronovyan {
namespace stdlib {

/**
 * @brief Type alias for paradox level
 *
 * The paradox level represents the amount of temporal instability
 * in a Chronovyan program. Higher values indicate more instability.
 */
using PARADOX_LEVEL = double;

/**
 * @brief Paradox alert callback function type
 *
 * This function is called when a paradox alert is triggered.
 */
using ParadoxAlertCallback = std::function<void(PARADOX_LEVEL)>;

/**
 * @brief Initializes the paradox management subsystem
 *
 * This function must be called before using any other paradox management functions.
 *
 * @return true if initialization was successful, false otherwise
 */
bool initialize_paradox_management();

/**
 * @brief Shuts down the paradox management subsystem
 *
 * This function should be called when the paradox management subsystem
 * is no longer needed.
 *
 * @return true if shutdown was successful, false otherwise
 */
bool shutdown_paradox_management();

/**
 * @brief Gets the current paradox level
 *
 * @return The current paradox level
 */
PARADOX_LEVEL get_paradox_level();

/**
 * @brief Sets a paradox alert callback
 *
 * The callback will be called when the paradox level reaches or exceeds
 * the specified threshold.
 *
 * @param threshold The paradox level threshold
 * @param callback The callback function to call
 * @return true if the alert was successfully set, false otherwise
 */
bool set_paradox_alert(PARADOX_LEVEL threshold, ParadoxAlertCallback callback);

/**
 * @brief Checks for paradoxes between dependencies
 *
 * This function checks if there are any paradoxes between the specified
 * dependencies.
 *
 * @param dependencies The dependencies to check
 * @return true if no paradoxes were detected, false otherwise
 */
bool paradox_check(const std::vector<std::string>& dependencies);

/**
 * @brief Stabilizes the timeline using Aethel
 *
 * This function consumes Aethel to reduce the paradox level.
 *
 * @param reserve The Aethel reserve to consume from
 * @param amount The amount of Aethel to consume
 * @return The new paradox level after stabilization
 */
PARADOX_LEVEL stabilize_timeline(AETHEL_RESERVE& reserve, int amount);

/**
 * @brief Triggers a paradox alert with a specific level
 *
 * This function is useful for testing paradox alert callbacks.
 *
 * @param level The paradox level to trigger
 */
void trigger_paradox_level(PARADOX_LEVEL level);

/**
 * @brief Advanced paradox management interface
 *
 * This class provides an interface for more complex paradox management
 * operations that may require stateful interactions.
 */
class ParadoxManager {
public:
    ParadoxManager();
    ~ParadoxManager();

    /**
     * @brief Analyzes the timeline for potential paradoxes
     *
     * @return A list of potential paradox issues
     */
    std::vector<std::string> analyze_timeline();

    /**
     * @brief Creates a containment field to isolate paradoxes
     *
     * @param name The name of the containment field
     * @param reserve The Aethel reserve to consume from
     * @param amount The amount of Aethel to consume
     * @return true if the containment field was created successfully, false otherwise
     */
    bool create_containment_field(const std::string& name, AETHEL_RESERVE& reserve, int amount);

private:
    // Implementation details
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

}  // namespace stdlib
}  // namespace chronovyan

#endif  // CHRONOVYAN_STDLIB_PARADOX_MANAGEMENT_H