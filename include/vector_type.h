#ifndef CHRONOVYAN_VECTOR_TYPE_H
#define CHRONOVYAN_VECTOR_TYPE_H

#include <functional>
#include <memory>
#include <vector>

#include "custom_type_system.h"
#include "temporal_runtime.h"
#include "value.h"

namespace chronovyan {

/**
 * @class ChronovyanVector
 * @brief Advanced vector implementation with temporal features
 *
 * This class extends the basic array functionality with temporal awareness,
 * resource optimization, and advanced vector operations. It supports
 * versioning of elements over time and efficient resource usage.
 */
class ChronovyanVector {
public:
    /**
     * @brief Create a new empty vector
     * @param runtime Optional temporal runtime for resource tracking
     */
    explicit ChronovyanVector(std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Create a new vector with initial elements
     * @param elements Initial elements to store in the vector
     * @param runtime Optional temporal runtime for resource tracking
     */
    ChronovyanVector(std::vector<Value> elements,
                     std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Get the number of elements in the vector
     * @return The size of the vector
     */
    size_t size() const;

    /**
     * @brief Check if the vector is empty
     * @return True if the vector is empty
     */
    bool empty() const;

    /**
     * @brief Get an element at a specific index
     * @param index The index of the element
     * @return Reference to the element
     * @throws std::out_of_range if index is out of bounds
     */
    const Value& at(size_t index) const;

    /**
     * @brief Get a mutable reference to an element
     * @param index The index of the element
     * @return Mutable reference to the element
     * @throws std::out_of_range if index is out of bounds
     */
    Value& at(size_t index);

    /**
     * @brief Add an element to the end of the vector
     * @param value The value to add
     */
    void push_back(const Value& value);

    /**
     * @brief Remove the last element from the vector
     * @throws std::out_of_range if the vector is empty
     */
    void pop_back();

    /**
     * @brief Insert an element at a specific position
     * @param index The position to insert at
     * @param value The value to insert
     * @throws std::out_of_range if index is out of bounds
     */
    void insert(size_t index, const Value& value);

    /**
     * @brief Remove an element at a specific position
     * @param index The position to remove from
     * @throws std::out_of_range if index is out of bounds
     */
    void erase(size_t index);

    /**
     * @brief Clear all elements from the vector
     */
    void clear();

    /**
     * @brief Get a slice of the vector
     * @param start The start index (inclusive)
     * @param end The end index (exclusive)
     * @return A new vector containing the sliced elements
     * @throws std::out_of_range if indices are out of bounds
     */
    std::shared_ptr<ChronovyanVector> slice(size_t start, size_t end) const;

    /**
     * @brief Concatenate this vector with another
     * @param other The vector to concatenate with
     * @return A new vector containing all elements from both vectors
     */
    std::shared_ptr<ChronovyanVector> concat(const ChronovyanVector& other) const;

    /**
     * @brief Map a function over each element of the vector
     * @param func The function to apply
     * @return A new vector with the mapped values
     */
    std::shared_ptr<ChronovyanVector> map(std::function<Value(const Value&)> func) const;

    /**
     * @brief Filter elements based on a predicate
     * @param pred The predicate function
     * @return A new vector with filtered elements
     */
    std::shared_ptr<ChronovyanVector> filter(std::function<bool(const Value&)> pred) const;

    /**
     * @brief Reduce the vector to a single value
     * @param func The reduction function
     * @param initial The initial value
     * @return The reduced value
     */
    Value reduce(std::function<Value(const Value&, const Value&)> func, const Value& initial) const;

    /**
     * @brief Sort the vector elements
     * @param comparator Optional custom comparator function
     */
    void sort(std::function<bool(const Value&, const Value&)> comparator = nullptr);

    /**
     * @brief Find the index of a value
     * @param value The value to find
     * @param start_index The index to start searching from
     * @return The index of the value, or -1 if not found
     */
    int64_t indexOf(const Value& value, size_t start_index = 0) const;

    /**
     * @brief Check if the vector contains a value
     * @param value The value to check for
     * @return True if the value exists in the vector
     */
    bool contains(const Value& value) const;

    /**
     * @brief Join vector elements into a string
     * @param separator The separator to use between elements
     * @return The joined string
     */
    std::string join(const std::string& separator) const;

    /**
     * @brief Get a string representation of the vector
     * @return String representation
     */
    std::string toString() const;

    /**
     * @brief Get all elements as a standard vector
     * @return Copy of all elements
     */
    std::vector<Value> toStdVector() const;

    /**
     * @brief Get access to the underlying data
     * @return The vector data
     */
    const std::vector<Value>& getData() const;

    // Temporal features

    /**
     * @brief Get the element value at a specific temporal cycle
     * @param index The element index
     * @param cycle The temporal cycle
     * @return The value at the specified cycle
     */
    Value getAtCycle(size_t index, int cycle) const;

    /**
     * @brief Save the current state as a history checkpoint
     * @param cycle The cycle to save for
     */
    void saveHistoryCheckpoint(int cycle);

    /**
     * @brief Restore the vector state from a checkpoint
     * @param cycle The cycle to restore from
     * @return True if successful, false if the cycle doesn't exist
     */
    bool restoreFromCycle(int cycle);

    /**
     * @brief Get all cycles for which history exists
     * @return Vector of cycle numbers
     */
    std::vector<int> getHistoricalCycles() const;

    /**
     * @brief Clear all temporal history
     */
    void clearHistory();

    /**
     * @brief Set the temporal runtime
     * @param runtime The runtime to use
     */
    void setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime);

    /**
     * @brief Get the temporal runtime
     * @return The current runtime
     */
    std::shared_ptr<TemporalRuntime> getTemporalRuntime() const;

    /**
     * @brief Get the estimated chronon cost of a vector operation
     * @param operation The operation name (e.g., "push_back", "sort")
     * @param vectorSize The size of the vector
     * @return The estimated chronon cost
     */
    static int64_t getChrononsForOperation(const std::string& operation, size_t vectorSize);

private:
    std::vector<Value> m_elements;
    std::shared_ptr<TemporalRuntime> m_runtime;

    // Temporal history: cycle -> vector state
    std::map<int, std::vector<Value>> m_history;

    /**
     * @brief Track chronon usage for an operation
     * @param operation The operation being performed
     */
    void trackResourceUsage(const std::string& operation);
};

// Register the VECTOR type with the custom type system
void registerVectorType(CustomTypeSystem& typeSystem);

}  // namespace chronovyan

#endif  // CHRONOVYAN_VECTOR_TYPE_H