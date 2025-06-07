#include <algorithm>
#include <numeric>

#include "resource_tracker.h"

namespace chronovyan {

ResourceTracker::ResourceTracker(size_t max_history) : max_history_size_(max_history) {
    // Reserve space to avoid frequent reallocations
    history_.reserve(max_history);
}

void ResourceTracker::recordCurrentUsage(double chronon_usage, double aethel_usage,
                                         double temporal_debt, double paradox_risk) {
    // Create new data point with current timestamp
    ResourceDataPoint point;
    point.timestamp = std::chrono::system_clock::now();
    point.chronon_usage = chronon_usage;
    point.aethel_usage = aethel_usage;
    point.temporal_debt = temporal_debt;
    point.paradox_risk = paradox_risk;

    // Add to history, maintaining maximum size
    history_.push_back(point);
    if (history_.size() > max_history_size_) {
        history_.erase(history_.begin());
    }

    // Update statistics after new data point
    updateStatistics();
}

const std::vector<ResourceDataPoint>& ResourceTracker::getHistoricalData() const {
    return history_;
}

std::map<std::string, double> ResourceTracker::getResourceStatistics() const { return statistics_; }

void ResourceTracker::reset() {
    history_.clear();
    statistics_.clear();
}

void ResourceTracker::updateStatistics() {
    if (history_.empty()) {
        statistics_.clear();
        return;
    }

    // Calculate basic statistics
    double total_chronon = 0.0;
    double total_aethel = 0.0;
    double max_chronon = 0.0;
    double max_aethel = 0.0;
    double max_debt = 0.0;
    double max_risk = 0.0;

    for (const auto& point : history_) {
        total_chronon += point.chronon_usage;
        total_aethel += point.aethel_usage;

        max_chronon = std::max(max_chronon, point.chronon_usage);
        max_aethel = std::max(max_aethel, point.aethel_usage);
        max_debt = std::max(max_debt, point.temporal_debt);
        max_risk = std::max(max_risk, point.paradox_risk);
    }

    const size_t count = history_.size();

    statistics_["avg_chronon_usage"] = total_chronon / count;
    statistics_["avg_aethel_usage"] = total_aethel / count;
    statistics_["max_chronon_usage"] = max_chronon;
    statistics_["max_aethel_usage"] = max_aethel;
    statistics_["max_temporal_debt"] = max_debt;
    statistics_["max_paradox_risk"] = max_risk;

    // Current values (most recent)
    const auto& latest = history_.back();
    statistics_["current_chronon_usage"] = latest.chronon_usage;
    statistics_["current_aethel_usage"] = latest.aethel_usage;
    statistics_["current_temporal_debt"] = latest.temporal_debt;
    statistics_["current_paradox_risk"] = latest.paradox_risk;
}

}  // namespace chronovyan