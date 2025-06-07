/**
 * @file temporal_manipulation.h
 * @brief Temporal manipulation functions for the Chronovyan Standard Library
 *
 * This file contains declarations for functions that allow manipulation
 * of time and timelines in Chronovyan programs. These functions enable
 * rewinding to specific time anchors, fast forwarding, and other temporal
 * operations that are central to the Chronovyan language philosophy.
 *
 * @copyright Chronovyan Project
 */

#ifndef CHRONOVYAN_STDLIB_TEMPORAL_MANIPULATION_H
#define CHRONOVYAN_STDLIB_TEMPORAL_MANIPULATION_H

#include <string>

#include "../value.h"  // For Value type definitions

namespace chronovyan {
namespace stdlib {

/**
 * @brief Type alias for timeline timestamps
 */
using TIMESTAMP = std::string;

/**
 * @brief Rewinds the timeline to a specified anchor
 *
 * Rewinds the current timeline to a specified anchor point. This function
 * has an Aethel cost of 50 and increases the PARADOX_LEVEL by 0.05.
 *
 * @param anchor The timestamp to rewind to (e.g., "@Fork")
 * @return true if successful, false if the anchor is invalid
 *
 * Lore: This function is whispered to echo the First Seeker's gambit,
 * unraveling a doomed thread to save Chronos Prime.
 *
 * Example usage:
 * @code
 * Success = rewind_to("@Origin");
 * @endcode
 */
bool rewind_to(const TIMESTAMP& anchor);

/**
 * @brief Advances the timeline by a number of ticks
 *
 * Fast forwards the current timeline by the specified number of ticks.
 * This function has an Aethel cost of 30 and increases the PARADOX_LEVEL by 0.03.
 *
 * @param ticks The number of ticks to advance (positive integer)
 * @return true if successful, false if beyond Temporal_Scope
 *
 * Example usage:
 * @code
 * Success = fast_forward(10);
 * @endcode
 */
bool fast_forward(int ticks);

/**
 * @brief Advanced temporal manipulation interface
 *
 * This class provides an interface for more complex temporal manipulation
 * operations that may require stateful interactions.
 */
class TemporalManipulator {
public:
    TemporalManipulator();
    ~TemporalManipulator();

    /**
     * @brief Creates a new branch in the timeline
     *
     * Creates a new timeline branch from the current point, allowing for
     * exploration of alternative timelines without affecting the original.
     *
     * @param branch_name The name to assign to the new branch
     * @return true if successful, false if branching failed
     */
    bool create_branch(const std::string& branch_name);

    /**
     * @brief Merges a branch back into the main timeline
     *
     * Attempts to merge the specified branch back into the main timeline.
     * This operation may cause paradoxes if the branches are incompatible.
     *
     * @param branch_name The name of the branch to merge
     * @return true if successful, false if merging failed or caused a paradox
     */
    bool merge_branch(const std::string& branch_name);

private:
    // Implementation details
    void* internal_state;  // Placeholder for internal state
};

}  // namespace stdlib
}  // namespace chronovyan

#endif  // CHRONOVYAN_STDLIB_TEMPORAL_MANIPULATION_H