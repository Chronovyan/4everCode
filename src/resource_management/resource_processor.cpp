#include <algorithm>
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include "resource_processor.h"

namespace chronovyan {

// Utility functions
namespace utils {

/**
 * @brief Safely serialize a time point to a file
 * @param file The output file stream
 * @param timePoint The time point to serialize
 * @return true if successful, false otherwise
 */
bool serializeTimePoint(
    std::ofstream &file,
    const std::chrono::time_point<std::chrono::system_clock> &timePoint) {
  // Convert time_point to a simple representation (time since epoch in
  // microseconds)
  auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
                    timePoint.time_since_epoch())
                    .count();
  file.write(reinterpret_cast<const char *>(&micros), sizeof(micros));
  return file.good();
}

/**
 * @brief Safely deserialize a time point from a file
 * @param file The input file stream
 * @param timePoint The time point to deserialize into
 * @return true if successful, false otherwise
 */
bool deserializeTimePoint(
    std::ifstream &file,
    std::chrono::time_point<std::chrono::system_clock> &timePoint) {
  // Read the simple representation
  int64_t micros;
  file.read(reinterpret_cast<char *>(&micros), sizeof(micros));
  if (!file.good()) {
    return false;
  }

  // Convert back to time_point
  timePoint =
      std::chrono::system_clock::time_point(std::chrono::microseconds(micros));
  return true;
}

/**
 * @brief Safely serialize a ResourceDataPoint to a file
 * @param file The output file stream
 * @param point The data point to serialize
 * @return true if successful, false otherwise
 */
bool serializeResourceDataPoint(std::ofstream &file,
                                const ResourceDataPoint &point) {
  // Serialize each field individually for safety
  if (!serializeTimePoint(file, point.timestamp)) {
    return false;
  }

  file.write(reinterpret_cast<const char *>(&point.chronon_usage),
             sizeof(point.chronon_usage));
  if (!file.good())
    return false;

  file.write(reinterpret_cast<const char *>(&point.aethel_usage),
             sizeof(point.aethel_usage));
  if (!file.good())
    return false;

  file.write(reinterpret_cast<const char *>(&point.temporal_debt),
             sizeof(point.temporal_debt));
  if (!file.good())
    return false;

  file.write(reinterpret_cast<const char *>(&point.paradox_risk),
             sizeof(point.paradox_risk));
  if (!file.good())
    return false;

  return true;
}

/**
 * @brief Safely deserialize a ResourceDataPoint from a file
 * @param file The input file stream
 * @param point The data point to deserialize into
 * @return true if successful, false otherwise
 */
bool deserializeResourceDataPoint(std::ifstream &file,
                                  ResourceDataPoint &point) {
  // Deserialize each field individually for safety
  if (!deserializeTimePoint(file, point.timestamp)) {
    return false;
  }

  file.read(reinterpret_cast<char *>(&point.chronon_usage),
            sizeof(point.chronon_usage));
  if (!file.good())
    return false;

  file.read(reinterpret_cast<char *>(&point.aethel_usage),
            sizeof(point.aethel_usage));
  if (!file.good())
    return false;

  file.read(reinterpret_cast<char *>(&point.temporal_debt),
            sizeof(point.temporal_debt));
  if (!file.good())
    return false;

  file.read(reinterpret_cast<char *>(&point.paradox_risk),
            sizeof(point.paradox_risk));
  if (!file.good())
    return false;

  return true;
}

} // namespace utils

ResourceProcessor::ResourceProcessor(size_t maxHistorySize)
    : tracker_(std::make_shared<ResourceTracker>(maxHistorySize)) {
  // Create the optimizer with the tracker (ResourceOptimizer expects a const
  // ResourceTracker&)
  optimizer_ = std::make_unique<ResourceOptimizer>(*tracker_);

  // Initialize the debt tracker with a temporal runtime
  auto runtime = std::make_shared<TemporalRuntime>();
  debt_tracker_ = std::make_shared<TemporalDebtTracker>(runtime);

  // Correctly instantiate ResourceVisualization
  // It needs ResourceTracker, TemporalRuntime, and TemporalDebtTracker
  visualizer_ = std::make_unique<chronovyan::ResourceVisualization>(
      tracker_, runtime, debt_tracker_);

  // Initialize the safe thresholds with default values
  chronon_threshold_ = 80.0;
  aethel_threshold_ = 80.0;
  temporal_debt_threshold_ = 50.0;
  paradox_risk_threshold_ = 0.3;
}

bool ResourceProcessor::processCurrentUsage(double chrononUsage,
                                            double aethelUsage,
                                            double temporalDebt,
                                            double paradoxRisk) {
  // Record the current usage
  tracker_->recordCurrentUsage(chrononUsage, aethelUsage, temporalDebt,
                               paradoxRisk);

  // Check if within safe thresholds
  return !checkResourceThresholds(chrononUsage, aethelUsage, temporalDebt,
                                  paradoxRisk);
}

std::string ResourceProcessor::getSummary() const {
  return visualizer_->generateSummaryReport(
      chronovyan::VisualizationFormat::TEXT);
}

std::string ResourceProcessor::getDetailedReport() const {
  return visualizer_->generateDetailedReport(
      chronovyan::VisualizationFormat::TEXT,
      20); // Default history length
}

std::string ResourceProcessor::getVisualization() const {
  // Use updateDashboard method instead, which produces output containing
  // "Chronovyan Resource Visualization"
  return visualizer_->updateDashboard(chronovyan::VisualizationFormat::TEXT);
}

const ResourceTracker &ResourceProcessor::getTracker() const {
  return *tracker_;
}

void ResourceProcessor::reset() { tracker_->reset(); }

bool ResourceProcessor::isResourceStateHealthy(unsigned int timeout_ms) const {
  if (timeout_ms == 0) {
    // No timeout, execute directly
    // Get statistics
    auto stats = tracker_->getResourceStatistics();

    // Calculate debt and risk factors
    double debt = debt_tracker_->getTotalChrononsDebt() +
                  debt_tracker_->getTotalAethelDebt();
    double risk = 1.0 - debt_tracker_->getStability();

    // Check all thresholds
    return !checkResourceThresholds(stats["current_chronon_usage"],
                                    stats["current_aethel_usage"], debt, risk);
  } else {
    // Use a future with timeout
    std::packaged_task<bool()> task([this]() {
      // Get statistics
      auto stats = tracker_->getResourceStatistics();

      // Calculate debt and risk factors
      double debt = debt_tracker_->getTotalChrononsDebt() +
                    debt_tracker_->getTotalAethelDebt();
      double risk = 1.0 - debt_tracker_->getStability();

      // Check all thresholds
      return !checkResourceThresholds(stats["current_chronon_usage"],
                                      stats["current_aethel_usage"], debt,
                                      risk);
    });

    // Get future from the task
    std::future<bool> future = task.get_future();

    // Launch the task in a separate thread
    std::thread thread(std::move(task));

    // Join the thread to ensure it completes before this function returns
    // and before the ResourceProcessor object might be destroyed.
    // thread.detach(); // Avoid detaching

    try {
      if (future.wait_for(std::chrono::milliseconds(timeout_ms)) ==
          std::future_status::timeout) {
        if (thread.joinable()) {
          thread.join(); // Ensure thread is joined even on timeout before
                         // returning
        }
        // Log timeout?
        return false; // Timed out
      }
      if (thread.joinable()) {
        thread.join(); // Join after successful completion
      }
      return future.get(); // Get result or rethrow exception
    } catch (const std::exception &e) {
      if (thread.joinable()) {
        thread.join(); // Ensure thread is joined on exception before returning
      }
      // Log exception?
      return false; // Exception in task
    }
  }
}

bool ResourceProcessor::saveToLog(const std::string &filename) const {
  try {
    // Check if we have data to save
    const auto &data = tracker_->getHistoricalData();
    if (data.empty()) {
      std::cerr << "Warning: No data available to save" << std::endl;
      // Return true as this isn't a technical failure, just a logical one
      return true;
    }

    // Open the file for writing
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error: Could not open file " << filename << " for writing"
                << std::endl;
      return false;
    }

    // Save resource tracker data
    size_t size = data.size();
    file.write(reinterpret_cast<const char *>(&size), sizeof(size));

    if (!file) {
      std::cerr << "Error: Failed to write data size to file" << std::endl;
      return false;
    }

    // Write each data point
    for (size_t i = 0; i < size; ++i) {
      if (!utils::serializeResourceDataPoint(file, data[i])) {
        std::cerr << "Error: Failed to write data point " << i << " to file"
                  << std::endl;
        return false;
      }
    }

    // Ensure all data is flushed to disk
    file.flush();

    if (!file) {
      std::cerr << "Error: Failed to flush data to file" << std::endl;
      return false;
    }

    return true;
  } catch (const std::exception &e) {
    std::cerr << "Error saving data: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error occurred while saving data" << std::endl;
    return false;
  }
}

bool ResourceProcessor::loadFromLog(const std::string &filename) {
  try {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
      std::cerr << "Error: Could not open file " << filename << std::endl;
      return false;
    }

    // Check file size
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    if (fileSize < static_cast<std::streamsize>(sizeof(size_t))) {
      std::cerr << "Error: File is too small to contain valid data"
                << std::endl;
      return false;
    }

    // Reset the tracker
    tracker_->reset();

    // Load resource tracker data
    size_t size;
    file.read(reinterpret_cast<char *>(&size), sizeof(size));

    // Sanity check for reasonable size
    if (size > 1000000) { // Arbitrary large number as safety limit
      std::cerr << "Error: Unreasonable data size in file: " << size
                << std::endl;
      return false;
    }

    // Estimate expected file size (approximate, since we're using safer
    // serialization) This is a rough estimate based on the size of each field
    const size_t estimatedPointSize = sizeof(int64_t) + 4 * sizeof(double);
    std::streamsize estimatedMinimumSize =
        sizeof(size_t) + (estimatedPointSize * size);
    if (fileSize < estimatedMinimumSize) {
      std::cerr << "Error: File size is smaller than expected for " << size
                << " records" << std::endl;
      return false;
    }

    for (size_t i = 0; i < size; ++i) {
      ResourceDataPoint point;

      if (!utils::deserializeResourceDataPoint(file, point)) {
        std::cerr << "Error: Failed to read data point " << i << std::endl;
        return false;
      }

      tracker_->recordCurrentUsage(point.chronon_usage, point.aethel_usage,
                                   point.temporal_debt, point.paradox_risk);
    }

    return true;
  } catch (const std::exception &e) {
    std::cerr << "Error loading data: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cerr << "Unknown error occurred while loading data" << std::endl;
    return false;
  }
}

std::vector<OptimizationRecommendation>
ResourceProcessor::getOptimizationRecommendations() const {
  // This method should return a list of optimization recommendations
  std::vector<OptimizationRecommendation> result;

  // Get efficiency metrics
  auto metrics = getEfficiencyMetrics();

  // Create recommendations based on metrics
  if (metrics["chronon_efficiency"] < 0.5) {
    result.push_back(OptimizationRecommendation(
        OptimizationRecommendation::Type::EFFICIENCY_IMPROVEMENT,
        "Consider optimizing chronon usage for better efficiency", 0.25, 0.8));
  } else {
    result.push_back(OptimizationRecommendation(
        OptimizationRecommendation::Type::RESOURCE_ALLOCATION,
        "Chronon usage is efficient", 0.0, 0.9));
  }

  if (metrics["aethel_efficiency"] < 0.5) {
    result.push_back(OptimizationRecommendation(
        OptimizationRecommendation::Type::EFFICIENCY_IMPROVEMENT,
        "Consider optimizing aethel usage for better efficiency", 0.3, 0.7));
  } else {
    result.push_back(OptimizationRecommendation(
        OptimizationRecommendation::Type::RESOURCE_ALLOCATION,
        "Aethel usage is efficient", 0.0, 0.9));
  }

  return result;
}

std::map<std::string, double> ResourceProcessor::getEfficiencyMetrics() const {
  std::map<std::string, double> metrics;

  // Get resource statistics
  auto stats = tracker_->getResourceStatistics();

  // Calculate efficiency metrics
  metrics["chronon_efficiency"] =
      1.0 - (stats["current_chronon_usage"] / 100.0);
  metrics["aethel_efficiency"] = 1.0 - (stats["current_aethel_usage"] / 100.0);
  metrics["temporal_stability"] = 1.0 - (stats["current_paradox_risk"]);

  return metrics;
}

std::string ResourceProcessor::getOptimizationReport() const {
  std::stringstream ss;

  // Add efficiency metrics
  auto metrics = getEfficiencyMetrics();
  ss << "===== Efficiency Metrics =====\n";
  for (const auto &[key, value] : metrics) {
    ss << key << ": " << value << "\n";
  }

  // Add recommendations
  auto recommendations = getOptimizationRecommendations();
  ss << "\n===== Optimization Recommendations =====\n";
  for (const auto &rec : recommendations) {
    ss << rec.description << " (Confidence: " << (rec.confidence * 100.0)
       << "%)\n";
  }

  return ss.str();
}

void ResourceProcessor::setMinimumDataPointsForOptimization(
    size_t min_data_points) {
  // This is a placeholder for future functionality
  // Mark as intentionally unused
  (void)min_data_points;
}

bool ResourceProcessor::processChronons(double amount,
                                        const std::string &operation_id,
                                        bool allow_debt, bool critical) {
  // Get current resource statistics
  auto stats = tracker_->getResourceStatistics();
  double available = stats["current_chronon_usage"];

  // First try using available chronons
  if (available >= amount) {
    // Record the usage
    tracker_->recordCurrentUsage(
        available - amount, stats["current_aethel_usage"],
        stats["current_temporal_debt"], stats["current_paradox_risk"]);
    return true;
  }

  // If debt is not allowed, fail
  if (!allow_debt) {
    return false;
  }

  // Otherwise, borrow chronons
  bool success = debt_tracker_->borrowChronons(amount, operation_id, critical);
  if (success) {
    // Record the change in resource state
    tracker_->recordCurrentUsage(
        available + amount -
            amount, // net effect is 0, but we record the transaction
        stats["current_aethel_usage"], stats["current_temporal_debt"] + amount,
        stats["current_paradox_risk"]);
  }

  return success;
}

bool ResourceProcessor::processAethel(double amount,
                                      const std::string &operation_id,
                                      bool allow_debt, bool critical) {
  // Get current resource statistics
  auto stats = tracker_->getResourceStatistics();
  double available = stats["current_aethel_usage"];

  // First try using available aethel
  if (available >= amount) {
    // Record the usage
    tracker_->recordCurrentUsage(
        stats["current_chronon_usage"], available - amount,
        stats["current_temporal_debt"], stats["current_paradox_risk"]);
    return true;
  }

  // If debt is not allowed, fail
  if (!allow_debt) {
    return false;
  }

  // Otherwise, borrow aethel
  bool success = debt_tracker_->borrowAethel(amount, operation_id, critical);
  if (success) {
    // Record the change in resource state
    tracker_->recordCurrentUsage(
        stats["current_chronon_usage"],
        available + amount -
            amount, // net effect is 0, but we record the transaction
        stats["current_temporal_debt"], stats["current_paradox_risk"]);
  }

  return success;
}

double ResourceProcessor::repayChrononsDebt(double amount,
                                            const std::string &operation_id) {
  // Get current resource statistics
  auto stats = tracker_->getResourceStatistics();
  double available = stats["current_chronon_usage"];
  double to_repay = std::min(amount, available);

  if (to_repay <= 0) {
    return 0.0;
  }

  // Repay the debt
  double repaid = debt_tracker_->repayChronons(to_repay, operation_id);

  // Record the change in resource state
  if (repaid > 0) {
    tracker_->recordCurrentUsage(
        available - repaid, stats["current_aethel_usage"],
        stats["current_temporal_debt"] - repaid, stats["current_paradox_risk"]);
  }

  return repaid;
}

double ResourceProcessor::repayAethelDebt(double amount,
                                          const std::string &operation_id) {
  // Get current resource statistics
  auto stats = tracker_->getResourceStatistics();
  double available = stats["current_aethel_usage"];
  double to_repay = std::min(amount, available);

  if (to_repay <= 0) {
    return 0.0;
  }

  // Repay the debt
  double repaid = debt_tracker_->repayAethel(to_repay, operation_id);

  // Record the change in resource state
  if (repaid > 0) {
    tracker_->recordCurrentUsage(
        stats["current_chronon_usage"], available - repaid,
        stats["current_temporal_debt"] - repaid, stats["current_paradox_risk"]);
  }

  return repaid;
}

int ResourceProcessor::advanceDebtCycle() {
  // Advance the debt cycle and get the number of paradoxes created
  int paradoxes = debt_tracker_->advanceCycle();

  // Update resource state to reflect the advancement
  auto stats = tracker_->getResourceStatistics();
  tracker_->recordCurrentUsage(
      stats["current_chronon_usage"], stats["current_aethel_usage"],
      stats["current_temporal_debt"],
      stats["current_paradox_risk"] + (paradoxes * 0.1) // Increase paradox risk
  );

  return paradoxes;
}

std::map<std::string, double> ResourceProcessor::getDebtStatus() const {
  std::map<std::string, double> status;

  status["chronons_debt"] = debt_tracker_->getTotalChrononsDebt();
  status["aethel_debt"] = debt_tracker_->getTotalAethelDebt();
  status["debt_limit"] = debt_tracker_->getDebtLimit();
  status["stability"] = debt_tracker_->getStability();
  status["past_due_count"] =
      static_cast<double>(debt_tracker_->getPastDueDebts().size());

  return status;
}

std::vector<std::map<std::string, std::variant<double, std::string, int, bool>>>
ResourceProcessor::getDetailedDebtInfo() const {
  std::vector<
      std::map<std::string, std::variant<double, std::string, int, bool>>>
      result;

  // Get all debts from the tracker
  auto debts = debt_tracker_->getAllDebts();

  // Convert each debt record to a map
  for (const auto &debt : debts) {
    std::map<std::string, std::variant<double, std::string, int, bool>>
        debt_map;

    debt_map["operation_id"] = debt.operation_id;
    debt_map["chronon_debt"] = debt.chronon_debt;
    debt_map["aethel_debt"] = debt.aethel_debt;
    debt_map["interest_rate"] = debt.interest_rate;
    debt_map["due_cycle"] = debt.due_cycle;
    debt_map["critical"] = debt.critical;

    result.push_back(debt_map);
  }

  return result;
}

std::string ResourceProcessor::generateResourceSummary() const {
  std::stringstream ss;

  // Get the basic resource summary from the visualizer
  ss << visualizer_->generateSummaryReport(
      chronovyan::VisualizationFormat::TEXT);

  // Add debt information
  auto status = getDebtStatus();

  ss << "\n===== Debt Status =====\n";
  ss << "Chronons Debt: " << status["chronons_debt"] << "\n";
  ss << "Aethel Debt: " << status["aethel_debt"] << "\n";
  ss << "System Stability: " << (status["stability"] * 100.0) << "%\n";

  // Add optimization recommendations
  auto recommendations = getOptimizationRecommendations();

  ss << "\n===== Optimization Recommendations =====\n";
  for (const auto &rec : recommendations) {
    ss << rec.description
       << " (Improvement: " << (rec.estimated_improvement * 100.0) << "%)\n";
  }

  return ss.str();
}

std::string ResourceProcessor::generateDetailedReport() const {
  std::stringstream ss;

  // Get detailed resource report from the visualizer
  ss << visualizer_->generateDetailedReport(
      chronovyan::VisualizationFormat::TEXT,
      20); // Default history length

  // Add debt information
  auto status = getDebtStatus();
  auto debts = getDetailedDebtInfo();

  ss << "\n===== Detailed Debt Report =====\n";
  ss << "Total Chronons Debt: " << status["chronons_debt"] << "\n";
  ss << "Total Aethel Debt: " << status["aethel_debt"] << "\n";
  ss << "Debt Limit: " << status["debt_limit"] << "\n";
  ss << "System Stability: " << (status["stability"] * 100.0) << "%\n";
  ss << "Past Due Debts: " << status["past_due_count"] << "\n";

  if (!debts.empty()) {
    ss << "\nIndividual Debts:\n";
    for (size_t i = 0; i < debts.size(); ++i) {
      const auto &debt = debts[i];
      ss << "Debt #" << i << ":\n";
      ss << "  Operation: " << std::get<std::string>(debt.at("operation_id"))
         << "\n";
      ss << "  Chronon Debt: " << std::get<double>(debt.at("chronon_debt"))
         << "\n";
      ss << "  Aethel Debt: " << std::get<double>(debt.at("aethel_debt"))
         << "\n";
      ss << "  Interest Rate: "
         << (std::get<double>(debt.at("interest_rate")) * 100.0) << "%\n";
      ss << "  Due Cycle: " << std::get<int>(debt.at("due_cycle")) << "\n";
      ss << "  Critical: "
         << (std::get<bool>(debt.at("critical")) ? "Yes" : "No") << "\n";
    }
  }

  // Add optimization metrics and recommendations
  auto metrics = getEfficiencyMetrics();

  ss << "\n===== Efficiency Metrics =====\n";
  for (const auto &[key, value] : metrics) {
    ss << key << ": " << value << "\n";
  }

  ss << "\n===== Optimization Recommendations =====\n";
  auto recommendations = getOptimizationRecommendations();
  for (const auto &rec : recommendations) {
    ss << rec.description << " (Confidence: " << (rec.confidence * 100.0)
       << "%, Improvement: " << (rec.estimated_improvement * 100.0) << "%)\n";
  }

  return ss.str();
}

std::string ResourceProcessor::generateResourceVisualization() const {
  return visualizer_->generateSummaryReport(
      chronovyan::VisualizationFormat::TEXT);
}

bool ResourceProcessor::saveData(const std::string &filename) const {
  return saveToLog(filename);
}

bool ResourceProcessor::loadData(const std::string &filename) {
  return loadFromLog(filename);
}

void ResourceProcessor::updateComponents() {
  // This is a placeholder for future functionality
}

bool ResourceProcessor::checkResourceThresholds(double chrononUsage,
                                                double aethelUsage,
                                                double temporalDebt,
                                                double paradoxRisk) const {
  // Check if any resource exceeds its threshold
  if (chrononUsage > chronon_threshold_ || aethelUsage > aethel_threshold_ ||
      temporalDebt > temporal_debt_threshold_ ||
      paradoxRisk > paradox_risk_threshold_) {
    return true;
  }
  return false;
}

} // namespace chronovyan