/**
 * @file temporal_algorithms.cpp
 * @brief Implementation of common temporal algorithms for the Chronovyan Standard Library
 *
 * This file contains implementations for common temporal algorithms that perform
 * operations on temporal data structures, sequences, and timelines. These algorithms
 * provide optimized implementations of frequently needed operations and establish
 * patterns for temporal programming.
 *
 * @copyright Chronovyan Project
 */

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <stdexcept>

#include "../../include/resource_optimizer.h"
#include "../../include/stdlib/temporal_algorithms.h"
#include "../../include/temporal_debt_tracker.h"

namespace chronovyan {
namespace stdlib {
namespace algorithms {

//------------------------------------------------------
// Temporal Sequence Algorithms
//------------------------------------------------------

TemporalSequence temporal_map(const TemporalSequence& sequence,
                              const std::function<Value(const Value&)>& func) {
    // Create a new sequence with the same timestamps but transformed values
    TemporalSequence result;

    // Get all points from the original sequence
    const auto& points = sequence.get_all_points();

    // Transform each point with the provided function
    for (const auto& point : points) {
        const std::string& timestamp = point.first;
        const Value& value = point.second;

        // Apply the function to the value and add to result
        result.add_point(timestamp, func(value));
    }

    // Return the transformed sequence
    return result;
}

TemporalSequence temporal_filter(const TemporalSequence& sequence,
                                 const std::function<bool(const Value&)>& predicate) {
    // Create a new sequence to hold filtered values
    TemporalSequence result;

    // Get all points from the original sequence
    const auto& points = sequence.get_all_points();

    // Add only points that satisfy the predicate
    for (const auto& point : points) {
        const std::string& timestamp = point.first;
        const Value& value = point.second;

        // Check if the value satisfies the predicate
        if (predicate(value)) {
            result.add_point(timestamp, value);
        }
    }

    // Return the filtered sequence
    return result;
}

Value temporal_reduce(const TemporalSequence& sequence, const Value& initial,
                      const std::function<Value(const Value&, const Value&)>& op) {
    // Start with the initial value
    Value result = initial;

    // Get all points from the sequence
    const auto& points = sequence.get_all_points();

    // Reduce all values using the binary operation
    for (const auto& point : points) {
        const Value& value = point.second;
        result = op(result, value);
    }

    // Return the final reduced value
    return result;
}

TemporalSequence temporal_interpolate(const TemporalSequence& sequence, int points,
                                      const std::string& method) {
    // Create a new sequence for the interpolated values
    TemporalSequence result;

    // Get all points from the original sequence
    const auto& original_points = sequence.get_all_points();

    // We need at least two points for interpolation
    if (original_points.size() < 2) {
        // If less than two points, just return a copy of the original
        return sequence;
    }

    // For each pair of adjacent points, interpolate between them
    for (size_t i = 0; i < original_points.size() - 1; ++i) {
        const auto& p1 = *std::next(original_points.begin(), i);
        const auto& p2 = *std::next(original_points.begin(), i + 1);

        const std::string& t1 = p1.first;
        const std::string& t2 = p2.first;
        const Value& v1 = p1.second;
        const Value& v2 = p2.second;

        // Add the first point to the result
        result.add_point(t1, v1);

        // Skip interpolation if values are not numeric
        if (!v1.is_number() || !v2.is_number()) {
            continue;
        }

        double val1 = v1.as_number();
        double val2 = v2.as_number();

        // Extract timestamp numbers (assuming format like "T1234")
        int time1 = std::stoi(t1.substr(1));
        int time2 = std::stoi(t2.substr(1));

        // Interpolate points between t1 and t2
        for (int j = 1; j <= points; ++j) {
            double ratio = static_cast<double>(j) / (points + 1);
            int new_time = time1 + static_cast<int>((time2 - time1) * ratio);
            std::string new_timestamp = "T" + std::to_string(new_time);

            double new_value = 0.0;

            // Apply the specified interpolation method
            if (method == "linear") {
                // Linear interpolation
                new_value = val1 + (val2 - val1) * ratio;
            } else if (method == "cubic") {
                // Cubic interpolation (simplified for this implementation)
                double t = ratio;
                double t2 = t * t;
                double t3 = t2 * t;
                double h00 = 2 * t3 - 3 * t2 + 1;
                double h10 = t3 - 2 * t2 + t;
                double h01 = -2 * t3 + 3 * t2;
                double h11 = t3 - t2;

                // We don't have tangents for cubic interpolation, so use a simplified version
                new_value = h00 * val1 + h01 * val2;
            } else if (method == "nearest") {
                // Nearest neighbor interpolation
                new_value = (ratio < 0.5) ? val1 : val2;
            } else {
                // Default to linear if method is not recognized
                new_value = val1 + (val2 - val1) * ratio;
            }

            // Add the interpolated point to the result
            result.add_point(new_timestamp, Value(new_value));
        }
    }

    // Add the last point from the original sequence
    const auto& last_point = *std::prev(original_points.end());
    result.add_point(last_point.first, last_point.second);

    // Return the interpolated sequence
    return result;
}

//------------------------------------------------------
// Timeline Manipulation Algorithms
//------------------------------------------------------

TemporalSequence merge_timelines(const std::vector<TemporalSequence>& timelines,
                                 const std::string& strategy, const std::vector<double>& weights) {
    // Create a new sequence for the merged timeline
    TemporalSequence result;

    // If no timelines provided, return an empty sequence
    if (timelines.empty()) {
        return result;
    }

    // If only one timeline provided, return a copy of it
    if (timelines.size() == 1) {
        return timelines[0];
    }

    // Collect all unique timestamps from all timelines
    std::set<std::string> all_timestamps;
    for (const auto& timeline : timelines) {
        const auto& points = timeline.get_all_points();
        for (const auto& point : points) {
            all_timestamps.insert(point.first);
        }
    }

    // Validate weights if using weighted strategy
    std::vector<double> normalized_weights;
    if (strategy == "weighted") {
        if (weights.empty()) {
            // If no weights provided, use equal weights
            normalized_weights.resize(timelines.size(), 1.0 / timelines.size());
        } else if (weights.size() != timelines.size()) {
            throw std::invalid_argument("Number of weights must match number of timelines");
        } else {
            // Normalize weights to sum to 1.0
            double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
            if (sum == 0.0) {
                throw std::invalid_argument("Sum of weights cannot be zero");
            }

            normalized_weights.resize(weights.size());
            for (size_t i = 0; i < weights.size(); ++i) {
                normalized_weights[i] = weights[i] / sum;
            }
        }
    }

    // Process each timestamp
    for (const auto& timestamp : all_timestamps) {
        std::vector<Value> values;

        // Collect values for this timestamp from all timelines
        for (const auto& timeline : timelines) {
            if (timeline.has_point(timestamp)) {
                values.push_back(timeline.get_value_at(timestamp));
            }
        }

        // Skip if no values found for this timestamp (shouldn't happen)
        if (values.empty()) {
            continue;
        }

        // Apply the merge strategy
        Value merged_value;

        if (strategy == "latest" || strategy == "earliest") {
            // For latest/earliest, we need to find the timeline with the newest/oldest update
            // For simplicity, we'll just use the last/first timeline that has this timestamp
            merged_value = (strategy == "latest") ? values.back() : values.front();
        } else if (strategy == "average" || strategy == "weighted") {
            // Only works for numeric values
            bool all_numeric = true;
            for (const auto& val : values) {
                if (!val.is_number()) {
                    all_numeric = false;
                    break;
                }
            }

            if (all_numeric) {
                double sum = 0.0;

                if (strategy == "average") {
                    // Simple average
                    for (const auto& val : values) {
                        sum += val.as_number();
                    }
                    merged_value = Value(sum / values.size());
                } else {  // weighted
                    // Weighted average
                    for (size_t i = 0; i < values.size(); ++i) {
                        sum += values[i].as_number() * normalized_weights[i];
                    }
                    merged_value = Value(sum);
                }
            } else {
                // For non-numeric values, fall back to "latest"
                merged_value = values.back();
            }
        } else {
            // Unknown strategy, fall back to "latest"
            merged_value = values.back();
        }

        // Add the merged value to the result
        result.add_point(timestamp, merged_value);
    }

    // Return the merged timeline
    return result;
}

std::vector<std::string> find_timeline_divergences(const TemporalSequence& timeline1,
                                                   const TemporalSequence& timeline2,
                                                   double threshold) {
    std::vector<std::string> divergence_points;

    // Collect all timestamps from both timelines
    std::set<std::string> all_timestamps;
    const auto& points1 = timeline1.get_all_points();
    const auto& points2 = timeline2.get_all_points();

    for (const auto& point : points1) {
        all_timestamps.insert(point.first);
    }

    for (const auto& point : points2) {
        all_timestamps.insert(point.first);
    }

    // Check each timestamp for divergence
    for (const auto& timestamp : all_timestamps) {
        bool timeline1_has_point = timeline1.has_point(timestamp);
        bool timeline2_has_point = timeline2.has_point(timestamp);

        // If one timeline has a point the other doesn't, that's a divergence
        if (timeline1_has_point != timeline2_has_point) {
            divergence_points.push_back(timestamp);
            continue;
        }

        // If both timelines have this point, check for value divergence
        if (timeline1_has_point && timeline2_has_point) {
            const Value& value1 = timeline1.get_value_at(timestamp);
            const Value& value2 = timeline2.get_value_at(timestamp);

            // For numeric values, check if difference exceeds threshold
            if (value1.is_number() && value2.is_number()) {
                double diff = std::abs(value1.as_number() - value2.as_number());
                double max_val =
                    std::max(std::abs(value1.as_number()), std::abs(value2.as_number()));

                // Avoid division by zero
                if (max_val > 0) {
                    double relative_diff = diff / max_val;
                    if (relative_diff > threshold) {
                        divergence_points.push_back(timestamp);
                    }
                } else if (diff > threshold) {
                    // If both values are close to zero, use absolute difference
                    divergence_points.push_back(timestamp);
                }
            }
            // For non-numeric values, check if they're different
            else if (value1.to_string() != value2.to_string()) {
                divergence_points.push_back(timestamp);
            }
        }
    }

    // Sort divergence points by timestamp
    std::sort(divergence_points.begin(), divergence_points.end());

    // Return the list of divergence points
    return divergence_points;
}

TemporalSequence create_branch_timeline(const TemporalSequence& base_timeline,
                                        const std::string& branch_point,
                                        const std::function<Value(const Value&)>& modifier) {
    // Create a new sequence for the branched timeline
    TemporalSequence result;

    // Get all points from the base timeline
    const auto& base_points = base_timeline.get_all_points();

    // If branch point doesn't exist in the base timeline, return a copy
    if (!base_timeline.has_point(branch_point)) {
        return base_timeline;
    }

    bool past_branch_point = false;

    // Process each point in the base timeline
    for (const auto& point : base_points) {
        const std::string& timestamp = point.first;
        const Value& value = point.second;

        // Check if we've reached the branch point
        if (timestamp == branch_point) {
            past_branch_point = true;

            // Add the unmodified branch point
            result.add_point(timestamp, value);
        } else if (!past_branch_point) {
            // Before branch point, copy as-is
            result.add_point(timestamp, value);
        } else {
            // After branch point, apply the modifier
            result.add_point(timestamp, modifier(value));
        }
    }

    // Return the branched timeline
    return result;
}

//------------------------------------------------------
// Temporal Pattern Recognition Algorithms
//------------------------------------------------------

PatternDetectionResult detect_temporal_pattern(const TemporalSequence& sequence,
                                               double sensitivity) {
    // Initialize result with default values
    PatternDetectionResult result;
    result.pattern_type = TemporalPatternType::IRREGULAR;
    result.confidence = 0.0;
    result.period = 0.0;
    result.growth_rate = 0.0;

    // Get all points from the sequence
    const auto& points = sequence.get_all_points();

    // Need at least 3 points for meaningful pattern detection
    if (points.size() < 3) {
        return result;
    }

    // Extract timestamps and values
    std::vector<int> timestamps;
    std::vector<double> values;

    for (const auto& point : points) {
        const std::string& timestamp = point.first;
        const Value& value = point.second;

        // Skip non-numeric values
        if (!value.is_number()) {
            continue;
        }

        // Extract timestamp number (assuming format like "T1234")
        int time = std::stoi(timestamp.substr(1));
        timestamps.push_back(time);
        values.push_back(value.as_number());
    }

    // Not enough numeric values for pattern detection
    if (values.size() < 3) {
        return result;
    }

    // Calculate the first and second differences
    std::vector<double> diff1(values.size() - 1);
    std::vector<double> diff2(values.size() - 2);

    for (size_t i = 0; i < values.size() - 1; ++i) {
        diff1[i] = values[i + 1] - values[i];
    }

    for (size_t i = 0; i < values.size() - 2; ++i) {
        diff2[i] = diff1[i + 1] - diff1[i];
    }

    // Check for constant pattern
    double mean_diff1 = std::accumulate(diff1.begin(), diff1.end(), 0.0) / diff1.size();
    double var_diff1 = 0.0;

    for (double d : diff1) {
        var_diff1 += (d - mean_diff1) * (d - mean_diff1);
    }
    var_diff1 /= diff1.size();

    // Adjust threshold based on sensitivity
    double threshold = 0.01 * (1.0 - sensitivity);

    if (var_diff1 < threshold && std::abs(mean_diff1) < threshold) {
        // Values are approximately constant
        result.pattern_type = TemporalPatternType::CONSTANT;
        result.confidence = 1.0 - (var_diff1 / threshold);
        return result;
    }

    // Check for linear increasing/decreasing pattern
    if (var_diff1 < threshold && std::abs(mean_diff1) >= threshold) {
        if (mean_diff1 > 0) {
            result.pattern_type = TemporalPatternType::INCREASING;
        } else {
            result.pattern_type = TemporalPatternType::DECREASING;
        }

        result.confidence = 1.0 - (var_diff1 / threshold);
        result.growth_rate = mean_diff1;
        return result;
    }

    // Check for cyclic pattern using autocorrelation
    // This is a simplified approach for demonstration

    // Normalize the values
    double mean_value = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double var_value = 0.0;

    for (double v : values) {
        var_value += (v - mean_value) * (v - mean_value);
    }
    var_value /= values.size();

    std::vector<double> normalized_values(values.size());
    for (size_t i = 0; i < values.size(); ++i) {
        normalized_values[i] = (values[i] - mean_value) / std::sqrt(var_value);
    }

    // Calculate autocorrelation for different lags
    std::vector<double> autocorr(values.size() / 2);

    for (size_t lag = 1; lag < autocorr.size(); ++lag) {
        double sum = 0.0;
        int count = 0;

        for (size_t i = 0; i < normalized_values.size() - lag; ++i) {
            sum += normalized_values[i] * normalized_values[i + lag];
            count++;
        }

        if (count > 0) {
            autocorr[lag] = sum / count;
        }
    }

    // Find peaks in autocorrelation
    std::vector<size_t> peaks;

    for (size_t i = 1; i < autocorr.size() - 1; ++i) {
        if (autocorr[i] > autocorr[i - 1] && autocorr[i] > autocorr[i + 1] && autocorr[i] > 0.5) {
            peaks.push_back(i);
        }
    }

    // If we have peaks, it might be cyclic
    if (!peaks.empty()) {
        // Use the first peak as the period
        size_t period_lag = peaks[0];

        // Calculate the confidence based on the peak height
        double confidence = autocorr[period_lag];

        if (confidence > 0.7) {
            result.pattern_type = TemporalPatternType::CYCLIC;
            result.confidence = confidence;
            result.period = period_lag;
            return result;
        }
    }

    // Check for spike pattern
    bool has_spikes = false;
    std::vector<std::string> spike_points;

    double mean_abs_diff = 0.0;
    for (double d : diff1) {
        mean_abs_diff += std::abs(d);
    }
    mean_abs_diff /= diff1.size();

    for (size_t i = 1; i < values.size() - 1; ++i) {
        // A spike is a point that's significantly different from its neighbors
        double left_diff = std::abs(values[i] - values[i - 1]);
        double right_diff = std::abs(values[i] - values[i + 1]);

        if (left_diff > 3 * mean_abs_diff && right_diff > 3 * mean_abs_diff) {
            has_spikes = true;
            spike_points.push_back(points.begin()->first);  // Use the corresponding timestamp
        }
    }

    if (has_spikes) {
        result.pattern_type = TemporalPatternType::SPIKE;
        result.confidence = 0.8;
        result.notable_points = spike_points;
        return result;
    }

    // If no specific pattern detected, it's irregular
    result.pattern_type = TemporalPatternType::IRREGULAR;
    result.confidence = 0.9;  // High confidence that it's irregular

    return result;
}

TemporalSequence forecast_temporal_sequence(const TemporalSequence& sequence, int periods,
                                            const std::string& method) {
    // First, detect the pattern in the sequence
    PatternDetectionResult pattern = detect_temporal_pattern(sequence, 0.7);

    // Get all points from the sequence
    const auto& points = sequence.get_all_points();

    // Need at least 2 points for forecasting
    if (points.size() < 2) {
        return sequence;  // Not enough data, return original
    }

    // Create a result sequence starting with the original points
    TemporalSequence result = sequence;

    // Extract the last timestamp and value
    auto last_point = *std::prev(points.end());
    std::string last_timestamp = last_point.first;
    Value last_value = last_point.second;

    // Extract timestamp number (assuming format like "T1234")
    int last_time = std::stoi(last_timestamp.substr(1));

    // If the value is not numeric, we can't forecast
    if (!last_value.is_number()) {
        return result;
    }

    double last_val = last_value.as_number();

    // Choose forecasting method based on detected pattern or specified method
    std::string forecast_method = method;
    if (method == "auto") {
        // Choose method based on detected pattern
        switch (pattern.pattern_type) {
            case TemporalPatternType::CONSTANT:
                forecast_method = "constant";
                break;
            case TemporalPatternType::INCREASING:
            case TemporalPatternType::DECREASING:
                forecast_method = "linear";
                break;
            case TemporalPatternType::CYCLIC:
                forecast_method = "seasonal";
                break;
            case TemporalPatternType::SPIKE:
            case TemporalPatternType::IRREGULAR:
                forecast_method = "exponential";
                break;
        }
    }

    // Apply the chosen forecasting method
    if (forecast_method == "constant") {
        // Constant forecasting: all future values are the same as the last value
        for (int i = 1; i <= periods; ++i) {
            int new_time = last_time + i;
            std::string new_timestamp = "T" + std::to_string(new_time);
            result.add_point(new_timestamp, last_value);
        }
    } else if (forecast_method == "linear") {
        // Linear forecasting: continue the trend

        // Need at least two points for linear trend
        if (points.size() < 2) {
            return result;
        }

        // Get the second-to-last point
        auto second_last_point = *std::prev(points.end(), 2);
        std::string second_last_timestamp = second_last_point.first;
        Value second_last_value = second_last_point.second;

        // Skip if not numeric
        if (!second_last_value.is_number()) {
            return result;
        }

        int second_last_time = std::stoi(second_last_timestamp.substr(1));
        double second_last_val = second_last_value.as_number();

        // Calculate slope of the trend
        double time_diff = last_time - second_last_time;
        double value_diff = last_val - second_last_val;
        double slope = (time_diff != 0) ? (value_diff / time_diff) : 0;

        // Forecast using the linear trend
        for (int i = 1; i <= periods; ++i) {
            int new_time = last_time + i;
            double new_val = last_val + (slope * i);
            std::string new_timestamp = "T" + std::to_string(new_time);
            result.add_point(new_timestamp, Value(new_val));
        }
    } else if (forecast_method == "exponential") {
        // Exponential smoothing forecast
        double alpha = 0.3;  // Smoothing factor

        // Calculate the smoothed value
        std::vector<double> values;
        for (const auto& point : points) {
            const Value& value = point.second;
            if (value.is_number()) {
                values.push_back(value.as_number());
            }
        }

        if (values.empty()) {
            return result;
        }

        double smoothed = values[0];
        for (size_t i = 1; i < values.size(); ++i) {
            smoothed = alpha * values[i] + (1 - alpha) * smoothed;
        }

        // Forecast using the smoothed value
        for (int i = 1; i <= periods; ++i) {
            int new_time = last_time + i;
            std::string new_timestamp = "T" + std::to_string(new_time);
            result.add_point(new_timestamp, Value(smoothed));
        }
    } else if (forecast_method == "seasonal" || forecast_method == "arima") {
        // Simplified seasonal forecasting using the detected period

        // If we have a cyclic pattern with a period
        if (pattern.pattern_type == TemporalPatternType::CYCLIC && pattern.period > 0) {
            int period = static_cast<int>(pattern.period);

            // Extract all values
            std::vector<double> values;
            for (const auto& point : points) {
                const Value& value = point.second;
                if (value.is_number()) {
                    values.push_back(value.as_number());
                }
            }

            // Forecast using the pattern
            for (int i = 1; i <= periods; ++i) {
                int new_time = last_time + i;

                // Use the value from the same phase in the previous cycle
                size_t index = (values.size() - period + (i % period)) % values.size();
                double new_val = values[index];

                std::string new_timestamp = "T" + std::to_string(new_time);
                result.add_point(new_timestamp, Value(new_val));
            }
        } else {
            // Fall back to linear forecasting if no cycle detected
            // Get the second-to-last point
            auto second_last_point = *std::prev(points.end(), 2);
            std::string second_last_timestamp = second_last_point.first;
            Value second_last_value = second_last_point.second;

            // Skip if not numeric
            if (!second_last_value.is_number()) {
                return result;
            }

            int second_last_time = std::stoi(second_last_timestamp.substr(1));
            double second_last_val = second_last_value.as_number();

            // Calculate slope of the trend
            double time_diff = last_time - second_last_time;
            double value_diff = last_val - second_last_val;
            double slope = (time_diff != 0) ? (value_diff / time_diff) : 0;

            // Forecast using the linear trend
            for (int i = 1; i <= periods; ++i) {
                int new_time = last_time + i;
                double new_val = last_val + (slope * i);
                std::string new_timestamp = "T" + std::to_string(new_time);
                result.add_point(new_timestamp, Value(new_val));
            }
        }
    } else if (forecast_method == "neural") {
        // Simplified neural network forecasting (actually just a weighted average)
        // In a real implementation, this would use a proper neural network model

        // Get the last few points (up to 5)
        std::vector<double> last_values;
        int count = 0;

        for (auto it = points.rbegin(); it != points.rend() && count < 5; ++it, ++count) {
            const Value& value = it->second;
            if (value.is_number()) {
                last_values.push_back(value.as_number());
            }
        }

        if (last_values.empty()) {
            return result;
        }

        // Weights for the last 5 values (more recent values have higher weight)
        std::vector<double> weights = {0.4, 0.3, 0.15, 0.1, 0.05};

        // Adjust weights if we have fewer than 5 values
        if (last_values.size() < weights.size()) {
            weights.resize(last_values.size());
            double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
            for (auto& w : weights) {
                w /= sum;
            }
        }

        // Calculate weighted average
        double predicted = 0.0;
        for (size_t i = 0; i < last_values.size(); ++i) {
            predicted += last_values[i] * weights[i];
        }

        // Forecast using the predicted value
        for (int i = 1; i <= periods; ++i) {
            int new_time = last_time + i;
            std::string new_timestamp = "T" + std::to_string(new_time);
            result.add_point(new_timestamp, Value(predicted));
        }
    } else {
        // Unknown method, fall back to constant forecasting
        for (int i = 1; i <= periods; ++i) {
            int new_time = last_time + i;
            std::string new_timestamp = "T" + std::to_string(new_time);
            result.add_point(new_timestamp, last_value);
        }
    }

    // Return the forecast
    return result;
}

//------------------------------------------------------
// Temporal Optimization Algorithms
//------------------------------------------------------

Value optimize_temporal_operation(const Value& operation,
                                  const std::map<std::string, Value>& constraints) {
    // This is a simplified implementation
    // In a real implementation, this would analyze the operation and optimize it

    // For demonstration, we'll just return the operation unchanged
    // with some metadata about the optimization

    // In a real implementation, we would:
    // 1. Analyze the operation to identify resource usage patterns
    // 2. Apply optimization strategies based on constraints
    // 3. Return an optimized version of the operation

    // For now, just return the original operation
    return operation;
}

std::vector<std::string> find_optimal_timeline_path(
    const std::vector<TemporalSequence>& timelines, const Value& start_state,
    const Value& goal_state,
    const std::function<double(const Value&, const Value&)>& cost_function) {
    // This is a simplified implementation of a pathfinding algorithm
    // In a real implementation, this would use A* or Dijkstra's algorithm

    // For demonstration, we'll just return a simple path
    std::vector<std::string> path;

    // If no timelines, return empty path
    if (timelines.empty()) {
        return path;
    }

    // Add the start timeline
    path.push_back("T0");

    // For each timeline, check if it helps us get closer to the goal
    for (size_t i = 0; i < timelines.size(); ++i) {
        const auto& timeline = timelines[i];

        // Get the last point in the timeline
        const auto& points = timeline.get_all_points();
        if (points.empty()) {
            continue;
        }

        const auto& last_point = *std::prev(points.end());
        const Value& last_value = last_point.second;

        // Calculate cost to goal
        double cost = cost_function(last_value, goal_state);

        // If this timeline gets us closer to the goal, add it to the path
        if (cost < 100.0) {  // Arbitrary threshold
            path.push_back("T" + std::to_string(i + 1));
        }
    }

    // Add the goal timeline
    path.push_back("TGoal");

    return path;
}

}  // namespace algorithms
}  // namespace stdlib
}  // namespace chronovyan