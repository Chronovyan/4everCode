#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

#include "temporal_runtime.h"

namespace chronovyan {

TemporalRuntime::TemporalRuntime()
    : m_paradoxLevel(0), m_aethelLevel(100.0), m_chrononsLevel(100.0),
      m_currentCycle(0), m_currentLevel(0.0), m_efficiency(1.0) {
  // Initialize with default values
}

// TemporalRuntime::~TemporalRuntime() {
//     // Clean up any resources
//     m_timelineSnapshots.clear();
// }

int TemporalRuntime::getParadoxLevel() const { return m_paradoxLevel; }

void TemporalRuntime::increaseParadoxLevel(int amount) {
  if (amount <= 0) {
    return;
  }

  m_paradoxLevel += amount;
  std::cout << "Paradox level increased by " << amount
            << ". New level: " << m_paradoxLevel << std::endl;

  // Check for loot after paradox level changes
  checkParadoxLoot();
}

void TemporalRuntime::decreaseParadoxLevel(int amount) {
  m_paradoxLevel = std::max(0, m_paradoxLevel - amount);
  std::cout << "Paradox level decreased by " << amount
            << ". New level: " << m_paradoxLevel << std::endl;
}

void TemporalRuntime::setParadoxLevel(int level) {
  if (level < 0) {
    level = 0;
  }

  int change = level - m_paradoxLevel;
  m_paradoxLevel = level;

  if (change > 0) {
    std::cout << "Paradox level increased by " << change
              << ". New level: " << m_paradoxLevel << std::endl;
  } else if (change < 0) {
    std::cout << "Paradox level decreased by " << -change
              << ". New level: " << m_paradoxLevel << std::endl;
  }
}

double TemporalRuntime::getAethelLevel() const { return m_aethelLevel; }

void TemporalRuntime::consumeAethel(double amount) {
  if (m_aethelLevel < amount) {
    throw std::runtime_error("Insufficient aethel resources available");
  }

  m_aethelLevel -= amount;
  std::cout << "Consumed " << amount << " aethel. Remaining: " << m_aethelLevel
            << std::endl;

  // Check for loot after significant resource usage
  if (amount > 20.0) {
    checkResourceLoot();
  }
}

void TemporalRuntime::replenishAethel(double amount) {
  m_aethelLevel += amount;
  std::cout << "Replenished " << amount
            << " aethel. New level: " << m_aethelLevel << std::endl;
}

double TemporalRuntime::getChrononsLevel() const { return m_chrononsLevel; }

void TemporalRuntime::consumeChronons(double amount) {
  if (m_chrononsLevel < amount) {
    throw std::runtime_error("Insufficient chronons resources available");
  }

  m_chrononsLevel -= amount;
  std::cout << "Consumed " << amount
            << " chronons. Remaining: " << m_chrononsLevel << std::endl;

  // Check for loot after significant resource usage
  if (amount > 20.0) {
    checkResourceLoot();
  }
}

void TemporalRuntime::replenishChronons(double amount) {
  m_chrononsLevel += amount;
  std::cout << "Replenished " << amount
            << " chronons. New level: " << m_chrononsLevel << std::endl;
}

double TemporalRuntime::getMaxChronons() const {
  // For now, return a fixed value of 1000.0
  // In a more complete implementation, this could be a configurable limit
  return 1000.0;
}

double TemporalRuntime::getMaxAethel() const {
  // For now, return a fixed value of 1000.0
  // In a more complete implementation, this could be a configurable limit
  return 1000.0;
}

void TemporalRuntime::createTimelineSnapshot() {
  // Generate a unique ID for this snapshot
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(1000, 9999);

  std::stringstream ss;
  ss << "timeline_" << std::time(nullptr) << "_" << distrib(gen);
  std::string snapshotId = ss.str();

  // In a real implementation, we would capture the current program state
  // Here we're just creating a placeholder
  m_timelineSnapshots[snapshotId] = std::make_shared<int>(1);

  std::cout << "Created timeline snapshot: " << snapshotId << std::endl;
  std::cout << "Timeline branch created with ID: " << snapshotId << std::endl;

  // Creating a snapshot consumes chronons
  try {
    consumeChronons(15.0);
  } catch (const std::exception &e) {
    // If we can't consume resources, still create the snapshot but warn
    std::cout << "Warning: " << e.what()
              << " - Creating snapshot with minimal resources" << std::endl;
  }

  // And increases paradox level slightly
  increaseParadoxLevel(1);

  // Check for loot discovery after timeline operations
  checkForLootDiscovery();

  snapshotId; // NOLINT - placeholder to use the variable
}

void TemporalRuntime::rewindToSnapshot(const std::string &snapshotId) {
  auto it = m_timelineSnapshots.find(snapshotId);
  if (it == m_timelineSnapshots.end()) {
    throw std::runtime_error("Timeline snapshot not found: " + snapshotId);
  }

  // In a real implementation, we would restore the program state
  std::cout << "Rewinding to timeline: " << snapshotId << std::endl;

  // Rewinding consumes resources
  consumeAethel(15.0);

  // And increases paradox level
  increaseParadoxLevel(10);

  // Check for loot discovery after timeline operations
  checkForLootDiscovery();
}

void TemporalRuntime::mergeTimelines(
    const std::vector<std::string> &timelineIds) {
  if (timelineIds.empty()) {
    std::cout << "No timelines to merge." << std::endl;
    return;
  }

  // Check if all timelines exist
  std::vector<std::string> validTimelineIds;
  for (const auto &id : timelineIds) {
    if (m_timelineSnapshots.find(id) != m_timelineSnapshots.end()) {
      validTimelineIds.push_back(id);
    } else {
      std::cout << "Warning: Timeline snapshot not found: " << id << std::endl;
    }
  }

  if (validTimelineIds.empty()) {
    std::cout << "No valid timelines to merge." << std::endl;
    return;
  }

  std::cout << "Merging " << validTimelineIds.size() << " timelines..."
            << std::endl;

  // Log the IDs of the timelines being merged
  std::cout << "Merging timelines: ";
  for (size_t i = 0; i < validTimelineIds.size(); ++i) {
    std::cout << validTimelineIds[i];
    if (i < validTimelineIds.size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << std::endl;

  // In a real implementation, we would merge program states
  // This is a complex operation that would depend on the specific language
  // semantics

  // Merging is resource intensive - scale with number of timelines
  double chrononsPerTimeline = 10.0;
  double aethelPerTimeline = 8.0;

  try {
    consumeChronons(chrononsPerTimeline * validTimelineIds.size());
    consumeAethel(aethelPerTimeline * validTimelineIds.size());
  } catch (const std::exception &e) {
    // If we can't consume all resources, still perform the merge but warn
    std::cout << "Warning: " << e.what() << " - Merging with minimal resources"
              << std::endl;
  }

  // Increase paradox level based on number of timelines merged
  increaseParadoxLevel(validTimelineIds.size());

  std::cout << "Timeline merge completed successfully." << std::endl;

  // Check for loot discovery after timeline operations
  checkForLootDiscovery();
}

size_t TemporalRuntime::getTimelineSnapshotCount() const {
  return m_timelineSnapshots.size();
}

// Loot system implementation
void TemporalRuntime::checkForLootDiscovery() {
  // This is the main entry point for loot discovery
  // Check both paradox and resource-based loot
  checkParadoxLoot();
  checkResourceLoot();

  // After discovering loot, check if any loot sets have been completed
  checkLootSetCompletion();
}

void TemporalRuntime::checkRareLootDiscovery(double rarityModifier) {
  // Check for loot with a bias toward rarer items
  checkParadoxLoot(rarityModifier);
  checkResourceLoot(rarityModifier);

  // After discovering loot, check if any loot sets have been completed
  checkLootSetCompletion();
}

std::vector<std::shared_ptr<Loot>> TemporalRuntime::getDiscoveredLoot() const {
  std::vector<std::shared_ptr<Loot>> discoveredLoot;
  auto &registry = LootRegistry::getInstance();

  for (const auto &lootId : m_discoveredLootIds) {
    auto loot = registry.getLootById(lootId);
    if (loot) {
      discoveredLoot.push_back(loot);
    }
  }

  return discoveredLoot;
}

bool TemporalRuntime::hasDiscoveredLoot(const std::string &lootId) const {
  return m_discoveredLootIds.find(lootId) != m_discoveredLootIds.end();
}

std::shared_ptr<Loot>
TemporalRuntime::getDiscoveredLootById(const std::string &lootId) const {
  if (hasDiscoveredLoot(lootId)) {
    return LootRegistry::getInstance().getLootById(lootId);
  }
  return nullptr;
}

void TemporalRuntime::registerLootTable(const LootTable &lootTable) {
  m_lootTables.push_back(lootTable);
}

bool TemporalRuntime::applyLootEffect(const std::shared_ptr<Loot> &loot) {
  if (!loot || !hasDiscoveredLoot(loot->getId())) {
    return false;
  }

  return loot->applyEffect(*this);
}

bool TemporalRuntime::addDiscoveredLoot(const std::string &lootId) {
  auto &registry = LootRegistry::getInstance();
  auto loot = registry.getLootById(lootId);

  if (!loot) {
    return false;
  }

  bool wasAdded = m_discoveredLootIds.insert(lootId).second;

  if (wasAdded) {
    // Record the discovery in the loot journal
    LootJournal::getInstance().recordDiscovery(lootId, *this, false);

    // Check if any loot sets have been completed after adding this item
    checkLootSetCompletion();
  }

  return wasAdded;
}

std::vector<std::shared_ptr<Loot>> TemporalRuntime::getCraftableLoot() const {
  return LootCrafter::getCraftableLoot(*this);
}

bool TemporalRuntime::craftLoot(const std::string &lootId) {
  bool success = LootCrafter::craftLoot(lootId, *this);

  if (success) {
    // Record the crafting in the loot journal
    LootJournal::getInstance().recordDiscovery(lootId, *this, true);
  }

  return success;
}

std::vector<std::shared_ptr<LootSet>>
TemporalRuntime::getCompletedLootSets() const {
  std::vector<std::shared_ptr<LootSet>> completedSets;
  auto &registry = LootRegistry::getInstance();

  // Check each registered loot set
  for (const auto &[id, lootSet] : registry.getAllLootSets()) {
    if (lootSet->isCompleted(m_discoveredLootIds)) {
      completedSets.push_back(lootSet);
    }
  }

  return completedSets;
}

bool TemporalRuntime::isLootSetCompleted(const std::string &lootSetId) const {
  auto &registry = LootRegistry::getInstance();
  auto lootSet = registry.getLootSetById(lootSetId);

  if (!lootSet) {
    return false;
  }

  return lootSet->isCompleted(m_discoveredLootIds);
}

bool TemporalRuntime::applyLootSetBonus(const std::string &lootSetId) {
  // Check if the set exists and is completed
  auto &registry = LootRegistry::getInstance();
  auto lootSet = registry.getLootSetById(lootSetId);

  if (!lootSet || !lootSet->isCompleted(m_discoveredLootIds)) {
    return false;
  }

  // Check if we've already applied this set bonus
  if (m_appliedLootSetBonuses.find(lootSetId) !=
      m_appliedLootSetBonuses.end()) {
    std::cout << "Set bonus for " << lootSet->getName()
              << " has already been applied." << std::endl;
    return false;
  }

  // Apply the set bonus
  bool bonusApplied = lootSet->applySetBonus(*this);

  if (bonusApplied) {
    // Record that we've applied this set bonus
    m_appliedLootSetBonuses.insert(lootSetId);
  }

  return bonusApplied;
}

void TemporalRuntime::checkLootSetCompletion() {
  auto &registry = LootRegistry::getInstance();

  // Check each registered loot set
  for (const auto &[id, lootSet] : registry.getAllLootSets()) {
    // If the set is completed and we haven't applied the bonus yet
    if (lootSet->isCompleted(m_discoveredLootIds) &&
        m_appliedLootSetBonuses.find(id) == m_appliedLootSetBonuses.end()) {
      std::cout << "\n**LOOT SET COMPLETED**: " << lootSet->getName()
                << std::endl;
      std::cout << lootSet->getDescription() << std::endl;
      std::cout << "You have collected all " << lootSet->size()
                << " items in this set!" << std::endl;
      std::cout << "Type 'use set " << id << "' to apply the set bonus."
                << std::endl;
    }
  }
}

void TemporalRuntime::checkParadoxLoot() {
  // Standard paradox loot check without rarity bias
  checkParadoxLoot(0.0);
}

void TemporalRuntime::checkParadoxLoot(double rarityModifier) {
  // Chance of loot discovery increases with paradox level
  double discoveryChance = m_paradoxLevel / 100.0;

  // Cap the chance at 80%
  discoveryChance = std::min(discoveryChance, 0.8);

  // Random roll for discovery
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> distrib(0.0, 1.0);

  if (distrib(gen) <= discoveryChance) {
    // Roll on each loot table
    for (const auto &table : m_lootTables) {
      std::shared_ptr<Loot> loot;

      // Use rarity-biased roll if modifier is specified
      if (rarityModifier > 0.0) {
        loot = table.rollForLootWithRarityBias(rarityModifier);
      } else {
        loot = table.rollForLoot();
      }

      if (loot) {
        if (m_discoveredLootIds.insert(loot->getId()).second) {
          // Successfully discovered new loot
          std::cout << "\n**LOOT DISCOVERED**: " << loot->getName() << " ("
                    << Loot::rarityToString(loot->getRarity()) << ")"
                    << std::endl;
          std::cout << loot->getDescription() << std::endl;

          // Show the visual representation
          std::cout << "\n" << loot->getVisualRepresentation() << std::endl;

          // Display effect information if it has an effect
          if (loot->getEffect() != LootEffect::NONE) {
            std::cout << "Effect: " << Loot::effectToString(loot->getEffect())
                      << " (" << std::fixed << std::setprecision(0)
                      << (loot->getEffectStrength() * 100) << "%)" << std::endl;
          }
        }
      }
    }
  }
}

void TemporalRuntime::checkResourceLoot() {
  // Standard resource loot check without rarity bias
  checkResourceLoot(0.0);
}

void TemporalRuntime::checkResourceLoot(double rarityModifier) {
  // Base chance of discovery
  double discoveryChance = 0.1;

  // Adjust based on resource levels (lower resources = higher chance)
  if (m_aethelLevel < 50.0)
    discoveryChance += 0.1;
  if (m_chrononsLevel < 50.0)
    discoveryChance += 0.1;

  // Random roll for discovery
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> distrib(0.0, 1.0);

  if (distrib(gen) <= discoveryChance) {
    // Roll on each loot table
    for (const auto &table : m_lootTables) {
      std::shared_ptr<Loot> loot;

      // Use rarity-biased roll if modifier is specified
      if (rarityModifier > 0.0) {
        loot = table.rollForLootWithRarityBias(rarityModifier);
      } else {
        loot = table.rollForLoot();
      }

      if (loot) {
        if (m_discoveredLootIds.insert(loot->getId()).second) {
          // Successfully discovered new loot
          std::cout << "\n**LOOT DISCOVERED**: " << loot->getName() << " ("
                    << Loot::rarityToString(loot->getRarity()) << ")"
                    << std::endl;
          std::cout << loot->getDescription() << std::endl;

          // Show the visual representation
          std::cout << "\n" << loot->getVisualRepresentation() << std::endl;

          // Display effect information if it has an effect
          if (loot->getEffect() != LootEffect::NONE) {
            std::cout << "Effect: " << Loot::effectToString(loot->getEffect())
                      << " (" << std::fixed << std::setprecision(0)
                      << (loot->getEffectStrength() * 100) << "%)" << std::endl;
          }
        }
      }
    }
  }
}

long long TemporalRuntime::getCycleNumber() const {
  return static_cast<long long>(m_currentCycle);
}

double TemporalRuntime::getEfficiency() const { return m_efficiency; }

double TemporalRuntime::getCurrentLevel() const { return m_currentLevel; }

int TemporalRuntime::advanceCycle() {
  // Increment the current cycle
  m_currentCycle++;

  // Record the current level in history
  m_resourceHistory.push_back(m_currentLevel);

  // Update efficiency and record it
  updateEfficiency();
  m_efficiencyHistory.push_back(m_efficiency);

  return m_currentCycle;
}

void TemporalRuntime::reset() {
  m_currentCycle = 0;
  m_currentLevel = 0.0;
  m_efficiency = 1.0;
  m_resourceHistory.clear();
  m_efficiencyHistory.clear();
}

std::vector<double> TemporalRuntime::getResourceHistory() const {
  return m_resourceHistory;
}

std::vector<double> TemporalRuntime::getEfficiencyHistory() const {
  return m_efficiencyHistory;
}

void TemporalRuntime::updateEfficiency() {
  // Simplified efficiency calculation - in a real implementation,
  // this would use more complex algorithms based on resource usage patterns

  if (m_resourceHistory.size() < 2) {
    // Not enough history to calculate trend
    m_efficiency = 1.0;
    return;
  }

  // Calculate efficiency based on the trend of resource usage
  double previousLevel = m_resourceHistory[m_resourceHistory.size() - 2];
  double currentLevel = m_resourceHistory[m_resourceHistory.size() - 1];

  if (previousLevel <= 0.0) {
    // Avoid division by zero
    m_efficiency = 1.0;
  } else {
    // Efficiency improves if resource usage decreases
    double ratio = previousLevel / currentLevel;

    // Clamp efficiency between 0.5 and 1.5
    m_efficiency = std::min(1.5, std::max(0.5, ratio));
  }
}

} // namespace chronovyan