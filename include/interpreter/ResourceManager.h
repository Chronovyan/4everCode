#ifndef CHRONOVYAN_RESOURCE_MANAGER_H
#define CHRONOVYAN_RESOURCE_MANAGER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace chronovyan {

// Forward declarations
class TemporalRuntime;
class ResourceOptimizer;
class TemporalDebtTracker;

/**
 * @class ResourceManager
 * @brief Manages resource tracking and allocation for the interpreter
 */
class ResourceManager {
public:
    /**
     * @brief Create a new resource manager
     * @param runtime Reference to the temporal runtime
     * @param optimizer Reference to the resource optimizer
     * @param debtTracker Reference to the temporal debt tracker
     */
    ResourceManager(std::shared_ptr<TemporalRuntime> runtime,
                    std::shared_ptr<ResourceOptimizer> optimizer,
                    std::shared_ptr<TemporalDebtTracker> debtTracker);

    /**
     * @brief Consume aethel resources
     * @param amount The amount to consume
     * @param operation Optional description of the operation
     * @return True if sufficient resources were available
     */
    bool consumeAethel(double amount, const std::string& operation = "");

    /**
     * @brief Consume chronon resources
     * @param amount The amount to consume
     * @param operation Optional description of the operation
     * @return True if sufficient resources were available
     */
    bool consumeChronons(double amount, const std::string& operation = "");

    /**
     * @brief Consume both aethel and chronon resources
     * @param aethelAmount The amount of aethel to consume
     * @param chrononAmount The amount of chronons to consume
     * @param operation Optional description of the operation
     * @return True if sufficient resources were available
     */
    bool consumeResources(double aethelAmount, double chrononAmount,
                          const std::string& operation = "");

    /**
     * @brief Get the current aethel level
     * @return The current aethel level
     */
    double getAethelLevel() const;

    /**
     * @brief Get the current chronon level
     * @return The current chronon level
     */
    double getChrononsLevel() const;

    /**
     * @brief Get the maximum aethel capacity
     * @return The maximum aethel capacity
     */
    double getMaxAethel() const;

    /**
     * @brief Get the maximum chronon capacity
     * @return The maximum chronon capacity
     */
    double getMaxChronons() const;

    /**
     * @brief Replenish aethel resources
     * @param amount The amount to replenish
     */
    void replenishAethel(double amount);

    /**
     * @brief Replenish chronon resources
     * @param amount The amount to replenish
     */
    void replenishChronons(double amount);

    /**
     * @brief Track resource usage for a specific operation
     * @param aethelAmount The amount of aethel used
     * @param chrononAmount The amount of chronons used
     * @param operation The operation description
     */
    void trackResourceUsage(double aethelAmount, double chrononAmount,
                            const std::string& operation);

    /**
     * @brief Get resource usage history
     * @return Map of operation names to resource usage statistics
     */
    std::map<std::string, std::pair<double, double>> getResourceUsageHistory() const;

    /**
     * @brief Calculate the cost of a temporal operation
     * @param operationType The type of operation
     * @param paradoxLevel The current paradox level
     * @param stabilizationFactor The current stabilization factor
     * @return The calculated resource cost
     */
    double calculateTemporalOperationCost(const std::string& operationType, int paradoxLevel,
                                          double stabilizationFactor) const;

private:
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::shared_ptr<ResourceOptimizer> m_optimizer;
    std::shared_ptr<TemporalDebtTracker> m_debtTracker;

    // Resource usage tracking
    struct ResourceUsage {
        double aethel;
        double chronons;
        int count;
    };

    std::map<std::string, ResourceUsage> m_resourceUsageHistory;

    // Resource cost constants
    const std::map<std::string, double> m_baseCosts = {
        {"REWIND_FLOW", 25.0},   {"STABILIZE", 15.0},       {"PREVENT_MODIFICATION", 10.0},
        {"TEMPORAL_LOOP", 30.0}, {"BRANCH_TIMELINE", 20.0}, {"MERGE_TIMELINES", 35.0}};
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_RESOURCE_MANAGER_H