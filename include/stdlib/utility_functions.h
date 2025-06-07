/**
 * @file utility_functions.h
 * @brief Utility functions for the Chronovyan Standard Library
 *
 * This file contains declarations for general-purpose utility functions that provide
 * essential tools for common programming tasks. These functions are designed with
 * temporal awareness and conform to Chronovyan's programming model.
 *
 * @copyright Chronovyan Project
 */

#ifndef CHRONOVYAN_STDLIB_UTILITY_FUNCTIONS_H
#define CHRONOVYAN_STDLIB_UTILITY_FUNCTIONS_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "../temporal_sequence.h"
#include "../value.h"

namespace chronovyan {
namespace stdlib {
namespace utility {

//------------------------------------------------------
// String Utilities
//------------------------------------------------------

/**
 * @brief Converts a value to its string representation
 *
 * This function creates a string representation of any Chronovyan value,
 * taking into account its temporal properties.
 *
 * @param value The value to convert to a string
 * @param format Optional formatting specification
 * @return String representation of the value
 *
 * Example usage:
 * @code
 * auto str = to_string(my_value, "temporal");
 * @endcode
 */
std::string to_string(const Value& value, const std::string& format = "default");

/**
 * @brief Parses a string into a Chronovyan value
 *
 * This function converts a string representation into a Chronovyan value
 * of the specified type.
 *
 * @param str The string to parse
 * @param type The target type ("number", "string", "boolean", "temporal", etc.)
 * @return The parsed value
 *
 * Example usage:
 * @code
 * auto value = parse_string("123.45", "number");
 * @endcode
 */
Value parse_string(const std::string& str, const std::string& type);

/**
 * @brief Splits a string into parts based on a delimiter
 *
 * @param str The string to split
 * @param delimiter The delimiter to split on
 * @return A vector of string parts
 *
 * Example usage:
 * @code
 * auto parts = split_string("a,b,c", ",");
 * @endcode
 */
std::vector<std::string> split_string(const std::string& str, const std::string& delimiter);

/**
 * @brief Joins a vector of strings into a single string with a delimiter
 *
 * @param parts The vector of string parts to join
 * @param delimiter The delimiter to insert between parts
 * @return The joined string
 *
 * Example usage:
 * @code
 * auto joined = join_strings({"a", "b", "c"}, ", ");
 * @endcode
 */
std::string join_strings(const std::vector<std::string>& parts, const std::string& delimiter);

/**
 * @brief Applies temporal transformation to a string
 *
 * This function applies temporal transformations to a string, such as
 * revealing characters over time or reflecting temporal state changes.
 *
 * @param str The input string
 * @param transformation The type of temporal transformation to apply
 * @param params Additional parameters for the transformation
 * @return The transformed string
 *
 * Example usage:
 * @code
 * auto transformed = temporal_string_transform("Hello", "reveal", {{"duration", Value(5)}});
 * @endcode
 */
std::string temporal_string_transform(const std::string& str, const std::string& transformation,
                                      const std::map<std::string, Value>& params = {});

//------------------------------------------------------
// Conversion Utilities
//------------------------------------------------------

/**
 * @brief Converts a value to a different type with temporal awareness
 *
 * This function converts a value to the specified type, preserving
 * temporal properties where applicable.
 *
 * @param value The value to convert
 * @param target_type The target type
 * @param options Optional conversion options
 * @return The converted value
 *
 * Example usage:
 * @code
 * auto num = convert_value(string_value, "number", {{"base", Value(16)}});
 * @endcode
 */
Value convert_value(const Value& value, const std::string& target_type,
                    const std::map<std::string, Value>& options = {});

/**
 * @brief Converts a temporal sequence to a different representation
 *
 * This function converts a temporal sequence to a different representation,
 * such as a vector, map, or string, based on the specified target type.
 *
 * @param sequence The temporal sequence to convert
 * @param target_type The target representation type
 * @param options Optional conversion options
 * @return The converted value
 *
 * Example usage:
 * @code
 * auto vec = convert_sequence(my_sequence, "vector");
 * @endcode
 */
Value convert_sequence(const TemporalSequence& sequence, const std::string& target_type,
                       const std::map<std::string, Value>& options = {});

//------------------------------------------------------
// Collection Utilities
//------------------------------------------------------

/**
 * @brief Sorts a vector of values with temporal awareness
 *
 * This function sorts a vector of values, taking into account
 * temporal properties if present.
 *
 * @param values The vector to sort
 * @param comp Optional comparison function
 * @return A sorted copy of the input vector
 *
 * Example usage:
 * @code
 * auto sorted = sort_values(my_values);
 * @endcode
 */
std::vector<Value> sort_values(const std::vector<Value>& values,
                               const std::function<bool(const Value&, const Value&)>& comp = {});

/**
 * @brief Finds a value in a collection
 *
 * This function searches for a value in a collection and returns its position.
 *
 * @param values The collection to search in
 * @param target The value to search for
 * @param options Optional search options
 * @return The position of the value, or -1 if not found
 *
 * Example usage:
 * @code
 * auto pos = find_value(my_values, Value(42));
 * @endcode
 */
int find_value(const std::vector<Value>& values, const Value& target,
               const std::map<std::string, Value>& options = {});

/**
 * @brief Removes duplicate values from a vector
 *
 * This function creates a new vector with duplicate values removed.
 *
 * @param values The input vector
 * @param temporal_equality Whether to consider temporal properties in equality comparison
 * @return A new vector with duplicates removed
 *
 * Example usage:
 * @code
 * auto unique = unique_values(my_values);
 * @endcode
 */
std::vector<Value> unique_values(const std::vector<Value>& values, bool temporal_equality = true);

//------------------------------------------------------
// Math and Numeric Utilities
//------------------------------------------------------

/**
 * @brief Calculates the sum of values in a collection
 *
 * @param values The collection of values to sum
 * @return The sum of the values
 *
 * Example usage:
 * @code
 * auto total = sum_values(my_values);
 * @endcode
 */
Value sum_values(const std::vector<Value>& values);

/**
 * @brief Calculates the average of values in a collection
 *
 * @param values The collection of values to average
 * @return The average of the values
 *
 * Example usage:
 * @code
 * auto avg = average_values(my_values);
 * @endcode
 */
Value average_values(const std::vector<Value>& values);

/**
 * @brief Calculates minimum and maximum values in a collection
 *
 * @param values The collection to analyze
 * @return A pair containing the minimum and maximum values
 *
 * Example usage:
 * @code
 * auto [min, max] = minmax_values(my_values);
 * @endcode
 */
std::pair<Value, Value> minmax_values(const std::vector<Value>& values);

/**
 * @brief Performs linear interpolation between two values
 *
 * @param start The starting value
 * @param end The ending value
 * @param t The interpolation factor (0.0 to 1.0)
 * @return The interpolated value
 *
 * Example usage:
 * @code
 * auto mid = lerp(Value(0), Value(10), 0.5);
 * @endcode
 */
Value lerp(const Value& start, const Value& end, double t);

/**
 * @brief Clamps a value between a minimum and maximum
 *
 * @param value The value to clamp
 * @param min The minimum allowed value
 * @param max The maximum allowed value
 * @return The clamped value
 *
 * Example usage:
 * @code
 * auto clamped = clamp(Value(15), Value(0), Value(10));
 * @endcode
 */
Value clamp(const Value& value, const Value& min, const Value& max);

//------------------------------------------------------
// General Utilities
//------------------------------------------------------

/**
 * @brief Generates a unique identifier with temporal properties
 *
 * This function generates a unique identifier that incorporates
 * temporal information.
 *
 * @param prefix Optional prefix for the identifier
 * @param options Optional generation options
 * @return A unique identifier string
 *
 * Example usage:
 * @code
 * auto id = generate_temporal_id("user");
 * @endcode
 */
std::string generate_temporal_id(const std::string& prefix = "",
                                 const std::map<std::string, Value>& options = {});

/**
 * @brief Measures the temporal cost of executing a function
 *
 * This utility executes a function and measures its temporal resource
 * usage, returning detailed metrics.
 *
 * @param func The function to execute and measure
 * @return A map of resource usage metrics
 *
 * Example usage:
 * @code
 * auto metrics = measure_temporal_cost([]() {
 *     // Function to measure
 *     return Value(42);
 * });
 * @endcode
 */
std::map<std::string, Value> measure_temporal_cost(const std::function<Value()>& func);

/**
 * @brief Caches the result of a function with temporal awareness
 *
 * This utility provides a temporally-aware caching mechanism for
 * expensive function calls.
 *
 * @param key The cache key
 * @param func The function to execute if the key is not in the cache
 * @param ttl Time-to-live for the cached value (in chronons)
 * @return The cached or computed value
 *
 * Example usage:
 * @code
 * auto result = cached_compute("my_key", []() {
 *     // Expensive computation
 *     return Value(42);
 * }, 100);
 * @endcode
 */
Value cached_compute(const std::string& key, const std::function<Value()>& func, int ttl = 0);

}  // namespace utility
}  // namespace stdlib
}  // namespace chronovyan

#endif  // CHRONOVYAN_STDLIB_UTILITY_FUNCTIONS_H