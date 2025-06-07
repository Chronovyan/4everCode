/**
 * @file temporal_algorithms.h
 * @brief Common temporal algorithms for the Chronovyan Standard Library
 *
 * This file contains declarations for common temporal algorithms that perform
 * operations on temporal data structures, sequences, and timelines. These algorithms
 * provide optimized implementations of frequently needed operations and establish
 * patterns for temporal programming.
 *
 * @copyright Chronovyan Project
 */

#ifndef CHRONOVYAN_STDLIB_TEMPORAL_ALGORITHMS_H
#define CHRONOVYAN_STDLIB_TEMPORAL_ALGORITHMS_H

#include <functional>
#include <string>
#include <vector>

#include "../temporal_sequence.h"
#include "../value.h"

namespace chronovyan {
namespace stdlib {
namespace algorithms {

//------------------------------------------------------
// Temporal Sequence Algorithms
//------------------------------------------------------

/**
 * @brief Applies a function to each element in a temporal sequence
 *
 * This algorithm traverses a temporal sequence and applies the given function
 * to each element, creating a new sequence with the transformed values.
 *
 * @param sequence The input temporal sequence
 * @param func The function to apply to each element
 * @return A new temporal sequence with transformed values
 *
 * Example usage:
 * @code
 * auto doubled = temporal_map(my_sequence, [](const Value& v) {
 *     return Value(v.as_number() * 2);
 * });
 * @endcode
 */
TemporalSequence temporal_map(const TemporalSequence& sequence,
                              const std::function<Value(const Value&)>& func);

/**
 * @brief Filters elements from a temporal sequence based on a predicate
 *
 * This algorithm creates a new temporal sequence containing only the elements
 * from the input sequence that satisfy the given predicate.
 *
 * @param sequence The input temporal sequence
 * @param predicate The function that determines which elements to keep
 * @return A new temporal sequence with filtered values
 *
 * Example usage:
 * @code
 * auto positive_only = temporal_filter(my_sequence, [](const Value& v) {
 *     return v.as_number() > 0;
 * });
 * @endcode
 */
TemporalSequence temporal_filter(const TemporalSequence& sequence,
                                 const std::function<bool(const Value&)>& predicate);

/**
 * @brief Reduces a temporal sequence to a single value
 *
 * This algorithm traverses a temporal sequence and combines all elements
 * using the given binary operation, starting with the initial value.
 *
 * @param sequence The input temporal sequence
 * @param initial The initial value for the reduction
 * @param op The binary operation to apply
 * @return The final reduced value
 *
 * Example usage:
 * @code
 * auto sum = temporal_reduce(my_sequence, Value(0), [](const Value& acc, const Value& v) {
 *     return Value(acc.as_number() + v.as_number());
 * });
 * @endcode
 */
Value temporal_reduce(const TemporalSequence& sequence, const Value& initial,
                      const std::function<Value(const Value&, const Value&)>& op);

/**
 * @brief Interpolates between values in a temporal sequence
 *
 * This algorithm creates a new temporal sequence with values interpolated
 * between the existing points in the input sequence, using the specified
 * interpolation method.
 *
 * @param sequence The input temporal sequence
 * @param points Number of points to insert between each existing point
 * @param method The interpolation method to use ("linear", "cubic", "nearest")
 * @return A new temporal sequence with interpolated values
 *
 * Example usage:
 * @code
 * auto interpolated = temporal_interpolate(my_sequence, 5, "linear");
 * @endcode
 */
TemporalSequence temporal_interpolate(const TemporalSequence& sequence, int points,
                                      const std::string& method = "linear");

//------------------------------------------------------
// Timeline Manipulation Algorithms
//------------------------------------------------------

/**
 * @brief Merges multiple timelines using the specified strategy
 *
 * This algorithm combines multiple timelines into a single timeline
 * using the specified merge strategy.
 *
 * @param timelines The vector of timelines to merge
 * @param strategy The merge strategy ("latest", "earliest", "average", "weighted")
 * @param weights Optional weights for weighted merge strategy
 * @return The merged timeline
 *
 * Example usage:
 * @code
 * auto merged = merge_timelines({timeline1, timeline2, timeline3}, "latest");
 * @endcode
 */
TemporalSequence merge_timelines(const std::vector<TemporalSequence>& timelines,
                                 const std::string& strategy = "latest",
                                 const std::vector<double>& weights = {});

/**
 * @brief Finds divergence points between timelines
 *
 * This algorithm analyzes two timelines and identifies points where
 * they significantly diverge from each other.
 *
 * @param timeline1 The first timeline
 * @param timeline2 The second timeline
 * @param threshold The threshold for considering a divergence significant
 * @return A vector of timestamps where divergence occurs
 *
 * Example usage:
 * @code
 * auto divergences = find_timeline_divergences(timeline1, timeline2, 0.1);
 * @endcode
 */
std::vector<std::string> find_timeline_divergences(const TemporalSequence& timeline1,
                                                   const TemporalSequence& timeline2,
                                                   double threshold = 0.05);

/**
 * @brief Creates a branched timeline with modifications
 *
 * This algorithm creates a new timeline that branches from the original
 * at the specified point and applies the given modification function.
 *
 * @param base_timeline The original timeline
 * @param branch_point The timestamp where branching occurs
 * @param modifier The function that modifies values after the branch point
 * @return The new branched timeline
 *
 * Example usage:
 * @code
 * auto branch = create_branch_timeline(main_timeline, "T1023", [](const Value& v) {
 *     return Value(v.as_number() * 1.5);
 * });
 * @endcode
 */
TemporalSequence create_branch_timeline(const TemporalSequence& base_timeline,
                                        const std::string& branch_point,
                                        const std::function<Value(const Value&)>& modifier);

//------------------------------------------------------
// Temporal Pattern Recognition Algorithms
//------------------------------------------------------

/**
 * @brief Enum representing different types of temporal patterns
 */
enum class TemporalPatternType { CYCLIC, INCREASING, DECREASING, CONSTANT, SPIKE, IRREGULAR };

/**
 * @brief Structure containing pattern detection results
 */
struct PatternDetectionResult {
    TemporalPatternType pattern_type;
    double confidence;
    double period;       // For cyclic patterns
    double growth_rate;  // For increasing/decreasing patterns
    std::vector<std::string> notable_points;
};

/**
 * @brief Detects patterns in a temporal sequence
 *
 * This algorithm analyzes a temporal sequence and detects common patterns
 * such as cycles, trends, or spikes.
 *
 * @param sequence The temporal sequence to analyze
 * @param sensitivity How sensitive the detection should be (0.0-1.0)
 * @return The pattern detection result
 *
 * Example usage:
 * @code
 * auto pattern = detect_temporal_pattern(temperature_sequence, 0.75);
 * if (pattern.pattern_type == TemporalPatternType::CYCLIC) {
 *     std::cout << "Cyclic pattern detected with period " << pattern.period << std::endl;
 * }
 * @endcode
 */
PatternDetectionResult detect_temporal_pattern(const TemporalSequence& sequence,
                                               double sensitivity = 0.5);

/**
 * @brief Forecasts future values in a temporal sequence
 *
 * This algorithm analyzes patterns in a temporal sequence and forecasts
 * future values based on those patterns.
 *
 * @param sequence The input temporal sequence
 * @param periods The number of periods to forecast
 * @param method The forecasting method ("auto", "arima", "exponential", "neural")
 * @return A new temporal sequence extended with forecasted values
 *
 * Example usage:
 * @code
 * auto forecast = forecast_temporal_sequence(data_sequence, 10, "auto");
 * @endcode
 */
TemporalSequence forecast_temporal_sequence(const TemporalSequence& sequence, int periods,
                                            const std::string& method = "auto");

//------------------------------------------------------
// Temporal Optimization Algorithms
//------------------------------------------------------

/**
 * @brief Optimizes resource usage in a temporal operation
 *
 * This algorithm analyzes a temporal operation and suggests optimizations
 * to reduce Chronon and Aethel consumption.
 *
 * @param operation The temporal operation to optimize
 * @param constraints Constraints for the optimization
 * @return An optimized version of the operation
 *
 * Example usage:
 * @code
 * auto optimized = optimize_temporal_operation(my_operation, {"max_chronons": 100});
 * @endcode
 */
Value optimize_temporal_operation(const Value& operation,
                                  const std::map<std::string, Value>& constraints);

/**
 * @brief Finds the most efficient path through multiple timelines
 *
 * This algorithm finds the most efficient path through multiple timelines
 * to reach a specified goal state.
 *
 * @param timelines Available timelines to traverse
 * @param start_state The starting state
 * @param goal_state The goal state to reach
 * @param cost_function Function that calculates the cost of a transition
 * @return The optimal sequence of timeline transitions
 *
 * Example usage:
 * @code
 * auto path = find_optimal_timeline_path(timelines, start, goal, cost_func);
 * @endcode
 */
std::vector<std::string> find_optimal_timeline_path(
    const std::vector<TemporalSequence>& timelines, const Value& start_state,
    const Value& goal_state,
    const std::function<double(const Value&, const Value&)>& cost_function);

}  // namespace algorithms
}  // namespace stdlib
}  // namespace chronovyan

#endif  // CHRONOVYAN_STDLIB_TEMPORAL_ALGORITHMS_H