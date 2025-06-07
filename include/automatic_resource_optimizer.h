#ifndef CHRONOVYAN_AUTOMATIC_RESOURCE_OPTIMIZER_H
#define CHRONOVYAN_AUTOMATIC_RESOURCE_OPTIMIZER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "resource_optimizer.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

namespace chronovyan {

/**
 * @struct OptimizationResult
 * @brief Records the results of an automatic optimization
 */
struct OptimizationResult {
    std::string operation_id;      // ID of the optimized operation
    std::string resource_type;     // Type of resource optimized ("chronons" or "aethel")
    double before_value;           // Resource value before optimization
    double after_value;            // Resource value after optimization
    double optimization_factor;    // Optimization factor achieved
    std::string pattern_detected;  // Pattern detected in the operation
    std::string algorithm_used;    // Algorithm used for optimization
    std::chrono::system_clock::time_point timestamp;  // When the optimization occurred

    OptimizationResult()
        : before_value(0)
        , after_value(0)
        , optimization_factor(1.0)
        , timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @struct OptimizationConfig
 * @brief Configuration settings for the automatic optimizer
 */
struct OptimizationConfig {
    bool enable_auto_chronon_optimization = true;  // Enable automatic chronon optimization
    bool enable_auto_aethel_optimization = true;   // Enable automatic aethel optimization
    std::chrono::seconds monitoring_interval{
        60};  // How often to check for optimization opportunities
    double minimum_improvement_threshold =
        0.05;                             // Minimum improvement to apply an optimization (5%)
    int max_optimizations_per_cycle = 3;  // Maximum optimizations to apply in one cycle
    bool optimize_on_resource_bottleneck = true;  // Trigger optimization when resources get low
    double resource_bottleneck_threshold =
        0.2;  // Threshold for resource bottleneck (20% remaining)
    bool learn_from_manual_optimizations = true;  // Learn from manually triggered optimizations
};

/**
 * @class AutomaticResourceOptimizer
 * @brief Extends ResourceOptimizer with automatic optimization capabilities
 *
 * This class continuously monitors resource usage and automatically applies
 * optimizations when beneficial, learning from the results to improve future
 * optimizations.
 */
class AutomaticResourceOptimizer : public ResourceOptimizer {
public:
    /**
     * @brief Create a new automatic resource optimizer
     * @param runtime The temporal runtime to optimize resources for
     * @param debt_tracker Optional debt tracker for debt-aware optimization
     * @param config Initial configuration for automatic optimization
     */
    explicit AutomaticResourceOptimizer(std::shared_ptr<TemporalRuntime> runtime,
                                        std::shared_ptr<TemporalDebtTracker> debt_tracker = nullptr,
                                        const OptimizationConfig& config = OptimizationConfig());

    /**
     * @brief Destructor that ensures monitoring thread is stopped
     */
    virtual ~AutomaticResourceOptimizer();

    /**
     * @brief Start automatic optimization in a background thread
     * @return True if monitoring was started successfully
     */
    bool startAutomaticOptimization();

    /**
     * @brief Stop automatic optimization
     */
    void stopAutomaticOptimization();

    /**
     * @brief Check if automatic optimization is currently active
     * @return True if automatic optimization is active
     */
    bool isAutomaticOptimizationActive() const;

    /**
     * @brief Get the current optimization configuration
     * @return The current configuration
     */
    OptimizationConfig getConfig() const;

    /**
     * @brief Set a new optimization configuration
     * @param config The new configuration to use
     */
    void setConfig(const OptimizationConfig& config);

    /**
     * @brief Get the history of automatic optimizations performed
     * @return Vector of optimization results
     */
    std::vector<OptimizationResult> getOptimizationHistory() const;

    /**
     * @brief Generate a report of automatic optimizations performed
     * @param detailed Whether to include detailed information for each optimization
     * @return String containing the report
     */
    std::string generateOptimizationReport(bool detailed = false) const;

    /**
     * @brief Register a callback to be notified when automatic optimizations occur
     * @param callback Function to call when an optimization is performed
     * @return ID that can be used to unregister the callback
     */
    int registerOptimizationCallback(std::function<void(const OptimizationResult&)> callback);

    /**
     * @brief Unregister a previously registered callback
     * @param callback_id The ID returned from registerOptimizationCallback
     * @return True if the callback was found and unregistered
     */
    bool unregisterOptimizationCallback(int callback_id);

    /**
     * @brief Prioritize specific operations for automatic optimization
     * @param operation_ids Set of operation IDs to prioritize
     */
    void setPriorityOperations(const std::set<std::string>& operation_ids);

    /**
     * @brief Exclude specific operations from automatic optimization
     * @param operation_ids Set of operation IDs to exclude
     */
    void setExcludedOperations(const std::set<std::string>& operation_ids);

    /**
     * @brief Identify optimization opportunities without applying them
     * @return Map of operation IDs to potential optimization factors
     */
    std::map<std::string, double> identifyOptimizationOpportunities();

    /**
     * @brief Manually trigger a full optimization cycle
     * @return Number of optimizations performed
     */
    int performOptimizationCycle();

    /**
     * @brief Override from ResourceOptimizer to track manual optimizations
     * @param operationId The ID of the operation to optimize
     * @return The optimization factor achieved
     */
    virtual double optimizeChronons(const std::string& operationId) override;

    /**
     * @brief Override from ResourceOptimizer to track manual optimizations
     * @param operationId The ID of the operation to optimize
     * @param operationPattern The pattern of the operation
     * @return The optimization factor achieved
     */
    virtual double optimizeChrononsAdvanced(const std::string& operationId,
                                            const std::string& operationPattern = "") override;

    /**
     * @brief Override from ResourceOptimizer to track manual optimizations
     * @param timelineId The ID of the timeline to optimize
     * @return The optimization factor achieved
     */
    virtual double optimizeAethel(const std::string& timelineId) override;

    /**
     * @brief Override from ResourceOptimizer to track manual optimizations
     * @param timelineId The ID of the timeline to optimize
     * @param operationPattern The pattern of the operation
     * @return The optimization factor achieved
     */
    virtual double optimizeAethelAdvanced(const std::string& timelineId,
                                          const std::string& operationPattern = "") override;

private:
    // Configuration
    OptimizationConfig m_config;

    // Thread management
    std::atomic<bool> m_optimizationActive;
    std::thread m_optimizationThread;
    std::mutex m_optimizationMutex;
    std::condition_variable m_optimizationCV;

    // Optimization history and tracking
    std::vector<OptimizationResult> m_optimizationHistory;
    std::mutex m_historyMutex;

    // Operation filtering
    std::set<std::string> m_priorityOperations;
    std::set<std::string> m_excludedOperations;

    // Callback management
    int m_nextCallbackId;
    std::map<int, std::function<void(const OptimizationResult&)>> m_optimizationCallbacks;
    std::mutex m_callbackMutex;

    // Private methods
    void optimizationThreadFunction();
    int performOptimizationCycleInternal();
    void recordOptimizationResult(const OptimizationResult& result);
    void notifyOptimizationCallbacks(const OptimizationResult& result);
    bool shouldOptimizeOperation(const std::string& operation_id) const;
    bool isResourceBottleneckReached() const;
    std::string detectOperationPattern(const std::string& operation_id) const;
    void learnFromOptimizationResult(const OptimizationResult& result);
    double calculateImprovementPercentage(double before, double after) const;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_AUTOMATIC_RESOURCE_OPTIMIZER_H