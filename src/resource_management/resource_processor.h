#ifndef CHRONOVYAN_RESOURCE_PROCESSOR_H
#define CHRONOVYAN_RESOURCE_PROCESSOR_H

#include <chrono>
#include <future>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "resource_management/resource_tracker.h"
#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"

namespace chronovyan {

/**
 * @brief The ResourceProcessor class manages resource tracking, analysis, and
 * visualization.
 *
 * This class combines resource tracking and visualization capabilities to
 * provide a comprehensive resource management system. It can process, analyze,
 * and report on resource usage in the Chronovyan system.
 */
class ResourceProcessor {
public:
  /**
   * @brief Construct a new Resource Processor
   *
   * @param maxHistorySize Maximum number of data points to keep in history
   */
  ResourceProcessor(size_t maxHistorySize = 100);

  /**
   * @brief Process and record current resource usage
   *
   * @param chrononUsage Current chronon energy usage
   * @param aethelUsage Current aethel flow usage
   * @param temporalDebt Current temporal debt
   * @param paradoxRisk Current paradox risk assessment
   * @return true if the resource state is within safe parameters
   * @return false if resource usage exceeds safe thresholds
   */
  bool processCurrentUsage(double chrononUsage, double aethelUsage,
                           double temporalDebt, double paradoxRisk);

  /**
   * @brief Get a textual summary of current resource state
   *
   * @return std::string A formatted summary of resource usage
   */
  std::string getSummary() const;

  /**
   * @brief Get a detailed report of resource usage and statistics
   *
   * @return std::string A comprehensive report on resource state
   */
  std::string getDetailedReport() const;

  /**
   * @brief Get an ASCII visualization of resource usage
   *
   * @return std::string An ASCII representation of resource usage
   */
  std::string getVisualization() const;

  /**
   * @brief Get access to the underlying resource tracker
   *
   * @return const ResourceTracker& A reference to the resource tracker
   */
  const ResourceTracker &getTracker() const;

  /**
   * @brief Reset all tracked data and statistics
   */
  void reset();

  /**
   * @brief Check if resource usage is within safe parameters
   *
   * @param timeout_ms Maximum time in milliseconds to wait for the operation (0
   * = no timeout)
   * @return true if resources are within safe limits
   * @return false if resources exceed safety thresholds or if operation times
   * out
   */
  bool isResourceStateHealthy(unsigned int timeout_ms = 1000) const;

  /**
   * @brief Save current resource data to a log file
   *
   * @param filename Path to the log file
   * @return true if the data was successfully saved
   * @return false if there was an error saving the data
   */
  bool saveToLog(const std::string &filename) const;

  /**
   * @brief Load resource data from a log file
   *
   * @param filename Path to the log file
   * @return true if the data was successfully loaded
   * @return false if there was an error loading the data
   */
  bool loadFromLog(const std::string &filename);

  /**
   * @brief Get optimization recommendations for resource usage
   *
   * @return std::vector<OptimizationRecommendation> List of recommendations
   */
  std::vector<OptimizationRecommendation>
  getOptimizationRecommendations() const;

  /**
   * @brief Get efficiency metrics for resource usage
   *
   * @return std::map<std::string, double> Map of efficiency metrics
   */
  std::map<std::string, double> getEfficiencyMetrics() const;

  /**
   * @brief Get a detailed optimization report
   *
   * @return std::string Formatted optimization report
   */
  std::string getOptimizationReport() const;

  /**
   * @brief Set minimum data points required for optimization analysis
   *
   * @param min_data_points Minimum number of data points
   */
  void setMinimumDataPointsForOptimization(size_t min_data_points);

  /**
   * @brief Process chronon consumption with potential debt management
   * @param amount The amount of chronons to consume
   * @param operation_id The operation requiring the chronons
   * @param allow_debt Whether to allow borrowing if insufficient resources
   * @param critical Whether the operation is critical (affects debt parameters)
   * @return True if the chronons were successfully consumed or borrowed
   */
  bool processChronons(double amount, const std::string &operation_id,
                       bool allow_debt = false, bool critical = false);

  /**
   * @brief Process aethel consumption with potential debt management
   * @param amount The amount of aethel to consume
   * @param operation_id The operation requiring the aethel
   * @param allow_debt Whether to allow borrowing if insufficient resources
   * @param critical Whether the operation is critical (affects debt parameters)
   * @return True if the aethel was successfully consumed or borrowed
   */
  bool processAethel(double amount, const std::string &operation_id,
                     bool allow_debt = false, bool critical = false);

  /**
   * @brief Repay chronon debt using available resources
   * @param amount The amount to repay
   * @param operation_id The specific operation to repay (empty for oldest debt)
   * @return The amount actually repaid
   */
  double repayChrononsDebt(double amount, const std::string &operation_id = "");

  /**
   * @brief Repay aethel debt using available resources
   * @param amount The amount to repay
   * @param operation_id The specific operation to repay (empty for oldest debt)
   * @return The amount actually repaid
   */
  double repayAethelDebt(double amount, const std::string &operation_id = "");

  /**
   * @brief Advance the debt cycle, applying interest and checking for due debts
   * @return The number of paradoxes created by overdue critical debts
   */
  int advanceDebtCycle();

  /**
   * @brief Get a comprehensive debt status report
   * @return A map containing debt status information
   */
  std::map<std::string, double> getDebtStatus() const;

  /**
   * @brief Get detailed information about all current debts
   * @return A vector of maps, each containing information about a debt
   */
  std::vector<
      std::map<std::string, std::variant<double, std::string, int, bool>>>
  getDetailedDebtInfo() const;

  /**
   * @brief Generate a textual summary of the current resource usage
   * @return A string containing the resource summary
   */
  std::string generateResourceSummary() const;

  /**
   * @brief Generate a detailed report of resource usage and debt status
   * @return A string containing the detailed report
   */
  std::string generateDetailedReport() const;

  /**
   * @brief Generate an ASCII visualization of resource usage over time
   * @return A string containing the ASCII visualization
   */
  std::string generateResourceVisualization() const;

  /**
   * @brief Save resource usage data to a file
   * @param filename The name of the file to save to
   * @return True if the save was successful
   */
  bool saveData(const std::string &filename) const;

  /**
   * @brief Load resource usage data from a file
   * @param filename The name of the file to load from
   * @return True if the load was successful
   */
  bool loadData(const std::string &filename);

private:
  std::shared_ptr<ResourceTracker> tracker_;
  std::unique_ptr<chronovyan::ResourceVisualization> visualizer_;
  std::unique_ptr<ResourceOptimizer> optimizer_;
  std::shared_ptr<TemporalDebtTracker> debt_tracker_;

  // Safe thresholds for resource parameters
  double chronon_threshold_;
  double aethel_threshold_;
  double temporal_debt_threshold_;
  double paradox_risk_threshold_;

  // Check if resource parameters exceed safe thresholds
  bool checkResourceThresholds(double chrononUsage, double aethelUsage,
                               double temporalDebt, double paradoxRisk) const;

  /**
   * @brief Update all components with the latest resource data
   */
  void updateComponents();
};

} // namespace chronovyan

#endif // CHRONOVYAN_RESOURCE_PROCESSOR_H