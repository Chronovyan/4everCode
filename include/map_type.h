#ifndef CHRONOVYAN_MAP_TYPE_H
#define CHRONOVYAN_MAP_TYPE_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "custom_type_system.h"
#include "temporal_runtime.h"
#include "value.h"

namespace chronovyan {

/**
 * @class TemporalMap
 * @brief Advanced key-value store with temporal features
 *
 * TemporalMap provides a key-value store with the following features:
 * - Key versioning across temporal cycles
 * - Efficient resource tracking
 * - Comprehensive map operations (get, set, remove, etc.)
 * - Temporal operations (get value at cycle, restore from cycle, etc.)
 */
class TemporalMap {
public:
    /**
     * @brief Construct an empty map
     * @param runtime Temporal runtime for resource tracking
     */
    TemporalMap(std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Construct a map with initial elements
     * @param elements Initial key-value pairs
     * @param runtime Temporal runtime for resource tracking
     */
    TemporalMap(std::map<std::string, Value> elements,
                std::shared_ptr<TemporalRuntime> runtime = nullptr);

    // Basic map operations

    /**
     * @brief Get the number of key-value pairs
     * @return Number of elements
     */
    size_t size() const;

    /**
     * @brief Check if map is empty
     * @return True if empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief Check if key exists
     * @param key Key to check
     * @return True if key exists, false otherwise
     */
    bool contains(const std::string& key) const;

    /**
     * @brief Get value for key
     * @param key Key to look up
     * @return Reference to the value
     * @throws std::out_of_range if key not found
     */
    const Value& get(const std::string& key) const;

    /**
     * @brief Set key-value pair
     * @param key Key to set
     * @param value Value to set
     * @param preserve_history Whether to preserve history of previous value
     */
    void set(const std::string& key, const Value& value, bool preserve_history = true);

    /**
     * @brief Remove key-value pair
     * @param key Key to remove
     * @return True if key was removed, false if key not found
     */
    bool remove(const std::string& key);

    /**
     * @brief Clear all key-value pairs
     */
    void clear();

    /**
     * @brief Get all keys
     * @return Vector of keys
     */
    std::vector<std::string> keys() const;

    /**
     * @brief Get all values
     * @return Vector of values
     */
    std::vector<Value> values() const;

    /**
     * @brief Get all key-value pairs
     * @return Map of key-value pairs
     */
    std::map<std::string, Value> entries() const;

    // Functional operations

    /**
     * @brief Apply a function to each key-value pair
     * @param func Function to apply (key, value) -> new value
     * @return New map with transformed values
     */
    std::shared_ptr<TemporalMap> map(
        std::function<Value(const std::string&, const Value&)> func) const;

    /**
     * @brief Filter key-value pairs
     * @param pred Predicate function (key, value) -> bool
     * @return New map with filtered key-value pairs
     */
    std::shared_ptr<TemporalMap> filter(
        std::function<bool(const std::string&, const Value&)> pred) const;

    /**
     * @brief Reduce to a single value
     * @param func Reducer function (accumulator, key, value) -> new accumulator
     * @param initial Initial accumulator value
     * @return Final reduced value
     */
    Value reduce(std::function<Value(const Value&, const std::string&, const Value&)> func,
                 const Value& initial) const;

    /**
     * @brief Merge with another map
     * @param other Other map to merge with
     * @param overwrite Whether to overwrite existing keys
     * @return New map with merged key-value pairs
     */
    std::shared_ptr<TemporalMap> merge(const TemporalMap& other, bool overwrite = true) const;

    /**
     * @brief Get string representation
     * @return String representation of the map
     */
    std::string toString() const;

    // Temporal features

    /**
     * @brief Get value at a specific cycle
     * @param key Key to look up
     * @param cycle Cycle to look up
     * @return Value at the specified cycle
     * @throws std::out_of_range if key not found at cycle
     */
    Value getAtCycle(const std::string& key, int cycle) const;

    /**
     * @brief Get key history
     * @param key Key to get history for
     * @return Map of cycle -> value
     * @throws std::out_of_range if key has no history
     */
    std::map<int, Value> getKeyHistory(const std::string& key) const;

    /**
     * @brief Get keys at a specific cycle
     * @param cycle Cycle to look up
     * @return Vector of keys at the specified cycle
     */
    std::vector<std::string> keysAtCycle(int cycle) const;

    /**
     * @brief Restore map state from a cycle
     * @param cycle Cycle to restore from
     * @return True if successful, false otherwise
     */
    bool restoreFromCycle(int cycle);

    /**
     * @brief Save current state as checkpoint
     * @param cycle Cycle to save state for
     */
    void saveHistoryCheckpoint(int cycle);

    /**
     * @brief Get all historical cycles
     * @return Vector of cycles with historical data
     */
    std::vector<int> getHistoricalCycles() const;

    /**
     * @brief Clear history
     */
    void clearHistory();

    /**
     * @brief Set temporal runtime
     * @param runtime Temporal runtime for resource tracking
     */
    void setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime);

    /**
     * @brief Get temporal runtime
     * @return Temporal runtime
     */
    std::shared_ptr<TemporalRuntime> getTemporalRuntime() const;

private:
    // Structure to store key history
    struct KeyHistory {
        std::map<int, Value> values;  // cycle -> value
        int latest_cycle = -1;        // latest cycle with change
    };

    // Current map state
    std::map<std::string, Value> m_elements;

    // Key history tracking
    std::map<std::string, KeyHistory> m_keyHistory;

    // Map snapshots at specific cycles
    std::map<int, std::map<std::string, Value>> m_mapHistory;

    // Temporal runtime for resource tracking
    std::shared_ptr<TemporalRuntime> m_runtime;

    // Helper methods for resource tracking
    int64_t getChrononsForOperation(const std::string& operation, size_t mapSize) const;
    void trackResourceUsage(const std::string& operation) const;
};

/**
 * @brief Register MAP type with custom type system
 * @param typeSystem Type system to register with
 */
void registerMapType(CustomTypeSystem& typeSystem);

}  // namespace chronovyan

#endif  // CHRONOVYAN_MAP_TYPE_H