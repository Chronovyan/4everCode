#ifndef CHRONOVYAN_RESOURCE_MANAGER_H
#define CHRONOVYAN_RESOURCE_MANAGER_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "resource_optimizer.h"
#include "resource_processor.h"
#include "temporal_runtime.h"

namespace chronovyan {

/**
 * @enum ResourceAllocationStrategy
 * @brief Defines strategies for allocating resources
 */
enum class ResourceAllocationStrategy {
    BALANCED,       // Balance between chronons and aethel
    CHRONON_HEAVY,  // Prioritize chronon allocation
    AETHEL_HEAVY,   // Prioritize aethel allocation
    STABILITY,      // Prioritize system stability
    EFFICIENCY,     // Prioritize resource efficiency
    DEBT_AWARE,     // Consider debt in allocation decisions
    ADAPTIVE        // Adapt strategy based on system state
};

/**
 * @struct ResourceAllocation
 * @brief Represents a resource allocation decision
 */
struct ResourceAllocation {
    int chronons;             // Number of chronons allocated
    int aethel;               // Number of aethel particles allocated
    double efficiency;        // Efficiency of the allocation (0.0-1.0)
    double stabilityImpact;   // Impact on system stability (-1.0 to 1.0)
    std::string operationId;  // Identifier for the operation

    ResourceAllocation(int c = 0, int a = 0, double eff = 1.0, double stab = 0.0,
                       const std::string& opId = "")
        : chronons(c), aethel(a), efficiency(eff), stabilityImpact(stab), operationId(opId) {}
};

/**
 * @using ResourceAllocationCallback
 * @brief Callback type for resource allocation events
 */
using ResourceAllocationCallback = std::function<void(const ResourceAllocation&)>;

/**
 * @class ResourceManager
 * @brief Manages temporal resources, combining processing and optimization
 */
class ResourceManager {
public:
    /**
     * @brief Create a new resource manager
     * @param runtime The temporal runtime to manage resources for
     */
    explicit ResourceManager(std::shared_ptr<TemporalRuntime> runtime);

    /**
     * @brief Allocate resources for an operation
     * @param operationId The ID of the operation requiring resources
     * @param chrononRequest The requested amount of chronons
     * @param aethelRequest The requested amount of aethel
     * @param strategy The allocation strategy to use
     * @return The allocation decision
     */
    ResourceAllocation allocateResources(
        const std::string& operationId, int chrononRequest, int aethelRequest,
        ResourceAllocationStrategy strategy = ResourceAllocationStrategy::BALANCED);

    /**
     * @brief Process a batch of resources
     * @param amount The amount of resources to process
     * @param strategy The processing strategy to use
     * @return Result of the processing operation
     */
    ResourceProcessingResult processResources(
        int amount, ResourceProcessingStrategy strategy = ResourceProcessingStrategy::ADAPTIVE);

    /**
     * @brief Optimize resource usage
     * @param resourceType The type of resource to optimize ("chronons", "aethel", or "both")
     * @return The optimization factor achieved (1.0 means no optimization)
     */
    double optimizeResources(const std::string& resourceType = "both");

    /**
     * @brief Register a callback for resource allocation events
     * @param callback The callback function
     * @return The callback ID
     */
    int registerAllocationCallback(ResourceAllocationCallback callback);

    /**
     * @brief Remove a registered callback
     * @param callbackId The ID of the callback to remove
     * @return True if the callback was removed, false otherwise
     */
    bool removeAllocationCallback(int callbackId);

    /**
     * @brief Set the default allocation strategy
     * @param strategy The strategy to use for future allocations
     */
    void setDefaultAllocationStrategy(ResourceAllocationStrategy strategy);

    /**
     * @brief Get the current resource levels
     * @return Pair of (chronons, aethel) values
     */
    std::pair<int, int> getCurrentResourceLevels() const;

    /**
     * @brief Analyze resource usage patterns
     * @return Map of pattern types to their confidence levels (0.0-1.0)
     */
    std::map<std::string, double> analyzeResourcePatterns() const;

    /**
     * @brief Predict future resource needs
     * @param steps Number of steps to predict into the future
     * @return Predicted resource levels for each step
     */
    std::vector<std::pair<int, int>> predictResourceNeeds(int steps);

    /**
     * @brief Get the current chronon resource level
     * @return The current amount of chronons
     */
    double getChronons() const;

    /**
     * @brief Get the current aethel resource level
     * @return The current amount of aethel
     */
    double getAethel() const;

    /**
     * @brief Set the chronon resource level
     * @param chronons The new chronon amount
     */
    void setChronons(double chronons);

    /**
     * @brief Set the aethel resource level
     * @param aethel The new aethel amount
     */
    void setAethel(double aethel);

    /**
     * @brief Get the temporal runtime used by this resource manager
     * @return A shared pointer to the temporal runtime
     */
    std::shared_ptr<TemporalRuntime> getRuntime() const { return m_runtime; }

    /**
     * @brief Get the temporal runtime (alias for getRuntime)
     * @return A shared pointer to the temporal runtime
     */
    std::shared_ptr<TemporalRuntime> getTemporalRuntime() const { return m_runtime; }

    /**
     * @brief Set the resource processor
     * @param processor The resource processor to use
     */
    void setResourceProcessor(std::shared_ptr<ResourceProcessor> processor);

    /**
     * @brief Set the resource optimizer
     * @param optimizer The resource optimizer to use
     */
    void setResourceOptimizer(std::shared_ptr<ResourceOptimizer> optimizer);

    /**
     * @brief Get resource efficiency metrics
     * @return Map of metric names to their values
     */
    std::map<std::string, double> getResourceEfficiencyMetrics() const;

    /**
     * @brief Allocate resources with debt awareness
     * @param operationId The ID of the operation requiring resources
     * @param chrononRequest The requested amount of chronons
     * @param aethelRequest The requested amount of aethel
     * @param maxDebtIncrease The maximum allowed debt increase
     * @return The allocation decision
     */
    ResourceAllocation allocateResourcesWithDebtAwareness(const std::string& operationId,
                                                          int chrononRequest, int aethelRequest,
                                                          double maxDebtIncrease);

private:
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::shared_ptr<ResourceProcessor> m_processor;
    std::shared_ptr<ResourceOptimizer> m_optimizer;
    ResourceAllocationStrategy m_defaultStrategy;
    std::vector<std::pair<int, ResourceAllocationCallback>> m_callbacks;
    std::vector<ResourceAllocation> m_allocationHistory;
    int m_nextCallbackId;

    /**
     * @brief Execute all registered callbacks
     * @param allocation The allocation to pass to callbacks
     */
    void executeCallbacks(const ResourceAllocation& allocation);

    /**
     * @brief Choose the best allocation strategy based on system state
     * @return The best strategy for the current state
     */
    ResourceAllocationStrategy chooseBestStrategy() const;

    /**
     * @brief Update resource metrics based on allocation result
     * @param allocation The allocation that was made
     */
    void updateResourceMetrics(const ResourceAllocation& allocation);

    /**
     * @brief Calculate the overall system efficiency
     * @return The system efficiency (0.0-1.0)
     */
    double calculateSystemEfficiency() const;

    /**
     * @brief Determine if resource levels are critical
     * @return True if resource levels are critical, false otherwise
     */
    bool areResourceLevelsCritical() const;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_RESOURCE_MANAGER_H