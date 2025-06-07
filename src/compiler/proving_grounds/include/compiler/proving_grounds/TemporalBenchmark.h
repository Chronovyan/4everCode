#ifndef CHRONOVYAN_COMPILER_PROVING_GROUNDS_TEMPORAL_BENCHMARK_H
#define CHRONOVYAN_COMPILER_PROVING_GROUNDS_TEMPORAL_BENCHMARK_H

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace chronovyan {
namespace compiler {
namespace proving_grounds {

/**
 * @class TemporalBenchmark
 * @brief Benchmarks temporal operations in Chronovyan programs.
 *
 * The TemporalBenchmark class provides tools for measuring the performance of
 * temporal operations, including execution time, resource usage, and efficiency.
 */
class TemporalBenchmark {
public:
    /**
     * @brief Constructor
     */
    TemporalBenchmark();

    /**
     * @brief Destructor
     */
    ~TemporalBenchmark();

    /**
     * @brief Start measuring performance
     */
    void startMeasuring();

    /**
     * @brief Stop measuring performance
     */
    void stopMeasuring();

    /**
     * @brief Reset all measurements
     */
    void reset();

    /**
     * @brief Mark the start of a specific operation
     * @param operationName The name of the operation
     */
    void markOperationStart(const std::string& operationName);

    /**
     * @brief Mark the end of a specific operation
     * @param operationName The name of the operation
     */
    void markOperationEnd(const std::string& operationName);

    /**
     * @brief Get the duration of a specific operation in milliseconds
     * @param operationName The name of the operation
     * @return The duration in milliseconds
     */
    double getOperationDuration(const std::string& operationName) const;

    /**
     * @brief Get all operation durations
     * @return A map of operation names to durations in milliseconds
     */
    std::map<std::string, double> getAllOperationDurations() const;

    /**
     * @brief Record resource usage
     * @param resourceName The name of the resource
     * @param usage The amount of resource used
     */
    void recordResourceUsage(const std::string& resourceName, double usage);

    /**
     * @brief Get the usage of a specific resource
     * @param resourceName The name of the resource
     * @return The resource usage
     */
    double getResourceUsage(const std::string& resourceName) const;

    /**
     * @brief Get all resource usages
     * @return A map of resource names to usage values
     */
    std::map<std::string, double> getAllResourceUsages() const;

    /**
     * @brief Calculate the efficiency of a specific operation
     * @param operationName The name of the operation
     * @param resourceName The name of the resource
     * @return The efficiency (output per unit of resource)
     */
    double calculateEfficiency(const std::string& operationName,
                               const std::string& resourceName) const;

    /**
     * @brief Generate a performance report
     * @param format The format of the report (e.g., "text", "json", "html")
     * @return The report as a string
     */
    std::string generateReport(const std::string& format = "text") const;

    /**
     * @brief Save a report to a file
     * @param report The report to save
     * @param filePath The path to save the report to
     * @return True if the report was saved successfully, false otherwise
     */
    bool saveReport(const std::string& report, const std::string& filePath) const;

    /**
     * @brief Benchmark result structure
     */
    struct BenchmarkResult {
        double duration;                              ///< Duration in milliseconds
        std::map<std::string, double> resourceUsage;  ///< Resource usage
        std::map<std::string, double> efficiency;     ///< Efficiency (output per unit of resource)
    };

    /**
     * @brief Get a benchmark result for a specific operation
     * @param operationName The name of the operation
     * @return The benchmark result
     */
    BenchmarkResult getBenchmarkResult(const std::string& operationName) const;

    /**
     * @brief Get all benchmark results
     * @return A map of operation names to benchmark results
     */
    std::map<std::string, BenchmarkResult> getAllBenchmarkResults() const;

private:
    bool isMeasuring;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>>
        operationStartTimes;
    std::map<std::string, double> operationDurations;
    std::map<std::string, double> resourceUsages;

    /**
     * @brief Calculate all efficiencies for an operation
     * @param operationName The name of the operation
     * @return A map of resource names to efficiency values
     */
    std::map<std::string, double> calculateAllEfficiencies(const std::string& operationName) const;
};

}  // namespace proving_grounds
}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_PROVING_GROUNDS_TEMPORAL_BENCHMARK_H