#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "resource_manager.h"
#include "temporal_debt_tracker.h"

namespace chronovyan {

/**
 * @class TemporalDebtBenchmark
 * @brief Utility for benchmarking and comparing different temporal debt repayment strategies
 *
 * This class provides tools to evaluate the effectiveness of different debt repayment
 * strategies under various scenarios and resource constraints.
 */
class TemporalDebtBenchmark {
public:
    /**
     * @struct BenchmarkResult
     * @brief Contains the results of a benchmark run for a specific strategy
     */
    struct BenchmarkResult {
        RepaymentStrategyType strategy;           ///< The strategy that was tested
        double initialStability;                  ///< Stability before applying the strategy
        double finalStability;                    ///< Stability after applying the strategy
        double stabilityImprovement;              ///< Net stability improvement
        double resourcesUsed;                     ///< Total resources consumed
        double stabilityPerResource;              ///< Efficiency ratio (stability/resource)
        std::chrono::microseconds executionTime;  ///< Time taken to execute the strategy
        int debtsFullyPaid;                       ///< Number of debts completely paid off
        int debtsPartiallyPaid;                   ///< Number of debts partially paid
        int debtsUnpaid;                          ///< Number of debts not paid at all
    };

    /**
     * @brief Constructor
     * @param resourceManager A shared pointer to a ResourceManager instance
     */
    TemporalDebtBenchmark(std::shared_ptr<ResourceManager> resourceManager);

    /**
     * @brief Destructor
     */
    ~TemporalDebtBenchmark() = default;

    /**
     * @brief Set up a standard test scenario with a predefined set of debts
     * @param scenario The name of the scenario to set up
     * @return True if the scenario was successfully set up, false otherwise
     */
    bool setupStandardScenario(const std::string& scenario);

    /**
     * @brief Add a custom debt to the benchmark
     * @param debt The temporal debt to add
     */
    void addDebt(const TemporalDebt& debt);

    /**
     * @brief Clear all debts from the benchmark
     */
    void clearDebts();

    /**
     * @brief Set available resources for the benchmark
     * @param chronons The amount of chronon resources
     * @param aethel The amount of aethel resources
     */
    void setAvailableResources(double chronons, double aethel);

    /**
     * @brief Run a benchmark for a specific strategy
     * @param strategy The repayment strategy to test
     * @return A BenchmarkResult containing the performance metrics
     */
    BenchmarkResult runStrategyBenchmark(RepaymentStrategyType strategy);

    /**
     * @brief Run benchmarks for all available strategies
     * @return A vector of BenchmarkResult, one for each strategy
     */
    std::vector<BenchmarkResult> runAllStrategies();

    /**
     * @brief Compare two strategies and determine which is more effective
     * @param strategy1 The first strategy to compare
     * @param strategy2 The second strategy to compare
     * @return A positive value if strategy1 is better, negative if strategy2 is better,
     *         or zero if they are equivalent
     */
    double compareStrategies(RepaymentStrategyType strategy1, RepaymentStrategyType strategy2);

    /**
     * @brief Find the optimal strategy for the current debt and resource situation
     * @return The RepaymentStrategyType that provides the best outcome
     */
    RepaymentStrategyType findOptimalStrategy();

    /**
     * @brief Get a string representation of the benchmark results
     * @param result The benchmark result to format
     * @return A formatted string with the benchmark results
     */
    std::string formatBenchmarkResult(const BenchmarkResult& result);

    /**
     * @brief Generate a comprehensive report of all strategy benchmarks
     * @param results Vector of benchmark results to include in the report
     * @return A formatted string with the complete benchmark report
     */
    std::string generateBenchmarkReport(const std::vector<BenchmarkResult>& results);

private:
    std::shared_ptr<ResourceManager> m_resourceManager;
    std::shared_ptr<TemporalDebtTracker> m_debtTracker;

    // Standard test scenarios
    void setupBalancedScenario();
    void setupCriticalHeavyScenario();
    void setupHighInterestScenario();
    void setupMixedAgeScenario();
    void setupResourceLimitedScenario();
};

}  // namespace chronovyan