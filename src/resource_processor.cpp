#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "resource_processor.h"

namespace chronovyan {

ResourceProcessor::ResourceProcessor(std::shared_ptr<TemporalRuntime> runtime)
    : m_runtime(runtime)
    , m_defaultStrategy(ResourceProcessingStrategy::ADAPTIVE)
    , m_nextCallbackId(0) {
    // Initialize strategy efficiency map with default values
    m_strategyEfficiency[ResourceProcessingStrategy::SEQUENTIAL] = 0.8;
    m_strategyEfficiency[ResourceProcessingStrategy::PARALLEL] = 0.9;
    m_strategyEfficiency[ResourceProcessingStrategy::BATCH] = 0.85;
    m_strategyEfficiency[ResourceProcessingStrategy::PRIORITY_BASED] = 0.75;
    m_strategyEfficiency[ResourceProcessingStrategy::ADAPTIVE] = 0.95;
}

ResourceProcessingResult ResourceProcessor::processBatch(int amount,
                                                         ResourceProcessingStrategy strategy,
                                                         ResourcePriority priority) {
    // Choose strategy if ADAPTIVE is specified
    if (strategy == ResourceProcessingStrategy::ADAPTIVE) {
        strategy = chooseBestStrategy(amount, priority);
    }

    // Process based on the chosen strategy
    ResourceProcessingResult result;
    switch (strategy) {
        case ResourceProcessingStrategy::SEQUENTIAL:
            result = processSequential(amount, priority);
            break;
        case ResourceProcessingStrategy::BATCH:
            result = processBatchStrategy(amount, priority);
            break;
        case ResourceProcessingStrategy::PRIORITY_BASED:
            result = processPriorityBased(amount, priority);
            break;
        case ResourceProcessingStrategy::PARALLEL:
            // For parallel, we convert the single amount into a vector of smaller amounts
            {
                int batchSize = std::max(1, amount / 4);
                std::vector<int> amounts;
                int remaining = amount;
                while (remaining > 0) {
                    int batch = std::min(batchSize, remaining);
                    amounts.push_back(batch);
                    remaining -= batch;
                }
                auto results = processParallel(amounts, priority);
                // Combine results
                for (const auto& r : results) {
                    result.chrononsConsumed += r.chrononsConsumed;
                    result.aethelGenerated += r.aethelGenerated;
                    result.efficiency = (result.efficiency + r.efficiency) / 2.0;
                    result.stabilityImpact += r.stabilityImpact;
                }
                result.operationId = "batch_" + std::to_string(m_processingHistory.size());
            }
            break;
        default:
            // Default to adaptive processing
            result = processAdaptively(amount);
            break;
    }

    // Update strategy efficiency
    updateStrategyEfficiency(strategy, result);

    // Execute callbacks
    executeCallbacks(result);

    // Record in history
    m_processingHistory.push_back(result);

    return result;
}

std::vector<ResourceProcessingResult> ResourceProcessor::processParallel(
    const std::vector<int>& amounts, ResourcePriority priority) {
    std::vector<ResourceProcessingResult> results;
    results.reserve(amounts.size());

    // Process each amount in parallel (simulated for now)
    for (int amount : amounts) {
        ResourceProcessingResult result;
        result.chrononsConsumed = amount;
        result.aethelGenerated = static_cast<int>(amount * 0.7);  // 70% conversion rate

        // Adjust based on priority
        switch (priority) {
            case ResourcePriority::LOW:
                result.efficiency = 0.6;
                result.stabilityImpact = -0.1;
                break;
            case ResourcePriority::MEDIUM:
                result.efficiency = 0.8;
                result.stabilityImpact = 0.0;
                break;
            case ResourcePriority::HIGH:
                result.efficiency = 0.9;
                result.stabilityImpact = 0.1;
                break;
            case ResourcePriority::CRITICAL:
                result.efficiency = 1.0;
                result.stabilityImpact = 0.2;
                break;
        }

        result.operationId = "parallel_" + std::to_string(m_processingHistory.size()) + "_" +
                             std::to_string(results.size());
        results.push_back(result);
    }

    return results;
}

int ResourceProcessor::registerCallback(ResourceProcessingCallback callback) {
    int callbackId = m_nextCallbackId++;
    m_callbacks.emplace_back(callbackId, callback);
    return callbackId;
}

bool ResourceProcessor::removeCallback(int callbackId) {
    auto it = std::find_if(m_callbacks.begin(), m_callbacks.end(),
                           [callbackId](const auto& pair) { return pair.first == callbackId; });

    if (it != m_callbacks.end()) {
        m_callbacks.erase(it);
        return true;
    }

    return false;
}

double ResourceProcessor::getStrategyEfficiency(ResourceProcessingStrategy strategy) const {
    auto it = m_strategyEfficiency.find(strategy);
    if (it != m_strategyEfficiency.end()) {
        return it->second;
    }
    return 0.5;  // Default moderate efficiency
}

void ResourceProcessor::setDefaultStrategy(ResourceProcessingStrategy strategy) {
    m_defaultStrategy = strategy;
}

double ResourceProcessor::optimizeForResource(const std::string& resourceType) {
    // Simulate optimization
    double optimizationFactor = 1.0;  // No optimization by default

    if (resourceType == "chronons") {
        // Optimize for chronon usage
        for (auto& pair : m_strategyEfficiency) {
            if (pair.first == ResourceProcessingStrategy::SEQUENTIAL ||
                pair.first == ResourceProcessingStrategy::BATCH) {
                pair.second *= 1.1;                        // 10% improvement
                pair.second = std::min(pair.second, 1.0);  // Cap at 1.0
            }
        }
        optimizationFactor = 1.1;  // 10% improvement
    } else if (resourceType == "aethel") {
        // Optimize for aethel generation
        for (auto& pair : m_strategyEfficiency) {
            if (pair.first == ResourceProcessingStrategy::PARALLEL ||
                pair.first == ResourceProcessingStrategy::ADAPTIVE) {
                pair.second *= 1.15;                       // 15% improvement
                pair.second = std::min(pair.second, 1.0);  // Cap at 1.0
            }
        }
        optimizationFactor = 1.15;  // 15% improvement
    }

    return optimizationFactor;
}

ResourceProcessingResult ResourceProcessor::processAdaptively(int amount) {
    // Determine the best strategy based on amount
    ResourceProcessingStrategy strategy;
    if (amount < 10) {
        strategy = ResourceProcessingStrategy::SEQUENTIAL;
    } else if (amount < 50) {
        strategy = ResourceProcessingStrategy::BATCH;
    } else {
        strategy = ResourceProcessingStrategy::PARALLEL;
    }

    // Process using the chosen strategy
    return processBatch(amount, strategy);
}

std::map<std::string, double> ResourceProcessor::analyzeProcessingPatterns() const {
    std::map<std::string, double> patterns;

    // If we don't have enough history, return empty patterns
    if (m_processingHistory.size() < 5) {
        patterns["insufficient_data"] = 1.0;
        return patterns;
    }

    // Calculate patterns based on processing history
    double avgChronons = 0.0;
    double avgAethel = 0.0;
    double avgEfficiency = 0.0;
    double avgStability = 0.0;

    for (const auto& result : m_processingHistory) {
        avgChronons += result.chrononsConsumed;
        avgAethel += result.aethelGenerated;
        avgEfficiency += result.efficiency;
        avgStability += result.stabilityImpact;
    }

    avgChronons /= m_processingHistory.size();
    avgAethel /= m_processingHistory.size();
    avgEfficiency /= m_processingHistory.size();
    avgStability /= m_processingHistory.size();

    // Determine patterns
    patterns["high_chronon_usage"] = avgChronons > 50 ? 0.9 : 0.1;
    patterns["high_aethel_generation"] = avgAethel > 30 ? 0.8 : 0.2;
    patterns["efficiency_focused"] = avgEfficiency > 0.8 ? 0.85 : 0.15;
    patterns["stability_focused"] = avgStability > 0.1 ? 0.7 : 0.3;

    // Check for trends
    if (m_processingHistory.size() >= 10) {
        bool increasingChronons = true;
        bool increasingAethel = true;

        for (size_t i = 1; i < m_processingHistory.size(); ++i) {
            if (m_processingHistory[i].chrononsConsumed <=
                m_processingHistory[i - 1].chrononsConsumed) {
                increasingChronons = false;
            }
            if (m_processingHistory[i].aethelGenerated <=
                m_processingHistory[i - 1].aethelGenerated) {
                increasingAethel = false;
            }
        }

        patterns["increasing_chronon_trend"] = increasingChronons ? 0.95 : 0.05;
        patterns["increasing_aethel_trend"] = increasingAethel ? 0.9 : 0.1;
    }

    return patterns;
}

void ResourceProcessor::executeCallbacks(const ResourceProcessingResult& result) {
    for (const auto& [id, callback] : m_callbacks) {
        callback(result);
    }
}

ResourceProcessingStrategy ResourceProcessor::chooseBestStrategy(int amount,
                                                                 ResourcePriority priority) const {
    // Choose based on amount and priority
    if (priority == ResourcePriority::CRITICAL) {
        return ResourceProcessingStrategy::PRIORITY_BASED;
    }

    if (amount < 10) {
        return ResourceProcessingStrategy::SEQUENTIAL;
    } else if (amount < 50) {
        return ResourceProcessingStrategy::BATCH;
    } else {
        return ResourceProcessingStrategy::PARALLEL;
    }
}

ResourceProcessingResult ResourceProcessor::processSequential(int amount,
                                                              ResourcePriority priority) {
    ResourceProcessingResult result;
    result.chrononsConsumed = amount;

    // Calculate aethel generation based on priority
    double generationFactor = 0.0;
    switch (priority) {
        case ResourcePriority::LOW:
            generationFactor = 0.5;
            break;
        case ResourcePriority::MEDIUM:
            generationFactor = 0.7;
            break;
        case ResourcePriority::HIGH:
            generationFactor = 0.9;
            break;
        case ResourcePriority::CRITICAL:
            generationFactor = 1.2;
            break;
    }

    result.aethelGenerated = static_cast<int>(amount * generationFactor);
    result.efficiency = 0.8;        // Sequential is moderately efficient
    result.stabilityImpact = 0.05;  // Low impact on stability
    result.operationId = "seq_" + std::to_string(m_processingHistory.size());

    return result;
}

ResourceProcessingResult ResourceProcessor::processBatchStrategy(int amount,
                                                                 ResourcePriority priority) {
    ResourceProcessingResult result;
    result.chrononsConsumed = amount;

    // Calculate aethel generation based on priority
    double generationFactor = 0.0;
    switch (priority) {
        case ResourcePriority::LOW:
            generationFactor = 0.6;
            break;
        case ResourcePriority::MEDIUM:
            generationFactor = 0.8;
            break;
        case ResourcePriority::HIGH:
            generationFactor = 1.0;
            break;
        case ResourcePriority::CRITICAL:
            generationFactor = 1.3;
            break;
    }

    result.aethelGenerated = static_cast<int>(amount * generationFactor);
    result.efficiency = 0.85;      // Batch is more efficient than sequential
    result.stabilityImpact = 0.0;  // Neutral impact on stability
    result.operationId = "batch_" + std::to_string(m_processingHistory.size());

    return result;
}

ResourceProcessingResult ResourceProcessor::processPriorityBased(int amount,
                                                                 ResourcePriority priority) {
    ResourceProcessingResult result;

    // Adjust chronon consumption based on priority
    double chrononFactor = 0.0;
    switch (priority) {
        case ResourcePriority::LOW:
            chrononFactor = 1.2;  // Uses more chronons for low priority
            break;
        case ResourcePriority::MEDIUM:
            chrononFactor = 1.0;  // Standard usage
            break;
        case ResourcePriority::HIGH:
            chrononFactor = 0.9;  // More efficient
            break;
        case ResourcePriority::CRITICAL:
            chrononFactor = 0.8;  // Most efficient for critical tasks
            break;
    }

    result.chrononsConsumed = static_cast<int>(amount * chrononFactor);

    // Calculate aethel generation based on priority
    double generationFactor = 0.0;
    switch (priority) {
        case ResourcePriority::LOW:
            generationFactor = 0.5;
            break;
        case ResourcePriority::MEDIUM:
            generationFactor = 0.8;
            break;
        case ResourcePriority::HIGH:
            generationFactor = 1.1;
            break;
        case ResourcePriority::CRITICAL:
            generationFactor = 1.4;
            break;
    }

    result.aethelGenerated = static_cast<int>(amount * generationFactor);
    result.efficiency = priority == ResourcePriority::CRITICAL ? 0.95 : 0.75;
    result.stabilityImpact = priority == ResourcePriority::CRITICAL ? 0.2 : -0.1;
    result.operationId = "priority_" + std::to_string(m_processingHistory.size());

    return result;
}

void ResourceProcessor::updateStrategyEfficiency(ResourceProcessingStrategy strategy,
                                                 const ResourceProcessingResult& result) {
    // Update efficiency based on result
    double currentEfficiency = m_strategyEfficiency[strategy];
    double newEfficiency = result.efficiency;

    // Blend old and new (80% old, 20% new)
    m_strategyEfficiency[strategy] = (currentEfficiency * 0.8) + (newEfficiency * 0.2);
}

}  // namespace chronovyan