#ifndef CHRONOVYAN_LOOT_H
#define CHRONOVYAN_LOOT_H

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace chronovyan {

// Forward declarations
class TemporalRuntime;
class Value;

/**
 * @enum LootRarity
 * @brief Represents the rarity levels for loot items
 */
enum class LootRarity {
    COMMON,     // Frequently discovered items
    UNCOMMON,   // Less common discoveries
    RARE,       // Rare discoveries, requiring specific conditions
    LEGENDARY,  // Very rare items, requiring exceptional conditions
    MYTHIC      // Extremely rare items, requiring complex combinations
};

/**
 * @enum LootEffect
 * @brief Represents the types of effects that loot can have
 */
enum class LootEffect {
    NONE,               // No special effect
    PARADOX_REDUCTION,  // Reduces paradox level
    CHRONON_BOOST,      // Increases available chronons
    AETHEL_BOOST,       // Increases available aethel
    STABILITY_BOOST,    // Increases timeline stability
    WEAVER_BOOST,       // Enhances quantum operations
    BRANCH_DISCOUNT     // Reduces the cost of branching timelines
};

/**
 * @struct LootRecipe
 * @brief Represents a recipe for crafting a loot item from other items
 */
struct LootRecipe {
    // Vector of ingredient loot IDs and required quantities
    std::vector<std::pair<std::string, int>> ingredients;

    // Resource costs for crafting
    double chrononCost = 0.0;
    double aethelCost = 0.0;
    int paradoxCost = 0;

    // Whether the ingredients are consumed when crafting
    bool consumesIngredients = true;
};

/**
 * @struct LootDiscovery
 * @brief Records information about how a loot item was discovered
 */
struct LootDiscovery {
    std::string lootId;
    std::chrono::system_clock::time_point timestamp;
    int paradoxLevel;
    double chrononLevel;
    double aethelLevel;
    bool wasCrafted;

    LootDiscovery(const std::string& id, int paradox, double chronon, double aethel, bool crafted)
        : lootId(id)
        , timestamp(std::chrono::system_clock::now())
        , paradoxLevel(paradox)
        , chrononLevel(chronon)
        , aethelLevel(aethel)
        , wasCrafted(crafted) {}
};

/**
 * @class Loot
 * @brief Represents a discoverable item in the Chronovyan language
 */
class Loot {
public:
    /**
     * @brief Construct a new Loot item
     * @param id Unique identifier for the loot
     * @param name Display name for the loot
     * @param description Detailed description of the loot
     * @param rarity Rarity level of the loot
     * @param effect The special effect of this loot item (default: NONE)
     * @param effectStrength The strength of the effect (0.0 to 1.0, default: 0.0)
     */
    Loot(const std::string& id, const std::string& name, const std::string& description,
         LootRarity rarity, LootEffect effect = LootEffect::NONE, double effectStrength = 0.0);

    /**
     * @brief Get the unique identifier for this loot
     * @return The loot ID
     */
    const std::string& getId() const;

    /**
     * @brief Get the display name for this loot
     * @return The loot name
     */
    const std::string& getName() const;

    /**
     * @brief Get the description for this loot
     * @return The loot description
     */
    const std::string& getDescription() const;

    /**
     * @brief Get the rarity level for this loot
     * @return The loot rarity
     */
    LootRarity getRarity() const;

    /**
     * @brief Get the effect type for this loot
     * @return The loot effect type
     */
    LootEffect getEffect() const;

    /**
     * @brief Get the effect strength for this loot
     * @return The effect strength (0.0 to 1.0)
     */
    double getEffectStrength() const;

    /**
     * @brief Apply this loot's effect to the runtime
     * @param runtime The temporal runtime to apply effects to
     * @return True if the effect was applied successfully
     */
    bool applyEffect(TemporalRuntime& runtime) const;

    /**
     * @brief Get a visual representation of this loot item
     * @return ASCII art or color-coded representation
     */
    std::string getVisualRepresentation() const;

    /**
     * @brief Convert rarity to string representation
     * @param rarity The rarity level
     * @return String representation of the rarity
     */
    static std::string rarityToString(LootRarity rarity);

    /**
     * @brief Convert effect to string representation
     * @param effect The loot effect
     * @return String representation of the effect
     */
    static std::string effectToString(LootEffect effect);

    /**
     * @brief Get the base discovery chance for a rarity level
     * @param rarity The rarity level
     * @return Base discovery chance (0.0 to 1.0)
     */
    static double getBaseDiscoveryChance(LootRarity rarity);

    /**
     * @brief Check if this loot is craftable
     * @return True if the loot has a recipe
     */
    bool isCraftable() const;

    /**
     * @brief Set the crafting recipe for this loot
     * @param recipe The recipe to set
     */
    void setRecipe(const LootRecipe& recipe);

    /**
     * @brief Get the crafting recipe for this loot
     * @return The loot recipe, or nullptr if not craftable
     */
    const LootRecipe* getRecipe() const;

private:
    std::string m_id;
    std::string m_name;
    std::string m_description;
    LootRarity m_rarity;
    LootEffect m_effect;
    double m_effectStrength;
    std::unique_ptr<LootRecipe> m_recipe;
};

/**
 * @class LootSet
 * @brief Manages a thematic collection of related loot items
 */
class LootSet {
public:
    /**
     * @brief Construct a new LootSet
     * @param id Unique identifier for the set
     * @param name Display name for the set
     * @param description Description of the set theme
     */
    LootSet(const std::string& id, const std::string& name, const std::string& description);

    /**
     * @brief Get the unique identifier for this set
     * @return The set ID
     */
    const std::string& getId() const;

    /**
     * @brief Get the display name for this set
     * @return The set name
     */
    const std::string& getName() const;

    /**
     * @brief Get the description for this set
     * @return The set description
     */
    const std::string& getDescription() const;

    /**
     * @brief Add a loot item to this set
     * @param loot The loot to add
     * @return True if added successfully, false if already exists
     */
    bool addLoot(std::shared_ptr<Loot> loot);

    /**
     * @brief Remove a loot item from this set
     * @param lootId The ID of the loot to remove
     * @return True if removed successfully, false if not found
     */
    bool removeLoot(const std::string& lootId);

    /**
     * @brief Get all loot items in this set
     * @return Vector of loot items
     */
    std::vector<std::shared_ptr<Loot>> getAllLoot() const;

    /**
     * @brief Check if a loot item is part of this set
     * @param lootId The ID of the loot to check
     * @return True if the loot is in this set
     */
    bool containsLoot(const std::string& lootId) const;

    /**
     * @brief Get a loot item from this set by ID
     * @param lootId The ID of the loot to get
     * @return The loot item, or nullptr if not found
     */
    std::shared_ptr<Loot> getLootById(const std::string& lootId) const;

    /**
     * @brief Get the number of loot items in this set
     * @return Number of items in the set
     */
    size_t size() const;

    /**
     * @brief Check if the set completion bonus is unlocked
     * @param discoveredLootIds Set of all discovered loot IDs
     * @return True if all items in the set are discovered
     */
    bool isCompleted(const std::set<std::string>& discoveredLootIds) const;

    /**
     * @brief Apply the set completion bonus to the runtime
     * @param runtime The temporal runtime to apply the bonus to
     * @return True if the bonus was applied successfully
     */
    bool applySetBonus(TemporalRuntime& runtime) const;

private:
    std::string m_id;
    std::string m_name;
    std::string m_description;
    std::map<std::string, std::shared_ptr<Loot>> m_lootItems;
};

/**
 * @class LootTable
 * @brief Manages discoverable loot with probability-based discovery
 */
class LootTable {
public:
    /**
     * @brief Add a loot item to the table with a specified discovery chance
     * @param loot The loot item to add
     * @param chance Probability of discovery (0.0 to 1.0)
     */
    void addLoot(std::shared_ptr<Loot> loot, double chance);

    /**
     * @brief Roll for a loot discovery
     * @return The discovered loot, or nullptr if nothing was discovered
     */
    std::shared_ptr<Loot> rollForLoot() const;

    /**
     * @brief Roll for a loot discovery with a rarity bias
     * @param rarityModifier Increases chance for specific rarities (higher = more rare loot)
     * @return The discovered loot, or nullptr if nothing was discovered
     */
    std::shared_ptr<Loot> rollForLootWithRarityBias(double rarityModifier) const;

    /**
     * @brief Get all loot in the table
     * @return Vector of loot items
     */
    std::vector<std::shared_ptr<Loot>> getAllLoot() const;

private:
    std::vector<std::pair<std::shared_ptr<Loot>, double>> m_lootEntries;
};

/**
 * @class LootCrafter
 * @brief Provides functionality for crafting loot items from recipes
 */
class LootCrafter {
public:
    /**
     * @brief Check if a loot item can be crafted
     * @param lootId The ID of the loot to check
     * @param runtime The temporal runtime with discovered loot
     * @return True if the loot can be crafted with available resources
     */
    static bool canCraft(const std::string& lootId, const TemporalRuntime& runtime);

    /**
     * @brief Craft a loot item from a recipe
     * @param lootId The ID of the loot to craft
     * @param runtime The temporal runtime to modify
     * @return True if the item was crafted successfully
     */
    static bool craftLoot(const std::string& lootId, TemporalRuntime& runtime);

    /**
     * @brief Get all craftable loot items with the current resources
     * @param runtime The temporal runtime with discovered loot
     * @return Vector of craftable loot items
     */
    static std::vector<std::shared_ptr<Loot>> getCraftableLoot(const TemporalRuntime& runtime);

private:
    /**
     * @brief Check if all ingredients are available
     * @param recipe The recipe to check
     * @param runtime The temporal runtime with discovered loot
     * @return True if all ingredients are available in sufficient quantities
     */
    static bool hasIngredients(const LootRecipe& recipe, const TemporalRuntime& runtime);

    /**
     * @brief Check if enough resources are available
     * @param recipe The recipe to check
     * @param runtime The temporal runtime with resources
     * @return True if enough resources are available
     */
    static bool hasResources(const LootRecipe& recipe, const TemporalRuntime& runtime);

    /**
     * @brief Consume ingredients for crafting
     * @param recipe The recipe to consume ingredients for
     * @param runtime The temporal runtime to modify
     * @return True if ingredients were consumed successfully
     */
    static bool consumeIngredients(const LootRecipe& recipe, TemporalRuntime& runtime);

    /**
     * @brief Consume resources for crafting
     * @param recipe The recipe to consume resources for
     * @param runtime The temporal runtime to modify
     * @return True if resources were consumed successfully
     */
    static bool consumeResources(const LootRecipe& recipe, TemporalRuntime& runtime);
};

/**
 * @enum AchievementType
 * @brief Represents the types of achievements that can be earned
 */
enum class AchievementType {
    LOOT_DISCOVERY,     // Discovering a specific number of loot items
    RARITY_COLLECTION,  // Collecting all loot of a specific rarity
    SET_COMPLETION,     // Completing a loot set
    CRAFTING_MASTERY,   // Crafting a specific number of items
    PARADOX_MASTERY,    // Reaching a high paradox level
    RESOURCE_MASTERY    // Accumulating large amounts of resources
};

/**
 * @struct Achievement
 * @brief Represents an achievement that can be earned through loot collection
 */
struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    AchievementType type;
    int threshold;  // The value needed to earn this achievement
    bool isEarned;
    std::chrono::system_clock::time_point earnedTimestamp;

    Achievement(const std::string& id, const std::string& name, const std::string& description,
                AchievementType type, int threshold)
        : id(id)
        , name(name)
        , description(description)
        , type(type)
        , threshold(threshold)
        , isEarned(false) {}
};

/**
 * @class LootJournal
 * @brief Tracks loot discoveries, statistics, and achievements
 */
class LootJournal {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the LootJournal
     */
    static LootJournal& getInstance();

    /**
     * @brief Record a new loot discovery
     * @param lootId The ID of the discovered loot
     * @param runtime The temporal runtime at the time of discovery
     * @param wasCrafted Whether the loot was crafted or found
     */
    void recordDiscovery(const std::string& lootId, const TemporalRuntime& runtime,
                         bool wasCrafted);

    /**
     * @brief Get all loot discovery records
     * @return Vector of loot discovery records
     */
    const std::vector<LootDiscovery>& getDiscoveryHistory() const;

    /**
     * @brief Get the discovery count for a specific rarity
     * @param rarity The rarity to check
     * @return Number of items discovered of that rarity
     */
    int getDiscoveryCountByRarity(LootRarity rarity) const;

    /**
     * @brief Get the total discovery count
     * @return Total number of unique loot items discovered
     */
    int getTotalDiscoveryCount() const;

    /**
     * @brief Register a new achievement
     * @param achievement The achievement to register
     */
    void registerAchievement(const Achievement& achievement);

    /**
     * @brief Check and update achievement progress
     * @param runtime The temporal runtime to check against
     */
    void updateAchievements(const TemporalRuntime& runtime);

    /**
     * @brief Get all registered achievements
     * @return Vector of achievements
     */
    const std::vector<Achievement>& getAchievements() const;

    /**
     * @brief Get all earned achievements
     * @return Vector of earned achievements
     */
    std::vector<Achievement> getEarnedAchievements() const;

    /**
     * @brief Check if a specific achievement has been earned
     * @param achievementId The ID of the achievement to check
     * @return True if the achievement has been earned
     */
    bool hasEarnedAchievement(const std::string& achievementId) const;

    /**
     * @brief Generate a summary report of loot discoveries and achievements
     * @return A formatted string with loot statistics and achievements
     */
    std::string generateSummaryReport() const;

private:
    LootJournal() = default;
    ~LootJournal() = default;

    // Prevent copying
    LootJournal(const LootJournal&) = delete;
    LootJournal& operator=(const LootJournal&) = delete;

    std::vector<LootDiscovery> m_discoveryHistory;
    std::vector<Achievement> m_achievements;
    std::map<LootRarity, int> m_rarityDiscoveryCount;
    std::set<std::string> m_discoveredLootIds;

    /**
     * @brief Check if an achievement has been earned
     * @param achievement The achievement to check
     * @param runtime The temporal runtime to check against
     * @return True if the achievement conditions are met
     */
    bool checkAchievementCondition(Achievement& achievement, const TemporalRuntime& runtime);
};

/**
 * @class LootRegistry
 * @brief Global registry of all available loot types
 */
class LootRegistry {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the LootRegistry
     */
    static LootRegistry& getInstance();

    /**
     * @brief Register a new loot type
     * @param loot The loot to register
     */
    void registerLoot(std::shared_ptr<Loot> loot);

    /**
     * @brief Get a loot by its ID
     * @param id The loot ID
     * @return The loot, or nullptr if not found
     */
    std::shared_ptr<Loot> getLootById(const std::string& id) const;

    /**
     * @brief Get all registered loot
     * @return Map of loot items by ID
     */
    const std::map<std::string, std::shared_ptr<Loot>>& getAllLoot() const;

    /**
     * @brief Get all loot of a specific rarity
     * @param rarity The rarity level to filter by
     * @return Vector of loot items with the specified rarity
     */
    std::vector<std::shared_ptr<Loot>> getLootByRarity(LootRarity rarity) const;

    /**
     * @brief Register a new loot set
     * @param lootSet The loot set to register
     */
    void registerLootSet(std::shared_ptr<LootSet> lootSet);

    /**
     * @brief Get a loot set by its ID
     * @param id The loot set ID
     * @return The loot set, or nullptr if not found
     */
    std::shared_ptr<LootSet> getLootSetById(const std::string& id) const;

    /**
     * @brief Get all registered loot sets
     * @return Map of loot sets by ID
     */
    const std::map<std::string, std::shared_ptr<LootSet>>& getAllLootSets() const;

    /**
     * @brief Get all craftable loot items
     * @return Vector of loot items that have recipes
     */
    std::vector<std::shared_ptr<Loot>> getCraftableLoot() const;

private:
    LootRegistry() = default;
    ~LootRegistry() = default;

    // Prevent copying
    LootRegistry(const LootRegistry&) = delete;
    LootRegistry& operator=(const LootRegistry&) = delete;

    std::map<std::string, std::shared_ptr<Loot>> m_lootRegistry;
    std::map<std::string, std::shared_ptr<LootSet>> m_lootSetRegistry;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_LOOT_H