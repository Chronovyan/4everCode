#ifndef CHRONOVYAN_TEMPORAL_RUNTIME_H
#define CHRONOVYAN_TEMPORAL_RUNTIME_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "loot.h"

namespace chronovyan {

/**
 * @class TemporalRuntime
 * @brief Manages the execution of temporal operations and tracks cycle information
 *
 * The TemporalRuntime is responsible for maintaining temporal state, cycle
 * management, and tracking the execution of operations across temporal dimensions.
 */
class TemporalRuntime {
public:
    /**
     * @brief Construct a new Temporal Runtime
     */
    TemporalRuntime();

    /**
     * @brief Get the current cycle number
     * @return The current cycle number
     */
    long long getCycleNumber() const;

    /**
     * @brief Get the current efficiency level
     * @return A value representing execution efficiency
     */
    double getEfficiency() const;

    /**
     * @brief Get the current resource level
     * @return The current resource level
     */
    double getCurrentLevel() const;

    /**
     * @brief Advance to the next cycle
     * @return The new cycle number
     */
    int advanceCycle();

    /**
     * @brief Reset the runtime to initial state
     */
    void reset();

    /**
     * @brief Get a history of resource levels across cycles
     * @return Vector of resource levels by cycle
     */
    std::vector<double> getResourceHistory() const;

    /**
     * @brief Get a history of efficiency metrics across cycles
     * @return Vector of efficiency metrics by cycle
     */
    std::vector<double> getEfficiencyHistory() const;

    // Paradox and timeline management
    int getParadoxLevel() const;
    void increaseParadoxLevel(int amount = 1);
    void decreaseParadoxLevel(int amount = 1);
    void setParadoxLevel(int level);

    // Resource management
    double getAethelLevel() const;
    void consumeAethel(double amount);
    void replenishAethel(double amount);
    void addAethel(double amount) { replenishAethel(amount); }
    double getAethel() const { return getAethelLevel(); }

    double getChrononsLevel() const;
    void consumeChronons(double amount);
    void replenishChronons(double amount);
    void addChronons(double amount) { replenishChronons(amount); }
    double getChronons() const { return getChrononsLevel(); }

    /**
     * @brief Get the maximum chronons capacity
     * @return The maximum chronons that can be stored
     */
    double getMaxChronons() const;

    /**
     * @brief Get the maximum aethel capacity
     * @return The maximum aethel that can be stored
     */
    double getMaxAethel() const;

    // Timeline operations
    void createTimelineSnapshot();
    void rewindToSnapshot(const std::string& snapshotId);
    void mergeTimelines(const std::vector<std::string>& timelineIds);

    /**
     * @brief Get the number of timeline snapshots
     * @return The count of timeline snapshots
     */
    size_t getTimelineSnapshotCount() const;

    // Loot system
    /**
     * @brief Check for potential loot discovery based on current conditions
     * This should be called after significant operations that might trigger loot
     */
    void checkForLootDiscovery();

    /**
     * @brief Check for rare loot discovery with a bias toward higher rarities
     * @param rarityModifier The strength of the bias (0.0 to 1.0, default 0.5)
     */
    void checkRareLootDiscovery(double rarityModifier = 0.5);

    /**
     * @brief Get all discovered loot
     * @return A vector of discovered loot items
     */
    std::vector<std::shared_ptr<Loot>> getDiscoveredLoot() const;

    /**
     * @brief Check if a specific loot has been discovered
     * @param lootId The ID of the loot to check
     * @return True if the loot has been discovered
     */
    bool hasDiscoveredLoot(const std::string& lootId) const;

    /**
     * @brief Get a specific discovered loot by ID
     * @param lootId The ID of the loot to get
     * @return The loot item, or nullptr if not discovered
     */
    std::shared_ptr<Loot> getDiscoveredLootById(const std::string& lootId) const;

    /**
     * @brief Register a loot table for potential discoveries
     * @param lootTable The loot table to register
     */
    void registerLootTable(const LootTable& lootTable);

    /**
     * @brief Apply a loot effect
     * @param loot The loot to apply the effect from
     * @return True if the effect was applied
     */
    bool applyLootEffect(const std::shared_ptr<Loot>& loot);

    /**
     * @brief Get all completed loot sets
     * @return Vector of completed loot sets
     */
    std::vector<std::shared_ptr<LootSet>> getCompletedLootSets() const;

    /**
     * @brief Check if a specific loot set is completed
     * @param lootSetId The ID of the loot set to check
     * @return True if the loot set is completed
     */
    bool isLootSetCompleted(const std::string& lootSetId) const;

    /**
     * @brief Apply a loot set bonus
     * @param lootSetId The ID of the loot set to apply the bonus from
     * @return True if the bonus was applied
     */
    bool applyLootSetBonus(const std::string& lootSetId);

    /**
     * @brief Check for newly completed loot sets
     * This should be called after loot discovery
     */
    void checkLootSetCompletion();

    /**
     * @brief Add a loot item to discovered loot
     * @param lootId The ID of the loot to add
     * @return True if the loot was added successfully
     */
    bool addDiscoveredLoot(const std::string& lootId);

    /**
     * @brief Get all loot items that can be crafted with current resources
     * @return Vector of craftable loot items
     */
    std::vector<std::shared_ptr<Loot>> getCraftableLoot() const;

    /**
     * @brief Craft a loot item
     * @param lootId The ID of the loot to craft
     * @return True if the item was crafted successfully
     */
    bool craftLoot(const std::string& lootId);

private:
    int m_paradoxLevel;
    double m_aethelLevel;
    double m_chrononsLevel;

    // Timeline snapshot storage
    std::map<std::string, std::shared_ptr<void>> m_timelineSnapshots;

    // Loot system
    std::set<std::string> m_discoveredLootIds;
    std::vector<LootTable> m_lootTables;
    std::set<std::string> m_appliedLootSetBonuses;

    int m_currentCycle;
    double m_currentLevel;
    double m_efficiency;
    std::vector<double> m_resourceHistory;
    std::vector<double> m_efficiencyHistory;

    /**
     * @brief Check for paradox-triggered loot
     */
    void checkParadoxLoot();

    /**
     * @brief Check for paradox-triggered loot with rarity bias
     * @param rarityModifier The strength of the bias (0.0 to 1.0)
     */
    void checkParadoxLoot(double rarityModifier);

    /**
     * @brief Check for resource-triggered loot
     */
    void checkResourceLoot();

    /**
     * @brief Check for resource-triggered loot with rarity bias
     * @param rarityModifier The strength of the bias (0.0 to 1.0)
     */
    void checkResourceLoot(double rarityModifier);

    /**
     * @brief Update efficiency metrics based on current state
     */
    void updateEfficiency();
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_TEMPORAL_RUNTIME_H