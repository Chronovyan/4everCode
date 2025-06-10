#include <algorithm>
#include <cmath>

#include "error_handler.h"
#include "interpreter/ResourceManager.h"
#include "resource_optimizer.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

namespace chronovyan {

ResourceManager::ResourceManager(
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<ResourceOptimizer> optimizer,
    std::shared_ptr<TemporalDebtTracker> debtTracker)
    : m_runtime(runtime), m_optimizer(optimizer), m_debtTracker(debtTracker) {}

bool ResourceManager::consumeAethel(double amount,
                                    const std::string &operation) {
  // Apply optimization if possible
  double optimizedAmount = amount;
  if (m_optimizer && !operation.empty()) {
    optimizedAmount = m_optimizer->optimizeAethel(operation);
    optimizedAmount = std::min(amount, optimizedAmount);
  }

  // Check if we have enough resources
  if (m_runtime->getAethelLevel() < optimizedAmount) {
    return false;
  }

  // Consume the resources
  m_runtime->consumeAethel(optimizedAmount);

  // Track usage
  if (!operation.empty()) {
    trackResourceUsage(optimizedAmount, 0.0, operation);
  }

  return true;
}

bool ResourceManager::consumeChronons(double amount,
                                      const std::string &operation) {
  // Apply optimization if possible
  double optimizedAmount = amount;
  if (m_optimizer && !operation.empty()) {
    optimizedAmount = m_optimizer->optimizeChronons(operation);
    optimizedAmount = std::min(amount, optimizedAmount);
  }

  // Check if we have enough resources
  if (m_runtime->getChrononsLevel() < optimizedAmount) {
    return false;
  }

  // Consume the resources
  m_runtime->consumeChronons(optimizedAmount);

  // Track usage
  if (!operation.empty()) {
    trackResourceUsage(0.0, optimizedAmount, operation);
  }

  return true;
}

bool ResourceManager::consumeResources(double aethelAmount,
                                       double chrononAmount,
                                       const std::string &operation) {
  // Apply optimizations if possible
  double optimizedAethel = aethelAmount;
  double optimizedChronons = chrononAmount;

  if (m_optimizer && !operation.empty()) {
    optimizedAethel = m_optimizer->optimizeAethel(operation);
    optimizedChronons = m_optimizer->optimizeChronons(operation);

    optimizedAethel = std::min(aethelAmount, optimizedAethel);
    optimizedChronons = std::min(chrononAmount, optimizedChronons);
  }

  // Check if we have enough resources
  if (m_runtime->getAethelLevel() < optimizedAethel ||
      m_runtime->getChrononsLevel() < optimizedChronons) {
    return false;
  }

  // Consume the resources
  m_runtime->consumeAethel(optimizedAethel);
  m_runtime->consumeChronons(optimizedChronons);

  // Track usage
  if (!operation.empty()) {
    trackResourceUsage(optimizedAethel, optimizedChronons, operation);
  }

  return true;
}

double ResourceManager::getAethelLevel() const {
  return m_runtime->getAethelLevel();
}

double ResourceManager::getChrononsLevel() const {
  return m_runtime->getChrononsLevel();
}

double ResourceManager::getMaxAethel() const {
  return m_runtime->getMaxAethel();
}

double ResourceManager::getMaxChronons() const {
  return m_runtime->getMaxChronons();
}

void ResourceManager::replenishAethel(double amount) {
  m_runtime->replenishAethel(amount);
}

void ResourceManager::replenishChronons(double amount) {
  m_runtime->replenishChronons(amount);
}

void ResourceManager::trackResourceUsage(double aethelAmount,
                                         double chrononAmount,
                                         const std::string &operation) {
  // If we don't have an entry for this operation yet, create one
  if (m_resourceUsageHistory.find(operation) == m_resourceUsageHistory.end()) {
    m_resourceUsageHistory[operation] = {0.0, 0.0, 0};
  }

  // Update the usage statistics
  auto &usage = m_resourceUsageHistory[operation];
  usage.aethel += aethelAmount;
  usage.chronons += chrononAmount;
  usage.count++;

  // If we have a debt tracker, track the resource usage
  if (m_debtTracker) {
    double totalCost = aethelAmount + chrononAmount;
    if (totalCost > 0.0) {
      // Add a small debt entry for high-cost operations
      if (totalCost > 10.0) {
        m_debtTracker->addDebtEntry(operation, totalCost * 0.1);
      }
    }
  }
}

std::map<std::string, std::pair<double, double>>
ResourceManager::getResourceUsageHistory() const {
  std::map<std::string, std::pair<double, double>> result;

  for (const auto &[operation, usage] : m_resourceUsageHistory) {
    result[operation] = {usage.aethel, usage.chronons};
  }

  return result;
}

double ResourceManager::calculateTemporalOperationCost(
    const std::string &operationType, int paradoxLevel,
    double stabilizationFactor) const {
  // Get the base cost for the operation type
  double baseCost = 10.0; // Default if not found

  auto it = m_baseCosts.find(operationType);
  if (it != m_baseCosts.end()) {
    baseCost = it->second;
  }

  // Apply stabilization factor (if any)
  if (stabilizationFactor > 0.0) {
    // Stabilization reduces cost
    baseCost *= (1.0 - stabilizationFactor * 0.5);
  }

  // Paradox level increases cost exponentially
  double paradoxFactor = 1.0 + (paradoxLevel / 10.0);

  return baseCost * paradoxFactor;
}

} // namespace chronovyan