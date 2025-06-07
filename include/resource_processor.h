#ifndef CHRONOVYAN_RESOURCE_PROCESSOR_H
#define CHRONOVYAN_RESOURCE_PROCESSOR_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "temporal_runtime.h"

namespace chronovyan {

/**
 * @enum ResourcePriority
 * @brief Defines the priority levels for resource processing
 */
enum class ResourcePriority {
    LOW,      // Low priority resources that can be processed last
    MEDIUM,   // Medium priority resources that should be processed in the middle
    HIGH,     // High priority resources that should be processed first
    CRITICAL  // Critical resources that must be processed immediately
};

/**
 * @enum ResourceProcessingStrategy
 * @brief Defines the strategy for processing resources
 */
enum class ResourceProcessingStrategy {
    SEQUENTIAL,      // Process resources in sequence
    PARALLEL,        // Process resources in parallel if possible
    BATCH,           // Process resources in batches
    PRIORITY_BASED,  // Process resources based on priority
    ADAPTIVE         // Adaptively choose strategy based on context
};

/**
 * @struct ResourceProcessingResult
 * @brief Result of a resource processing operation
 */
struct ResourceProcessingResult {
    int chrononsConsumed;     // Number of chronons consumed during processing
    int aethelGenerated;      // Number of aethel particles generated during processing
    double efficiency;        // Efficiency of the operation (0.0-1.0)
    double stabilityImpact;   // Impact on system stability (-1.0 to 1.0)
    std::string operationId;  // Identifier for the operation

    ResourceProcessingResult(int chronons = 0, int aethel = 0, double eff = 1.0,
                             double stability = 0.0, const std::string& opId = "")
        : chrononsConsumed(chronons)
        , aethelGenerated(aethel)
        , efficiency(eff)
        , stabilityImpact(stability)
        , operationId(opId) {}
};

/**
 * @using ResourceProcessingCallback
 * @brief Callback type for resource processing events
 */
using ResourceProcessingCallback = std::function<void(const ResourceProcessingResult&)>;

/**
 * @class ResourceProcessor
 * @brief Processes temporal resources according to various strategies
 */
class ResourceProcessor {
public:
    /**
     * @brief Create a new resource processor
     * @param runtime The temporal runtime to use for resource operations
     */
    explicit ResourceProcessor(std::shared_ptr<TemporalRuntime> runtime);

    /**
     * @brief Process a batch of resources
     * @param amount The amount of resources to process
     * @param strategy The strategy to use for processing
     * @param priority The priority of the resources
     * @return Result of the processing operation
     */
    ResourceProcessingResult processBatch(
        int amount, ResourceProcessingStrategy strategy = ResourceProcessingStrategy::ADAPTIVE,
        ResourcePriority priority = ResourcePriority::MEDIUM);

    /**
     * @brief Process resources in parallel
     * @param amounts Vector of resource amounts to process in parallel
     * @param priority The priority of the resources
     * @return Vector of results for each parallel operation
     */
    std::vector<ResourceProcessingResult> processParallel(
        const std::vector<int>& amounts, ResourcePriority priority = ResourcePriority::MEDIUM);

    /**
     * @brief Register a callback for resource processing events
     * @param callback The callback function
     * @return The callback ID
     */
    int registerCallback(ResourceProcessingCallback callback);

    /**
     * @brief Remove a registered callback
     * @param callbackId The ID of the callback to remove
     * @return True if the callback was removed, false otherwise
     */
    bool removeCallback(int callbackId);

    /**
     * @brief Get the processing efficiency for a given strategy
     * @param strategy The strategy to check
     * @return The efficiency of the strategy (0.0-1.0)
     */
    double getStrategyEfficiency(ResourceProcessingStrategy strategy) const;

    /**
     * @brief Set the processing strategy
     * @param strategy The strategy to use for future operations
     */
    void setDefaultStrategy(ResourceProcessingStrategy strategy);

    /**
     * @brief Optimize processing for a specific resource type
     * @param resourceType The type of resource to optimize for ("chronons" or "aethel")
     * @return The optimization factor achieved (1.0 means no optimization)
     */
    double optimizeForResource(const std::string& resourceType);

    /**
     * @brief Process resources adaptively based on system conditions
     * @param amount The amount of resources to process
     * @return Result of the processing operation
     */
    ResourceProcessingResult processAdaptively(int amount);

    /**
     * @brief Analyze resource processing patterns
     * @return Map of pattern types to their confidence levels (0.0-1.0)
     */
    std::map<std::string, double> analyzeProcessingPatterns() const;

private:
    std::shared_ptr<TemporalRuntime> m_runtime;
    ResourceProcessingStrategy m_defaultStrategy;
    std::map<ResourceProcessingStrategy, double> m_strategyEfficiency;
    std::vector<std::pair<int, ResourceProcessingCallback>> m_callbacks;
    std::vector<ResourceProcessingResult> m_processingHistory;
    int m_nextCallbackId;

    /**
     * @brief Execute all registered callbacks
     * @param result The result to pass to callbacks
     */
    void executeCallbacks(const ResourceProcessingResult& result);

    /**
     * @brief Choose the best strategy for processing based on amount and priority
     * @param amount The amount of resources to process
     * @param priority The priority of the resources
     * @return The best strategy for the given parameters
     */
    ResourceProcessingStrategy chooseBestStrategy(int amount, ResourcePriority priority) const;

    /**
     * @brief Process resources using sequential strategy
     * @param amount The amount of resources to process
     * @param priority The priority of the resources
     * @return Result of the processing operation
     */
    ResourceProcessingResult processSequential(int amount, ResourcePriority priority);

    /**
     * @brief Process resources using batch strategy
     * @param amount The amount of resources to process
     * @param priority The priority of the resources
     * @return Result of the processing operation
     */
    ResourceProcessingResult processBatchStrategy(int amount, ResourcePriority priority);

    /**
     * @brief Process resources using priority-based strategy
     * @param amount The amount of resources to process
     * @param priority The priority of the resources
     * @return Result of the processing operation
     */
    ResourceProcessingResult processPriorityBased(int amount, ResourcePriority priority);

    /**
     * @brief Update strategy efficiency based on processing result
     * @param strategy The strategy that was used
     * @param result The result of the processing operation
     */
    void updateStrategyEfficiency(ResourceProcessingStrategy strategy,
                                  const ResourceProcessingResult& result);
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_RESOURCE_PROCESSOR_H