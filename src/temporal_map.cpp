#include <algorithm>
#include <cmath>
#include <set>
#include <sstream>

#include "../include/error_handler.h"
#include "../include/temporal_map.h"

namespace chronovyan {

// Constructor for empty map
TemporalMap::TemporalMap(std::shared_ptr<TemporalRuntime> runtime) : m_runtime(runtime) {
    // Track resource usage for initialization
    trackResourceUsage("init");
}

// Constructor with initial elements
TemporalMap::TemporalMap(std::map<Value, Value> elements, std::shared_ptr<TemporalRuntime> runtime)
    : m_elements(std::move(elements)), m_runtime(runtime) {
    // Track resource usage for initialization with elements
    trackResourceUsage("init_with_elements");
}

// Size of the map
size_t TemporalMap::size() const { return m_elements.size(); }

// Check if map is empty
bool TemporalMap::empty() const { return m_elements.empty(); }

// Check if map contains key
bool TemporalMap::contains(const Value& key) const {
    return m_elements.find(key) != m_elements.end();
}

// Get value for key (const)
const Value& TemporalMap::at(const Value& key) const {
    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        throw std::out_of_range("Map key not found");
    }
    return it->second;
}

// Get value for key (mutable)
Value& TemporalMap::at(const Value& key) {
    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        throw std::out_of_range("Map key not found");
    }
    return it->second;
}

// Set value for key
void TemporalMap::set(const Value& key, const Value& value) {
    trackResourceUsage("set");
    m_elements[key] = value;
}

// Remove key-value pair
bool TemporalMap::remove(const Value& key) {
    trackResourceUsage("remove");
    return m_elements.erase(key) > 0;
}

// Get all keys
std::vector<Value> TemporalMap::keys() const {
    trackResourceUsage("keys");
    std::vector<Value> result;
    result.reserve(m_elements.size());

    for (const auto& [key, _] : m_elements) {
        result.push_back(key);
    }

    return result;
}

// Get all values
std::vector<Value> TemporalMap::values() const {
    trackResourceUsage("values");
    std::vector<Value> result;
    result.reserve(m_elements.size());

    for (const auto& [_, value] : m_elements) {
        result.push_back(value);
    }

    return result;
}

// Get all key-value pairs
const std::map<Value, Value>& TemporalMap::getElements() const { return m_elements; }

// Apply function to each key-value pair
void TemporalMap::forEach(std::function<void(const Value& key, const Value& value)> func) const {
    trackResourceUsage("forEach");

    for (const auto& [key, value] : m_elements) {
        func(key, value);
    }
}

// Map values using a transformation function
std::shared_ptr<TemporalMap> TemporalMap::mapValues(
    std::function<Value(const Value& key, const Value& value)> func) const {
    trackResourceUsage("mapValues");

    std::map<Value, Value> result;

    for (const auto& [key, value] : m_elements) {
        result[key] = func(key, value);
    }

    return std::make_shared<TemporalMap>(result, m_runtime);
}

// Filter key-value pairs
std::shared_ptr<TemporalMap> TemporalMap::filter(
    std::function<bool(const Value& key, const Value& value)> pred) const {
    trackResourceUsage("filter");

    std::map<Value, Value> result;

    for (const auto& [key, value] : m_elements) {
        if (pred(key, value)) {
            result[key] = value;
        }
    }

    return std::make_shared<TemporalMap>(result, m_runtime);
}

// Merge two maps
std::shared_ptr<TemporalMap> TemporalMap::merge(
    const TemporalMap& other,
    std::function<Value(const Value& key, const Value& value1, const Value& value2)>
        conflictResolver) const {
    trackResourceUsage("merge");

    std::map<Value, Value> result = m_elements;

    for (const auto& [key, value] : other.getElements()) {
        auto it = result.find(key);

        if (it == result.end()) {
            // No conflict, just add the key-value pair
            result[key] = value;
        } else if (conflictResolver) {
            // Use the conflict resolver function
            result[key] = conflictResolver(key, it->second, value);
        } else {
            // Default behavior: overwrite with the value from other
            result[key] = value;
        }
    }

    return std::make_shared<TemporalMap>(result, m_runtime);
}

// String representation
std::string TemporalMap::toString() const {
    trackResourceUsage("toString");

    std::stringstream ss;
    ss << "{";
    bool first = true;

    for (const auto& [key, value] : m_elements) {
        if (!first) {
            ss << ", ";
        }
        ss << key.toString() << ": " << value.toString();
        first = false;
    }

    ss << "}";
    return ss.str();
}

// Temporal features

// Get value at cycle
Value TemporalMap::getAtCycle(const Value& key, int cycle) const {
    // Check if we have history for this cycle
    auto it = m_history.find(cycle);
    if (it == m_history.end()) {
        // No history for this cycle, return current value
        return at(key);
    }

    // Check if key existed in the historical map
    auto keyIt = it->second.find(key);
    if (keyIt == it->second.end()) {
        throw std::out_of_range("Key did not exist at the specified cycle");
    }

    return keyIt->second;
}

// Check if key existed at cycle
bool TemporalMap::containedAtCycle(const Value& key, int cycle) const {
    // Check if we have history for this cycle
    auto it = m_history.find(cycle);
    if (it == m_history.end()) {
        // No history for this cycle, check current map
        return contains(key);
    }

    // Check if key existed in the historical map
    return it->second.find(key) != it->second.end();
}

// Get keys at cycle
std::vector<Value> TemporalMap::keysAtCycle(int cycle) const {
    // Check if we have history for this cycle
    auto it = m_history.find(cycle);
    if (it == m_history.end()) {
        // No history for this cycle, return current keys
        return keys();
    }

    // Get keys from the historical map
    std::vector<Value> result;
    result.reserve(it->second.size());

    for (const auto& [key, _] : it->second) {
        result.push_back(key);
    }

    return result;
}

// Save history checkpoint
void TemporalMap::saveHistoryCheckpoint(int cycle) {
    trackResourceUsage("saveHistoryCheckpoint");
    m_history[cycle] = m_elements;
}

// Restore from checkpoint
bool TemporalMap::restoreFromCycle(int cycle) {
    auto it = m_history.find(cycle);
    if (it == m_history.end()) {
        return false;
    }

    trackResourceUsage("restoreFromCycle");
    m_elements = it->second;
    return true;
}

// Get all cycles with history
std::vector<int> TemporalMap::getHistoricalCycles() const {
    std::vector<int> cycles;
    cycles.reserve(m_history.size());

    for (const auto& entry : m_history) {
        cycles.push_back(entry.first);
    }

    return cycles;
}

// Clear history
void TemporalMap::clearHistory() {
    trackResourceUsage("clearHistory");
    m_history.clear();
}

// Track key history
std::map<int, Value> TemporalMap::trackKeyHistory(const Value& key, int startCycle,
                                                  int endCycle) const {
    trackResourceUsage("trackKeyHistory");

    std::map<int, Value> history;

    // Get all cycles with history checkpoints
    std::vector<int> cycles = getHistoricalCycles();

    // Add current state as a cycle
    int currentCycle = endCycle + 1;

    // Find all cycles between start and end
    for (int cycle : cycles) {
        if (cycle >= startCycle && cycle <= endCycle) {
            // Check if key existed at this cycle
            if (containedAtCycle(key, cycle)) {
                history[cycle] = getAtCycle(key, cycle);
            }
        }
    }

    // Add current state if it falls within the range
    if (startCycle <= currentCycle && currentCycle <= endCycle && contains(key)) {
        history[currentCycle] = at(key);
    }

    return history;
}

// Find frequently changed keys
std::map<Value, int> TemporalMap::getFrequentlyChangedKeys(int startCycle, int endCycle,
                                                           size_t limit) const {
    trackResourceUsage("getFrequentlyChangedKeys");

    // Map of key to change count
    std::map<Value, int> changeCount;

    // Get all cycles with history checkpoints
    std::vector<int> cycles = getHistoricalCycles();

    // Sort cycles for proper iteration
    std::sort(cycles.begin(), cycles.end());

    // Build a set of all keys ever used
    std::set<Value> allKeys;

    for (int cycle : cycles) {
        if (cycle >= startCycle && cycle <= endCycle) {
            for (const auto& [key, _] : m_history.at(cycle)) {
                allKeys.insert(key);
            }
        }
    }

    // Add current keys
    for (const auto& [key, _] : m_elements) {
        allKeys.insert(key);
    }

    // Count changes for each key
    for (const Value& key : allKeys) {
        int changes = 0;
        Value lastValue;
        bool hasLastValue = false;

        for (int cycle : cycles) {
            if (cycle >= startCycle && cycle <= endCycle) {
                if (containedAtCycle(key, cycle)) {
                    Value currentValue = getAtCycle(key, cycle);

                    if (hasLastValue && !areEqual(lastValue, currentValue)) {
                        changes++;
                    }

                    lastValue = currentValue;
                    hasLastValue = true;
                }
            }
        }

        // Check for change against current state
        if (hasLastValue && contains(key) && !areEqual(lastValue, at(key))) {
            changes++;
        }

        changeCount[key] = changes;
    }

    // Sort keys by change count
    std::vector<std::pair<Value, int>> sortedChanges;
    for (const auto& [key, count] : changeCount) {
        sortedChanges.push_back({key, count});
    }

    std::sort(sortedChanges.begin(), sortedChanges.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    // Apply limit if specified
    if (limit > 0 && sortedChanges.size() > limit) {
        sortedChanges.resize(limit);
    }

    // Create result map
    std::map<Value, int> result;
    for (const auto& [key, count] : sortedChanges) {
        result[key] = count;
    }

    return result;
}

// Detect key pattern
std::string TemporalMap::detectKeyPattern(const Value& key, const std::vector<int>& cycles) const {
    trackResourceUsage("detectKeyPattern");

    // Get historical values for the key
    std::map<int, Value> history;
    for (int cycle : cycles) {
        if (containedAtCycle(key, cycle)) {
            history[cycle] = getAtCycle(key, cycle);
        }
    }

    // Not enough data points to detect a pattern
    if (history.size() < 2) {
        return "insufficient_data";
    }

    // Check if all values are numeric
    bool allNumeric = true;
    for (const auto& [_, value] : history) {
        if (!value.isNumeric()) {
            allNumeric = false;
            break;
        }
    }

    if (allNumeric) {
        // Analyze numeric patterns
        std::vector<double> values;
        for (const auto& [_, value] : history) {
            double numValue = value.isInteger() ? value.asInteger() : value.asFloat();
            values.push_back(numValue);
        }

        // Check for monotonically increasing
        bool increasing = true;
        for (size_t i = 1; i < values.size(); ++i) {
            if (values[i] <= values[i - 1]) {
                increasing = false;
                break;
            }
        }

        if (increasing) {
            return "increasing";
        }

        // Check for monotonically decreasing
        bool decreasing = true;
        for (size_t i = 1; i < values.size(); ++i) {
            if (values[i] >= values[i - 1]) {
                decreasing = false;
                break;
            }
        }

        if (decreasing) {
            return "decreasing";
        }

        // Check for cyclic pattern (min 4 values needed)
        if (values.size() >= 4) {
            // A simple check for repeating subsequences
            size_t patternLength = 2;  // Try patterns of length 2 first
            while (patternLength <= values.size() / 2) {
                bool isPattern = true;

                for (size_t i = 0; i < values.size() - patternLength; ++i) {
                    if (std::abs(values[i] - values[i + patternLength]) > 1e-6) {
                        isPattern = false;
                        break;
                    }
                }

                if (isPattern) {
                    return "cyclic";
                }

                patternLength++;
            }
        }

        // Check for alternating pattern (up-down-up-down)
        bool alternating = true;
        for (size_t i = 2; i < values.size(); ++i) {
            bool shouldBeGreater = (i % 2 == 0);
            if (shouldBeGreater) {
                if (values[i] <= values[i - 1]) {
                    alternating = false;
                    break;
                }
            } else {
                if (values[i] >= values[i - 1]) {
                    alternating = false;
                    break;
                }
            }
        }

        if (alternating) {
            return "alternating";
        }

        // No specific pattern detected
        return "irregular";
    } else {
        // For non-numeric values, check for repetition
        std::vector<Value> values;
        for (const auto& [_, value] : history) {
            values.push_back(value);
        }

        // Check if all values are the same
        bool allSame = true;
        for (size_t i = 1; i < values.size(); ++i) {
            if (!values[i].equals(values[0])) {
                allSame = false;
                break;
            }
        }

        if (allSame) {
            return "stable";
        }

        // Check for repeating pattern
        size_t patternLength = 2;  // Try patterns of length 2 first
        while (patternLength <= values.size() / 2) {
            bool isPattern = true;

            for (size_t i = 0; i < values.size() - patternLength; ++i) {
                if (!values[i].equals(values[i + patternLength])) {
                    isPattern = false;
                    break;
                }
            }

            if (isPattern) {
                return "repetitive";
            }

            patternLength++;
        }

        // No specific pattern detected
        return "irregular";
    }
}

// Predict future value
Value TemporalMap::predictFutureValue(const Value& key, int futureCycle,
                                      const std::vector<int>& historicalCycles) const {
    trackResourceUsage("predictFutureValue");

    // Get historical values for the key
    std::map<int, Value> history;
    for (int cycle : historicalCycles) {
        if (containedAtCycle(key, cycle)) {
            history[cycle] = getAtCycle(key, cycle);
        }
    }

    // Not enough data points to make a prediction
    if (history.size() < 2) {
        // Return the current value if available, otherwise nil
        if (contains(key)) {
            return at(key);
        }
        return Value();
    }

    // Check if all values are numeric
    bool allNumeric = true;
    for (const auto& [_, value] : history) {
        if (!value.isNumeric()) {
            allNumeric = false;
            break;
        }
    }

    if (allNumeric) {
        // For numeric values, perform linear regression
        std::vector<std::pair<int, double>> points;
        for (const auto& [cycle, value] : history) {
            double numValue = value.isInteger() ? value.asInteger() : value.asFloat();
            points.push_back({cycle, numValue});
        }

        // Sort points by cycle
        std::sort(points.begin(), points.end());

        // Simple linear regression (y = mx + b)
        double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
        int n = static_cast<int>(points.size());

        for (const auto& [x, y] : points) {
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }

        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;

        // Predict using the linear model
        double predictedValue = slope * futureCycle + intercept;

        // Return as integer if the original values were integers
        bool allIntegers = true;
        for (const auto& [_, value] : history) {
            if (!value.isInteger()) {
                allIntegers = false;
                break;
            }
        }

        if (allIntegers) {
            return Value(static_cast<int64_t>(std::round(predictedValue)));
        } else {
            return Value(predictedValue);
        }
    } else {
        // For non-numeric values, use the most recent value
        auto lastEntry = history.rbegin();
        if (lastEntry != history.rend()) {
            return lastEntry->second;
        }

        // Default to nil if no valid prediction can be made
        return Value();
    }
}

// Set temporal runtime
void TemporalMap::setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime) {
    m_runtime = runtime;
}

// Get temporal runtime
std::shared_ptr<TemporalRuntime> TemporalMap::getTemporalRuntime() const { return m_runtime; }

// Resource costs
int64_t TemporalMap::getChrononsForOperation(const std::string& operation, size_t mapSize) {
    // Base cost for any operation
    int64_t baseCost = 1;

    // Scaling factor based on map size (logarithmic to be efficient for large maps)
    double sizeFactor = mapSize > 0 ? std::log2(mapSize + 1) : 0;

    // Operation-specific costs
    if (operation == "init") {
        return baseCost;
    } else if (operation == "init_with_elements") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "set" || operation == "remove") {
        return baseCost +
               static_cast<int64_t>(std::log2(mapSize + 1));  // Log complexity for map operations
    } else if (operation == "keys" || operation == "values") {
        return baseCost + static_cast<int64_t>(mapSize);  // Linear complexity
    } else if (operation == "forEach" || operation == "mapValues" || operation == "filter") {
        return baseCost + static_cast<int64_t>(mapSize);  // Linear complexity
    } else if (operation == "merge") {
        return baseCost + static_cast<int64_t>(mapSize * 1.5);  // Higher cost for merging
    } else if (operation == "toString") {
        return baseCost + static_cast<int64_t>(mapSize);
    } else if (operation == "saveHistoryCheckpoint" || operation == "restoreFromCycle") {
        return baseCost + static_cast<int64_t>(mapSize * 2);  // Higher cost for temporal operations
    } else if (operation == "clearHistory") {
        return baseCost + static_cast<int64_t>(mapSize);
    } else if (operation == "trackKeyHistory") {
        return baseCost + static_cast<int64_t>(sizeFactor * 3);  // Complex temporal operation
    } else if (operation == "getFrequentlyChangedKeys") {
        return baseCost + static_cast<int64_t>(mapSize * 3);  // Very complex temporal operation
    } else if (operation == "detectKeyPattern") {
        return baseCost + static_cast<int64_t>(sizeFactor * 5);  // Pattern detection is expensive
    } else if (operation == "predictFutureValue") {
        return baseCost + static_cast<int64_t>(sizeFactor * 4);  // Prediction is expensive
    }

    // Default cost for unknown operations
    return baseCost + static_cast<int64_t>(sizeFactor);
}

// Track resource usage for operations
void TemporalMap::trackResourceUsage(const std::string& operation) {
    if (m_runtime) {
        int64_t cost = getChrononsForOperation(operation, m_elements.size());
        m_runtime->consumeChronons(static_cast<int>(cost));
    }
}

// Register the TEMPORAL_MAP type with the custom type system
void registerTemporalMapType(CustomTypeSystem& typeSystem) {
    // Define properties for the TEMPORAL_MAP type
    auto mapProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"container", Value(true)},
        {"temporal", Value(true)},
        {"key_value", Value(true)},
        {"description", Value("Key-value map type with temporal features and history tracking")}};

    // Type parameters for generic map (K and V)
    auto mapParams = std::vector<std::string>{"K", "V"};

    // Create the type definition
    auto mapType = std::make_shared<CustomTypeDefinition>("TEMPORAL_MAP", CustomTypeKind::STRUCT,
                                                          mapProps, mapParams);

    // Register the type
    typeSystem.registerType(mapType);
}

}  // namespace chronovyan