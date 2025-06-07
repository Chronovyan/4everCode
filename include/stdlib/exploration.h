/**
 * @file exploration.h
 * @brief Exploration and loot functions for the Chronovyan Standard Library
 *
 * This file contains declarations for functions that allow exploration of
 * timelines and collection of rewards in Chronovyan programs. These functions
 * enable users to probe timelines for insights and claim loot from temporal rifts.
 *
 * @copyright Chronovyan Project
 */

#ifndef CHRONOVYAN_STDLIB_EXPLORATION_H
#define CHRONOVYAN_STDLIB_EXPLORATION_H

#include <map>
#include <string>
#include <unordered_map>

#include "resource_management.h"  // For CHRONON_STREAM type

namespace chronovyan {
namespace stdlib {

/**
 * @brief Type alias for temporal signatures
 */
using TEMPORAL_SIGNATURE = std::string;

/**
 * @brief Type alias for loot tables
 */
using LOOT_TABLE = std::map<double, CHRONON_STREAM>;

/**
 * @brief Probes a timeline or rift for insights
 *
 * Explores a timeline or rift to gather information about its composition,
 * stability, and potential rewards. This operation has an Aethel cost of 40
 * and may increase the PARADOX_LEVEL by 0.1 if the target is unstable.
 *
 * @param target The timeline or rift to explore (e.g., "Temporal_Rift")
 * @return A TEMPORAL_SIGNATURE that can be used for further weaves
 *
 * Example usage:
 * @code
 * DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift");
 * @endcode
 */
TEMPORAL_SIGNATURE deepen_exploration(const std::string& target);

/**
 * @brief Resolves a LOOT_TABLE to yield rewards
 *
 * Resolves a loot table to determine the rewards obtained from an exploration.
 * This operation has an Aethel cost of 10.
 *
 * @param table The loot table to resolve
 * @return The resulting CHRONON_STREAM or LOOT
 *
 * Lore: This function mirrors the Seeker's Hunt, where bold weaves yield treasures from the Void's
 * edge.
 *
 * Example usage:
 * @code
 * Reward = loot_drop({ 0.95: LOOT("Legendary_Thread") });
 * @endcode
 */
CHRONON_STREAM loot_drop(const LOOT_TABLE& table);

/**
 * @brief Helper function to create a LOOT object
 *
 * Creates a LOOT object with the specified name, which can be used
 * in loot tables and other temporal operations.
 *
 * @param name The name of the loot item
 * @return A CHRONON_STREAM representing the loot
 *
 * Example usage:
 * @code
 * CHRONON_STREAM reward = LOOT("Aethel_Shard");
 * @endcode
 */
CHRONON_STREAM LOOT(const std::string& name);

/**
 * @brief Advanced exploration interface
 *
 * This class provides an interface for more complex exploration
 * operations that may require stateful interactions.
 */
class Explorer {
public:
    Explorer();
    ~Explorer();

    /**
     * @brief Analyzes a temporal signature to extract detailed information
     *
     * Examines a temporal signature to determine its properties, composition,
     * and potential uses in weaving operations.
     *
     * @param signature The TEMPORAL_SIGNATURE to analyze
     * @return A map of properties to their values
     */
    std::unordered_map<std::string, std::string> analyze_signature(
        const TEMPORAL_SIGNATURE& signature);

    /**
     * @brief Creates a new exploration team
     *
     * Establishes a team of explorers that can be sent on missions to
     * investigate temporal phenomena. Teams with more members have a higher
     * chance of success but also a higher Aethel cost.
     *
     * @param name The name for the exploration team
     * @param size The number of members in the team
     * @return true if the team was created successfully, false otherwise
     */
    bool create_team(const std::string& name, int size);

    /**
     * @brief Sends an exploration team on a mission
     *
     * Dispatches an exploration team to investigate a temporal target.
     * The team will return with a temporal signature and potentially loot.
     *
     * @param team_name The name of the team to send
     * @param target The target to explore
     * @param duration The duration of the mission in ticks
     * @return A TEMPORAL_SIGNATURE representing the team's findings
     */
    TEMPORAL_SIGNATURE send_team(const std::string& team_name, const std::string& target,
                                 int duration);

private:
    // Implementation details
    void* internal_state;  // Placeholder for internal state
};

}  // namespace stdlib
}  // namespace chronovyan

#endif  // CHRONOVYAN_STDLIB_EXPLORATION_H