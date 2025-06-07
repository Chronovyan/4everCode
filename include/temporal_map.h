#ifndef CHRONOVYAN_TEMPORAL_MAP_H
#define CHRONOVYAN_TEMPORAL_MAP_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "custom_type_system.h"
#include "temporal_runtime.h"
#include "value.h"

namespace chronovyan {

/**
 * @class TemporalMap
 * @brief Advanced map implementation with temporal features
 *
 * This class extends the basic key-value store functionality with temporal awareness,
 * resource optimization, and advanced map operations. It supports versioning of
 * key-value pairs over time and efficient resource usage.
 */
class TemporalMap {
public:
    /**
     * @brief Create a new empty temporal map
     * @param runtime Optional temporal runtime for resource tracking
     */
    explicit TemporalMap(std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Create a new temporal map with initial key-value pairs
     * @param elements Initial key-value pairs to store in the map
     * @param runtime Optional temporal runtime for resource tracking
     */
    TemporalMap(std::map<Value, Value> elements,
                std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Get the number of key-value pairs in the map
     * @return The size of the map
     */
    size_t size() const;

    /**
     * @brief Check if the map is empty
     * @return True if the map is empty
     */
    bool empty() const;

    /**
     * @brief Check if the map contains a key
     * @param key The key to check
     * @return True if the key exists in the map
     */
    bool contains(const Value& key) const;

    /**
     * @brief Get a value for a specific key
     * @param key The key to look up
     * @return The value associated with the key
     * @throws std::out_of_range if key doesn't exist
     */
    const Value& at(const Value& key) const;

    /**
     * @brief Get a mutable reference to a value
     * @param key The key to look up
     * @return Mutable reference to the value
     * @throws std::out_of_range if key doesn't exist
     */
    Value& at(const Value& key);

    /**
     * @brief Set a value for a key
     * @param key The key to set
     * @param value The value to associate with the key
     */
    void set(const Value& key, const Value& value);

    /**
     * @brief Remove a key-value pair
     * @param key The key to remove
     * @return True if the key was found and removed
     */
    bool remove(const Value& key);

    /**
     * @brief Get all keys in the map
     * @return A vector of all keys
     */
    std::vector<Value> keys() const;

    /**
     * @brief Get all values in the map
     * @return A vector of all values
     */
    std::vector<Value> values() const;

    /**
     * @brief Get all key-value pairs in the map
     * @return The map of key-value pairs
     */
    const std::map<Value, Value>& getElements() const;

    /**
     * @brief Apply a function to each key-value pair
     * @param func The function to apply
     */
    void forEach(std::function<void(const Value& key, const Value& value)> func) const;

    /**
     * @brief Create a new map by applying a transformation function to each value
     * @param func The transformation function
     * @return A new map with transformed values
     */
    std::shared_ptr<TemporalMap> mapValues(
        std::function<Value(const Value& key, const Value& value)> func) const;

    /**
     * @brief Filter the map based on a predicate
     * @param pred The predicate function
     * @return A new map containing only the key-value pairs that satisfy the predicate
     */
    std::shared_ptr<TemporalMap> filter(
        std::function<bool(const Value& key, const Value& value)> pred) const;

    /**
     * @brief Merge two maps
     * @param other The other map to merge with
     * @param conflictResolver Function to resolve conflicts (optional)
     * @return A new map containing all key-value pairs from both maps
     */
    std::shared_ptr<TemporalMap> merge(
        const TemporalMap& other,
        std::function<Value(const Value& key, const Value& value1, const Value& value2)>
            conflictResolver = nullptr) const;

    /**
     * @brief Get a string representation of the map
     * @return String representation
     */
    std::string toString() const;

    // Temporal features

    /**
     * @brief Get a value for a key at a specific temporal cycle
     * @param key The key to look up
     * @param cycle The temporal cycle
     * @return The value associated with the key at the specified cycle
     * @throws std::out_of_range if key doesn't exist at that cycle
     */
    Value getAtCycle(const Value& key, int cycle) const;

    /**
     * @brief Check if the map contained a key at a specific temporal cycle
     * @param key The key to check
     * @param cycle The temporal cycle
     * @return True if the key existed in the map at the specified cycle
     */
    bool containedAtCycle(const Value& key, int cycle) const;

    /**
     * @brief Get all keys that existed at a specific temporal cycle
     * @param cycle The temporal cycle
     * @return A vector of keys that existed at the specified cycle
     */
    std::vector<Value> keysAtCycle(int cycle) const;

    /**
     * @brief Save the current state as a history checkpoint
     * @param cycle The cycle to save for
     */
    void saveHistoryCheckpoint(int cycle);

    /**
     * @brief Restore the map state from a checkpoint
     * @param cycle The cycle to restore from
     * @return True if successful, false if the cycle doesn't exist
     */
    bool restoreFromCycle(int cycle);

    /**
     * @brief Get all cycles with history checkpoints
     * @return A vector of cycle numbers
     */
    std::vector<int> getHistoricalCycles() const;

    /**
     * @brief Clear all history checkpoints
     */
    void clearHistory();

    /**
     * @brief Track key modifications over time
     * @param key The key to track
     * @param startCycle The starting cycle
     * @param endCycle The ending cycle
     * @return A map of cycle numbers to values for the key
     */
    std::map<int, Value> trackKeyHistory(const Value& key, int startCycle, int endCycle) const;

    /**
     * @brief Find keys that changed most frequently between cycles
     * @param startCycle The starting cycle
     * @param endCycle The ending cycle
     * @param limit Maximum number of keys to return (0 for all)
     * @return A map of keys to their change frequency
     */
    std::map<Value, int> getFrequentlyChangedKeys(int startCycle, int endCycle,
                                                  size_t limit = 0) const;

    /**
     * @brief Detect temporal patterns in key modifications
     * @param key The key to analyze
     * @param cycles Vector of cycles to consider
     * @return A string describing the detected pattern (e.g., "increasing", "cyclic", "stable")
     */
    std::string detectKeyPattern(const Value& key, const std::vector<int>& cycles) const;

    /**
     * @brief Predict future value for a key based on temporal patterns
     * @param key The key to predict for
     * @param futureCycle The cycle to predict
     * @param historicalCycles List of historical cycles to use for prediction
     * @return The predicted value
     */
    Value predictFutureValue(const Value& key, int futureCycle,
                             const std::vector<int>& historicalCycles) const;

    /**
     * @brief Set the temporal runtime
     * @param runtime The temporal runtime
     */
    void setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime);

    /**
     * @brief Get the temporal runtime
     * @return The temporal runtime
     */
    std::shared_ptr<TemporalRuntime> getTemporalRuntime() const;

private:
    // The current key-value pairs
    std::map<Value, Value> m_elements;

    // Historical states of the map
    std::map<int, std::map<Value, Value>> m_history;

    // Temporal runtime for resource tracking
    std::shared_ptr<TemporalRuntime> m_runtime;

    // Track resource usage for operations
    void trackResourceUsage(const std::string& operation) const;

    // Calculate chronon cost for an operation
    static int64_t getChrononsForOperation(const std::string& operation, size_t mapSize);
};

// Register the TEMPORAL_MAP type with the custom type system
void registerTemporalMapType(CustomTypeSystem& typeSystem);

}  // namespace chronovyan

#endif  // CHRONOVYAN_TEMPORAL_MAP_H