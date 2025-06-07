#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

#include "../include/error_handler.h"
#include "../include/temporal_sequence.h"

namespace chronovyan {

// Constructor for empty sequence
TemporalSequence::TemporalSequence(std::shared_ptr<TemporalRuntime> runtime) : m_runtime(runtime) {
    trackResourceUsage("init");
}

// Constructor with initial time points
TemporalSequence::TemporalSequence(const std::vector<TimePoint>& points,
                                   std::shared_ptr<TemporalRuntime> runtime)
    : m_timePoints(points), m_runtime(runtime) {
    ensureSorted();
    trackResourceUsage("init_with_points");
}

// Size of the sequence
size_t TemporalSequence::size() const { return m_timePoints.size(); }

// Check if sequence is empty
bool TemporalSequence::empty() const { return m_timePoints.empty(); }

// Get all cycles
std::vector<int> TemporalSequence::getCycles() const {
    std::vector<int> cycles;
    cycles.reserve(m_timePoints.size());

    for (const auto& point : m_timePoints) {
        cycles.push_back(point.cycle);
    }

    return cycles;
}

// Check if cycle exists
bool TemporalSequence::hasCycle(int cycle) const { return findIndexByCycle(cycle) != -1; }

// Get value at cycle (const)
const Value& TemporalSequence::at(int cycle) const {
    int index = findIndexByCycle(cycle);
    if (index == -1) {
        throw std::out_of_range("Cycle not found in temporal sequence");
    }

    return m_timePoints[index].value;
}

// Get value at cycle (mutable)
Value& TemporalSequence::at(int cycle) {
    int index = findIndexByCycle(cycle);
    if (index == -1) {
        throw std::out_of_range("Cycle not found in temporal sequence");
    }

    return m_timePoints[index].value;
}

// Set value at cycle
void TemporalSequence::set(int cycle, const Value& value) {
    trackResourceUsage("set");

    int index = findIndexByCycle(cycle);
    if (index != -1) {
        // Update existing time point
        m_timePoints[index].value = value;
    } else {
        // Add new time point
        m_timePoints.emplace_back(cycle, value);
        ensureSorted();
    }
}

// Remove time point at cycle
bool TemporalSequence::remove(int cycle) {
    trackResourceUsage("remove");

    int index = findIndexByCycle(cycle);
    if (index == -1) {
        return false;
    }

    m_timePoints.erase(m_timePoints.begin() + index);
    return true;
}

// Clear all time points
void TemporalSequence::clear() {
    trackResourceUsage("clear");
    m_timePoints.clear();
}

// Get slice of sequence
std::shared_ptr<TemporalSequence> TemporalSequence::slice(int startCycle, int endCycle) const {
    if (startCycle > endCycle) {
        throw std::invalid_argument("Start cycle must be less than or equal to end cycle");
    }

    trackResourceUsage("slice");

    std::vector<TimePoint> sliced;

    for (const auto& point : m_timePoints) {
        if (point.cycle >= startCycle && point.cycle <= endCycle) {
            sliced.push_back(point);
        }
    }

    return std::make_shared<TemporalSequence>(sliced, m_runtime);
}

// Get earliest cycle
int TemporalSequence::getEarliestCycle() const {
    if (m_timePoints.empty()) {
        return -1;
    }

    return m_timePoints.front().cycle;
}

// Get latest cycle
int TemporalSequence::getLatestCycle() const {
    if (m_timePoints.empty()) {
        return -1;
    }

    return m_timePoints.back().cycle;
}

// Interpolate value at cycle
Value TemporalSequence::interpolate(int cycle, const std::string& method) const {
    trackResourceUsage("interpolate");

    // If the cycle exists exactly, return that value
    int index = findIndexByCycle(cycle);
    if (index != -1) {
        return m_timePoints[index].value;
    }

    // If sequence is empty, return null
    if (m_timePoints.empty()) {
        return Value();  // Null value
    }

    // If cycle is before earliest point, return earliest value
    if (cycle < m_timePoints.front().cycle) {
        return m_timePoints.front().value;
    }

    // If cycle is after latest point, return latest value
    if (cycle > m_timePoints.back().cycle) {
        return m_timePoints.back().value;
    }

    // Find the points before and after the requested cycle
    size_t afterIndex = 0;
    while (afterIndex < m_timePoints.size() && m_timePoints[afterIndex].cycle < cycle) {
        afterIndex++;
    }

    size_t beforeIndex = afterIndex - 1;

    const TimePoint& before = m_timePoints[beforeIndex];
    const TimePoint& after = m_timePoints[afterIndex];

    // Different interpolation methods
    if (method == "step") {
        // Step interpolation (use the value before)
        return before.value;
    } else if (method == "linear") {
        // Linear interpolation
        // Only works for numeric values
        if (before.value.getType() == Value::Type::NUMBER &&
            after.value.getType() == Value::Type::NUMBER) {
            double v1 = before.value.asNumber();
            double v2 = after.value.asNumber();
            double ratio = static_cast<double>(cycle - before.cycle) / (after.cycle - before.cycle);
            double interpolated = v1 + ratio * (v2 - v1);
            return Value(interpolated);
        } else {
            // For non-numeric types, fallback to step interpolation
            return before.value;
        }
    } else if (method == "spline") {
        // For now, spline is same as linear (would require more points)
        // Only works for numeric values
        if (before.value.getType() == Value::Type::NUMBER &&
            after.value.getType() == Value::Type::NUMBER) {
            double v1 = before.value.asNumber();
            double v2 = after.value.asNumber();
            double ratio = static_cast<double>(cycle - before.cycle) / (after.cycle - before.cycle);
            double interpolated = v1 + ratio * (v2 - v1);
            return Value(interpolated);
        } else {
            // For non-numeric types, fallback to step interpolation
            return before.value;
        }
    }

    // Default to step interpolation
    return before.value;
}

// Resample sequence to regular intervals
std::shared_ptr<TemporalSequence> TemporalSequence::resample(int interval,
                                                             const std::string& method) const {
    trackResourceUsage("resample");

    if (m_timePoints.empty()) {
        return std::make_shared<TemporalSequence>(m_runtime);
    }

    int start = getEarliestCycle();
    int end = getLatestCycle();

    std::vector<TimePoint> resampled;

    for (int cycle = start; cycle <= end; cycle += interval) {
        Value value = interpolate(cycle, method);
        resampled.emplace_back(cycle, value);
    }

    return std::make_shared<TemporalSequence>(resampled, m_runtime);
}

// Detect patterns (simple implementation - could be extended)
std::vector<std::shared_ptr<TemporalSequence>> TemporalSequence::detectPatterns() const {
    trackResourceUsage("detectPatterns");

    // This is a simplified pattern detection
    // A real implementation would use more sophisticated algorithms

    std::vector<std::shared_ptr<TemporalSequence>> patterns;

    // Only works with at least 4 points
    if (m_timePoints.size() < 4) {
        return patterns;
    }

    // Look for simple repeating patterns (this is highly simplified)
    // In a real implementation, this would use autocorrelation, Fourier analysis, etc.

    // For now, just return the full sequence as a "pattern"
    patterns.push_back(std::make_shared<TemporalSequence>(m_timePoints, m_runtime));

    return patterns;
}

// Project sequence forward
std::shared_ptr<TemporalSequence> TemporalSequence::project(int cycles) const {
    trackResourceUsage("project");

    if (m_timePoints.empty() || cycles <= 0) {
        return std::make_shared<TemporalSequence>(m_timePoints, m_runtime);
    }

    // Simple projection - repeat the pattern of the last few points
    // In a real implementation, this would use the patterns detected above

    std::vector<TimePoint> projected = m_timePoints;

    // Get the last few points to use as a pattern
    size_t patternSize = std::min(m_timePoints.size(), static_cast<size_t>(5));
    std::vector<TimePoint> pattern(m_timePoints.end() - patternSize, m_timePoints.end());

    // Calculate pattern cycle length
    int patternLength = pattern.back().cycle - pattern.front().cycle;
    if (patternLength <= 0)
        patternLength = 1;

    // Get the last cycle
    int lastCycle = m_timePoints.back().cycle;

    // Project forward
    for (int i = 0; i < cycles; i++) {
        for (const auto& point : pattern) {
            int newCycle = lastCycle + (point.cycle - pattern.front().cycle) + i * patternLength;
            projected.emplace_back(newCycle, point.value);
        }
    }

    return std::make_shared<TemporalSequence>(projected, m_runtime);
}

// Map function over sequence
std::shared_ptr<TemporalSequence> TemporalSequence::map(
    std::function<Value(int, const Value&)> func) const {
    trackResourceUsage("map");

    std::vector<TimePoint> mapped;
    mapped.reserve(m_timePoints.size());

    for (const auto& point : m_timePoints) {
        mapped.emplace_back(point.cycle, func(point.cycle, point.value));
    }

    return std::make_shared<TemporalSequence>(mapped, m_runtime);
}

// Filter sequence
std::shared_ptr<TemporalSequence> TemporalSequence::filter(
    std::function<bool(int, const Value&)> pred) const {
    trackResourceUsage("filter");

    std::vector<TimePoint> filtered;

    for (const auto& point : m_timePoints) {
        if (pred(point.cycle, point.value)) {
            filtered.push_back(point);
        }
    }

    return std::make_shared<TemporalSequence>(filtered, m_runtime);
}

// Reduce sequence to a single value
Value TemporalSequence::reduce(std::function<Value(const Value&, int, const Value&)> func,
                               const Value& initial) const {
    trackResourceUsage("reduce");

    Value result = initial;

    for (const auto& point : m_timePoints) {
        result = func(result, point.cycle, point.value);
    }

    return result;
}

// Calculate statistics
std::map<std::string, Value> TemporalSequence::statistics() const {
    trackResourceUsage("statistics");

    std::map<std::string, Value> stats;

    if (m_timePoints.empty()) {
        return stats;
    }

    // Add cycle statistics
    stats["min_cycle"] = Value(static_cast<int64_t>(getEarliestCycle()));
    stats["max_cycle"] = Value(static_cast<int64_t>(getLatestCycle()));
    stats["cycle_range"] = Value(static_cast<int64_t>(getLatestCycle() - getEarliestCycle()));
    stats["point_count"] = Value(static_cast<int64_t>(m_timePoints.size()));

    // For numeric values, calculate value statistics
    bool hasNumeric = false;
    double sum = 0.0;
    double sumSquares = 0.0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();

    for (const auto& point : m_timePoints) {
        double value = 0.0;

        if (point.value.getType() == Value::Type::NUMBER) {
            value = point.value.asNumber();
            hasNumeric = true;
        }

        sum += value;
        sumSquares += value * value;
        min = std::min(min, value);
        max = std::max(max, value);
    }

    if (hasNumeric) {
        double count = static_cast<double>(m_timePoints.size());
        double mean = sum / count;
        double variance = (sumSquares / count) - (mean * mean);
        double stdDev = std::sqrt(variance);

        stats["min_value"] = Value(min);
        stats["max_value"] = Value(max);
        stats["mean"] = Value(mean);
        stats["variance"] = Value(variance);
        stats["std_dev"] = Value(stdDev);
    }

    return stats;
}

// Merge with another sequence
std::shared_ptr<TemporalSequence> TemporalSequence::merge(const TemporalSequence& other,
                                                          const std::string& strategy) const {
    trackResourceUsage("merge");

    std::vector<TimePoint> merged = m_timePoints;

    for (const auto& point : other.m_timePoints) {
        int existingIndex = findIndexByCycle(point.cycle);

        if (existingIndex == -1) {
            // No conflict, just add the point
            merged.push_back(point);
        } else if (strategy == "replace") {
            // Replace existing point
            merged[existingIndex] = point;
        } else if (strategy == "keep") {
            // Keep existing point
            // Do nothing
        } else if (strategy == "combine") {
            // Combine values (only works for numeric types)
            const Value& existingValue = merged[existingIndex].value;
            const Value& newValue = point.value;

            if (existingValue.getType() == Value::Type::NUMBER &&
                newValue.getType() == Value::Type::NUMBER) {
                // For numbers, use sum
                double combined = existingValue.asNumber() + newValue.asNumber();
                merged[existingIndex].value = Value(combined);
            } else {
                // For non-numeric types, prefer the new value
                merged[existingIndex] = point;
            }
        }
    }

    // Ensure the merged sequence is sorted
    std::sort(merged.begin(), merged.end());

    return std::make_shared<TemporalSequence>(merged, m_runtime);
}

// String representation
std::string TemporalSequence::toString() const {
    trackResourceUsage("toString");

    std::stringstream ss;
    ss << "TemporalSequence[";

    bool first = true;
    for (const auto& point : m_timePoints) {
        if (!first) {
            ss << ", ";
        }
        ss << "(" << point.cycle << ":" << point.value.toString() << ")";
        first = false;
    }

    ss << "]";
    return ss.str();
}

// Set temporal runtime
void TemporalSequence::setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime) {
    m_runtime = runtime;
}

// Get temporal runtime
std::shared_ptr<TemporalRuntime> TemporalSequence::getTemporalRuntime() const { return m_runtime; }

// Get chronons for operation
int64_t TemporalSequence::getChrononsForOperation(const std::string& operation,
                                                  size_t sequenceSize) {
    // Base cost for any operation
    int64_t baseCost = 1;

    // Scaling factor based on sequence size (logarithmic to be efficient for large sequences)
    double sizeFactor = sequenceSize > 0 ? std::log2(sequenceSize + 1) : 0;

    // Operation-specific costs
    if (operation == "init") {
        return baseCost;
    } else if (operation == "init_with_points") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "set" || operation == "remove") {
        return baseCost + static_cast<int64_t>(sizeFactor / 2);
    } else if (operation == "clear") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "slice") {
        return baseCost + static_cast<int64_t>(sizeFactor * 2);
    } else if (operation == "interpolate") {
        return baseCost + static_cast<int64_t>(sizeFactor * 3);
    } else if (operation == "resample") {
        return baseCost + static_cast<int64_t>(sequenceSize * 2);
    } else if (operation == "detectPatterns") {
        // Pattern detection is expensive
        return baseCost + static_cast<int64_t>(sequenceSize * sizeFactor);
    } else if (operation == "project") {
        return baseCost + static_cast<int64_t>(sequenceSize * 3);
    } else if (operation == "map" || operation == "filter") {
        // Linear operations that touch every point
        return baseCost + static_cast<int64_t>(sequenceSize);
    } else if (operation == "reduce") {
        return baseCost + static_cast<int64_t>(sequenceSize);
    } else if (operation == "statistics") {
        return baseCost + static_cast<int64_t>(sequenceSize * 2);
    } else if (operation == "merge") {
        return baseCost + static_cast<int64_t>(sequenceSize * 2);
    } else if (operation == "toString") {
        return baseCost + static_cast<int64_t>(sequenceSize);
    }

    // Default cost for unknown operations
    return baseCost;
}

// Track resource usage
void TemporalSequence::trackResourceUsage(const std::string& operation) const {
    if (m_runtime) {
        int64_t cost = getChrononsForOperation(operation, m_timePoints.size());
        m_runtime->consumeChronons(static_cast<int>(cost));
    }
}

// Ensure time points are sorted
void TemporalSequence::ensureSorted() { std::sort(m_timePoints.begin(), m_timePoints.end()); }

// Find index by cycle
int TemporalSequence::findIndexByCycle(int cycle) const {
    // Binary search since the points are sorted
    auto it =
        std::lower_bound(m_timePoints.begin(), m_timePoints.end(), TimePoint(cycle, Value()),
                         [](const TimePoint& a, const TimePoint& b) { return a.cycle < b.cycle; });

    if (it != m_timePoints.end() && it->cycle == cycle) {
        return static_cast<int>(std::distance(m_timePoints.begin(), it));
    }

    return -1;
}

// Register the TEMPORAL_SEQUENCE type with the custom type system
void registerTemporalSequenceType(CustomTypeSystem& typeSystem) {
    // Define properties for the TEMPORAL_SEQUENCE type
    auto props = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"container", Value(true)},
        {"temporal", Value(true)},
        {"description", Value("Specialized collection for time-variant data")}};

    // Type parameters for generic sequence
    auto params = std::vector<std::string>{"T"};

    // Create the type definition
    auto seqType = std::make_shared<CustomTypeDefinition>("TEMPORAL_SEQUENCE",
                                                          CustomTypeKind::STRUCT, props, params);

    // Register the type
    typeSystem.registerType(seqType);
}

}  // namespace chronovyan