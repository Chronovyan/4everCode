#include <algorithm>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>

#include "../include/automatic_resource_optimizer.h"

namespace chronovyan {

// Constructor
AutomaticResourceOptimizer::AutomaticResourceOptimizer(
    std::shared_ptr<TemporalRuntime> runtime, std::shared_ptr<TemporalDebtTracker> debt_tracker,
    const OptimizationConfig& config)
    : ResourceOptimizer(runtime, debt_tracker)
    , m_config(config)
    , m_optimizationActive(false)
    , m_nextCallbackId(1) {
    // Initialize with sensible defaults if none are provided
}

// Destructor
AutomaticResourceOptimizer::~AutomaticResourceOptimizer() {
    // Make sure monitoring thread is stopped
    stopAutomaticOptimization();
}

// Start automatic optimization
bool AutomaticResourceOptimizer::startAutomaticOptimization() {
    if (m_optimizationActive) {
        // Already running
        return false;
    }

    // Set flag and start thread
    m_optimizationActive = true;
    m_optimizationThread =
        std::thread(&AutomaticResourceOptimizer::optimizationThreadFunction, this);

    return true;
}

// Stop automatic optimization
void AutomaticResourceOptimizer::stopAutomaticOptimization() {
    if (!m_optimizationActive) {
        // Not running
        return;
    }

    {
        std::unique_lock<std::mutex> lock(m_optimizationMutex);
        m_optimizationActive = false;
        m_optimizationCV.notify_one();
    }

    // Wait for thread to finish
    if (m_optimizationThread.joinable()) {
        m_optimizationThread.join();
    }
}

// Check if automatic optimization is active
bool AutomaticResourceOptimizer::isAutomaticOptimizationActive() const {
    return m_optimizationActive;
}

// Get the current configuration
OptimizationConfig AutomaticResourceOptimizer::getConfig() const { return m_config; }

// Set a new configuration
void AutomaticResourceOptimizer::setConfig(const OptimizationConfig& config) {
    // Stop and restart if needed to apply new settings
    bool wasActive = m_optimizationActive;
    if (wasActive) {
        stopAutomaticOptimization();
    }

    m_config = config;

    if (wasActive) {
        startAutomaticOptimization();
    }
}

// Get optimization history
std::vector<OptimizationResult> AutomaticResourceOptimizer::getOptimizationHistory() const {
    std::lock_guard<std::mutex> lock(m_historyMutex);
    return m_optimizationHistory;
}

// Generate an optimization report
std::string AutomaticResourceOptimizer::generateOptimizationReport(bool detailed) const {
    std::lock_guard<std::mutex> lock(m_historyMutex);

    std::stringstream ss;

    // Report header
    ss << "=== Automatic Resource Optimization Report ===\n";

    // Optimization status
    ss << "Status: " << (m_optimizationActive ? "Active" : "Inactive") << "\n";

    // Basic statistics
    int totalOptimizations = m_optimizationHistory.size();
    int chrononOptimizations = 0;
    int aethelOptimizations = 0;
    double totalChrononsSaved = 0.0;
    double totalAethelGenerated = 0.0;

    for (const auto& result : m_optimizationHistory) {
        if (result.resource_type == "chronons") {
            chrononOptimizations++;
            totalChrononsSaved += (result.before_value - result.after_value);
        } else if (result.resource_type == "aethel") {
            aethelOptimizations++;
            totalAethelGenerated += (result.after_value - result.before_value);
        }
    }

    ss << "\nOptimization Statistics:\n";
    ss << "  Total Optimizations: " << totalOptimizations << "\n";
    ss << "  Chronon Optimizations: " << chrononOptimizations << "\n";
    ss << "  Aethel Optimizations: " << aethelOptimizations << "\n";
    ss << "  Total Chronons Saved: " << std::fixed << std::setprecision(1) << totalChrononsSaved
       << "\n";
    ss << "  Total Aethel Generated: " << std::fixed << std::setprecision(1) << totalAethelGenerated
       << "\n";

    // Pattern statistics
    std::map<std::string, int> patternCounts;
    for (const auto& result : m_optimizationHistory) {
        patternCounts[result.pattern_detected]++;
    }

    ss << "\nDetected Patterns:\n";
    for (const auto& [pattern, count] : patternCounts) {
        ss << "  " << pattern << ": " << count << " optimizations\n";
    }

    // Algorithm statistics
    std::map<std::string, int> algorithmCounts;
    for (const auto& result : m_optimizationHistory) {
        algorithmCounts[result.algorithm_used]++;
    }

    ss << "\nAlgorithms Used:\n";
    for (const auto& [algorithm, count] : algorithmCounts) {
        ss << "  " << algorithm << ": " << count << " optimizations\n";
    }

    // Operation statistics
    std::map<std::string, int> operationCounts;
    for (const auto& result : m_optimizationHistory) {
        operationCounts[result.operation_id]++;
    }

    ss << "\nTop Operations Optimized:\n";

    // Sort operations by optimization count
    std::vector<std::pair<std::string, int>> sortedOperations(operationCounts.begin(),
                                                              operationCounts.end());
    std::sort(sortedOperations.begin(), sortedOperations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Display top 5 operations (or fewer if less than 5)
    int numToShow = std::min(5, static_cast<int>(sortedOperations.size()));
    for (int i = 0; i < numToShow; ++i) {
        ss << "  " << sortedOperations[i].first << ": " << sortedOperations[i].second
           << " optimizations\n";
    }

    // Detailed history (if requested)
    if (detailed && !m_optimizationHistory.empty()) {
        ss << "\nDetailed Optimization History:\n";

        for (size_t i = 0; i < m_optimizationHistory.size(); ++i) {
            const auto& result = m_optimizationHistory[i];
            auto time_t = std::chrono::system_clock::to_time_t(result.timestamp);
            std::tm* tm = std::localtime(&time_t);

            ss << "[" << i + 1 << "] " << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << " - "
               << result.operation_id << " (" << result.resource_type << ")\n";
            ss << "    Pattern: " << result.pattern_detected
               << ", Algorithm: " << result.algorithm_used << "\n";
            ss << "    Before: " << std::fixed << std::setprecision(1) << result.before_value
               << ", After: " << std::fixed << std::setprecision(1) << result.after_value
               << ", Factor: " << std::fixed << std::setprecision(3) << result.optimization_factor
               << "\n";

            // Calculate improvement percentage
            double improvement =
                calculateImprovementPercentage(result.before_value, result.after_value);
            ss << "    Improvement: " << std::fixed << std::setprecision(1) << (improvement * 100.0)
               << "%\n\n";
        }
    }

    ss << "==========================================\n";
    return ss.str();
}

// Register a callback for optimization events
int AutomaticResourceOptimizer::registerOptimizationCallback(
    std::function<void(const OptimizationResult&)> callback) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    int id = m_nextCallbackId++;
    m_optimizationCallbacks[id] = callback;
    return id;
}

// Unregister a callback
bool AutomaticResourceOptimizer::unregisterOptimizationCallback(int callback_id) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    auto it = m_optimizationCallbacks.find(callback_id);
    if (it != m_optimizationCallbacks.end()) {
        m_optimizationCallbacks.erase(it);
        return true;
    }
    return false;
}

// Set priority operations for optimization
void AutomaticResourceOptimizer::setPriorityOperations(const std::set<std::string>& operation_ids) {
    m_priorityOperations = operation_ids;
}

// Set excluded operations that should not be optimized
void AutomaticResourceOptimizer::setExcludedOperations(const std::set<std::string>& operation_ids) {
    m_excludedOperations = operation_ids;
}

// Identify optimization opportunities without applying them
std::map<std::string, double> AutomaticResourceOptimizer::identifyOptimizationOpportunities() {
    std::map<std::string, double> opportunities;

    // Get all operations from history
    std::set<std::string> operations;

    // Add operations from history
    for (const auto& entry : getOperationHistory()) {
        operations.insert(entry.first);
    }

    // Evaluate each operation for potential optimization
    for (const auto& operation : operations) {
        // Skip excluded operations
        if (!shouldOptimizeOperation(operation)) {
            continue;
        }

        // Detect the pattern
        std::string pattern = detectOperationPattern(operation);

        // Estimate potential optimization factor without actually applying it
        double potentialFactor = 1.0;

        if (pattern == "repetitive") {
            // Repetitive operations can benefit more from caching and batching
            potentialFactor = 0.8;  // Potential 20% savings
        } else if (pattern == "spike") {
            // Spike patterns can be smoothed
            potentialFactor = 0.85;  // Potential 15% savings
        } else if (pattern == "cyclic") {
            // Cyclic patterns can be predicted
            potentialFactor = 0.9;  // Potential 10% savings
        } else {
            // Default pattern with less potential
            potentialFactor = 0.95;  // Potential 5% savings
        }

        // Only include if the potential improvement meets the threshold
        if ((1.0 - potentialFactor) >= m_config.minimum_improvement_threshold) {
            opportunities[operation] = potentialFactor;
        }
    }

    return opportunities;
}

// Manually trigger a full optimization cycle
int AutomaticResourceOptimizer::performOptimizationCycle() {
    return performOptimizationCycleInternal();
}

// Override optimizeChronons to track manual optimizations
double AutomaticResourceOptimizer::optimizeChronons(const std::string& operationId) {
    // Get value before optimization
    double before = m_runtime->getChronons();

    // Call base class implementation
    double factor = ResourceOptimizer::optimizeChronons(operationId);

    // Get value after optimization
    double after = m_runtime->getChronons();

    // Record the result if learning from manual optimizations is enabled
    if (m_config.learn_from_manual_optimizations) {
        OptimizationResult result;
        result.operation_id = operationId;
        result.resource_type = "chronons";
        result.before_value = before;
        result.after_value = after;
        result.optimization_factor = factor;
        result.pattern_detected = "manual";
        result.algorithm_used = "basic";

        // Learn from this result
        learnFromOptimizationResult(result);
    }

    return factor;
}

// Override optimizeChrononsAdvanced to track manual optimizations
double AutomaticResourceOptimizer::optimizeChrononsAdvanced(const std::string& operationId,
                                                            const std::string& operationPattern) {
    // Get value before optimization
    double before = m_runtime->getChronons();

    // Call base class implementation
    double factor = ResourceOptimizer::optimizeChrononsAdvanced(operationId, operationPattern);

    // Get value after optimization
    double after = m_runtime->getChronons();

    // Record the result if learning from manual optimizations is enabled
    if (m_config.learn_from_manual_optimizations) {
        OptimizationResult result;
        result.operation_id = operationId;
        result.resource_type = "chronons";
        result.before_value = before;
        result.after_value = after;
        result.optimization_factor = factor;
        result.pattern_detected = operationPattern.empty() ? "auto-detected" : operationPattern;
        result.algorithm_used = "advanced";

        // Learn from this result
        learnFromOptimizationResult(result);
    }

    return factor;
}

// Override optimizeAethel to track manual optimizations
double AutomaticResourceOptimizer::optimizeAethel(const std::string& timelineId) {
    // Get value before optimization
    double before = m_runtime->getAethel();

    // Call base class implementation
    double factor = ResourceOptimizer::optimizeAethel(timelineId);

    // Get value after optimization
    double after = m_runtime->getAethel();

    // Record the result if learning from manual optimizations is enabled
    if (m_config.learn_from_manual_optimizations) {
        OptimizationResult result;
        result.operation_id = timelineId;
        result.resource_type = "aethel";
        result.before_value = before;
        result.after_value = after;
        result.optimization_factor = factor;
        result.pattern_detected = "manual";
        result.algorithm_used = "basic";

        // Learn from this result
        learnFromOptimizationResult(result);
    }

    return factor;
}

// Override optimizeAethelAdvanced to track manual optimizations
double AutomaticResourceOptimizer::optimizeAethelAdvanced(const std::string& timelineId,
                                                          const std::string& operationPattern) {
    // Get value before optimization
    double before = m_runtime->getAethel();

    // Call base class implementation
    double factor = ResourceOptimizer::optimizeAethelAdvanced(timelineId, operationPattern);

    // Get value after optimization
    double after = m_runtime->getAethel();

    // Record the result if learning from manual optimizations is enabled
    if (m_config.learn_from_manual_optimizations) {
        OptimizationResult result;
        result.operation_id = timelineId;
        result.resource_type = "aethel";
        result.before_value = before;
        result.after_value = after;
        result.optimization_factor = factor;
        result.pattern_detected = operationPattern.empty() ? "auto-detected" : operationPattern;
        result.algorithm_used = "advanced";

        // Learn from this result
        learnFromOptimizationResult(result);
    }

    return factor;
}

// Optimization thread function
void AutomaticResourceOptimizer::optimizationThreadFunction() {
    while (m_optimizationActive) {
        // Check if we need to perform an optimization cycle
        bool shouldOptimize = false;

        // Check for resource bottlenecks if enabled
        if (m_config.optimize_on_resource_bottleneck && isResourceBottleneckReached()) {
            shouldOptimize = true;
        }

        // If we should optimize, do it
        if (shouldOptimize) {
            performOptimizationCycleInternal();
        }

        // Wait for the monitoring interval or until stopped
        std::unique_lock<std::mutex> lock(m_optimizationMutex);
        m_optimizationCV.wait_for(lock, m_config.monitoring_interval,
                                  [this]() { return !m_optimizationActive; });
    }
}

// Internal implementation of the optimization cycle
int AutomaticResourceOptimizer::performOptimizationCycleInternal() {
    int optimizationsPerformed = 0;

    // Identify optimization opportunities
    auto opportunities = identifyOptimizationOpportunities();

    // Sort opportunities by potential improvement (highest first)
    std::vector<std::pair<std::string, double>> sortedOpportunities(opportunities.begin(),
                                                                    opportunities.end());
    std::sort(sortedOpportunities.begin(), sortedOpportunities.end(),
              [](const auto& a, const auto& b) { return a.second < b.second; });

    // Apply optimizations up to the maximum per cycle
    for (const auto& [operation, factor] : sortedOpportunities) {
        if (optimizationsPerformed >= m_config.max_optimizations_per_cycle) {
            break;
        }

        // Determine resource type based on operation
        // For simplicity, assuming even operations are chronons and odd are aethel
        bool isChronons = (std::hash<std::string>{}(operation) % 2 == 0);
        std::string resourceType = isChronons ? "chronons" : "aethel";

        // Create result object to track the optimization
        OptimizationResult result;
        result.operation_id = operation;
        result.resource_type = resourceType;
        result.pattern_detected = detectOperationPattern(operation);

        // Store value before optimization
        result.before_value = isChronons ? m_runtime->getChronons() : m_runtime->getAethel();

        // Apply the appropriate optimization
        if (isChronons && m_config.enable_auto_chronon_optimization) {
            result.optimization_factor =
                optimizeChrononsAdvanced(operation, result.pattern_detected);
            result.algorithm_used = "auto_chronon_" + result.pattern_detected;
            optimizationsPerformed++;
        } else if (!isChronons && m_config.enable_auto_aethel_optimization) {
            result.optimization_factor = optimizeAethelAdvanced(operation, result.pattern_detected);
            result.algorithm_used = "auto_aethel_" + result.pattern_detected;
            optimizationsPerformed++;
        }

        // Store value after optimization
        result.after_value = isChronons ? m_runtime->getChronons() : m_runtime->getAethel();

        // Record the result if optimization was performed
        if (optimizationsPerformed > 0) {
            recordOptimizationResult(result);

            // Learn from this result to improve future optimizations
            learnFromOptimizationResult(result);
        }
    }

    return optimizationsPerformed;
}

// Record an optimization result
void AutomaticResourceOptimizer::recordOptimizationResult(const OptimizationResult& result) {
    // Add to history
    {
        std::lock_guard<std::mutex> lock(m_historyMutex);
        m_optimizationHistory.push_back(result);

        // Trim history if it gets too large (keep the most recent 1000 entries)
        if (m_optimizationHistory.size() > 1000) {
            m_optimizationHistory.erase(m_optimizationHistory.begin());
        }
    }

    // Notify callbacks
    notifyOptimizationCallbacks(result);
}

// Notify callbacks about an optimization
void AutomaticResourceOptimizer::notifyOptimizationCallbacks(const OptimizationResult& result) {
    std::lock_guard<std::mutex> lock(m_callbackMutex);
    for (const auto& [id, callback] : m_optimizationCallbacks) {
        callback(result);
    }
}

// Check if an operation should be optimized
bool AutomaticResourceOptimizer::shouldOptimizeOperation(const std::string& operation_id) const {
    // Skip excluded operations
    if (m_excludedOperations.find(operation_id) != m_excludedOperations.end()) {
        return false;
    }

    // Prioritize operations in the priority list
    if (!m_priorityOperations.empty()) {
        return m_priorityOperations.find(operation_id) != m_priorityOperations.end();
    }

    // By default, all non-excluded operations can be optimized
    return true;
}

// Check if a resource bottleneck is reached
bool AutomaticResourceOptimizer::isResourceBottleneckReached() const {
    // Get current resource levels
    int currentChronons = m_runtime->getChronons();
    int currentAethel = m_runtime->getAethel();

    // Get maximum resource levels (assuming they're initialized to reasonable values)
    int maxChronons = 1000;  // Default value, adjust as needed
    int maxAethel = 1000;    // Default value, adjust as needed

    // Calculate ratios
    double chrononRatio = static_cast<double>(currentChronons) / maxChronons;
    double aethelRatio = static_cast<double>(currentAethel) / maxAethel;

    // Check if either resource is below the bottleneck threshold
    return (chrononRatio <= m_config.resource_bottleneck_threshold ||
            aethelRatio <= m_config.resource_bottleneck_threshold);
}

// Detect the operation pattern
std::string AutomaticResourceOptimizer::detectOperationPattern(
    const std::string& operation_id) const {
    // Get operation history
    const auto& history = getOperationHistory();

    // Filter history for this specific operation
    std::vector<std::pair<std::string, int>> operationHistory;
    for (const auto& entry : history) {
        if (entry.first == operation_id) {
            operationHistory.push_back(entry);
        }
    }

    // Not enough history to determine pattern
    if (operationHistory.size() < 3) {
        return "unknown";
    }

    // Check for repetitive pattern (similar resource usage)
    std::vector<int> values;
    for (const auto& entry : operationHistory) {
        values.push_back(entry.second);
    }

    // Calculate variance
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double variance = 0.0;
    for (const auto& value : values) {
        variance += std::pow(value - mean, 2);
    }
    variance /= values.size();
    double standardDeviation = std::sqrt(variance);

    // Low variance indicates repetitive pattern
    if (standardDeviation / mean < 0.1) {
        return "repetitive";
    }

    // Check for spike pattern (occasional high values)
    int highValues = 0;
    for (const auto& value : values) {
        if (value > mean * 1.5) {
            highValues++;
        }
    }

    if (highValues > 0 && highValues <= values.size() / 3) {
        return "spike";
    }

    // Check for cyclic pattern (alternating values)
    bool isCyclic = true;
    int cycleLength = 2;

    if (values.size() >= cycleLength * 2) {
        for (size_t i = 0; i < values.size() - cycleLength; ++i) {
            if (std::abs(values[i] - values[i + cycleLength]) / mean > 0.2) {
                isCyclic = false;
                break;
            }
        }

        if (isCyclic) {
            return "cyclic";
        }
    }

    // Check for increasing or decreasing pattern
    bool increasing = true;
    bool decreasing = true;

    for (size_t i = 1; i < values.size(); ++i) {
        if (values[i] <= values[i - 1]) {
            increasing = false;
        }
        if (values[i] >= values[i - 1]) {
            decreasing = false;
        }
    }

    if (increasing) {
        return "increasing";
    }
    if (decreasing) {
        return "decreasing";
    }

    // Default to "variable" if no specific pattern is detected
    return "variable";
}

// Learn from optimization results to improve future optimizations
void AutomaticResourceOptimizer::learnFromOptimizationResult(const OptimizationResult& result) {
    // This method would implement machine learning techniques to improve future optimizations
    // For now, we'll use a simple adaptive approach:

    // Update algorithm weights based on the success of this optimization
    double improvement = calculateImprovementPercentage(result.before_value, result.after_value);

    // If the improvement was good, increase the weight of this algorithm
    if (improvement > 0.1) {  // More than 10% improvement
        if (result.resource_type == "chronons") {
            // Increase weight for the chronon algorithm used
            if (result.algorithm_used.find("caching") != std::string::npos) {
                adjustAlgorithmWeight("chronons_caching", 0.05);
            } else if (result.algorithm_used.find("batching") != std::string::npos) {
                adjustAlgorithmWeight("chronons_batching", 0.05);
            } else if (result.algorithm_used.find("paradox") != std::string::npos) {
                adjustAlgorithmWeight("chronons_paradox", 0.05);
            }
        } else if (result.resource_type == "aethel") {
            // Increase weight for the aethel algorithm used
            if (result.algorithm_used.find("resonance") != std::string::npos) {
                adjustAlgorithmWeight("aethel_resonance", 0.05);
            } else if (result.algorithm_used.find("alignment") != std::string::npos) {
                adjustAlgorithmWeight("aethel_alignment", 0.05);
            } else if (result.algorithm_used.find("stabilization") != std::string::npos) {
                adjustAlgorithmWeight("aethel_stabilization", 0.05);
            }
        }
    }
    // If the improvement was poor, decrease the weight of this algorithm
    else if (improvement < 0.02) {  // Less than 2% improvement
        if (result.resource_type == "chronons") {
            // Decrease weight for the chronon algorithm used
            if (result.algorithm_used.find("caching") != std::string::npos) {
                adjustAlgorithmWeight("chronons_caching", -0.03);
            } else if (result.algorithm_used.find("batching") != std::string::npos) {
                adjustAlgorithmWeight("chronons_batching", -0.03);
            } else if (result.algorithm_used.find("paradox") != std::string::npos) {
                adjustAlgorithmWeight("chronons_paradox", -0.03);
            }
        } else if (result.resource_type == "aethel") {
            // Decrease weight for the aethel algorithm used
            if (result.algorithm_used.find("resonance") != std::string::npos) {
                adjustAlgorithmWeight("aethel_resonance", -0.03);
            } else if (result.algorithm_used.find("alignment") != std::string::npos) {
                adjustAlgorithmWeight("aethel_alignment", -0.03);
            } else if (result.algorithm_used.find("stabilization") != std::string::npos) {
                adjustAlgorithmWeight("aethel_stabilization", -0.03);
            }
        }
    }
}

// Calculate improvement percentage
double AutomaticResourceOptimizer::calculateImprovementPercentage(double before,
                                                                  double after) const {
    if (before == 0) {
        return 0.0;
    }

    if (after > before) {
        // For aethel, improvement is positive (generated more)
        return (after - before) / before;
    } else {
        // For chronons, improvement is negative (used less)
        return (before - after) / before;
    }
}

}  // namespace chronovyan