#ifndef CHRONOVYAN_TEMPORAL_SEQUENCE_H
#define CHRONOVYAN_TEMPORAL_SEQUENCE_H

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include "custom_type_system.h"
#include "temporal_runtime.h"
#include "value.h"

namespace chronovyan {

/**
 * @class TemporalSequence
 * @brief Specialized collection for time-variant data
 *
 * Unlike regular collections with temporal features added on, TemporalSequence
 * is explicitly designed for storing and manipulating data that varies over time.
 * It tracks values across multiple cycles natively and provides operations
 * for temporal analysis and manipulation.
 *
 * Key features:
 * - Natural representation of time-series data
 * - Efficient storage of temporal values
 * - Operations specifically designed for temporal analysis
 * - Interpolation between temporal points
 * - Temporal pattern detection and projection
 */
class TemporalSequence {
public:
    /**
     * @struct TimePoint
     * @brief A single point in the temporal sequence
     */
    struct TimePoint {
        int cycle;    // The temporal cycle
        Value value;  // The value at this cycle

        TimePoint(int c, const Value& v) : cycle(c), value(v) {}

        bool operator<(const TimePoint& other) const { return cycle < other.cycle; }

        bool operator==(const TimePoint& other) const { return cycle == other.cycle; }
    };

    /**
     * @brief Create a new empty temporal sequence
     * @param runtime Optional temporal runtime for resource tracking
     */
    explicit TemporalSequence(std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Create a new temporal sequence with initial points
     * @param points Initial time points to store
     * @param runtime Optional temporal runtime for resource tracking
     */
    TemporalSequence(const std::vector<TimePoint>& points,
                     std::shared_ptr<TemporalRuntime> runtime = nullptr);

    // Basic operations

    /**
     * @brief Get the number of time points in the sequence
     * @return The size of the sequence
     */
    size_t size() const;

    /**
     * @brief Check if the sequence is empty
     * @return True if the sequence is empty
     */
    bool empty() const;

    /**
     * @brief Get all cycles in the sequence
     * @return Vector of cycles
     */
    std::vector<int> getCycles() const;

    /**
     * @brief Check if a specific cycle exists in the sequence
     * @param cycle The cycle to check
     * @return True if the cycle exists
     */
    bool hasCycle(int cycle) const;

    /**
     * @brief Get the value at a specific cycle
     * @param cycle The cycle to get the value for
     * @return The value at the specified cycle
     * @throws std::out_of_range if cycle not found
     */
    const Value& at(int cycle) const;

    /**
     * @brief Get a mutable reference to the value at a specific cycle
     * @param cycle The cycle to get the value for
     * @return Mutable reference to the value
     * @throws std::out_of_range if cycle not found
     */
    Value& at(int cycle);

    /**
     * @brief Set the value at a specific cycle
     * @param cycle The cycle to set the value for
     * @param value The value to set
     */
    void set(int cycle, const Value& value);

    /**
     * @brief Remove a time point at a specific cycle
     * @param cycle The cycle to remove
     * @return True if the cycle was removed, false if not found
     */
    bool remove(int cycle);

    /**
     * @brief Clear all time points
     */
    void clear();

    // Advanced temporal operations

    /**
     * @brief Get a slice of the sequence between two cycles
     * @param startCycle The start cycle (inclusive)
     * @param endCycle The end cycle (inclusive)
     * @return A new sequence containing the sliced time points
     */
    std::shared_ptr<TemporalSequence> slice(int startCycle, int endCycle) const;

    /**
     * @brief Get the earliest cycle in the sequence
     * @return The earliest cycle, or -1 if the sequence is empty
     */
    int getEarliestCycle() const;

    /**
     * @brief Get the latest cycle in the sequence
     * @return The latest cycle, or -1 if the sequence is empty
     */
    int getLatestCycle() const;

    /**
     * @brief Get the value at a specific cycle with interpolation
     *
     * If the exact cycle exists, returns that value. Otherwise, interpolates
     * between the nearest cycles before and after.
     *
     * @param cycle The cycle to interpolate for
     * @param method Interpolation method ("linear", "step", "spline")
     * @return Interpolated value, or null if interpolation not possible
     */
    Value interpolate(int cycle, const std::string& method = "linear") const;

    /**
     * @brief Resample the sequence to have values at regular cycle intervals
     * @param interval The cycle interval
     * @param method Interpolation method ("linear", "step", "spline")
     * @return A new sequence with resampled time points
     */
    std::shared_ptr<TemporalSequence> resample(int interval,
                                               const std::string& method = "linear") const;

    /**
     * @brief Detect temporal patterns in the sequence
     * @return Vector of detected patterns as sub-sequences
     */
    std::vector<std::shared_ptr<TemporalSequence>> detectPatterns() const;

    /**
     * @brief Project the sequence forward based on detected patterns
     * @param cycles Number of cycles to project forward
     * @return A new sequence extended with projected time points
     */
    std::shared_ptr<TemporalSequence> project(int cycles) const;

    /**
     * @brief Apply a function to each time point
     * @param func Function to apply (cycle, value) -> new value
     * @return New sequence with transformed values
     */
    std::shared_ptr<TemporalSequence> map(std::function<Value(int, const Value&)> func) const;

    /**
     * @brief Filter time points
     * @param pred Predicate function (cycle, value) -> bool
     * @return New sequence with filtered time points
     */
    std::shared_ptr<TemporalSequence> filter(std::function<bool(int, const Value&)> pred) const;

    /**
     * @brief Reduce to a single value
     * @param func Reducer function (accumulator, cycle, value) -> new accumulator
     * @param initial Initial accumulator value
     * @return Final reduced value
     */
    Value reduce(std::function<Value(const Value&, int, const Value&)> func,
                 const Value& initial) const;

    /**
     * @brief Calculate temporal statistics for the sequence
     * @return Map of statistic name to value
     */
    std::map<std::string, Value> statistics() const;

    /**
     * @brief Merge with another temporal sequence
     * @param other Other sequence to merge with
     * @param strategy Merge strategy ("replace", "keep", "combine")
     * @return New sequence with merged time points
     */
    std::shared_ptr<TemporalSequence> merge(const TemporalSequence& other,
                                            const std::string& strategy = "replace") const;

    /**
     * @brief Get string representation
     * @return String representation of the sequence
     */
    std::string toString() const;

    // Temporal Runtime

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
     * @brief Get the estimated chronon cost of an operation
     * @param operation The operation name
     * @param sequenceSize The size of the sequence
     * @return The estimated chronon cost
     */
    static int64_t getChrononsForOperation(const std::string& operation, size_t sequenceSize);

private:
    std::vector<TimePoint> m_timePoints;
    std::shared_ptr<TemporalRuntime> m_runtime;

    /**
     * @brief Track chronon usage for an operation
     * @param operation The operation being performed
     */
    void trackResourceUsage(const std::string& operation) const;

    /**
     * @brief Ensure the time points are sorted by cycle
     */
    void ensureSorted();

    /**
     * @brief Find the index of a time point by cycle
     * @param cycle The cycle to find
     * @return The index, or -1 if not found
     */
    int findIndexByCycle(int cycle) const;
};

// Register the TEMPORAL_SEQUENCE type with the custom type system
void registerTemporalSequenceType(CustomTypeSystem& typeSystem);

}  // namespace chronovyan

#endif  // CHRONOVYAN_TEMPORAL_SEQUENCE_H