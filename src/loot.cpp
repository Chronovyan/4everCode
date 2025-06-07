#include <algorithm>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>

#include "loot.h"
#include "temporal_runtime.h"

namespace chronovyan {

// Loot implementation
Loot::Loot(const std::string& id, const std::string& name, const std::string& description,
           LootRarity rarity, LootEffect effect, double effectStrength)
    : m_id(id)
    , m_name(name)
    , m_description(description)
    , m_rarity(rarity)
    , m_effect(effect)
    , m_effectStrength(effectStrength) {}

const std::string& Loot::getId() const { return m_id; }

const std::string& Loot::getName() const { return m_name; }

const std::string& Loot::getDescription() const { return m_description; }

LootRarity Loot::getRarity() const { return m_rarity; }

LootEffect Loot::getEffect() const { return m_effect; }

double Loot::getEffectStrength() const { return m_effectStrength; }

bool Loot::isCraftable() const { return m_recipe != nullptr; }

void Loot::setRecipe(const LootRecipe& recipe) { m_recipe = std::make_unique<LootRecipe>(recipe); }

const LootRecipe* Loot::getRecipe() const { return m_recipe.get(); }

std::string Loot::rarityToString(LootRarity rarity) {
    switch (rarity) {
        case LootRarity::COMMON:
            return "Common";
        case LootRarity::UNCOMMON:
            return "Uncommon";
        case LootRarity::RARE:
            return "Rare";
        case LootRarity::LEGENDARY:
            return "Legendary";
        case LootRarity::MYTHIC:
            return "Mythic";
        default:
            return "Unknown";
    }
}

std::string Loot::effectToString(LootEffect effect) {
    switch (effect) {
        case LootEffect::NONE:
            return "None";
        case LootEffect::PARADOX_REDUCTION:
            return "Paradox Reduction";
        case LootEffect::CHRONON_BOOST:
            return "Chronon Boost";
        case LootEffect::AETHEL_BOOST:
            return "Aethel Boost";
        case LootEffect::STABILITY_BOOST:
            return "Timeline Stability";
        case LootEffect::WEAVER_BOOST:
            return "Quantum Weaving";
        case LootEffect::BRANCH_DISCOUNT:
            return "Branch Cost Reduction";
        default:
            return "Unknown";
    }
}

double Loot::getBaseDiscoveryChance(LootRarity rarity) {
    switch (rarity) {
        case LootRarity::COMMON:
            return 0.3;  // 30% base chance
        case LootRarity::UNCOMMON:
            return 0.15;  // 15% base chance
        case LootRarity::RARE:
            return 0.05;  // 5% base chance
        case LootRarity::LEGENDARY:
            return 0.01;  // 1% base chance
        case LootRarity::MYTHIC:
            return 0.001;  // 0.1% base chance
        default:
            return 0.0;
    }
}

bool Loot::applyEffect(TemporalRuntime& runtime) const {
    if (m_effect == LootEffect::NONE || m_effectStrength <= 0.0) {
        return false;
    }

    bool effectApplied = false;

    switch (m_effect) {
        case LootEffect::PARADOX_REDUCTION:
            // Reduce paradox level based on effect strength (1.0 = complete reduction)
            {
                int currentParadox = runtime.getParadoxLevel();
                int reduction = std::max(1, static_cast<int>(currentParadox * m_effectStrength));
                runtime.setParadoxLevel(std::max(0, currentParadox - reduction));
                std::cout << "Paradox level reduced by " << reduction << " points!" << std::endl;
                effectApplied = true;
            }
            break;

        case LootEffect::CHRONON_BOOST:
            // Boost available chronons by effect strength percentage
            {
                double currentChronons = runtime.getChronons();
                double boost = currentChronons * m_effectStrength;
                runtime.addChronons(boost);
                std::cout << "Chronons boosted by " << std::fixed << std::setprecision(1)
                          << (m_effectStrength * 100) << "%!" << std::endl;
                effectApplied = true;
            }
            break;

        case LootEffect::AETHEL_BOOST:
            // Boost available aethel by effect strength percentage
            {
                double currentAethel = runtime.getAethel();
                double boost = currentAethel * m_effectStrength;
                runtime.addAethel(boost);
                std::cout << "Aethel boosted by " << std::fixed << std::setprecision(1)
                          << (m_effectStrength * 100) << "%!" << std::endl;
                effectApplied = true;
            }
            break;

        case LootEffect::STABILITY_BOOST:
            // Increase timeline stability (implementation depends on runtime)
            {
                // This is a placeholder - the actual implementation would depend on
                // how timeline stability is modeled in the runtime
                std::cout << "Timeline stability increased by " << std::fixed
                          << std::setprecision(1) << (m_effectStrength * 100) << "%!" << std::endl;
                effectApplied = true;
            }
            break;

        case LootEffect::WEAVER_BOOST:
            // Enhance quantum operations (implementation depends on runtime)
            {
                // This is a placeholder - the actual implementation would depend on
                // how quantum operations are modeled in the runtime
                std::cout << "Quantum weaving strength increased by " << std::fixed
                          << std::setprecision(1) << (m_effectStrength * 100) << "%!" << std::endl;
                effectApplied = true;
            }
            break;

        case LootEffect::BRANCH_DISCOUNT:
            // Reduce the cost of branching timelines
            {
                // This is a placeholder - the actual implementation would depend on
                // how branching costs are modeled in the runtime
                std::cout << "Timeline branching cost reduced by " << std::fixed
                          << std::setprecision(1) << (m_effectStrength * 100) << "%!" << std::endl;
                effectApplied = true;
            }
            break;

        default:
            break;
    }

    return effectApplied;
}

std::string Loot::getVisualRepresentation() const {
    std::stringstream ss;

    // Create a border based on rarity
    std::string borderChar;
    switch (m_rarity) {
        case LootRarity::COMMON:
            borderChar = "-";  // Simple border for common
            break;
        case LootRarity::UNCOMMON:
            borderChar = "=";  // Double line for uncommon
            break;
        case LootRarity::RARE:
            borderChar = "*";  // Stars for rare
            break;
        case LootRarity::LEGENDARY:
            borderChar = "★";  // Star symbols for legendary
            break;
        case LootRarity::MYTHIC:
            borderChar = "✧";  // Special symbols for mythic
            break;
        default:
            borderChar = "-";
    }

    // Create the top border
    std::string border(30, borderChar[0]);
    ss << border << std::endl;

    // Add the item name with padding
    int padding = (30 - m_name.length()) / 2;
    ss << std::string(padding, ' ') << m_name << std::endl;

    // Add the rarity
    ss << "Rarity: " << rarityToString(m_rarity) << std::endl;

    // Add the effect if it exists
    if (m_effect != LootEffect::NONE) {
        ss << "Effect: " << effectToString(m_effect) << " (" << std::fixed << std::setprecision(0)
           << (m_effectStrength * 100) << "%)" << std::endl;
    }

    // Add a separator
    ss << std::string(30, '-') << std::endl;

    // Add a condensed description (if too long, truncate with ellipsis)
    if (m_description.length() > 28) {
        ss << m_description.substr(0, 25) << "..." << std::endl;
    } else {
        ss << m_description << std::endl;
    }

    // Add craftable indicator if it has a recipe
    if (isCraftable()) {
        ss << std::string(30, '-') << std::endl;
        ss << "Craftable" << std::endl;
    }

    // Add the bottom border
    ss << border;

    return ss.str();
}

// LootSet implementation
LootSet::LootSet(const std::string& id, const std::string& name, const std::string& description)
    : m_id(id), m_name(name), m_description(description) {}

const std::string& LootSet::getId() const { return m_id; }

const std::string& LootSet::getName() const { return m_name; }

const std::string& LootSet::getDescription() const { return m_description; }

bool LootSet::addLoot(std::shared_ptr<Loot> loot) {
    if (!loot) {
        return false;
    }

    // Check if already exists
    if (m_lootItems.find(loot->getId()) != m_lootItems.end()) {
        return false;
    }

    m_lootItems[loot->getId()] = loot;
    return true;
}

bool LootSet::removeLoot(const std::string& lootId) {
    auto it = m_lootItems.find(lootId);
    if (it == m_lootItems.end()) {
        return false;
    }

    m_lootItems.erase(it);
    return true;
}

std::vector<std::shared_ptr<Loot>> LootSet::getAllLoot() const {
    std::vector<std::shared_ptr<Loot>> result;
    result.reserve(m_lootItems.size());

    for (const auto& [id, loot] : m_lootItems) {
        result.push_back(loot);
    }

    return result;
}

bool LootSet::containsLoot(const std::string& lootId) const {
    return m_lootItems.find(lootId) != m_lootItems.end();
}

std::shared_ptr<Loot> LootSet::getLootById(const std::string& lootId) const {
    auto it = m_lootItems.find(lootId);
    if (it != m_lootItems.end()) {
        return it->second;
    }

    return nullptr;
}

size_t LootSet::size() const { return m_lootItems.size(); }

bool LootSet::isCompleted(const std::set<std::string>& discoveredLootIds) const {
    if (m_lootItems.empty()) {
        return false;
    }

    // Check if all items in the set have been discovered
    for (const auto& [id, loot] : m_lootItems) {
        if (discoveredLootIds.find(id) == discoveredLootIds.end()) {
            return false;
        }
    }

    return true;
}

bool LootSet::applySetBonus(TemporalRuntime& runtime) const {
    if (m_lootItems.empty()) {
        return false;
    }

    // Apply a bonus based on the set's contents
    // For now, a simple implementation that applies each loot's effect with a 50% strength bonus
    bool anyEffectApplied = false;

    std::cout << "\n**LOOT SET BONUS APPLIED**: " << m_name << std::endl;
    std::cout << m_description << std::endl;

    for (const auto& [id, loot] : m_lootItems) {
        // Only apply effects from items that actually have effects
        if (loot->getEffect() != LootEffect::NONE && loot->getEffectStrength() > 0.0) {
            // Create a temporary loot with 50% more effect strength
            double boostedStrength = std::min(1.0, loot->getEffectStrength() * 1.5);

            auto boostedLoot = std::make_shared<Loot>(
                loot->getId() + "_set_bonus", loot->getName() + " (Set Bonus)",
                loot->getDescription(), loot->getRarity(), loot->getEffect(), boostedStrength);

            // Apply the boosted effect
            if (boostedLoot->applyEffect(runtime)) {
                anyEffectApplied = true;
            }
        }
    }

    return anyEffectApplied;
}

// LootCrafter implementation
bool LootCrafter::canCraft(const std::string& lootId, const TemporalRuntime& runtime) {
    auto& registry = LootRegistry::getInstance();
    auto loot = registry.getLootById(lootId);

    if (!loot || !loot->isCraftable()) {
        return false;
    }

    const LootRecipe* recipe = loot->getRecipe();
    return hasIngredients(*recipe, runtime) && hasResources(*recipe, runtime);
}

bool LootCrafter::craftLoot(const std::string& lootId, TemporalRuntime& runtime) {
    auto& registry = LootRegistry::getInstance();
    auto loot = registry.getLootById(lootId);

    if (!loot || !loot->isCraftable()) {
        std::cout << "This item cannot be crafted." << std::endl;
        return false;
    }

    const LootRecipe* recipe = loot->getRecipe();

    // Check if we have all ingredients and resources
    if (!hasIngredients(*recipe, runtime)) {
        std::cout << "Missing ingredients for crafting " << loot->getName() << "." << std::endl;
        return false;
    }

    if (!hasResources(*recipe, runtime)) {
        std::cout << "Insufficient resources for crafting " << loot->getName() << "." << std::endl;
        return false;
    }

    // Consume ingredients if the recipe requires it
    if (recipe->consumesIngredients) {
        if (!consumeIngredients(*recipe, runtime)) {
            std::cout << "Failed to consume ingredients." << std::endl;
            return false;
        }
    }

    // Consume resources
    if (!consumeResources(*recipe, runtime)) {
        std::cout << "Failed to consume resources." << std::endl;

        // If we couldn't consume resources but already consumed ingredients,
        // we'd need to restore them, but for simplicity we'll skip that in this implementation
        return false;
    }

    // Add the crafted item to discovered loot
    runtime.addDiscoveredLoot(lootId);

    std::cout << "\n**LOOT CRAFTED**: " << loot->getName() << " ("
              << Loot::rarityToString(loot->getRarity()) << ")" << std::endl;
    std::cout << loot->getDescription() << std::endl;

    // Show the visual representation
    std::cout << "\n" << loot->getVisualRepresentation() << std::endl;

    // Display effect information if it has an effect
    if (loot->getEffect() != LootEffect::NONE) {
        std::cout << "Effect: " << Loot::effectToString(loot->getEffect()) << " (" << std::fixed
                  << std::setprecision(0) << (loot->getEffectStrength() * 100) << "%)" << std::endl;
    }

    return true;
}

std::vector<std::shared_ptr<Loot>> LootCrafter::getCraftableLoot(const TemporalRuntime& runtime) {
    std::vector<std::shared_ptr<Loot>> craftableLoot;
    auto& registry = LootRegistry::getInstance();

    // Get all loot items that have recipes
    auto allCraftableLoot = registry.getCraftableLoot();

    // Filter by what can be crafted with current resources and ingredients
    for (const auto& loot : allCraftableLoot) {
        if (canCraft(loot->getId(), runtime)) {
            craftableLoot.push_back(loot);
        }
    }

    return craftableLoot;
}

bool LootCrafter::hasIngredients(const LootRecipe& recipe, const TemporalRuntime& runtime) {
    // Check if all ingredients are discovered
    for (const auto& [ingredientId, quantity] : recipe.ingredients) {
        if (!runtime.hasDiscoveredLoot(ingredientId)) {
            return false;
        }

        // TODO: Check quantity when inventory system is implemented
        // For now, we'll just check if the item is discovered
    }

    return true;
}

bool LootCrafter::hasResources(const LootRecipe& recipe, const TemporalRuntime& runtime) {
    // Check if we have enough resources
    if (recipe.chrononCost > 0.0 && runtime.getChrononsLevel() < recipe.chrononCost) {
        return false;
    }

    if (recipe.aethelCost > 0.0 && runtime.getAethelLevel() < recipe.aethelCost) {
        return false;
    }

    // Paradox cost is a bit different - it checks if adding the paradox would be allowed
    // For now, we'll always allow it

    return true;
}

bool LootCrafter::consumeIngredients(const LootRecipe& recipe, TemporalRuntime& runtime) {
    // TODO: Implement when inventory system is added
    // For now, we'll just assume ingredients are consumed successfully

    // Mark parameters as intentionally unused until implementation is complete
    (void)recipe;
    (void)runtime;

    // In a real implementation, we would:
    // 1. Check if we have enough of each ingredient
    // 2. Remove the required quantity from inventory
    // 3. Return success/failure

    return true;
}

bool LootCrafter::consumeResources(const LootRecipe& recipe, TemporalRuntime& runtime) {
    try {
        // Consume chronons if needed
        if (recipe.chrononCost > 0.0) {
            runtime.consumeChronons(recipe.chrononCost);
            std::cout << "Consumed " << recipe.chrononCost << " chronons for crafting."
                      << std::endl;
        }

        // Consume aethel if needed
        if (recipe.aethelCost > 0.0) {
            runtime.consumeAethel(recipe.aethelCost);
            std::cout << "Consumed " << recipe.aethelCost << " aethel for crafting." << std::endl;
        }

        // Apply paradox cost if needed
        if (recipe.paradoxCost > 0) {
            runtime.increaseParadoxLevel(recipe.paradoxCost);
            std::cout << "Paradox increased by " << recipe.paradoxCost << " for crafting."
                      << std::endl;
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error consuming resources: " << e.what() << std::endl;
        return false;
    }
}

// LootTable implementation
void LootTable::addLoot(std::shared_ptr<Loot> loot, double chance) {
    if (chance < 0.0 || chance > 1.0) {
        throw std::invalid_argument("Loot chance must be between 0.0 and 1.0");
    }

    m_lootEntries.emplace_back(loot, chance);
}

std::shared_ptr<Loot> LootTable::rollForLoot() const {
    if (m_lootEntries.empty()) {
        return nullptr;
    }

    // Get a random seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // Roll for each loot entry
    for (const auto& entry : m_lootEntries) {
        double roll = distribution(generator);
        if (roll <= entry.second) {
            return entry.first;
        }
    }

    return nullptr;
}

std::shared_ptr<Loot> LootTable::rollForLootWithRarityBias(double rarityModifier) const {
    if (m_lootEntries.empty()) {
        return nullptr;
    }

    // Get a random seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // First, try to roll for higher rarity loot with the modifier
    if (rarityModifier > 0.0) {
        // Sort entries by rarity (highest first)
        std::vector<std::pair<std::shared_ptr<Loot>, double>> sortedEntries = m_lootEntries;
        std::sort(sortedEntries.begin(), sortedEntries.end(), [](const auto& a, const auto& b) {
            return static_cast<int>(a.first->getRarity()) > static_cast<int>(b.first->getRarity());
        });

        // Apply rarity modifier to the chances
        for (const auto& entry : sortedEntries) {
            double adjustedChance = entry.second;

            // Increase chance based on rarity and modifier
            switch (entry.first->getRarity()) {
                case LootRarity::MYTHIC:
                    adjustedChance += rarityModifier * 0.1;  // Big boost for mythic
                    break;
                case LootRarity::LEGENDARY:
                    adjustedChance += rarityModifier * 0.05;  // Good boost for legendary
                    break;
                case LootRarity::RARE:
                    adjustedChance += rarityModifier * 0.03;  // Moderate boost for rare
                    break;
                case LootRarity::UNCOMMON:
                    adjustedChance += rarityModifier * 0.01;  // Small boost for uncommon
                    break;
                default:
                    break;  // No boost for common
            }

            // Cap at 1.0
            adjustedChance = std::min(1.0, adjustedChance);

            // Roll with adjusted chance
            double roll = distribution(generator);
            if (roll <= adjustedChance) {
                return entry.first;
            }
        }
    }

    // Fall back to normal roll if nothing was found with the rarity bias
    return rollForLoot();
}

std::vector<std::shared_ptr<Loot>> LootTable::getAllLoot() const {
    std::vector<std::shared_ptr<Loot>> result;
    result.reserve(m_lootEntries.size());

    for (const auto& entry : m_lootEntries) {
        result.push_back(entry.first);
    }

    return result;
}

// LootRegistry implementation
LootRegistry& LootRegistry::getInstance() {
    static LootRegistry instance;
    return instance;
}

void LootRegistry::registerLoot(std::shared_ptr<Loot> loot) {
    if (!loot) {
        throw std::invalid_argument("Cannot register null loot");
    }

    m_lootRegistry[loot->getId()] = loot;
}

std::shared_ptr<Loot> LootRegistry::getLootById(const std::string& id) const {
    auto it = m_lootRegistry.find(id);
    if (it != m_lootRegistry.end()) {
        return it->second;
    }

    return nullptr;
}

const std::map<std::string, std::shared_ptr<Loot>>& LootRegistry::getAllLoot() const {
    return m_lootRegistry;
}

std::vector<std::shared_ptr<Loot>> LootRegistry::getLootByRarity(LootRarity rarity) const {
    std::vector<std::shared_ptr<Loot>> result;

    for (const auto& entry : m_lootRegistry) {
        if (entry.second->getRarity() == rarity) {
            result.push_back(entry.second);
        }
    }

    return result;
}

void LootRegistry::registerLootSet(std::shared_ptr<LootSet> lootSet) {
    if (!lootSet) {
        throw std::invalid_argument("Cannot register null loot set");
    }

    m_lootSetRegistry[lootSet->getId()] = lootSet;
}

std::shared_ptr<LootSet> LootRegistry::getLootSetById(const std::string& id) const {
    auto it = m_lootSetRegistry.find(id);
    if (it != m_lootSetRegistry.end()) {
        return it->second;
    }

    return nullptr;
}

const std::map<std::string, std::shared_ptr<LootSet>>& LootRegistry::getAllLootSets() const {
    return m_lootSetRegistry;
}

std::vector<std::shared_ptr<Loot>> LootRegistry::getCraftableLoot() const {
    std::vector<std::shared_ptr<Loot>> craftableLoot;

    for (const auto& [id, loot] : m_lootRegistry) {
        if (loot->isCraftable()) {
            craftableLoot.push_back(loot);
        }
    }

    return craftableLoot;
}

// LootJournal implementation
LootJournal& LootJournal::getInstance() {
    static LootJournal instance;
    return instance;
}

void LootJournal::recordDiscovery(const std::string& lootId, const TemporalRuntime& runtime,
                                  bool wasCrafted) {
    // Check if we've already recorded this loot discovery
    if (m_discoveredLootIds.find(lootId) != m_discoveredLootIds.end()) {
        return;
    }

    // Create a new discovery record
    LootDiscovery discovery(lootId, runtime.getParadoxLevel(), runtime.getChrononsLevel(),
                            runtime.getAethelLevel(), wasCrafted);

    // Add to the history
    m_discoveryHistory.push_back(discovery);
    m_discoveredLootIds.insert(lootId);

    // Update rarity count
    auto& registry = LootRegistry::getInstance();
    auto loot = registry.getLootById(lootId);
    if (loot) {
        auto rarity = loot->getRarity();
        m_rarityDiscoveryCount[rarity]++;

        std::cout << "LootJournal: Recorded discovery of " << loot->getName() << " ("
                  << Loot::rarityToString(rarity) << ")" << std::endl;
    }

    // Check for achievements
    updateAchievements(runtime);
}

const std::vector<LootDiscovery>& LootJournal::getDiscoveryHistory() const {
    return m_discoveryHistory;
}

int LootJournal::getDiscoveryCountByRarity(LootRarity rarity) const {
    auto it = m_rarityDiscoveryCount.find(rarity);
    if (it != m_rarityDiscoveryCount.end()) {
        return it->second;
    }
    return 0;
}

int LootJournal::getTotalDiscoveryCount() const { return m_discoveredLootIds.size(); }

void LootJournal::registerAchievement(const Achievement& achievement) {
    // Check if we already have this achievement
    for (const auto& existing : m_achievements) {
        if (existing.id == achievement.id) {
            return;
        }
    }

    // Add the achievement
    m_achievements.push_back(achievement);
    std::cout << "LootJournal: Registered achievement: " << achievement.name << std::endl;
}

void LootJournal::updateAchievements(const TemporalRuntime& runtime) {
    for (auto& achievement : m_achievements) {
        if (!achievement.isEarned && checkAchievementCondition(achievement, runtime)) {
            // Set the achievement as earned
            achievement.isEarned = true;
            achievement.earnedTimestamp = std::chrono::system_clock::now();

            // Announce the achievement
            std::cout << "\n\033[1;33m** ACHIEVEMENT UNLOCKED **\033[0m" << std::endl;
            std::cout << "\033[1;33m" << achievement.name << "\033[0m" << std::endl;
            std::cout << achievement.description << std::endl;
        }
    }
}

const std::vector<Achievement>& LootJournal::getAchievements() const { return m_achievements; }

std::vector<Achievement> LootJournal::getEarnedAchievements() const {
    std::vector<Achievement> earned;

    for (const auto& achievement : m_achievements) {
        if (achievement.isEarned) {
            earned.push_back(achievement);
        }
    }

    return earned;
}

bool LootJournal::hasEarnedAchievement(const std::string& achievementId) const {
    for (const auto& achievement : m_achievements) {
        if (achievement.id == achievementId && achievement.isEarned) {
            return true;
        }
    }

    return false;
}

std::string LootJournal::generateSummaryReport() const {
    std::stringstream ss;

    // Header
    ss << "\n\033[1;36m===== CHRONOVYAN LOOT JOURNAL =====\033[0m\n" << std::endl;

    // Discovery statistics
    ss << "\033[1;32mLoot Discoveries:\033[0m" << std::endl;
    ss << "Total unique items found: " << getTotalDiscoveryCount() << std::endl;

    // Discoveries by rarity
    ss << "\nDiscoveries by Rarity:" << std::endl;
    for (int i = 0; i <= static_cast<int>(LootRarity::MYTHIC); i++) {
        auto rarity = static_cast<LootRarity>(i);
        int count = getDiscoveryCountByRarity(rarity);

        // Get total possible for this rarity
        auto& registry = LootRegistry::getInstance();
        auto lootOfRarity = registry.getLootByRarity(rarity);

        ss << "  " << std::left << std::setw(10) << Loot::rarityToString(rarity) << ": " << count
           << " / " << lootOfRarity.size() << std::endl;
    }

    // Achievements
    auto earned = getEarnedAchievements();
    ss << "\n\033[1;32mAchievements Earned:\033[0m " << earned.size() << " / "
       << m_achievements.size() << std::endl;

    for (const auto& achievement : earned) {
        // Convert the time point to a readable format
        auto time = std::chrono::system_clock::to_time_t(achievement.earnedTimestamp);
        ss << "  \033[1;33m" << achievement.name << "\033[0m" << std::endl;
        ss << "    " << achievement.description << std::endl;

        // Replace deprecated ctime with safer ctime_s
        char time_str[26];  // ctime_s requires a buffer of at least 26 characters
        errno_t err = ctime_s(time_str, sizeof(time_str), &time);
        if (err == 0) {
            ss << "    Earned: " << time_str;
        } else {
            ss << "    Earned: [Time conversion error]";
        }
    }

    // Recent discoveries
    ss << "\n\033[1;32mRecent Discoveries:\033[0m" << std::endl;

    int numToShow = std::min(5, static_cast<int>(m_discoveryHistory.size()));
    auto startIdx = m_discoveryHistory.size() - numToShow;

    for (size_t i = startIdx; i < m_discoveryHistory.size(); i++) {
        const auto& discovery = m_discoveryHistory[i];
        auto loot = LootRegistry::getInstance().getLootById(discovery.lootId);

        if (loot) {
            auto time = std::chrono::system_clock::to_time_t(discovery.timestamp);
            ss << "  " << loot->getName() << " (" << Loot::rarityToString(loot->getRarity()) << ")"
               << std::endl;

            // Replace deprecated ctime with safer ctime_s
            char time_str[26];  // ctime_s requires a buffer of at least 26 characters
            errno_t err = ctime_s(time_str, sizeof(time_str), &time);
            if (err == 0) {
                ss << "    " << (discovery.wasCrafted ? "Crafted" : "Discovered") << ": "
                   << time_str;
            } else {
                ss << "    " << (discovery.wasCrafted ? "Crafted" : "Discovered")
                   << ": [Time conversion error]";
            }
        }
    }

    return ss.str();
}

bool LootJournal::checkAchievementCondition(Achievement& achievement,
                                            const TemporalRuntime& runtime) {
    switch (achievement.type) {
        case AchievementType::LOOT_DISCOVERY:
            return getTotalDiscoveryCount() >= achievement.threshold;

        case AchievementType::RARITY_COLLECTION: {
            // Threshold here represents the rarity level
            auto rarity = static_cast<LootRarity>(achievement.threshold);

            // Get all loot of this rarity
            auto& registry = LootRegistry::getInstance();
            auto lootOfRarity = registry.getLootByRarity(rarity);

            // Check if we've discovered all of them
            for (const auto& loot : lootOfRarity) {
                if (m_discoveredLootIds.find(loot->getId()) == m_discoveredLootIds.end()) {
                    return false;
                }
            }

            return !lootOfRarity.empty();
        }

        case AchievementType::SET_COMPLETION: {
            // Count completed sets
            int completedSets = 0;
            auto& registry = LootRegistry::getInstance();

            for (const auto& [id, lootSet] : registry.getAllLootSets()) {
                if (runtime.isLootSetCompleted(id)) {
                    completedSets++;
                }
            }

            return completedSets >= achievement.threshold;
        }

        case AchievementType::CRAFTING_MASTERY: {
            // Count crafted items
            int craftedItems = 0;
            for (const auto& discovery : m_discoveryHistory) {
                if (discovery.wasCrafted) {
                    craftedItems++;
                }
            }

            return craftedItems >= achievement.threshold;
        }

        case AchievementType::PARADOX_MASTERY:
            return runtime.getParadoxLevel() >= achievement.threshold;

        case AchievementType::RESOURCE_MASTERY:
            // Check if either resource is above the threshold
            return runtime.getChrononsLevel() >= achievement.threshold ||
                   runtime.getAethelLevel() >= achievement.threshold;

        default:
            return false;
    }
}

}  // namespace chronovyan