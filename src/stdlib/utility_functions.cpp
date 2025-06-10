/**
 * @file utility_functions.cpp
 * @brief Implementation of utility functions for the Chronovyan Standard
 * Library
 *
 * This file contains implementations for general-purpose utility functions that
 * provide essential tools for common programming tasks. These functions are
 * designed with temporal awareness and conform to Chronovyan's programming
 * model.
 *
 * @copyright Chronovyan Project
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <unordered_map>

#include "../include/stdlib/utility_functions.h"
#include "../include/temporal_runtime.h"
#include "../include/temporal_sequence.h"
#include "../include/value.h"

namespace chronovyan {
namespace stdlib {
namespace utility {

// Cache for the cached_compute function
static std::unordered_map<
    std::string, std::pair<Value, std::chrono::steady_clock::time_point>>
    cache;

//------------------------------------------------------
// String Utilities
//------------------------------------------------------

std::string to_string(const Value &value, const std::string &format) {
  std::ostringstream oss;

  if (format == "temporal") {
    // Include temporal properties in the string representation
    oss << value.to_string() << " [T:" << value.get_creation_time() << "]";
  } else if (format == "json") {
    // JSON format
    if (value.is_string()) {
      oss << "\"" << value.as_string() << "\"";
    } else if (value.is_number()) {
      oss << value.as_number();
    } else if (value.is_boolean()) {
      oss << (value.as_boolean() ? "true" : "false");
    } else if (value.is_null()) {
      oss << "null";
    } else if (value.is_object()) {
      // Simple JSON object representation
      oss << "{...}";
    } else if (value.is_array()) {
      // Simple JSON array representation
      oss << "[...]";
    } else {
      oss << value.to_string();
    }
  } else {
    // Default format
    oss << value.to_string();
  }

  return oss.str();
}

Value parse_string(const std::string &str, const std::string &type) {
  if (type == "number") {
    try {
      // Try to parse as double
      return Value(std::stod(str));
    } catch (const std::exception &e) {
      // Return NaN on failure
      return Value(std::numeric_limits<double>::quiet_NaN());
    }
  } else if (type == "boolean") {
    // Convert to lowercase for comparison
    std::string lower_str = str;
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (lower_str == "true" || lower_str == "yes" || lower_str == "1") {
      return Value(true);
    } else {
      return Value(false);
    }
  } else if (type == "temporal") {
    // Parse as temporal value (implementation depends on specific format)
    // This is a placeholder implementation
    try {
      double timestamp = std::stod(str);
      Value val(0.0);
      val.set_creation_time(timestamp);
      return val;
    } catch (const std::exception &e) {
      // Return default temporal value on failure
      return Value(0.0);
    }
  } else {
    // Default to string type
    return Value(str);
  }
}

std::vector<std::string> split_string(const std::string &str,
                                      const std::string &delimiter) {
  std::vector<std::string> parts;
  size_t start = 0;
  size_t end = 0;

  while ((end = str.find(delimiter, start)) != std::string::npos) {
    parts.push_back(str.substr(start, end - start));
    start = end + delimiter.length();
  }

  // Add the last part
  parts.push_back(str.substr(start));

  return parts;
}

std::string join_strings(const std::vector<std::string> &parts,
                         const std::string &delimiter) {
  std::ostringstream oss;

  for (size_t i = 0; i < parts.size(); ++i) {
    oss << parts[i];
    if (i < parts.size() - 1) {
      oss << delimiter;
    }
  }

  return oss.str();
}

std::string
temporal_string_transform(const std::string &str,
                          const std::string &transformation,
                          const std::map<std::string, Value> &params) {
  if (transformation == "reveal") {
    // Reveal characters over time based on current temporal state
    double duration = 5.0; // Default duration

    // Check if duration parameter is provided
    auto it = params.find("duration");
    if (it != params.end() && it->second.is_number()) {
      duration = it->second.as_number();
    }

    // Get current time from temporal runtime
    double current_time = TemporalRuntime::get_instance().get_current_time();

    // Calculate how many characters to reveal
    size_t revealed = static_cast<size_t>(
        std::min(1.0, current_time / duration) * str.length());

    return str.substr(0, revealed);
  } else if (transformation == "reverse") {
    // Reverse the string
    std::string reversed = str;
    std::reverse(reversed.begin(), reversed.end());
    return reversed;
  } else if (transformation == "encrypt") {
    // Simple Caesar cipher encryption (shift by 3)
    std::string encrypted = str;
    for (char &c : encrypted) {
      if (std::isalpha(c)) {
        char base = std::islower(c) ? 'a' : 'A';
        c = static_cast<char>(base + (c - base + 3) % 26);
      }
    }
    return encrypted;
  } else if (transformation == "decrypt") {
    // Simple Caesar cipher decryption (shift by -3)
    std::string decrypted = str;
    for (char &c : decrypted) {
      if (std::isalpha(c)) {
        char base = std::islower(c) ? 'a' : 'A';
        c = static_cast<char>(base + (c - base + 23) % 26);
      }
    }
    return decrypted;
  } else {
    // No transformation or unknown transformation
    return str;
  }
}

//------------------------------------------------------
// Conversion Utilities
//------------------------------------------------------

Value convert_value(const Value &value, const std::string &target_type,
                    const std::map<std::string, Value> &options) {
  if (target_type == "string") {
    // Convert to string
    std::string format = "default";

    // Check if format option is provided
    auto it = options.find("format");
    if (it != options.end() && it->second.is_string()) {
      format = it->second.as_string();
    }

    return Value(to_string(value, format));
  } else if (target_type == "number") {
    // Convert to number
    if (value.is_number()) {
      return value;
    } else if (value.is_string()) {
      int base = 10; // Default base

      // Check if base option is provided
      auto it = options.find("base");
      if (it != options.end() && it->second.is_number()) {
        base = static_cast<int>(it->second.as_number());
      }

      try {
        if (base == 10) {
          return Value(std::stod(value.as_string()));
        } else if (base == 16) {
          // Hexadecimal conversion
          std::string hex_str = value.as_string();
          // Remove "0x" prefix if present
          if (hex_str.length() > 2 && hex_str.substr(0, 2) == "0x") {
            hex_str = hex_str.substr(2);
          }
          return Value(static_cast<double>(std::stoul(hex_str, nullptr, 16)));
        } else {
          // Other bases
          return Value(static_cast<double>(
              std::stoul(value.as_string(), nullptr, base)));
        }
      } catch (const std::exception &e) {
        return Value(std::numeric_limits<double>::quiet_NaN());
      }
    } else if (value.is_boolean()) {
      return Value(value.as_boolean() ? 1.0 : 0.0);
    } else {
      return Value(std::numeric_limits<double>::quiet_NaN());
    }
  } else if (target_type == "boolean") {
    // Convert to boolean
    if (value.is_boolean()) {
      return value;
    } else if (value.is_number()) {
      return Value(value.as_number() != 0.0);
    } else if (value.is_string()) {
      std::string str = value.as_string();
      std::transform(str.begin(), str.end(), str.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      return Value(str == "true" || str == "yes" || str == "1");
    } else {
      return Value(false);
    }
  } else {
    // Unknown target type, return the original value
    return value;
  }
}

Value convert_sequence(const TemporalSequence &sequence,
                       const std::string &target_type,
                       const std::map<std::string, Value> &options) {
  if (target_type == "vector") {
    // Convert to a vector of values
    std::vector<Value> values;

    for (const auto &entry : sequence.get_entries()) {
      values.push_back(entry.value);
    }

    return Value(values);
  } else if (target_type == "map") {
    // Convert to a map with timestamps as keys
    std::map<std::string, Value> map_values;

    for (const auto &entry : sequence.get_entries()) {
      map_values[std::to_string(entry.timestamp)] = entry.value;
    }

    return Value(map_values);
  } else if (target_type == "string") {
    // Convert to a string representation
    std::string format = "default";
    std::string delimiter = ", ";

    // Check if format option is provided
    auto it = options.find("format");
    if (it != options.end() && it->second.is_string()) {
      format = it->second.as_string();
    }

    // Check if delimiter option is provided
    it = options.find("delimiter");
    if (it != options.end() && it->second.is_string()) {
      delimiter = it->second.as_string();
    }

    std::ostringstream oss;

    if (format == "json") {
      oss << "[";
      bool first = true;

      for (const auto &entry : sequence.get_entries()) {
        if (!first) {
          oss << delimiter;
        }
        first = false;

        oss << "{\"timestamp\":" << entry.timestamp
            << ", \"value\":" << to_string(entry.value, "json") << "}";
      }

      oss << "]";
    } else {
      // Default format
      oss << "[";
      bool first = true;

      for (const auto &entry : sequence.get_entries()) {
        if (!first) {
          oss << delimiter;
        }
        first = false;

        oss << entry.timestamp << ":" << to_string(entry.value, format);
      }

      oss << "]";
    }

    return Value(oss.str());
  } else {
    // Unknown target type, return an empty value
    return Value();
  }
}

//------------------------------------------------------
// Collection Utilities
//------------------------------------------------------

std::vector<Value>
sort_values(const std::vector<Value> &values,
            const std::function<bool(const Value &, const Value &)> &comp) {
  std::vector<Value> sorted = values;

  if (comp) {
    // Use the provided comparison function
    std::sort(sorted.begin(), sorted.end(), comp);
  } else {
    // Default comparison based on natural ordering with temporal awareness
    std::sort(sorted.begin(), sorted.end(), [](const Value &a, const Value &b) {
      // Compare based on value type
      if (a.get_type() != b.get_type()) {
        return a.get_type() < b.get_type();
      }

      // Same type, compare values
      if (a.is_number()) {
        return a.as_number() < b.as_number();
      } else if (a.is_string()) {
        return a.as_string() < b.as_string();
      } else if (a.is_boolean()) {
        return !a.as_boolean() && b.as_boolean();
      } else {
        // For complex types or if types don't match for comparison,
        // fall back to temporal properties
        return a.get_creation_time() < b.get_creation_time();
      }
    });
  }

  return sorted;
}

int find_value(const std::vector<Value> &values, const Value &target,
               const std::map<std::string, Value> &options) {
  bool temporal_match = false;

  // Check if temporal_match option is provided
  auto it = options.find("temporal_match");
  if (it != options.end() && it->second.is_boolean()) {
    temporal_match = it->second.as_boolean();
  }

  for (size_t i = 0; i < values.size(); ++i) {
    if (temporal_match) {
      // Match based on both value and temporal properties
      if (values[i] == target &&
          values[i].get_creation_time() == target.get_creation_time()) {
        return static_cast<int>(i);
      }
    } else {
      // Match based on value only
      if (values[i] == target) {
        return static_cast<int>(i);
      }
    }
  }

  return -1; // Not found
}

std::vector<Value> unique_values(const std::vector<Value> &values,
                                 bool temporal_equality) {
  std::vector<Value> unique;

  for (const auto &value : values) {
    bool is_duplicate = false;

    for (const auto &existing : unique) {
      if (temporal_equality) {
        // Consider temporal properties in equality comparison
        if (value == existing &&
            value.get_creation_time() == existing.get_creation_time()) {
          is_duplicate = true;
          break;
        }
      } else {
        // Compare only the values
        if (value == existing) {
          is_duplicate = true;
          break;
        }
      }
    }

    if (!is_duplicate) {
      unique.push_back(value);
    }
  }

  return unique;
}

//------------------------------------------------------
// Math and Numeric Utilities
//------------------------------------------------------

Value sum_values(const std::vector<Value> &values) {
  double sum = 0.0;
  bool all_numbers = true;

  for (const auto &value : values) {
    if (value.is_number()) {
      sum += value.as_number();
    } else {
      all_numbers = false;
      break;
    }
  }

  if (all_numbers) {
    return Value(sum);
  } else {
    // If not all values are numbers, return NaN
    return Value(std::numeric_limits<double>::quiet_NaN());
  }
}

Value average_values(const std::vector<Value> &values) {
  if (values.empty()) {
    return Value(std::numeric_limits<double>::quiet_NaN());
  }

  double sum = 0.0;
  bool all_numbers = true;

  for (const auto &value : values) {
    if (value.is_number()) {
      sum += value.as_number();
    } else {
      all_numbers = false;
      break;
    }
  }

  if (all_numbers) {
    return Value(sum / values.size());
  } else {
    // If not all values are numbers, return NaN
    return Value(std::numeric_limits<double>::quiet_NaN());
  }
}

std::pair<Value, Value> minmax_values(const std::vector<Value> &values) {
  if (values.empty()) {
    return {Value(std::numeric_limits<double>::quiet_NaN()),
            Value(std::numeric_limits<double>::quiet_NaN())};
  }

  bool all_numbers = true;
  double min_val = std::numeric_limits<double>::max();
  double max_val = std::numeric_limits<double>::lowest();

  for (const auto &value : values) {
    if (value.is_number()) {
      double num = value.as_number();
      min_val = std::min(min_val, num);
      max_val = std::max(max_val, num);
    } else {
      all_numbers = false;
      break;
    }
  }

  if (all_numbers) {
    return {Value(min_val), Value(max_val)};
  } else {
    // If values are mixed types, try to determine min/max based on string
    // comparison
    Value min_value = values[0];
    Value max_value = values[0];

    for (size_t i = 1; i < values.size(); ++i) {
      std::string current_str = to_string(values[i], "default");
      std::string min_str = to_string(min_value, "default");
      std::string max_str = to_string(max_value, "default");

      if (current_str < min_str) {
        min_value = values[i];
      }

      if (current_str > max_str) {
        max_value = values[i];
      }
    }

    return {min_value, max_value};
  }
}

Value lerp(const Value &start, const Value &end, double t) {
  // Clamp t to [0, 1]
  t = std::max(0.0, std::min(1.0, t));

  if (start.is_number() && end.is_number()) {
    // Linear interpolation between numbers
    double start_val = start.as_number();
    double end_val = end.as_number();
    return Value(start_val + t * (end_val - start_val));
  } else if (start.is_string() && end.is_string()) {
    // Interpolation between strings (by character position)
    const std::string &start_str = start.as_string();
    const std::string &end_str = end.as_string();

    // Create a string with length based on interpolation
    size_t length = static_cast<size_t>(
        start_str.length() +
        t * (static_cast<double>(end_str.length()) - start_str.length()));

    std::string result;
    for (size_t i = 0; i < length; ++i) {
      if (i < start_str.length() && i < end_str.length()) {
        // Interpolate between characters
        char start_char = start_str[i];
        char end_char = end_str[i];
        char interpolated = static_cast<char>(
            start_char + static_cast<char>(t * (end_char - start_char)));
        result.push_back(interpolated);
      } else if (i < start_str.length()) {
        // Start string is longer
        result.push_back(start_str[i]);
      } else if (i < end_str.length()) {
        // End string is longer
        result.push_back(end_str[i]);
      }
    }

    return Value(result);
  } else {
    // For mixed types or non-interpolatable types, return the start or end
    // value based on t
    return (t < 0.5) ? start : end;
  }
}

Value clamp(const Value &value, const Value &min, const Value &max) {
  if (value.is_number() && min.is_number() && max.is_number()) {
    double val = value.as_number();
    double min_val = min.as_number();
    double max_val = max.as_number();

    return Value(std::max(min_val, std::min(val, max_val)));
  } else {
    // For non-numeric values, return the original value
    return value;
  }
}

//------------------------------------------------------
// General Utilities
//------------------------------------------------------

std::string generate_temporal_id(const std::string &prefix,
                                 const std::map<std::string, Value> &options) {
  // Generate a random component
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
  uint32_t random_component = dist(gen);

  // Get current time
  auto now = std::chrono::system_clock::now();
  auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
  auto epoch = now_ms.time_since_epoch();
  uint64_t timestamp = static_cast<uint64_t>(epoch.count());

  // Generate the ID
  std::ostringstream oss;

  if (!prefix.empty()) {
    oss << prefix << "-";
  }

  // Check if format option is provided
  std::string format = "standard";
  auto it = options.find("format");
  if (it != options.end() && it->second.is_string()) {
    format = it->second.as_string();
  }

  if (format == "hex") {
    oss << std::hex << std::setfill('0') << std::setw(16) << timestamp << "-"
        << std::setw(8) << random_component;
  } else if (format == "compact") {
    // Use base64-like encoding for more compact representation
    static const char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

    // Encode timestamp (48 bits)
    for (int i = 0; i < 8; ++i) {
      oss << charset[(timestamp >> (6 * i)) & 0x3F];
    }

    oss << "-";

    // Encode random component (32 bits)
    for (int i = 0; i < 6; ++i) {
      oss << charset[(random_component >> (6 * i)) & 0x3F];
    }
  } else {
    // Standard format (decimal)
    oss << timestamp << "-" << random_component;
  }

  // Add temporal context if requested
  bool include_temporal = false;
  it = options.find("include_temporal");
  if (it != options.end() && it->second.is_boolean()) {
    include_temporal = it->second.as_boolean();
  }

  if (include_temporal) {
    double current_time = TemporalRuntime::get_instance().get_current_time();
    oss << "-T" << static_cast<uint64_t>(current_time * 1000);
  }

  return oss.str();
}

std::map<std::string, Value>
measure_temporal_cost(const std::function<Value()> &func) {
  std::map<std::string, Value> metrics;

  // Record start time
  auto start_time = std::chrono::high_resolution_clock::now();

  // Record initial resource state
  auto &runtime = TemporalRuntime::get_instance();
  double initial_chronons = runtime.get_chronon_usage();
  double initial_aethel = runtime.get_aethel_generation();
  double initial_debt = runtime.get_temporal_debt();

  // Execute the function
  Value result = func();

  // Record end time
  auto end_time = std::chrono::high_resolution_clock::now();

  // Calculate elapsed time in milliseconds
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                      end_time - start_time)
                      .count();

  // Record final resource state
  double final_chronons = runtime.get_chronon_usage();
  double final_aethel = runtime.get_aethel_generation();
  double final_debt = runtime.get_temporal_debt();

  // Calculate resource usage
  double chronon_usage = final_chronons - initial_chronons;
  double aethel_generation = final_aethel - initial_aethel;
  double debt_accrual = final_debt - initial_debt;

  // Store metrics
  metrics["result"] = result;
  metrics["execution_time_ms"] = Value(static_cast<double>(duration));
  metrics["chronon_usage"] = Value(chronon_usage);
  metrics["aethel_generation"] = Value(aethel_generation);
  metrics["temporal_debt"] = Value(debt_accrual);
  metrics["efficiency"] =
      Value(aethel_generation / (chronon_usage > 0 ? chronon_usage : 1.0));

  return metrics;
}

Value cached_compute(const std::string &key, const std::function<Value()> &func,
                     int ttl) {
  auto now = std::chrono::steady_clock::now();

  // Check if the value is in cache and not expired
  auto it = cache.find(key);
  if (it != cache.end()) {
    if (ttl <= 0 || std::chrono::duration_cast<std::chrono::seconds>(
                        now - it->second.second)
                            .count() < ttl) {
      // Cache hit
      return it->second.first;
    }
  }

  // Cache miss or expired, compute the value
  Value result = func();

  // Store in cache
  cache[key] = {result, now};

  return result;
}

} // namespace utility
} // namespace stdlib
} // namespace chronovyan