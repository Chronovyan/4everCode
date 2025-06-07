#pragma once
#ifndef CHRONOVYAN_RESOURCE_MANAGEMENT_RESOURCE_TRACKER_H
#define CHRONOVYAN_RESOURCE_MANAGEMENT_RESOURCE_TRACKER_H

#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace chronovyan {

// Data point structure to store resource measurements at a specific point in time
struct ResourceDataPoint {
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    double chronon_usage;  // Current Chronon usage
    double aethel_usage;   // Current Aethel usage
    double temporal_debt;  // Current temporal debt level
    double paradox_risk;   // Current paradox risk assessment
};

// Time series collection for resource tracking
class ResourceTracker {
public:
    ResourceTracker(size_t max_history = 1000);

    // Record current resource state
    void recordCurrentUsage(double chronon_usage, double aethel_usage, double temporal_debt = 0.0,
                            double paradox_risk = 0.0);

    // Get historical data for visualization
    const std::vector<ResourceDataPoint>& getHistoricalData() const;

    // Get statistical summary of resource usage
    std::map<std::string, double> getResourceStatistics() const;

    // Clear recorded data
    void reset();

private:
    std::vector<ResourceDataPoint> history_;
    size_t max_history_size_;

    // Calculate basic statistics from collected data
    void updateStatistics();

    // Internal statistics cache
    std::map<std::string, double> statistics_;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_RESOURCE_MANAGEMENT_RESOURCE_TRACKER_H