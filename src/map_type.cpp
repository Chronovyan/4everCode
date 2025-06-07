#include <algorithm>
#include <cmath>
#include <sstream>

#include "../include/error_handler.h"
#include "../include/map_type.h"

namespace chronovyan {

// Constructor for empty map
TemporalMap::TemporalMap(std::shared_ptr<TemporalRuntime> runtime) : m_runtime(runtime) {
    trackResourceUsage("init");
}

// Constructor with initial elements
TemporalMap::TemporalMap(std::map<std::string, Value> elements,
                         std::shared_ptr<TemporalRuntime> runtime)
    : m_elements(std::move(elements)), m_runtime(runtime) {
    trackResourceUsage("init_with_elements");
}

// Get the number of key-value pairs
size_t TemporalMap::size() const { return m_elements.size(); }

// Check if map is empty
bool TemporalMap::empty() const { return m_elements.empty(); }

// Check if key exists
bool TemporalMap::contains(const std::string& key) const {
    trackResourceUsage("contains");
    return m_elements.find(key) != m_elements.end();
}

// Get value for key
const Value& TemporalMap::get(const std::string& key) const {
    trackResourceUsage("get");
    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        throw std::out_of_range("Key not found: " + key);
    }
    return it->second;
}

// Set key-value pair
void TemporalMap::set(const std::string& key, const Value& value, bool preserve_history) {
    trackResourceUsage("set");

    // If preserving history and key exists, add to history
    if (preserve_history && m_elements.find(key) != m_elements.end()) {
        auto& history = m_keyHistory[key];

        // Only save if we have a temporal runtime to get current cycle
        if (m_runtime) {
            int currentCycle = static_cast<int>(m_runtime->getCycleNumber());
            history.values[currentCycle] = m_elements[key];
            history.latest_cycle = currentCycle;
        }
    }

    // Update the current value
    m_elements[key] = value;
}

// Remove key-value pair
bool TemporalMap::remove(const std::string& key) {
    trackResourceUsage("remove");

    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        return false;
    }

    // Add removal to history
    if (m_runtime) {
        int currentCycle = static_cast<int>(m_runtime->getCycleNumber());
        auto& history = m_keyHistory[key];
        history.values[currentCycle] = Value();  // nil value indicates key was removed
        history.latest_cycle = currentCycle;
    }

    m_elements.erase(it);
    return true;
}

// Clear all key-value pairs
void TemporalMap::clear() {
    trackResourceUsage("clear");

    // Add removal history for all keys
    if (m_runtime) {
        int currentCycle = static_cast<int>(m_runtime->getCycleNumber());
        for (const auto& [key, value] : m_elements) {
            auto& history = m_keyHistory[key];
            history.values[currentCycle] = Value();  // nil value indicates key was removed
            history.latest_cycle = currentCycle;
        }
    }

    m_elements.clear();
}

// Get all keys
std::vector<std::string> TemporalMap::keys() const {
    trackResourceUsage("keys");

    std::vector<std::string> result;
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
std::map<std::string, Value> TemporalMap::entries() const {
    trackResourceUsage("entries");
    return m_elements;
}

// Apply a function to each key-value pair
std::shared_ptr<TemporalMap> TemporalMap::map(
    std::function<Value(const std::string&, const Value&)> func) const {
    trackResourceUsage("map");

    std::map<std::string, Value> result;

    for (const auto& [key, value] : m_elements) {
        result[key] = func(key, value);
    }

    return std::make_shared<TemporalMap>(result, m_runtime);
}

// Filter key-value pairs
std::shared_ptr<TemporalMap> TemporalMap::filter(
    std::function<bool(const std::string&, const Value&)> pred) const {
    trackResourceUsage("filter");

    std::map<std::string, Value> result;

    for (const auto& [key, value] : m_elements) {
        if (pred(key, value)) {
            result[key] = value;
        }
    }

    return std::make_shared<TemporalMap>(result, m_runtime);
}

// Reduce to a single value
Value TemporalMap::reduce(std::function<Value(const Value&, const std::string&, const Value&)> func,
                          const Value& initial) const {
    trackResourceUsage("reduce");

    Value result = initial;

    for (const auto& [key, value] : m_elements) {
        result = func(result, key, value);
    }

    return result;
}

// Merge with another map
std::shared_ptr<TemporalMap> TemporalMap::merge(const TemporalMap& other, bool overwrite) const {
    trackResourceUsage("merge");

    std::map<std::string, Value> result = m_elements;

    for (const auto& [key, value] : other.entries()) {
        if (overwrite || result.find(key) == result.end()) {
            result[key] = value;
        }
    }

    return std::make_shared<TemporalMap>(result, m_runtime);
}

// Get string representation
std::string TemporalMap::toString() const {
    trackResourceUsage("toString");

    std::stringstream ss;
    ss << "{";
    bool first = true;

    for (const auto& [key, value] : m_elements) {
        if (!first) {
            ss << ", ";
        }
        ss << "\"" << key << "\": " << value.toString();
        first = false;
    }

    ss << "}";
    return ss.str();
}

// Temporal features

// Get value at a specific cycle
Value TemporalMap::getAtCycle(const std::string& key, int cycle) const {
    trackResourceUsage("getAtCycle");

    // Check if we have a map snapshot for this cycle
    auto mapIt = m_mapHistory.find(cycle);
    if (mapIt != m_mapHistory.end()) {
        auto valueIt = mapIt->second.find(key);
        if (valueIt != mapIt->second.end()) {
            return valueIt->second;
        }
    }

    // Check key history
    auto historyIt = m_keyHistory.find(key);
    if (historyIt != m_keyHistory.end()) {
        const auto& history = historyIt->second;

        // Find the closest cycle before or equal to the requested cycle
        auto valueIt = history.values.upper_bound(cycle);
        if (valueIt != history.values.begin()) {
            --valueIt;
            // If the cycle is less than or equal to the requested one
            if (valueIt->first <= cycle) {
                return valueIt->second;
            }
        }
    }

    // If we can't find historical data, return current value or throw exception
    auto currentIt = m_elements.find(key);
    if (currentIt != m_elements.end()) {
        return currentIt->second;
    }

    // Key doesn't exist now and no historical data
    throw std::out_of_range("Key not found at cycle " + std::to_string(cycle) + ": " + key);
}

// Get key history
std::map<int, Value> TemporalMap::getKeyHistory(const std::string& key) const {
    trackResourceUsage("getKeyHistory");

    auto historyIt = m_keyHistory.find(key);
    if (historyIt == m_keyHistory.end()) {
        throw std::out_of_range("No history for key: " + key);
    }

    return historyIt->second.values;
}

// Get keys at a specific cycle
std::vector<std::string> TemporalMap::keysAtCycle(int cycle) const {
    trackResourceUsage("keysAtCycle");

    // If we have a map snapshot for this cycle, use it
    auto mapIt = m_mapHistory.find(cycle);
    if (mapIt != m_mapHistory.end()) {
        std::vector<std::string> result;
        result.reserve(mapIt->second.size());

        for (const auto& [key, _] : mapIt->second) {
            result.push_back(key);
        }

        return result;
    }

    // Otherwise reconstruct from key histories
    std::map<std::string, Value> mapAtCycle;

    // Start with current keys
    for (const auto& [key, value] : m_elements) {
        mapAtCycle[key] = value;
    }

    // Apply historical changes
    for (const auto& [key, history] : m_keyHistory) {
        for (const auto& [historyCycle, value] : history.values) {
            if (historyCycle <= cycle) {
                if (value.isNil()) {
                    // Key was removed at this cycle
                    mapAtCycle.erase(key);
                } else {
                    // Key was updated at this cycle
                    mapAtCycle[key] = value;
                }
            }
        }
    }

    // Extract keys
    std::vector<std::string> result;
    result.reserve(mapAtCycle.size());

    for (const auto& [key, _] : mapAtCycle) {
        result.push_back(key);
    }

    return result;
}

// Restore map state from a cycle
bool TemporalMap::restoreFromCycle(int cycle) {
    trackResourceUsage("restoreFromCycle");

    // Check if we have a map snapshot for this cycle
    auto mapIt = m_mapHistory.find(cycle);
    if (mapIt != m_mapHistory.end()) {
        m_elements = mapIt->second;
        return true;
    }

    // If we don't have a snapshot, we can't reliably restore
    // We would need to reconstruct from the key histories,
    // but that's error-prone without knowing all keys that existed
    return false;
}

// Save current state as checkpoint
void TemporalMap::saveHistoryCheckpoint(int cycle) {
    trackResourceUsage("saveHistoryCheckpoint");
    m_mapHistory[cycle] = m_elements;
}

// Get all historical cycles
std::vector<int> TemporalMap::getHistoricalCycles() const {
    std::vector<int> cycles;

    // Add cycles from map snapshots
    for (const auto& [cycle, _] : m_mapHistory) {
        cycles.push_back(cycle);
    }

    // Add cycles from key histories
    for (const auto& [_, history] : m_keyHistory) {
        for (const auto& [cycle, _] : history.values) {
            // Only add if not already in the list
            if (std::find(cycles.begin(), cycles.end(), cycle) == cycles.end()) {
                cycles.push_back(cycle);
            }
        }
    }

    // Sort cycles
    std::sort(cycles.begin(), cycles.end());

    return cycles;
}

// Clear history
void TemporalMap::clearHistory() {
    trackResourceUsage("clearHistory");
    m_keyHistory.clear();
    m_mapHistory.clear();
}

// Set temporal runtime
void TemporalMap::setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime) {
    m_runtime = runtime;
}

// Get temporal runtime
std::shared_ptr<TemporalRuntime> TemporalMap::getTemporalRuntime() const { return m_runtime; }

// Get chronons for operation
int64_t TemporalMap::getChrononsForOperation(const std::string& operation, size_t mapSize) const {
    // Base cost for any operation
    int64_t baseCost = 1;

    // Scaling factor based on map size (logarithmic to be efficient for large maps)
    double sizeFactor = mapSize > 0 ? std::log2(mapSize + 1) : 0;

    // Operation-specific costs
    if (operation == "init") {
        return baseCost;
    } else if (operation == "init_with_elements") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "contains" || operation == "get") {
        // Map lookup is typically O(log n)
        return baseCost + static_cast<int64_t>(std::log2(mapSize + 1));
    } else if (operation == "set" || operation == "remove") {
        // Set/remove includes lookup plus history tracking
        return baseCost + static_cast<int64_t>(std::log2(mapSize + 1) * 1.5);
    } else if (operation == "clear") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "keys" || operation == "values" || operation == "entries") {
        // These return collections of all elements
        return baseCost + static_cast<int64_t>(mapSize);
    } else if (operation == "map" || operation == "filter" || operation == "reduce") {
        // Linear operations that touch every element
        return baseCost + static_cast<int64_t>(mapSize);
    } else if (operation == "merge") {
        // Need to check each key in both maps
        return baseCost + static_cast<int64_t>(mapSize * 1.5);
    } else if (operation == "toString") {
        return baseCost + static_cast<int64_t>(mapSize);
    } else if (operation == "getAtCycle" || operation == "getKeyHistory") {
        // Temporal lookups may need to search through history
        return baseCost + static_cast<int64_t>(std::log2(mapSize + 1) * 2);
    } else if (operation == "keysAtCycle") {
        // Needs to reconstruct the map state at a cycle
        return baseCost + static_cast<int64_t>(mapSize * 2);
    } else if (operation == "restoreFromCycle") {
        // Either direct lookup or complex reconstruction
        return baseCost + static_cast<int64_t>(mapSize * 2);
    } else if (operation == "saveHistoryCheckpoint") {
        // Need to copy the entire map
        return baseCost + static_cast<int64_t>(mapSize);
    } else if (operation == "clearHistory") {
        // Proportional to history size, not current map size
        return baseCost + static_cast<int64_t>(mapSize * 2);
    }

    // Default cost for unknown operations
    return baseCost;
}

// Track resource usage
void TemporalMap::trackResourceUsage(const std::string& operation) const {
    if (m_runtime) {
        int64_t cost = getChrononsForOperation(operation, m_elements.size());

        // Since this is a const method, we can only call const methods on m_runtime
        // Unfortunately, consumeChronons is not const, so we need to use a const_cast
        // This is a safe use of const_cast since we know the object is non-const in reality
        if (auto nonConstRuntime = const_cast<TemporalRuntime*>(m_runtime.get())) {
            nonConstRuntime->consumeChronons(static_cast<int>(cost));
        }
    }
}

// Register MAP type with custom type system
void registerMapType(CustomTypeSystem& typeSystem) {
    // Define properties for the MAP type
    auto mapProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"container", Value(true)},
        {"temporal", Value(true)},  // Map supports temporal features
        {"description", Value("Advanced map type with temporal key versioning")}};

    // Type parameters for generic map
    auto mapParams = std::vector<std::string>{"K", "V"};

    // Create the type definition
    auto mapType =
        std::make_shared<CustomTypeDefinition>("MAP", CustomTypeKind::STRUCT, mapProps, mapParams);

    // Register the type
    typeSystem.registerType(mapType);
}

}  // namespace chronovyan