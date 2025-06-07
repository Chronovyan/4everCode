#ifndef CHRONOVYAN_LOOT_SYSTEM_H
#define CHRONOVYAN_LOOT_SYSTEM_H

#include <memory>
#include <string>

#include "loot.h"

namespace chronovyan {

/**
 * @class LootSystem
 * @brief Manages loot discovery, collection, and application in the Chronovyan system
 */
class LootSystem {
public:
    /**
     * @brief Default constructor
     */
    LootSystem() {}

    /**
     * @brief Initialize the loot system with standard loot items
     */
    void initialize();

    /**
     * @brief Check for loot discovery opportunities based on runtime state
     * @param runtime The current temporal runtime state
     * @return True if loot was discovered
     */
    bool checkForDiscovery(const TemporalRuntime& runtime);

    /**
     * @brief Apply a specific loot item's effect
     * @param lootId The ID of the loot to apply
     * @param runtime The temporal runtime to modify
     * @return True if the effect was successfully applied
     */
    bool applyLoot(const std::string& lootId, TemporalRuntime& runtime);

private:
    // Internal state tracking
    bool m_initialized = false;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_LOOT_SYSTEM_H